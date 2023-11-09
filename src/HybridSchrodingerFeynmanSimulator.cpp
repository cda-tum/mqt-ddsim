#include "HybridSchrodingerFeynmanSimulator.hpp"

#include <cassert>
#include <cmath>
#include <taskflow/taskflow.hpp>

template<class Config>
std::size_t HybridSchrodingerFeynmanSimulator<Config>::getNDecisions(qc::Qubit splitQubit) {
    std::size_t ndecisions = 0;
    // calculate number of decisions
    for (const auto& op: *CircuitSimulator<Config>::qc) {
        if (op->getType() == qc::Barrier) {
            continue;
        }
        if (op->isStandardOperation()) {
            bool targetInLowerSlice  = false;
            bool targetInUpperSlice  = false;
            bool controlInLowerSlice = false;
            bool controlInUpperSlice = false;
            for (const auto& target: op->getTargets()) {
                targetInLowerSlice = targetInLowerSlice || target < splitQubit;
                targetInUpperSlice = targetInUpperSlice || target >= splitQubit;
            }
            for (const auto& control: op->getControls()) {
                controlInLowerSlice = controlInLowerSlice || control.qubit < splitQubit;
                controlInUpperSlice = controlInUpperSlice || control.qubit >= splitQubit;
            }
            if ((targetInLowerSlice && controlInUpperSlice) ||
                (targetInUpperSlice && controlInLowerSlice)) {
                ndecisions++;
            }
        } else {
            throw std::invalid_argument("Only StandardOperations are supported for now.");
        }
    }
    return ndecisions;
}

template<class Config>
qc::VectorDD HybridSchrodingerFeynmanSimulator<Config>::simulateSlicing(std::unique_ptr<dd::Package<Config>>& sliceDD, unsigned int splitQubit, size_t controls) {
    Slice lower(sliceDD, 0, splitQubit - 1, controls);
    Slice upper(sliceDD, splitQubit, static_cast<qc::Qubit>(CircuitSimulator<Config>::getNumberOfQubits() - 1), controls);

    for (const auto& op: *CircuitSimulator<Config>::qc) {
        if (op->isUnitary()) {
            [[maybe_unused]] auto l = lower.apply(sliceDD, op);
            [[maybe_unused]] auto u = upper.apply(sliceDD, op);
            assert(l == u);
        }
        sliceDD->garbageCollect();
    }

    auto result = sliceDD->kronecker(upper.edge, lower.edge, false);
    sliceDD->incRef(result);

    return result;
}

template<class Config>
bool HybridSchrodingerFeynmanSimulator<Config>::Slice::apply(std::unique_ptr<dd::Package<Config>>& sliceDD, const std::unique_ptr<qc::Operation>& op) {
    bool isSplitOp = false;
    if (dynamic_cast<qc::StandardOperation*>(op.get()) != nullptr) { // TODO change control and target if wrong direction
        qc::Targets  opTargets{};
        qc::Controls opControls{};

        // check targets
        bool targetInSplit      = false;
        bool targetInOtherSplit = false;
        for (const auto& target: op->getTargets()) {
            if (start <= target && target <= end) {
                opTargets.push_back(target);
                targetInSplit = true;
            } else {
                targetInOtherSplit = true;
            }
        }

        if (targetInSplit && targetInOtherSplit && !op->getControls().empty()) {
            throw std::invalid_argument("Multiple Targets that are in different slices are not supported at the moment");
        }

        // check controls
        for (const auto& control: op->getControls()) {
            if (start <= control.qubit && control.qubit <= end) {
                opControls.emplace(control.qubit, control.type);
            } else { // other controls are set to the corresponding value
                if (targetInSplit) {
                    isSplitOp              = true;
                    const bool nextControl = getNextControl();
                    if ((control.type == qc::Control::Type::Pos && !nextControl) || // break if control is not activated
                        (control.type == qc::Control::Type::Neg && nextControl)) {
                        nDecisionsExecuted++;
                        return true;
                    }
                }
            }
        }

        if (targetInOtherSplit && !opControls.empty()) { // control slice for split
            if (opControls.size() > 1) {
                throw std::invalid_argument("Multiple controls in control slice of operation are not supported at the moment");
            }

            isSplitOp          = true;
            const bool control = getNextControl();
            for (const auto& c: opControls) {
                sliceDD->decRef(edge); // TODO incref and decref could be integrated in delete edge
                edge = sliceDD->deleteEdge(edge, static_cast<dd::Qubit>(c.qubit), control ? (c.type == qc::Control::Type::Pos ? 0 : 1) : (c.type == qc::Control::Type::Pos ? 1 : 0));
                sliceDD->incRef(edge);
            }
        } else if (targetInSplit) { // target slice for split or operation in split
            const auto&           param = op->getParameter();
            qc::StandardOperation newOp(nqubits, opControls, opTargets, op->getType(), param, start);
            sliceDD->decRef(edge);
            edge = sliceDD->multiply(dd::getDD(&newOp, sliceDD), edge, static_cast<dd::Qubit>(start));
            sliceDD->incRef(edge);
        }
    } else {
        throw std::invalid_argument("Only StandardOperations are supported for now.");
    }
    if (isSplitOp) {
        nDecisionsExecuted++;
    }
    return isSplitOp;
}

template<class Config>
std::map<std::string, std::size_t> HybridSchrodingerFeynmanSimulator<Config>::simulate(std::size_t shots) {
    auto nqubits    = CircuitSimulator<Config>::getNumberOfQubits();
    auto splitQubit = static_cast<qc::Qubit>(nqubits / 2);
    if (mode == Mode::DD) {
        simulateHybridTaskflow(splitQubit);
        return Simulator<Config>::measureAllNonCollapsing(shots);
    }
    simulateHybridAmplitudes(splitQubit);

    if (shots > 0) {
        return Simulator<Config>::sampleFromAmplitudeVectorInPlace(finalAmplitudes, shots);
    }
    // in case no shots were requested, the final amplitudes remain untouched
    return {};
}

template<class Config>
void HybridSchrodingerFeynmanSimulator<Config>::simulateHybridTaskflow(unsigned int splitQubit) {
    const auto ndecisions          = getNDecisions(splitQubit);
    const auto maxControl          = 1ULL << ndecisions;
    const auto actuallyUsedThreads = std::min<std::size_t>(maxControl, nthreads);
    const auto chunkSize           = static_cast<std::size_t>(std::ceil(static_cast<double>(maxControl) / static_cast<double>(actuallyUsedThreads)));
    const auto nslicesOnOneCpu     = std::min<std::size_t>(16, chunkSize);
    const auto nqubits             = CircuitSimulator<Config>::getNumberOfQubits();
    const auto nresults            = static_cast<std::size_t>(std::ceil(static_cast<double>(maxControl) / static_cast<double>(nslicesOnOneCpu)));
    const auto lastLevel           = static_cast<std::size_t>(std::ceil(std::log2(nresults)));
    Simulator<Config>::rootEdge    = qc::VectorDD::zero();

    std::vector<std::vector<bool>> computed(ndecisions, std::vector<bool>(maxControl, false));

    tf::Executor executor(actuallyUsedThreads);

    std::function<void(std::pair<std::size_t, std::size_t>)> computePair = [this, &computePair, &computed, &executor, nslicesOnOneCpu, splitQubit, maxControl, nqubits, lastLevel](std::pair<std::size_t, std::size_t> current) {
        if (current.first == 0) { // slice
            std::unique_ptr<dd::Package<Config>> oldDD;
            qc::VectorDD                         edge{};
            for (std::size_t i = 0; i < nslicesOnOneCpu; ++i) {
                const auto totalControl = current.second + i;
                if (totalControl >= maxControl) {
                    break;
                }
                auto sliceDD = std::make_unique<dd::Package<Config>>(nqubits);
                auto result  = simulateSlicing(sliceDD, splitQubit, totalControl);
                if (i > 0) {
                    edge = sliceDD->add(sliceDD->transfer(edge), result);
                } else {
                    edge = result;
                }
                oldDD = std::move(sliceDD); // this might seem unused, but it keeps the DD package alive for the serialization below
            }
            dd::serialize(edge, "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd", true);
        } else { // adding
            const std::string filename     = "slice_" + std::to_string(current.first - 1) + "_";
            const std::string filenameLeft = filename + std::to_string(current.second) + ".dd";
            const auto        offset       = (1ULL << (current.first - 1)) * nslicesOnOneCpu;
            const auto        idx          = current.second + offset;
            if (idx >= maxControl) {
                return;
            }
            const std::string filenameRight = filename + std::to_string(idx) + ".dd";

            auto sliceDD = std::make_unique<dd::Package<Config>>(nqubits);
            auto result  = sliceDD->template deserialize<dd::vNode>(filenameLeft, true);
            auto result2 = sliceDD->template deserialize<dd::vNode>(filenameRight, true);
            result       = sliceDD->add(result, result2);
            dd::serialize(result, "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd", true);

            remove(filenameLeft.c_str());
            remove(filenameRight.c_str());
        }

        if (current.first < lastLevel) {
            computed.at(current.first).at(current.second) = true;

            auto offset       = (1ULL << current.first) * nslicesOnOneCpu;
            auto chunk        = current.second / offset;
            auto secondResult = (chunk % 2 == 0) ? (current.second + offset) : (current.second - offset);
            if (secondResult >= maxControl) {
                // lonely result, promote to next level
                const auto filename    = "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd";
                const auto newFilename = "slice_" + std::to_string(current.first + 1) + "_" + std::to_string(current.second) + ".dd";
                rename(filename.c_str(), newFilename.c_str());
                current.first += 1;
                computed.at(current.first).at(current.second) = true;
                // compute new second result
                offset *= 2;
                chunk        = current.second / offset;
                secondResult = (chunk % 2 == 0) ? (current.second + offset) : (current.second - offset);
            }
            if (computed.at(current.first).at(secondResult)) {
                current.first += 1;
                current.second = std::min(current.second, static_cast<std::size_t>(secondResult));
                executor.silent_async([&computePair, current]() { computePair(current); });
            }
        }
    };

    for (std::uint64_t i = 0U; i < maxControl; i += nslicesOnOneCpu) {
        executor.silent_async([&computePair, i]() { computePair(std::make_pair(0, i)); });
    }
    executor.wait_for_all();

    const auto filename         = "slice_" + std::to_string(lastLevel) + "_0.dd";
    Simulator<Config>::rootEdge = Simulator<Config>::dd->template deserialize<dd::vNode>(filename, true);
    Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);
    remove(filename.c_str());
}

template<class Config>
void HybridSchrodingerFeynmanSimulator<Config>::simulateHybridAmplitudes(qc::Qubit splitQubit) {
    const auto ndecisions          = getNDecisions(splitQubit);
    const auto maxControl          = 1ULL << ndecisions;
    const auto actuallyUsedThreads = std::min<std::size_t>(maxControl, nthreads);
    const auto chunkSize           = static_cast<std::size_t>(std::ceil(static_cast<double>(maxControl) / static_cast<double>(actuallyUsedThreads)));
    const auto nslicesOnOneCpu     = std::min<std::size_t>(64, chunkSize);
    const auto nqubits             = CircuitSimulator<Config>::getNumberOfQubits();
    const auto requiredVectors     = static_cast<std::size_t>(std::ceil(static_cast<double>(maxControl) / static_cast<double>(nslicesOnOneCpu)));
    Simulator<Config>::rootEdge    = qc::VectorDD::zero();

    std::vector<dd::CVec> amplitudes(requiredVectors, dd::CVec(1ULL << nqubits, 0));

    tf::Executor executor(actuallyUsedThreads);
    for (std::size_t control = 0, i = 0; control < maxControl; control += nslicesOnOneCpu, i++) {
        executor.silent_async([this, i, &amplitudes, nslicesOnOneCpu, control, splitQubit, maxControl]() {
            const auto                         currentThread    = i;
            std::vector<std::complex<dd::fp>>& threadAmplitudes = amplitudes.at(currentThread);

            for (std::size_t localControl = 0; localControl < nslicesOnOneCpu; localControl++) {
                const std::size_t totalControl = control + localControl;
                if (totalControl >= maxControl) {
                    break;
                }
                std::unique_ptr<dd::Package<Config>> sliceDD = std::make_unique<dd::Package<Config>>(CircuitSimulator<Config>::getNumberOfQubits());
                auto                                 result  = simulateSlicing(sliceDD, splitQubit, totalControl);
                result.addToVector(threadAmplitudes);
            }
        });
    }
    executor.wait_for_all();

    std::size_t oldIncrement    = 1;
    const auto  nAdditionLevels = static_cast<std::uint16_t>(std::ceil(std::log2(requiredVectors)));
    for (std::uint16_t level = 0; level < nAdditionLevels; ++level) {
        const std::size_t increment = 2ULL << level;
        for (std::size_t idx = 0; idx + oldIncrement < requiredVectors; idx += increment) {
            // in-place addition of amplitudes
            std::transform(amplitudes[idx].begin(), amplitudes[idx].end(),
                           amplitudes[idx + oldIncrement].begin(),
                           amplitudes[idx].begin(),
                           std::plus<>());
        }
        oldIncrement = increment;
    }
    finalAmplitudes = std::move(amplitudes[0]);
}

template<class Config>
void HybridSchrodingerFeynmanSimulator<Config>::exportDDtoGraphviz(std::ostream& os, const bool colored, const bool edgeLabels, const bool classic, const bool memory, const bool formatAsPolar) {
    if (mode == Mode::Amplitude) {
        Simulator<Config>::rootEdge = Simulator<Config>::dd->makeStateFromVector(finalAmplitudes);
    }
    return Simulator<Config>::exportDDtoGraphviz(os, colored, edgeLabels, classic, memory, formatAsPolar);
}

template class HybridSchrodingerFeynmanSimulator<dd::DDPackageConfig>;
