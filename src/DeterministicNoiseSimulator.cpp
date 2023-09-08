#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN = dd::ComplexNumbers;

template<class Config>
std::map<std::string, double> DeterministicNoiseSimulator<Config>::deterministicSimulate() {
    rootEdge = Simulator<Config>::dd->makeZeroDensityOperator(static_cast<dd::Qubit>(qc->getNqubits()));
    Simulator<Config>::dd->incRef(rootEdge);
    std::map<std::size_t, bool> classicValues;

    auto deterministicNoiseFunctionality = dd::DeterministicNoiseFunctionality<Config>(
            Simulator<Config>::dd,
            static_cast<dd::Qubit>(qc->getNqubits()),
            noiseProbSingleQubit,
            noiseProbMultiQubit,
            ampDampingProbSingleQubit,
            ampDampingProbMultiQubit,
            noiseEffects,
            useDensityMatrixType,
            sequentiallyApplyNoise);

    for (auto const& op: *qc) {
        Simulator<Config>::dd->garbageCollect();
        if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
            if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (nuOp->getType() == qc::Measure) {
                    auto quantum = nuOp->getTargets();
                    auto classic = nuOp->getClassics();

                    assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                    for (std::size_t i = 0U; i < quantum.size(); ++i) {
                        char result;
                        std::tie(rootEdge, result) = Simulator<Config>::dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(quantum.at(i)), true, Simulator<Config>::mt);
                        assert(result == '0' || result == '1');
                        classicValues[classic.at(i)] = (result == '0');
                    }
                } else if (op->getType() == qc::Barrier) {
                    continue;
                } else if (op->getType() == qc::Reset) {
                    // Reset qubit
                    auto qubits = nuOp->getTargets();
                    for (const auto& qubit: qubits) {
                        char result;
                        std::tie(rootEdge, result) = Simulator<Config>::dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(qubits.at(qubit)), true, Simulator<Config>::mt);
                        if (result == '1') {
                            const auto x   = qc::StandardOperation(qc->getNqubits(), qubit, qc::X);
                            auto       tmp = Simulator<Config>::dd->multiply(dd::getDD(&x, Simulator<Config>::dd), Simulator<Config>::rootEdge);
                            Simulator<Config>::dd->incRef(tmp);
                            Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
                            Simulator<Config>::rootEdge = tmp;
                            Simulator<Config>::dd->garbageCollect();
                        }
                    }
                }
            } else {
                throw std::runtime_error(std::string{"Unsupported non-unitary functionality: \""} + nuOp->getName() + "\"");
            }
        } else {
            if (op->isClassicControlledOperation()) {
                // Check if the operation is controlled by a classical register
                auto* classicOp = dynamic_cast<qc::ClassicControlledOperation*>(op.get());
                if (classicOp == nullptr) {
                    throw std::runtime_error("Dynamic cast to ClassicControlledOperation* failed.");
                }
                bool executeOp = true;
                auto expValue  = classicOp->getExpectedValue();

                for (auto i = classicOp->getControlRegister().first; i < classicOp->getControlRegister().second; i++) {
                    if (static_cast<std::uint64_t>(classicValues[i]) != (expValue % 2U)) {
                        executeOp = false;
                        break;
                    }
                    expValue = expValue >> 1U;
                }
                if (!executeOp) {
                    continue;
                }
            }

            auto operation = dd::getDD(op.get(), Simulator<Config>::dd);

            // Applying the operation to the density matrix
            Simulator<Config>::dd->applyOperationToDensity(rootEdge, operation, useDensityMatrixType);

            deterministicNoiseFunctionality.applyNoiseEffects(rootEdge, op);
        }
    }
    return Simulator<Config>::dd->getProbVectorFromDensityMatrix(rootEdge, measurementThreshold);
}

template<class Config>
std::map<std::string, std::size_t> DeterministicNoiseSimulator<Config>::sampleFromProbabilityMap(const std::map<std::string, dd::fp>& resultProbabilityMap, std::size_t shots) {
    // Create probability distribution from measure probabilities
    std::vector<dd::fp> weights;
    weights.reserve(resultProbabilityMap.size());

    for (const auto& [state, prob]: resultProbabilityMap) {
        weights.emplace_back(prob);
    }
    std::discrete_distribution<std::size_t> d(weights.begin(), weights.end()); // NOLINT(misc-const-correctness) false-positive

    //Sample n shots elements from the prob distribution
    std::map<std::size_t, std::size_t> results; // NOLINT(misc-const-correctness) false-positive
    for (size_t n = 0; n < shots; ++n) {
        ++results[d(Simulator<Config>::mt)];
    }

    // Create the final map containing the measurement results and the corresponding shots
    std::map<std::string, std::size_t> resultShotsMap;

    for (const auto& [state, prob]: results) {
        resultShotsMap.emplace(std::next(resultProbabilityMap.begin(), static_cast<std::int64_t>(state))->first, prob);
    }

    return resultShotsMap;
}

template class DeterministicNoiseSimulator<DensityMatrixSimulatorDDPackageConfig>;
