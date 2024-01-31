#include "StochasticNoiseSimulator.hpp"
#include "dd/Operations.hpp"

#include <queue>
#include <stdexcept>

using CN = dd::ComplexNumbers;

template<class Config>
std::map<std::string, std::size_t> StochasticNoiseSimulator<Config>::simulate(size_t nshots) {
    std::map<std::size_t, bool> classicValues;
    stochasticRuns = nshots;
        if (op->isNonUnitaryOperation()) {
            if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (op->getType() == qc::Measure) {
                    const auto& quantum = nuOp->getTargets();
                    const auto& classic = nuOp->getClassics();

                    assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                    for (std::size_t i = 0U; i < quantum.size(); ++i) {
                        const auto result = Simulator<Config>::measureOneCollapsing(quantum.at(i));
                        assert(result == '0' || result == '1');
                        classicValues[classic.at(i)] = (result == '1');
                    }
                } else {
                    throw std::runtime_error(std::string("Unsupported non-unitary functionality '") + op->getName() + "'.");
                }
            } else {
                throw std::runtime_error(std::string("Dynamic cast to NonUnitaryOperation failed for '") + op->getName() + "'.");
            }
            Simulator<Config>::dd->garbageCollect();
        } else {
            if (op->isClassicControlledOperation()) {
                if (auto* ccOp = dynamic_cast<qc::ClassicControlledOperation*>(op.get())) {
                    const auto startIndex    = static_cast<std::uint16_t>(ccOp->getParameter().at(0U));
                    const auto length        = static_cast<std::uint16_t>(ccOp->getParameter().at(1U));
                    const auto expectedValue = ccOp->getExpectedValue();

                    std::size_t actualValue = 0U;
                    for (std::size_t i = 0U; i < length; i++) {
                        actualValue |= (classicValues[startIndex + i] ? 1U : 0U) << i;
                    }

                    //std::clog << "expected " << expected_value << " and actual value was " << actual_value << "\n";

                    if (actualValue != expectedValue) {
                        continue;
                    }
                } else {
                    throw std::runtime_error("Dynamic cast to ClassicControlledOperation failed.");
                }
            }

            auto operation = dd::getDD(op.get(), Simulator<Config>::dd);
            auto tmp       = Simulator<Config>::dd->multiply(operation, Simulator<Config>::rootEdge);
            Simulator<Config>::dd->incRef(tmp);
            Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
            Simulator<Config>::rootEdge = tmp;

            Simulator<Config>::dd->garbageCollect();
        }
    }
}

template<class Config>
std::map<std::string, double> StochasticNoiseSimulator<Config>::stochSimulate() {
    // Generate a vector for each instance.
    recordedPropertiesPerInstance.resize(maxInstances, std::vector<double>(recordedProperties.size(), 0.0));
    classicalMeasurementsMaps.resize(maxInstances);
    //std::clog << "Conducting " << stochasticRuns << " runs using " << maxInstances << " cores...\n";
    std::vector<std::thread> threadArray;
    // The stochastic runs are applied in parallel
    //std::clog << "Starting " << maxInstances << " threads\n";
    const auto t1Stoch = std::chrono::steady_clock::now();
    for (std::size_t runID = 0U; runID < maxInstances; runID++) {
        threadArray.emplace_back(&StochasticNoiseSimulator<Config>::runStochSimulationForId,
                                 this,
                                 runID,
                                 qc->getNqubits(),
                                 std::ref(classicalMeasurementsMaps[runID]),
                                 static_cast<std::uint64_t>(Simulator<Config>::mt()));
    }
    // wait for threads to finish
    for (auto& thread: threadArray) {
        thread.join();
    }
    const auto t2Stoch = std::chrono::steady_clock::now();
    stochRunTime       = std::chrono::duration<double>(t2Stoch - t1Stoch).count();

    for (const auto& classicalMeasurementsMap: classicalMeasurementsMaps) {
        for (const auto& [state, count]: classicalMeasurementsMap) {
            finalClassicalMeasurementsMap[state] += count;
        }
    }

    // Adding the result of classical registers to the measureResult map
    std::map<std::string, double> measureResult;
    for (const auto& [state, count]: finalClassicalMeasurementsMap) {
        const auto probability = count;
        measureResult.emplace(state, probability); //todo maybe print both classical and quantum register ? classical register:"
    }

    return finalClassicalMeasurementsMap;
}

template<class Config>
void StochasticNoiseSimulator<Config>::runStochSimulationForId(std::size_t stochRun, qc::Qubit nQubits, std::map<std::string, unsigned int>& classicalMeasurementsMap, std::uint64_t localSeed) {
    std::mt19937_64 generator(localSeed);

    const std::uint64_t numberOfRuns = stochasticRuns / maxInstances + (stochRun < stochasticRuns % maxInstances ? 1U : 0U);
    const auto          approxMod    = static_cast<unsigned>(std::ceil(static_cast<double>(qc->getNops()) / (static_cast<double>(stepNumber + 1))));

    //printf("Running %d times and using the dd at %p, using the cn object at %p\n", numberOfRuns, (void *) &package, (void *) &package->cn);
    for (std::size_t currentRun = 0U; currentRun < numberOfRuns; currentRun++) {
        auto localDD                      = std::make_unique<dd::Package<StochasticNoiseSimulatorDDPackageConfig>>(qc->getNqubits());
        auto stochasticNoiseFunctionality = dd::StochasticNoiseFunctionality<StochasticNoiseSimulatorDDPackageConfig>(
                localDD,
                static_cast<dd::Qubit>(nQubits),
                noiseProbability,
                amplitudeDampingProb,
                multiQubitGateFactor,
                noiseEffects);

        std::map<std::size_t, bool> classicValues;

        std::size_t                  opCount     = 0U;
        [[maybe_unused]] std::size_t approxCount = 0U;

        dd::vEdge localRootEdge = localDD->makeZeroState(static_cast<dd::Qubit>(nQubits));
        localDD->incRef(localRootEdge);

        for (auto& op: *qc) {
            if (op->getType() == qc::Barrier) {
                continue;
            }
            if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
                if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                    if (nuOp->getType() == qc::Measure) {
                        auto quantum = nuOp->getTargets();
                        auto classic = nuOp->getClassics();

                        assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                        for (std::size_t i = 0U; i < quantum.size(); ++i) {
                            const auto result = localDD->measureOneCollapsing(localRootEdge, static_cast<dd::Qubit>(quantum.at(i)), true, generator);
                            assert(result == '0' || result == '1');
                            classicValues[classic.at(i)] = (result == '1');
                        }
                    } else if (op->getType() == qc::Reset) {
                        // Reset qubit
                        auto qubits = nuOp->getTargets();
                        for (const auto& qubit: qubits) {
                            const auto result = localDD->measureOneCollapsing(localRootEdge, static_cast<dd::Qubit>(qubits.at(qubit)), true, generator);
                            if (result == '1') {
                                const auto x   = qc::StandardOperation(qc->getNqubits(), qubit, qc::X);
                                auto       tmp = localDD->multiply(dd::getDD(&x, localDD), localRootEdge);
                                localDD->incRef(tmp);
                                localDD->decRef(localRootEdge);
                                localRootEdge = tmp;
                                localDD->garbageCollect();
                            }
                        }
                    } else {
                        throw std::runtime_error("Unsupported non-unitary functionality.");
                    }
                } else {
                    throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
                }
            } else {
                dd::mEdge operation;
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
                    operation = dd::getDD(classicOp->getOperation(), *localDD);
                    if (!executeOp) {
                        continue;
                    }
                } else {
                    const auto targets  = op->getTargets();
                    const auto controls = op->getControls();

                    if (targets.size() == 1 && controls.empty()) {
                        auto* oper = localDD->stochasticNoiseOperationCache.lookup(op->getType(), static_cast<dd::Qubit>(targets.front()));
                        if (oper == nullptr) {
                            operation = dd::getDD(op.get(), *localDD);
                            localDD->stochasticNoiseOperationCache.insert(op->getType(), static_cast<dd::Qubit>(targets.front()), operation);
                        } else {
                            operation = *oper;
                        }
                    } else {
                        operation = dd::getDD(op.get(), *localDD);
                    }
                }

                const auto usedQubits = op->getUsedQubits();

                stochasticNoiseFunctionality.applyNoiseOperation(usedQubits, operation, localRootEdge, generator);

                if (stepFidelity < 1. && (opCount + 1U) % approxMod == 0U) {
                    approxCount++;
                    Simulator<Config>::approximateByFidelity(localDD, localRootEdge, stepFidelity, false, true);
                }
            }
            localDD->garbageCollect();
            opCount++;
        }
        localDD->decRef(localRootEdge);

        if (!classicValues.empty()) {
            const auto  cbits = qc->getNcbits();
            std::string classicRegisterString(qc->getNcbits(), '0');

            for (const auto& [bitIndex, value]: classicValues) {
                classicRegisterString[cbits - bitIndex - 1] = value ? '1' : '0';
            }
            classicalMeasurementsMap[classicRegisterString] += 1U;
        }
    }
}

template class StochasticNoiseSimulator<StochasticNoiseSimulatorDDPackageConfig>;
