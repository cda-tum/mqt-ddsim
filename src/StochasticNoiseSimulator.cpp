#include "StochasticNoiseSimulator.hpp"

#include <queue>
#include <stdexcept>

using CN = dd::ComplexNumbers;

std::map<std::string, std::size_t> StochasticNoiseSimulator::Simulate(unsigned int shots) {
    bool has_nonunitary = false;
    for (auto& op: *qc) {
        if (op->isNonUnitaryOperation()) {
            has_nonunitary = true;
            break;
        }
    }

    if (!has_nonunitary) {
        perfect_simulation_run();
        return MeasureAllNonCollapsing(shots);
    }

    std::map<std::string, std::size_t> m_counter;

    for (unsigned int i = 0; i < shots; i++) {
        perfect_simulation_run();
        m_counter[MeasureAll()]++;
    }

    return m_counter;
}

void StochasticNoiseSimulator::perfect_simulation_run() {
    const unsigned short n_qubits = qc->getNqubits();

    rootEdge = dd->makeZeroState(n_qubits);
    dd->incRef(rootEdge);

    unsigned long       op_num = 0;
    std::map<int, bool> classic_values;

    for (auto& op: *qc) {
        if (op->isNonUnitaryOperation()) {
            if (auto* nu_op = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (op->getType() == qc::Measure) {
                    auto quantum = nu_op->getTargets();
                    auto classic = nu_op->getClassics();

                    assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                    for (unsigned int i = 0; i < quantum.size(); ++i) {
                        auto result = MeasureOneCollapsing(quantum.at(i));
                        assert(result == '0' || result == '1');
                        classic_values[classic.at(i)] = (result == '1');
                    }
                } else if (op->getType() == qc::Barrier) {
                    continue;
                } else {
                    throw std::runtime_error(std::string("Unsupported non-unitary functionality '") + op->getName() + "'.");
                }
            } else {
                throw std::runtime_error(std::string("Dynamic cast to NonUnitaryOperation failed for '") + op->getName() + "'.");
            }
            dd->garbageCollect();
        } else {
            if (op->isClassicControlledOperation()) {
                if (auto* cc_op = dynamic_cast<qc::ClassicControlledOperation*>(op.get())) {
                    const auto         start_index    = static_cast<unsigned short>(cc_op->getParameter().at(0));
                    const auto         length         = static_cast<unsigned short>(cc_op->getParameter().at(1));
                    const unsigned int expected_value = cc_op->getExpectedValue();

                    unsigned int actual_value = 0;
                    for (unsigned int i = 0; i < length; i++) {
                        actual_value |= (classic_values[start_index + i] ? 1u : 0u) << i;
                    }

                    //std::clog << "expected " << expected_value << " and actual value was " << actual_value << "\n";

                    if (actual_value != expected_value) {
                        continue;
                    }
                } else {
                    throw std::runtime_error("Dynamic cast to ClassicControlledOperation failed.");
                }
            }

            auto operation = dd::getDD(op.get(), dd);
            auto tmp       = dd->multiply(operation, rootEdge);
            dd->incRef(tmp);
            dd->decRef(rootEdge);
            rootEdge = tmp;

            dd->garbageCollect();
        }
        op_num++;
    }
}

std::map<std::string, double> StochasticNoiseSimulator::StochSimulate() {
    // Generate a vector for each instance + 1. the final vector stores the average of all runs and is calculated
    // after the runs have finished
    for (unsigned int i = 0; i < maxInstances + 1; i++) {
        recordedPropertiesPerInstance.emplace_back(std::vector<double>(recordedProperties.size(), 0));
        classicalMeasurementsMaps.emplace_back(std::map<std::string, unsigned int>());
    }
    //std::clog << "Conducting " << stochasticRuns << " runs using " << maxInstances << " cores...\n";
    std::vector<std::thread> threadArray;
    // The stochastic runs are applied in parallel
    //std::clog << "Starting " << maxInstances << " threads\n";
    const auto t1_stoch = std::chrono::steady_clock::now();
    for (unsigned int runID = 0; runID < maxInstances; runID++) {
        threadArray.emplace_back(&StochasticNoiseSimulator::runStochSimulationForId,
                                 this,
                                 runID,
                                 qc->getNqubits(),
                                 std::ref(recordedPropertiesPerInstance[runID]),
                                 std::ref(recordedProperties),
                                 std::ref(classicalMeasurementsMaps[runID]),
                                 static_cast<unsigned long long>(mt()));
    }
    // wait for threads to finish
    for (auto& thread: threadArray) {
        thread.join();
    }
    const auto t2_stoch = std::chrono::steady_clock::now();
    stoch_run_time      = std::chrono::duration<float>(t2_stoch - t1_stoch).count();

    // Adding the measured results from all instances into the last entry of classicalMeasurementsMaps (i.e, classicalMeasurementsMaps[maxInstances])
    for (unsigned long j = 0; j < maxInstances; j++) {
        for (const auto& classical_measurements_map: classicalMeasurementsMaps[j]) {
            classicalMeasurementsMaps[maxInstances][classical_measurements_map.first] += classical_measurements_map.second;
        }
    }

    //std::clog <<"Calculating amplitudes from all runs...\n";
    for (unsigned long j = 0; j < recordedProperties.size(); j++) {
        for (unsigned int i = 0; i < maxInstances; i++) {
            recordedPropertiesPerInstance[maxInstances][j] += recordedPropertiesPerInstance[i][j];
        }
        recordedPropertiesPerInstance[maxInstances][j] /= (double)stochasticRuns;
    }

    // Adding the result of classical registers to the measure_result map
    std::map<std::string, double> measure_result;
    for (auto& classical_measurements_map: classicalMeasurementsMaps[maxInstances]) {
        const auto probability = (double)classical_measurements_map.second / (double)stochasticRuns;
        measure_result.insert({classical_measurements_map.first, probability});
    }

    //std::clog << "Probabilities are ... (probabilities < 0.001 are omitted)\n";
    //    std::map<std::string, double> measure_result;
    for (unsigned long m = 0; m < recordedProperties.size(); m++) {
        if (std::get<0>(recordedProperties[m]) == -2) {
            mean_stoch_time = recordedPropertiesPerInstance[maxInstances][m];
        } else if (std::get<0>(recordedProperties[m]) == -1) {
            approximation_runs = recordedPropertiesPerInstance[maxInstances][m];
        } else if (recordedPropertiesPerInstance[maxInstances][m] > 0 || m < 2) {
            // Print all probabilities that are larger than 0, and always print the probabilities for state 0 and 1
            std::string amplitude = std::get<1>(recordedProperties[m]);
            std::replace(amplitude.begin(), amplitude.end(), '2', '1');
            measure_result.insert({amplitude, recordedPropertiesPerInstance[maxInstances][m]});
        }
    }
    return measure_result;
}

void StochasticNoiseSimulator::runStochSimulationForId(std::size_t                                 stochRun,
                                                       dd::Qubit                                   nQubits,
                                                       std::vector<double>&                        recordedPropertiesStorage,
                                                       std::vector<std::tuple<long, std::string>>& recordedPropertiesList,
                                                       std::map<std::string, unsigned int>&        classicalMeasurementsMap,
                                                       unsigned long long                          localSeed) {
    std::mt19937_64                        generator(localSeed);
    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);

    const unsigned long numberOfRuns = stochasticRuns / maxInstances + (stochRun < stochasticRuns % maxInstances ? 1 : 0);
    const int           approx_mod   = std::ceil(static_cast<double>(qc->getNops()) / (step_number + 1));

    //printf("Running %d times and using the dd at %p, using the cn object at %p\n", numberOfRuns, (void *) &package, (void *) &package->cn);
    for (unsigned long current_run = 0; current_run < numberOfRuns; current_run++) {
        const auto t1 = std::chrono::steady_clock::now();

        auto localDD                      = std::make_unique<StochasticNoisePackage>(getNumberOfQubits());
        auto stochasticNoiseFunctionality = dd::StochasticNoiseFunctionality<StochasticNoisePackage>(
                localDD,
                nQubits,
                noiseProbability,
                amplitudeDampingProb,
                multiQubitGateFactor,
                noiseEffects);

        std::map<unsigned int, bool> classic_values;

        unsigned int op_count     = 0;
        unsigned int approx_count = 0;

        dd::vEdge localRootEdge = localDD->makeZeroState(nQubits);
        localDD->incRef(localRootEdge);

        for (auto& op: *qc) {
            if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
                if (auto* nu_op = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                    if (nu_op->getType() == qc::Measure) {
                        auto quantum = nu_op->getTargets();
                        auto classic = nu_op->getClassics();

                        assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                        for (unsigned int i = 0; i < quantum.size(); ++i) {
                            char result = localDD->measureOneCollapsing(localRootEdge, quantum.at(i), true, generator);
                            assert(result == '0' || result == '1');
                            classic_values[classic.at(i)] = (result == '0');
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
                    auto* classic_op = dynamic_cast<qc::ClassicControlledOperation*>(op.get());
                    bool  execute_op = true;
                    auto  expValue   = classic_op->getExpectedValue();

                    for (unsigned int i = classic_op->getControlRegister().first; i < classic_op->getControlRegister().second; i++) {
                        if (classic_values[i] != (expValue % 2)) {
                            execute_op = false;
                            break;
                        } else {
                            expValue = expValue >> 1u;
                        }
                    }
                    operation = dd::getDD(classic_op->getOperation(), localDD);
                    targets   = classic_op->getOperation()->getTargets();
                    controls  = classic_op->getOperation()->getControls();
                    if (!execute_op) {
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

                std::vector usedQubits = targets;
                for (auto control: controls) {
                    usedQubits.push_back(control.qubit);
                }

                if (sequentiallyApplyNoise) {
                    stochasticNoiseFunctionality.setNoiseEffects(std::vector<dd::NoiseOperations>{dd::identity});
                    stochasticNoiseFunctionality.applyNoiseOperation(usedQubits, operation, localRootEdge, generator);
                    for (auto noiseEffect: noiseEffects) {
                        stochasticNoiseFunctionality.setNoiseEffects(std::vector<dd::NoiseOperations>{noiseEffect});
                        stochasticNoiseFunctionality.applyNoiseOperation(usedQubits, stochasticNoiseFunctionality.identityDD, localRootEdge, generator);
                    }
                } else {
                    stochasticNoiseFunctionality.applyNoiseOperation(usedQubits, operation, localRootEdge, generator);
                }

                if (step_fidelity < 1 && (op_count + 1) % approx_mod == 0) {
                    approx_count++;
                    ApproximateByFidelity(localDD, localRootEdge, step_fidelity, false, true);
                }
            }
            localDD->garbageCollect();
            op_count++;
        }
        localDD->decRef(localRootEdge);
        const auto t2 = std::chrono::steady_clock::now();

        std::string classic_register_string;
        if (!classic_values.empty()) {
            for (const auto val: classic_values)
                classic_register_string.push_back(val.second ? '0' : '1');
            classicalMeasurementsMap[classic_register_string] += 1;
        }

        for (unsigned long i = 0; i < recordedPropertiesStorage.size(); i++) {
            if (std::get<0>(recordedPropertiesList[i]) == -2) {
                recordedPropertiesStorage[i] += std::chrono::duration<float>(t2 - t1).count();
            } else if (std::get<0>(recordedPropertiesList[i]) == -1) {
                recordedPropertiesStorage[i] += approx_count;
            } else {
                // extract amplitude for state
                const auto basisVector = std::get<1>(recordedPropertiesList[i]);
                auto       amplitude   = localDD->getValueByPath(localRootEdge, basisVector);
                auto       prob        = amplitude.r * amplitude.r + amplitude.i * amplitude.i;
                recordedPropertiesStorage[i] += prob;
            }
        }
    }
}

void StochasticNoiseSimulator::setRecordedProperties(const std::string& input) {
    std::string subString;
    int         list_begin = std::numeric_limits<decltype(list_begin)>::min();
    int         list_end   = std::numeric_limits<decltype(list_begin)>::min();
    for (char i: input) {
        if (i == ' ') {
            continue;
        }
        if (i == ',') {
            if (list_begin > std::numeric_limits<decltype(list_begin)>::min()) {
                list_end = std::stoi(subString);
                if (list_end > std::pow(2, getNumberOfQubits())) {
                    list_end = (int)std::pow(2, getNumberOfQubits());
                }
                assert(list_begin < list_end);
                for (int m = list_begin; m <= list_end; m++) {
                    recordedProperties.emplace_back(std::make_tuple(m, intToString(m)));
                }
            } else {
                recordedProperties.emplace_back(std::make_tuple(std::stoi(subString), intToString(std::stoi(subString))));
            }
            subString  = "";
            list_begin = std::numeric_limits<decltype(list_begin)>::min();
            list_end   = std::numeric_limits<decltype(list_begin)>::min();
            continue;
        }
        if (i == '-' && subString.length() > 0) {
            list_begin = std::stoi(subString);
            subString  = "";
            continue;
        }
        subString += i;
    }
    if (list_begin > std::numeric_limits<decltype(list_begin)>::min()) {
        list_end = std::stoi(subString);
        if (list_end > std::pow(2, getNumberOfQubits())) {
            list_end = (int)std::pow(2, getNumberOfQubits()) - 1;
        }
        assert(list_begin < list_end);
        for (int m = list_begin; m <= list_end; m++) {
            recordedProperties.emplace_back(std::make_tuple(m, intToString(m)));
        }
    } else {
        recordedProperties.emplace_back(std::make_tuple(std::stoi(subString), intToString(std::stoi(subString))));
    }
}

std::string StochasticNoiseSimulator::intToString(long target_number) const {
    if (target_number < 0) {
        return "X";
    }
    auto        qubits = getNumberOfQubits();
    std::string path(qubits, '0');
    auto        number = (unsigned long)target_number;
    for (int i = 1; i <= qubits; i++) {
        if (number % 2) {
            path[qubits - i] = '1';
        }
        number = number >> 1u;
    }
    return path;
}
