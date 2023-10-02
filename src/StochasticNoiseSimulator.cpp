#include "StochasticNoiseSimulator.hpp"

#include <queue>
#include <stdexcept>

using CN = dd::ComplexNumbers;

template<class Config>
std::map<std::string, std::size_t> StochasticNoiseSimulator<Config>::simulate(size_t shots) {
    const bool hasNonunitary = std::any_of(qc->cbegin(), qc->cend(), [&](const auto& p) { return p->isNonUnitaryOperation(); });

    if (!hasNonunitary) {
        perfectSimulationRun();
        return Simulator<Config>::measureAllNonCollapsing(shots);
    }

    std::map<std::string, std::size_t> mCounter;

    for (unsigned int i = 0; i < shots; i++) {
        perfectSimulationRun();
        mCounter[Simulator<Config>::measureAll()]++;
    }

    return mCounter;
}

template<class Config>
void StochasticNoiseSimulator<Config>::perfectSimulationRun() {
    const auto nQubits = qc->getNqubits();

    Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(static_cast<dd::Qubit>(nQubits));
    Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);

    std::map<std::size_t, bool> classicValues;
    for (auto& op: *qc) {
        if (op->getType() == qc::Barrier) {
            continue;
        }

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
    // the final vector stores the average of all runs and is calculated after the runs have finished
    finalProperties.resize(recordedProperties.size(), 0);
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
                                 std::ref(recordedPropertiesPerInstance[runID]),
                                 std::ref(recordedProperties),
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

    for (std::uint64_t j = 0U; j < recordedProperties.size(); j++) {
        for (const auto& instanceProperties: recordedPropertiesPerInstance) {
            finalProperties[j] += instanceProperties[j];
        }
        finalProperties[j] /= static_cast<double>(stochasticRuns);
    }

    // Adding the result of classical registers to the measureResult map
    std::map<std::string, double> measureResult;
    for (const auto& [state, count]: finalClassicalMeasurementsMap) {
        const auto probability = count / static_cast<double>(stochasticRuns);
        measureResult.emplace(state, probability); //todo maybe print both classical and quantum register ? classical register:"
    }

    //std::clog << "Probabilities are ... (probabilities < 0.001 are omitted)\n";
    for (std::size_t m = 0U; m < recordedProperties.size(); m++) {
        if (recordedProperties[m].first == -2) {
            meanStochTime = finalProperties[m];
        } else if (recordedProperties[m].first == -1) {
            approximationRuns = finalProperties[m];
        } else if (finalProperties[m] > 0 || m < 2U) {
            // Print all probabilities that are larger than 0, and always print the probabilities for state 0 and 1
            std::string amplitude = recordedProperties[m].second;
            std::replace(amplitude.begin(), amplitude.end(), '2', '1');
            measureResult.emplace(amplitude, finalProperties[m]);
        }
    }
    return measureResult;
}

template<class Config>
void StochasticNoiseSimulator<Config>::runStochSimulationForId(std::size_t                                        stochRun,
                                                               qc::Qubit                                          nQubits,
                                                               std::vector<double>&                               recordedPropertiesStorage,
                                                               std::vector<std::pair<std::int64_t, std::string>>& recordedPropertiesList,
                                                               std::map<std::string, unsigned int>&               classicalMeasurementsMap,
                                                               std::uint64_t                                      localSeed) {
    std::mt19937_64 generator(localSeed);

    const std::uint64_t numberOfRuns = stochasticRuns / maxInstances + (stochRun < stochasticRuns % maxInstances ? 1U : 0U);
    const auto          approxMod    = static_cast<unsigned>(std::ceil(static_cast<double>(qc->getNops()) / (static_cast<double>(stepNumber + 1))));

    //printf("Running %d times and using the dd at %p, using the cn object at %p\n", numberOfRuns, (void *) &package, (void *) &package->cn);
    for (std::size_t currentRun = 0U; currentRun < numberOfRuns; currentRun++) {
        const auto t1 = std::chrono::steady_clock::now();

        auto localDD                      = std::make_unique<dd::Package<StochasticNoiseSimulatorDDPackageConfig>>(qc->getNqubits());
        auto stochasticNoiseFunctionality = dd::StochasticNoiseFunctionality<StochasticNoiseSimulatorDDPackageConfig>(
                localDD,
                static_cast<dd::Qubit>(nQubits),
                noiseProbability,
                amplitudeDampingProb,
                multiQubitGateFactor,
                noiseEffects);

        std::map<std::size_t, bool> classicValues;

        std::size_t opCount     = 0U;
        std::size_t approxCount = 0U;

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
                            classicValues[classic.at(i)] = (result == '0');
                        }
                    } else {
                        throw std::runtime_error("Unsupported non-unitary functionality.");
                    }
                } else {
                    throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
                }
            } else {
                dd::mEdge    operation{};
                qc::Targets  targets;
                qc::Controls controls;
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
                    operation = dd::getDD(classicOp->getOperation(), localDD);
                    targets   = classicOp->getOperation()->getTargets();
                    controls  = classicOp->getOperation()->getControls();
                    if (!executeOp) {
                        continue;
                    }
                } else {
                    targets  = op->getTargets();
                    controls = op->getControls();

                    if (targets.size() == 1 && controls.empty()) {
                        auto* oper = localDD->stochasticNoiseOperationCache.lookup(op->getType(), static_cast<dd::Qubit>(targets.front()));
                        if (oper == nullptr) {
                            operation = dd::getDD(op.get(), localDD);
                            localDD->stochasticNoiseOperationCache.insert(op->getType(), static_cast<dd::Qubit>(targets.front()), operation);
                        } else {
                            operation = *oper;
                        }
                    } else {
                        operation = dd::getDD(op.get(), localDD);
                    }
                }

                const auto usedQubits = op->getUsedQubits();

                if (sequentiallyApplyNoise) {
                    stochasticNoiseFunctionality.setNoiseEffects(std::vector<dd::NoiseOperations>{dd::Identity});
                    stochasticNoiseFunctionality.applyNoiseOperation(usedQubits, operation, localRootEdge, generator);
                    for (auto noiseEffect: noiseEffects) {
                        stochasticNoiseFunctionality.setNoiseEffects(std::vector<dd::NoiseOperations>{noiseEffect});
                        stochasticNoiseFunctionality.applyNoiseOperation(usedQubits, stochasticNoiseFunctionality.getIdentityDD(), localRootEdge, generator);
                    }
                } else {
                    stochasticNoiseFunctionality.applyNoiseOperation(usedQubits, operation, localRootEdge, generator);
                }

                if (stepFidelity < 1. && (opCount + 1U) % approxMod == 0U) {
                    approxCount++;
                    Simulator<Config>::approximateByFidelity(localDD, localRootEdge, stepFidelity, false, true);
                }
            }
            localDD->garbageCollect();
            opCount++;
        }
        localDD->decRef(localRootEdge);
        const auto t2 = std::chrono::steady_clock::now();

        if (!classicValues.empty()) {
            std::string classicRegisterString;
            for (const auto& [val, isSet]: classicValues) {
                classicRegisterString.push_back(isSet ? '0' : '1');
            }
            classicalMeasurementsMap[classicRegisterString] += 1U;
        }

        for (std::size_t i = 0U; i < recordedPropertiesStorage.size(); i++) {
            if (recordedPropertiesList[i].first == -2) {
                recordedPropertiesStorage[i] += std::chrono::duration<double>(t2 - t1).count();
            } else if (recordedPropertiesList[i].first == -1) {
                recordedPropertiesStorage[i] += static_cast<double>(approxCount);
            } else {
                // extract amplitude for state
                const auto basisVector = recordedPropertiesList[i].second;
                const auto amplitude   = localRootEdge.getValueByPath(basisVector);
                recordedPropertiesStorage[i] += std::norm(amplitude);
            }
        }
    }
}

template<class Config>
void StochasticNoiseSimulator<Config>::setRecordedProperties(const std::string& input) {
    std::string subString;
    const int   min = std::numeric_limits<int>::min();

    auto listBegin = min;
    auto listEnd   = min;
    for (const auto i: input) {
        if (i == ' ') {
            continue;
        }
        if (i == ',') {
            if (listBegin > min) {
                listEnd = std::stoi(subString);
                if (listEnd > std::pow(2, getNumberOfQubits())) {
                    listEnd = static_cast<int>(std::pow(2, getNumberOfQubits()));
                }
                assert(listBegin < listEnd);
                for (int m = listBegin; m <= listEnd; m++) {
                    recordedProperties.emplace_back(m, intToString(m));
                }
            } else {
                recordedProperties.emplace_back(std::stoi(subString), intToString(std::stoi(subString)));
            }
            subString = "";
            listBegin = min;
            listEnd   = min;
            continue;
        }
        if (i == '-' && !subString.empty()) {
            listBegin = std::stoi(subString);
            subString = "";
            continue;
        }
        subString += i;
    }
    if (listBegin > min) {
        listEnd = std::stoi(subString);
        if (listEnd > std::pow(2, getNumberOfQubits())) {
            listEnd = static_cast<int>(std::pow(2, getNumberOfQubits()) - 1);
        }
        assert(listBegin < listEnd);
        for (int m = listBegin; m <= listEnd; m++) {
            recordedProperties.emplace_back(m, intToString(m));
        }
    } else {
        recordedProperties.emplace_back(std::stoi(subString), intToString(std::stoi(subString)));
    }
}

template<class Config>
std::string StochasticNoiseSimulator<Config>::intToString(std::int64_t targetNumber) const {
    if (targetNumber < 0) {
        return "X";
    }
    auto        qubits = getNumberOfQubits();
    std::string path(qubits, '0');
    auto        number = static_cast<std::uint64_t>(targetNumber);
    for (std::size_t i = 1U; i <= qubits; i++) {
        if ((number % 2U) != 0U) {
            path[qubits - i] = '1';
        }
        number = number >> 1U;
    }
    return path;
}

template class StochasticNoiseSimulator<StochasticNoiseSimulatorDDPackageConfig>;
