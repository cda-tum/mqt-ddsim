#include "StochasticNoiseSimulator.hpp"

#include <queue>
#include <stdexcept>

using CN = dd::ComplexNumbers;

template<class DDPackage>
std::map<std::string, std::size_t> StochasticNoiseSimulator<DDPackage>::Simulate(unsigned int shots) {
    bool hasNonunitary = false;
    for (auto& op: *qc) {
        if (op->isNonUnitaryOperation()) {
            hasNonunitary = true;
            break;
        }
    }

    if (!hasNonunitary) {
        perfectSimulationRun();
        return Simulator<DDPackage>::MeasureAllNonCollapsing(shots);
    }

    std::map<std::string, std::size_t> mCounter;

    for (unsigned int i = 0; i < shots; i++) {
        perfectSimulationRun();
        mCounter[Simulator<DDPackage>::MeasureAll()]++;
    }

    return mCounter;
}

template<class DDPackage>
void StochasticNoiseSimulator<DDPackage>::perfectSimulationRun() {
    const auto nQubits = qc->getNqubits();

    Simulator<DDPackage>::rootEdge = Simulator<DDPackage>::dd->makeZeroState(nQubits);
    Simulator<DDPackage>::dd->incRef(Simulator<DDPackage>::rootEdge);

    std::map<std::size_t, bool> classicValues;
    for (auto& op: *qc) {
        if (op->isNonUnitaryOperation()) {
            if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (op->getType() == qc::Measure) {
                    const auto& quantum = nuOp->getTargets();
                    const auto& classic = nuOp->getClassics();

                    assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                    for (std::size_t i = 0U; i < quantum.size(); ++i) {
                        const auto result = Simulator<DDPackage>::MeasureOneCollapsing(quantum.at(i));
                        assert(result == '0' || result == '1');
                        classicValues[classic.at(i)] = (result == '1');
                    }
                } else if (op->getType() == qc::Barrier) {
                    continue;
                } else {
                    throw std::runtime_error(std::string("Unsupported non-unitary functionality '") + op->getName() + "'.");
                }
            } else {
                throw std::runtime_error(std::string("Dynamic cast to NonUnitaryOperation failed for '") + op->getName() + "'.");
            }
            Simulator<DDPackage>::dd->garbageCollect();
        } else {
            if (op->isClassicControlledOperation()) {
                if (auto* ccOp = dynamic_cast<qc::ClassicControlledOperation*>(op.get())) {
                    const auto startIndex    = static_cast<unsigned short>(ccOp->getParameter().at(0U));
                    const auto length        = static_cast<unsigned short>(ccOp->getParameter().at(1U));
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

            auto operation = dd::getDD(op.get(), Simulator<DDPackage>::dd);
            auto tmp       = Simulator<DDPackage>::dd->multiply(operation, Simulator<DDPackage>::rootEdge);
            Simulator<DDPackage>::dd->incRef(tmp);
            Simulator<DDPackage>::dd->decRef(Simulator<DDPackage>::rootEdge);
            Simulator<DDPackage>::rootEdge = tmp;

            Simulator<DDPackage>::dd->garbageCollect();
        }
    }
}

template<class DDPackage>
std::map<std::string, double> StochasticNoiseSimulator<DDPackage>::StochSimulate() {
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
        threadArray.emplace_back(&StochasticNoiseSimulator<DDPackage>::runStochSimulationForId,
                                 this,
                                 runID,
                                 qc->getNqubits(),
                                 std::ref(recordedPropertiesPerInstance[runID]),
                                 std::ref(recordedProperties),
                                 std::ref(classicalMeasurementsMaps[runID]),
                                 static_cast<unsigned long long>(Simulator<DDPackage>::mt()));
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

    //std::clog <<"Calculating amplitudes from all runs...\n";
    for (unsigned long j = 0U; j < recordedProperties.size(); j++) {
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

template<class DDPackage>
void StochasticNoiseSimulator<DDPackage>::runStochSimulationForId(std::size_t                                stochRun,
                                                                  dd::Qubit                                  nQubits,
                                                                  std::vector<double>&                       recordedPropertiesStorage,
                                                                  std::vector<std::pair<long, std::string>>& recordedPropertiesList,
                                                                  std::map<std::string, unsigned int>&       classicalMeasurementsMap,
                                                                  unsigned long long                         localSeed) {
    std::mt19937_64                        generator(localSeed);
    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0);

    const unsigned long numberOfRuns = stochasticRuns / maxInstances + (stochRun < stochasticRuns % maxInstances ? 1U : 0U);
    const int           approxMod    = std::ceil(static_cast<double>(qc->getNops()) / (stepNumber + 1));

    //printf("Running %d times and using the dd at %p, using the cn object at %p\n", numberOfRuns, (void *) &package, (void *) &package->cn);
    for (std::size_t currentRun = 0U; currentRun < numberOfRuns; currentRun++) {
        const auto t1 = std::chrono::steady_clock::now();

        auto localDD                      = std::make_unique<StochasticNoisePackage>(getNumberOfQubits());
        auto stochasticNoiseFunctionality = dd::StochasticNoiseFunctionality<StochasticNoisePackage>(
                localDD,
                nQubits,
                noiseProbability,
                amplitudeDampingProb,
                multiQubitGateFactor,
                noiseEffects);

        std::map<std::size_t, bool> classicValues;

        std::size_t opCount     = 0U;
        std::size_t approxCount = 0U;

        dd::vEdge localRootEdge = localDD->makeZeroState(nQubits);
        localDD->incRef(localRootEdge);

        for (auto& op: *qc) {
            if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
                if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                    if (nuOp->getType() == qc::Measure) {
                        auto quantum = nuOp->getTargets();
                        auto classic = nuOp->getClassics();

                        assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                        for (std::size_t i = 0U; i < quantum.size(); ++i) {
                            const auto result = localDD->measureOneCollapsing(localRootEdge, quantum.at(i), true, generator);
                            assert(result == '0' || result == '1');
                            classicValues[classic.at(i)] = (result == '0');
                        }
                    } else {
                        //Skipping barrier
                        if (op->getType() == qc::Barrier) {
                            continue;
                        }
                        throw std::runtime_error("Unsupported non-unitary functionality.");
                    }
                } else {
                    throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
                }
            } else {
                dd::mEdge    operation{};
                qc::Targets  targets;
                dd::Controls controls;
                if (op->isClassicControlledOperation()) {
                    // Check if the operation is controlled by a classical register
                    auto* classicOp = dynamic_cast<qc::ClassicControlledOperation*>(op.get());
                    bool  executeOp = true;
                    auto  expValue  = classicOp->getExpectedValue();

                    for (auto i = static_cast<std::size_t>(classicOp->getControlRegister().first); i < classicOp->getControlRegister().second; i++) {
                        if (classicValues[i] != (expValue % 2U)) {
                            executeOp = false;
                            break;
                        } else {
                            expValue = expValue >> 1U;
                        }
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
                        operation = localDD->stochasticNoiseOperationCache.lookup(op->getType(), targets.front());
                        if (operation.p == nullptr) {
                            operation = dd::getDD(op.get(), localDD);
                            localDD->stochasticNoiseOperationCache.insert(op->getType(), targets.front(), operation);
                        }
                    } else {
                        operation = dd::getDD(op.get(), localDD);
                    }
                }

                auto usedQubits = targets;
                for (auto control: controls) {
                    usedQubits.emplace_back(control.qubit);
                }

                if (sequentiallyApplyNoise) {
                    stochasticNoiseFunctionality.setNoiseEffects(std::vector<dd::NoiseOperations>{dd::identity});
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
                    Simulator<DDPackage>::ApproximateByFidelity(localDD, localRootEdge, stepFidelity, false, true);
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
                const auto amplitude   = localDD->getValueByPath(localRootEdge, basisVector);
                const auto prob        = amplitude.r * amplitude.r + amplitude.i * amplitude.i;
                recordedPropertiesStorage[i] += prob;
            }
        }
    }
}

template<class DDPackage>
void StochasticNoiseSimulator<DDPackage>::setRecordedProperties(const std::string& input) {
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

template<class DDPackage>
std::string StochasticNoiseSimulator<DDPackage>::intToString(long targetNumber) const {
    if (targetNumber < 0) {
        return "X";
    }
    auto        qubits = getNumberOfQubits();
    std::string path(qubits, '0');
    auto        number = static_cast<unsigned long>(targetNumber);
    for (std::size_t i = 1U; i <= qubits; i++) {
        if (number % 2U) {
            path[qubits - i] = '1';
        }
        number = number >> 1U;
    }
    return path;
}

template class StochasticNoiseSimulator<StochasticNoisePackage>;
