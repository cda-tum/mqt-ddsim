#include "HybridSchrodingerFeynmanSimulator.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <taskflow/taskflow.hpp>

static void printAmpFile(std::vector<std::complex<dd::fp>>& finalAmp, std::string filename) {
    std::ofstream output(filename);

    output << std::setprecision(16);

    for (auto& cn: finalAmp) {
        output << cn.real() << " " << cn.imag() << "i" << std::endl;
    }
    output.close();
}

template<class Config>
std::size_t HybridSchrodingerFeynmanSimulator<Config>::getNDecisions(qc::Qubit splitQubit) {
    std::size_t ndecisions = 0;
    // calculate number of decisions
    for (const auto& op: *CircuitSimulator<Config>::qc) {
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
        } else if (op->getType() == qc::Barrier || op->getType() == qc::Snapshot || op->getType() == qc::ShowProbabilities) {
            continue;
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
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (reinterpret_cast<qc::StandardOperation*>(op.get()) != nullptr) { // TODO change control and target if wrong direction
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
                opControls.emplace(qc::Control{control.qubit, control.type});
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

    if (partitionSuccess && circuitPartitioner_ != nullptr) {
        splitQubit = circuitPartitioner_->getSplitQubit();
    } else if (!partitionSuccess && circuitPartitioner_ != nullptr) {
        //std::cout << "Partition fail!" << std::endl;
    }

    if (mode == Mode::DD) {
        simulateHybridTaskflow(splitQubit);
        return Simulator<Config>::measureAllNonCollapsing(shots);
    }
    simulateHybridAmplitudes(splitQubit);
    if (partitionSuccess && circuitPartitioner_ != nullptr) {
        //printAmpFile(finalAmplitudes, "beforeRestore.txt");
        restoreAmplitudes();
        circuitPartitioner_->restoreQuantumComputation(*(CircuitSimulator<Config>::qc));
    }

    if (shots > 0) {
        return Simulator<Config>::sampleFromAmplitudeVectorInPlace(finalAmplitudes, shots);
    }
    // in case no shots were requested, the final amplitudes remain untouched

    //printAmpFile(finalAmplitudes, "finalAmp.txt");
    return {};
}

template<class Config>
void HybridSchrodingerFeynmanSimulator<Config>::simulateHybridTaskflow(unsigned int splitQubit) {
    const std::size_t ndecisions          = getNDecisions(splitQubit);
    const std::size_t maxControl          = 1ULL << ndecisions;
    const std::size_t actuallyUsedThreads = std::min<std::size_t>(maxControl, nthreads);
    const std::size_t nslicesAtOnce       = std::min<std::size_t>(16, maxControl / actuallyUsedThreads);

    //std::cout << "[Partitioner] Number of branches = " << ndecisions << std::endl;

    Simulator<Config>::rootEdge = qc::VectorDD::zero;

    std::vector<std::vector<bool>> computed(ndecisions, std::vector<bool>(maxControl, false));

    tf::Executor executor(nthreads);

    std::function<void(std::pair<std::size_t, std::size_t>)> computePair = [this, &computePair, &computed, &executor, ndecisions, nslicesAtOnce, splitQubit](std::pair<std::size_t, std::size_t> current) {
        if (current.first == 0) { // slice
            std::unique_ptr<dd::Package<Config>> oldDD;
            qc::VectorDD                         edge{};
            for (std::size_t i = 0; i < nslicesAtOnce; i++) {
                auto sliceDD = std::make_unique<dd::Package<Config>>(CircuitSimulator<Config>::getNumberOfQubits());
                auto result  = simulateSlicing(sliceDD, splitQubit, current.second + i);
                if (i > 0) {
                    edge = sliceDD->add(sliceDD->transfer(edge), result);
                } else {
                    edge = result;
                }
                oldDD = std::move(sliceDD);
            }

            current.first = static_cast<std::size_t>(std::log2(nslicesAtOnce));
            // NOLINTNEXTLINE(clang-analyzer-core.DivideZero): clang-tidy mistakenly thinks the next line divides by zero
            current.second /= nslicesAtOnce;
            dd::serialize(edge, "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd", true);
        } else { // adding
            auto              sliceDD       = std::make_unique<dd::Package<Config>>(CircuitSimulator<Config>::getNumberOfQubits());
            const std::string filename      = "slice_" + std::to_string(current.first - 1) + "_";
            const std::string filenameLeft  = filename + std::to_string(current.second * 2) + ".dd";
            const std::string filenameRight = filename + std::to_string(current.second * 2 + 1) + ".dd";

            auto result  = sliceDD->template deserialize<dd::vNode>(filenameLeft, true);
            auto result2 = sliceDD->template deserialize<dd::vNode>(filenameRight, true);
            result       = sliceDD->add(result, result2);
            dd::serialize(result, "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd", true);

            remove(filenameLeft.c_str());
            remove(filenameRight.c_str());
        }

        if (current.first < ndecisions) {
            computed.at(current.first).at(current.second) = true;

            const auto compSecond = (current.second % 2 != 0) ? (current.second - 1) : (current.second + 1);

            if (computed.at(current.first).at(compSecond)) {
                executor.silent_async([&computePair, current]() { computePair(std::make_pair(current.first + 1, current.second / 2)); });
            }
        }
    };

    for (auto i = static_cast<std::int64_t>(maxControl - 1); i >= 0; i -= static_cast<std::int64_t>(nslicesAtOnce)) {
        executor.silent_async([&computePair, i]() { computePair(std::make_pair(0, i)); });
    }
    executor.wait_for_all();

    Simulator<Config>::rootEdge = Simulator<Config>::dd->template deserialize<dd::vNode>("slice_" + std::to_string(ndecisions) + "_0.dd", true);
    Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);
}

template<class Config>
void HybridSchrodingerFeynmanSimulator<Config>::simulateHybridAmplitudes(qc::Qubit splitQubit) {
    const std::size_t ndecisions          = getNDecisions(splitQubit);
    const std::size_t maxControl          = 1ULL << ndecisions;
    const std::size_t actuallyUsedThreads = std::min<std::size_t>(maxControl, nthreads);
    const std::size_t nslicesOnOneCpu     = std::min<std::size_t>(64, maxControl / actuallyUsedThreads);
    const std::size_t nqubits             = CircuitSimulator<Config>::getNumberOfQubits();
    Simulator<Config>::rootEdge           = qc::VectorDD::zero;

    //std::cout << "[Simulator] Number of Decisions = " << ndecisions << std::endl;
    //std::cout << "[Simulator] Number of Branches = 2^" << ndecisions << " = " << maxControl << std::endl;

    std::vector<std::vector<std::complex<dd::fp>>> amplitudes(maxControl / nslicesOnOneCpu, std::vector<std::complex<dd::fp>>(1U << nqubits, {0, 0}));

    tf::Executor executor;
    for (std::size_t control = 0, i = 0; control < maxControl; control += nslicesOnOneCpu, i++) {
        executor.silent_async([this, i, &amplitudes, nslicesOnOneCpu, control, nqubits, splitQubit]() {
            const auto                         currentThread    = i;
            std::vector<std::complex<dd::fp>>& threadAmplitudes = amplitudes.at(currentThread);

            for (std::size_t localControl = 0; localControl < nslicesOnOneCpu; localControl++) {
                const std::size_t                    totalControl = control + localControl;
                std::unique_ptr<dd::Package<Config>> sliceDD      = std::make_unique<dd::Package<Config>>(CircuitSimulator<Config>::getNumberOfQubits());
                auto                                 result       = simulateSlicing(sliceDD, splitQubit, totalControl);
                sliceDD->addAmplitudes(result, threadAmplitudes, static_cast<dd::QubitCount>(nqubits));
            }
        });
    }
    executor.wait_for_all();

    std::size_t oldIncrement = 1;
    for (std::uint16_t level = 0; level < static_cast<std::uint16_t>(std::log2(actuallyUsedThreads)); ++level) {
        const std::size_t increment = 2ULL << level;
        for (std::size_t idx = 0; idx < actuallyUsedThreads; idx += increment) {
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
void HybridSchrodingerFeynmanSimulator<Config>::oneSwap(qc::Qubit i_t, qc::Qubit j_t) {
    qc::Qubit nQubit = static_cast<qc::Qubit>(CircuitSimulator<Config>::getNumberOfQubits());

    uint32_t i = nQubit - i_t - 1;
    uint32_t j = nQubit - j_t - 1;

    i = nQubit - i - 1;
    j = nQubit - j - 1;

    uint32_t target_mask1 = 1u << i;
    uint32_t target_mask2 = 1u << j;

    uint32_t mask = 1u;
    for (int k = 0; k < nQubit - 1; k++) {
        mask = mask | (mask << 1);
    }

    uint32_t mask1 = mask >> (nQubit - j);
    mask1          = mask1 & mask;

    uint32_t mask3 = mask << (i + 1);
    mask3          = mask3 & mask;

    uint32_t mask2 = mask ^ mask1 ^ mask3 ^ target_mask1 ^ target_mask2;
    mask2          = mask2 & mask;

    uint32_t idx         = 0;
    uint32_t real_idx    = 0;
    uint32_t target_idx1 = 0;
    uint32_t target_idx2 = 0;

    while (idx < 1u << (nQubit - 2)) {
        real_idx = (idx & mask1) | ((idx << 1) & mask2) | ((idx << 2) & mask3);

        target_idx1 = real_idx | target_mask1;
        target_idx2 = real_idx | target_mask2;

        std::complex<dd::fp> temp;
        temp                         = finalAmplitudes[target_idx1];
        finalAmplitudes[target_idx1] = finalAmplitudes[target_idx2];
        finalAmplitudes[target_idx2] = temp;

        idx = idx + 1u;
    }
}

template<class Config>
void HybridSchrodingerFeynmanSimulator<Config>::restoreAmplitudes() {
    for (auto& kv: circuitPartitioner_->swapList()) {
        if (kv.first > kv.second)
            oneSwap(kv.first, kv.second);
        else
            oneSwap(kv.second, kv.first);
    }
}

template class HybridSchrodingerFeynmanSimulator<dd::DDPackageConfig>;
