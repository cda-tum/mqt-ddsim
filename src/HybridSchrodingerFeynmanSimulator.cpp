#include "HybridSchrodingerFeynmanSimulator.hpp"

std::size_t HybridSchrodingerFeynmanSimulator::getNDecisions(dd::Qubit split_qubit) {
    std::size_t ndecisions = 0;
    // calculate number of decisions
    for (const auto& op: *qc) {
        if (dynamic_cast<qc::StandardOperation*>(op.get())) {
            bool target_in_lower_slice = false, target_in_upper_slice = false;
            bool control_in_lower_slice = false, control_in_upper_slice = false;
            for (const auto& target: op->getTargets()) {
                target_in_lower_slice = target_in_lower_slice || target < split_qubit;
                target_in_upper_slice = target_in_upper_slice || target >= split_qubit;
            }
            for (const auto& control: op->getControls()) {
                control_in_lower_slice = control_in_lower_slice || control.qubit < split_qubit;
                control_in_upper_slice = control_in_upper_slice || control.qubit >= split_qubit;
            }
            if ((target_in_lower_slice && control_in_upper_slice) ||
                (target_in_upper_slice && control_in_lower_slice)) {
                ndecisions++;
            }
        } else {
            throw std::invalid_argument("Only StandardOperations are supported for now.");
        }
    }
    return ndecisions;
}

qc::VectorDD HybridSchrodingerFeynmanSimulator::SimulateSlicing(std::unique_ptr<dd::Package>& slice_dd, dd::Qubit split_qubit, std::size_t controls) {
    Slice lower(slice_dd, 0, static_cast<dd::Qubit>(split_qubit - 1), controls);
    Slice upper(slice_dd, split_qubit, static_cast<dd::Qubit>(getNumberOfQubits() - 1), controls);

    for (const auto& op: *qc) {
        if (op->isUnitary()) {
            [[maybe_unused]] auto l = lower.apply(slice_dd, op);
            [[maybe_unused]] auto u = upper.apply(slice_dd, op);
            assert(l == u);
        }
        slice_dd->garbageCollect();
    }

    auto result = slice_dd->kronecker(upper.edge, lower.edge, false);
    slice_dd->incRef(result);

    return result;
}

bool HybridSchrodingerFeynmanSimulator::Slice::apply(std::unique_ptr<dd::Package>& slice_dd, const std::unique_ptr<qc::Operation>& op) {
    bool is_split_op = false;
    if (reinterpret_cast<qc::StandardOperation*>(op.get())) { // TODO change control and target if wrong direction
        qc::Targets  op_targets{};
        dd::Controls op_controls{};

        // check targets
        bool target_in_split = false, target_in_other_split = false;
        for (const auto& target: op->getTargets()) {
            if (start <= target && target <= end) {
                op_targets.push_back(target);
                target_in_split = true;
            } else {
                target_in_other_split = true;
            }
        }

        if (target_in_split && target_in_other_split && !op->getControls().empty()) {
            throw std::invalid_argument("Multiple Targets that are in different slices are not supported at the moment");
        }

        // check controls
        for (const auto& control: op->getControls()) {
            if (start <= control.qubit && control.qubit <= end) {
                op_controls.emplace(dd::Control{control.qubit, control.type});
            } else { // other controls are set to the corresponding value
                if (target_in_split) {
                    is_split_op       = true;
                    bool next_control = getNextControl();
                    if ((control.type == dd::Control::Type::pos && !next_control) || // break if control is not activated
                        (control.type == dd::Control::Type::neg && next_control)) {
                        nDecisionsExecuted++;
                        return true;
                    }
                }
            }
        }

        if (target_in_other_split && !op_controls.empty()) { // control slice for split
            if (op_controls.size() > 1) {
                throw std::invalid_argument("Multiple controls in control slice of operation are not supported at the moment");
            }

            is_split_op  = true;
            bool control = getNextControl();
            for (const auto& c: op_controls) {
                slice_dd->decRef(edge); // TODO incref and decref could be integrated in delete edge
                edge = slice_dd->deleteEdge(edge, c.qubit, control ? (c.type == dd::Control::Type::pos ? 0 : 1) : (c.type == dd::Control::Type::pos ? 1 : 0));
                slice_dd->incRef(edge);
            }
        } else if (target_in_split) { // target slice for split or operation in split
            const auto&           param = op->getParameter();
            qc::StandardOperation new_op(nqubits, op_controls, op_targets, op->getType(), param[0], param[1], param[2], start);
            slice_dd->decRef(edge);
            edge = slice_dd->multiply(new_op.getDD(slice_dd), edge, start);
            slice_dd->incRef(edge);
        }
    } else {
        throw std::invalid_argument("Only StandardOperations are supported for now.");
    }
    if (is_split_op) {
        nDecisionsExecuted++;
    }
    return is_split_op;
}

std::map<std::string, std::size_t> HybridSchrodingerFeynmanSimulator::Simulate(unsigned int shots) {
    auto nqubits    = getNumberOfQubits();
    auto splitQubit = static_cast<dd::Qubit>(nqubits / 2);
    if (mode == Mode::DD) {
        SimulateHybrid(splitQubit);
        return MeasureAllNonCollapsing(shots);
    } else {
        SimulateHybridAmplitudes(splitQubit);

        if (shots > 0) {
            return SampleFromAmplitudeVectorInPlace(finalAmplitudes, shots);
        } else {
            // in case no shots were requested, the final amplitudes remain untouched
            return {};
        }
    }
}

void HybridSchrodingerFeynmanSimulator::SimulateHybrid(dd::Qubit split_qubit) {
    auto               ndecisions  = getNDecisions(split_qubit);
    const std::int64_t max_control = 1LL << ndecisions;

    int actuallyUsedThreads = static_cast<int>(nthreads);
    if (static_cast<std::size_t>(max_control) < nthreads) {
        actuallyUsedThreads = static_cast<int>(max_control);
    }
    omp_set_num_threads(actuallyUsedThreads);
    root_edge                    = qc::VectorDD::zero;
    std::int64_t nslices_at_once = 16;
    nslices_at_once              = std::min(nslices_at_once, max_control / static_cast<std::int64_t>(actuallyUsedThreads));
    std::stack<std::pair<std::int64_t, std::int64_t>> stack{};
    for (auto i = max_control - 1; i >= 0; i -= nslices_at_once) {
        stack.emplace(0, i);
    }
    std::vector<std::vector<bool>> computed(ndecisions, std::vector<bool>(max_control, false));

    omp_lock_t stacklock;
    omp_init_lock(&stacklock);
#pragma omp parallel // NOLINT(openmp-use-default-none)
    {
        bool                                  isempty;
        std::pair<std::int64_t, std::int64_t> current{};
        omp_set_lock(&stacklock);
        isempty = stack.empty();
        if (!isempty) {
            current = stack.top();
            stack.pop();
        }
        omp_unset_lock(&stacklock);

        while (!isempty) {
            if (current.first == 0) { // slice
                std::unique_ptr<dd::Package> old_dd;
                qc::VectorDD                 edge{};
                for (std::int64_t i = 0; i < nslices_at_once; i++) {
                    auto slice_dd = std::make_unique<dd::Package>(getNumberOfQubits());
                    auto result   = SimulateSlicing(slice_dd, split_qubit, current.second + i);
                    if (i > 0) {
                        edge = slice_dd->add(slice_dd->transfer(edge), result);
                    } else {
                        edge = result;
                    }
                    old_dd = std::move(slice_dd);
                }

                current.first = static_cast<std::int64_t>(std::log2(nslices_at_once));
                current.second /= nslices_at_once;
                dd::serialize(edge, "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd", true);
            } else { // adding
                auto        slice_dd       = std::make_unique<dd::Package>(getNumberOfQubits());
                std::string filename       = "slice_" + std::to_string(current.first - 1) + "_";
                std::string filename_left  = filename + std::to_string(current.second * 2) + ".dd";
                std::string filename_right = filename + std::to_string(current.second * 2 + 1) + ".dd";

                auto result  = slice_dd->deserialize<dd::Package::vNode>(filename_left, true);
                auto result2 = slice_dd->deserialize<dd::Package::vNode>(filename_right, true);
                result       = slice_dd->add(result, result2);
                dd::serialize(result, "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd", true);

                remove(filename_left.c_str());
                remove(filename_right.c_str());
            }

            omp_set_lock(&stacklock);
            if (current.first < static_cast<std::int64_t>(ndecisions)) {
                computed[current.first][current.second] = true;

                if (computed[current.first][current.second + (current.second % 2 ? (-1) : 1)]) {
                    stack.push(std::make_pair(current.first + 1, current.second / 2));
                }
            }
            isempty = stack.empty();
            if (!isempty) {
                current = stack.top();
                stack.pop();
            }
            omp_unset_lock(&stacklock);
        }
    }
    omp_destroy_lock(&stacklock);
    root_edge = dd->deserialize<dd::Package::vNode>("slice_" + std::to_string(ndecisions) + "_0.dd", true);
    dd->incRef(root_edge);
}
void HybridSchrodingerFeynmanSimulator::SimulateHybridAmplitudes(dd::Qubit split_qubit) {
    auto               ndecisions  = getNDecisions(split_qubit);
    const std::int64_t max_control = 1LL << ndecisions;

    int actuallyUsedThreads = static_cast<int>(nthreads);
    if (static_cast<std::size_t>(max_control) < nthreads) {
        actuallyUsedThreads = static_cast<int>(max_control);
    }
    omp_set_num_threads(actuallyUsedThreads);

    std::vector<std::vector<dd::ComplexValue>> amplitudes(actuallyUsedThreads);
    std::vector<bool>                          initialized(actuallyUsedThreads, false);

    root_edge                         = qc::VectorDD::zero;
    std::int64_t   nslices_on_one_cpu = std::min(static_cast<std::int64_t>(64), static_cast<std::int64_t>(max_control / actuallyUsedThreads));
    dd::QubitCount nqubits            = getNumberOfQubits();

#pragma omp parallel for schedule(dynamic, 1) // NOLINT(openmp-use-default-none)
    for (std::int64_t control = 0; control < max_control; control += nslices_on_one_cpu) {
        auto                           current_thread     = omp_get_thread_num();
        std::vector<dd::ComplexValue>& thread_amplitudes  = amplitudes[current_thread];
        bool                           thread_initialized = initialized[current_thread];
        for (std::int64_t local_control = 0; local_control < nslices_on_one_cpu; local_control++) {
            std::int64_t                 total_control = control + local_control;
            std::unique_ptr<dd::Package> slice_dd      = std::make_unique<dd::Package>(getNumberOfQubits());
            auto                         result        = SimulateSlicing(slice_dd, split_qubit, total_control);

            if (thread_initialized) {
                slice_dd->addAmplitudes(result, thread_amplitudes, nqubits);
            } else {
                initialized[current_thread] = true;
                thread_initialized          = true;
                thread_amplitudes           = std::vector<dd::ComplexValue>(1 << nqubits);
                slice_dd->exportAmplitudes(result, thread_amplitudes, nqubits);
            }
        }
    }

    std::size_t old_increment = 1;
    for (unsigned short level = 0; level < static_cast<unsigned short>(std::log2(actuallyUsedThreads)); ++level) {
        const int increment = 2 << level;
        for (int idx = 0; idx < actuallyUsedThreads; idx += increment) {
            // in-place addition of amplitudes
            std::transform(amplitudes[idx].begin(), amplitudes[idx].end(),
                           amplitudes[idx + old_increment].begin(),
                           amplitudes[idx].begin(),
                           std::plus<>());
        }
        old_increment = increment;
    }
    finalAmplitudes = std::move(amplitudes[0]);
}
