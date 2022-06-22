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

    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);

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
                } else if (op->getType() == qc::Reset) {
                    // Reset qubit
                    auto quantum = nu_op->getTargets();
                    for (signed char i: quantum) {
                        auto result = MeasureOneCollapsing(i, true);
                        if (result == '1') {
                            setMeasuredQubitToZero(i, root_edge, dd);
                        }
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

            auto dd_op = dd::getDD(op.get(), dd);
            auto tmp   = dd->multiply(dd_op, root_edge);
            dd->incRef(tmp);
            dd->decRef(root_edge);
            root_edge = tmp;

            dd->garbageCollect();
        }
        op_num++;
    }
}

std::map<std::string, double> StochasticNoiseSimulator::StochSimulate() {
    // Generate a vector for each instance + 1. the final vector stores the average of all runs and is calculated
    // after the runs have finished
    for (unsigned int i = 0; i < max_instances + 1; i++) {
        recorded_properties_per_instance.emplace_back(std::vector<double>(recorded_properties.size(), 0));
        classical_measurements_maps.emplace_back(std::map<std::string, int>());
    }
    //std::clog << "Conducting " << stochasticRuns << " runs using " << max_instances << " cores...\n";
    std::vector<std::thread> threadArray;
    // The stochastic runs are applied in parallel
    //std::clog << "Starting " << max_instances << " threads\n";
    const auto t1_stoch = std::chrono::steady_clock::now();
    for (unsigned int runID = 0; runID < max_instances; runID++) {
        threadArray.emplace_back(&StochasticNoiseSimulator::runStochSimulationForId,
                                 this,
                                 runID,
                                 qc->getNqubits(),
                                 std::ref(recorded_properties_per_instance[runID]),
                                 std::ref(recorded_properties),
                                 std::ref(classical_measurements_maps[runID]),
                                 static_cast<unsigned long long>(mt()));
    }
    // wait for threads to finish
    for (auto& thread: threadArray) {
        thread.join();
    }
    const auto t2_stoch = std::chrono::steady_clock::now();
    stoch_run_time      = std::chrono::duration<float>(t2_stoch - t1_stoch).count();

    for (unsigned long j = 0; j < max_instances; j++) {
        for (const auto& classical_measurements_map: classical_measurements_maps[j]) {
            classical_measurements_maps[max_instances][classical_measurements_map.first] += classical_measurements_map.second;
        }
    }

    //std::clog <<"Calculating amplitudes from all runs...\n";
    for (unsigned long j = 0; j < recorded_properties.size(); j++) {
        for (unsigned int i = 0; i < max_instances; i++) {
            recorded_properties_per_instance[max_instances][j] += recorded_properties_per_instance[i][j];
        }
        recorded_properties_per_instance[max_instances][j] /= stochasticRuns;
    }

    // Adding the result of classical registers to the measure_result map
    std::map<std::string, double> measure_result;
    for (const std::pair<const std::basic_string<char>, int>& classical_measurements_map: classical_measurements_maps[max_instances]) {
        auto probability = (double)classical_measurements_map.second / stochasticRuns;
        measure_result.insert({classical_measurements_map.first, probability}); //todo maybe print both classical and quantum register ? classical register:"
    }

    //std::clog << "Probabilities are ... (probabilities < 0.001 are omitted)\n";
    //    std::map<std::string, double> measure_result;
    for (unsigned long m = 0; m < recorded_properties.size(); m++) {
        if (std::get<0>(recorded_properties[m]) == -2) {
            mean_stoch_time = recorded_properties_per_instance[max_instances][m];
        } else if (std::get<0>(recorded_properties[m]) == -1) {
            approximation_runs = recorded_properties_per_instance[max_instances][m];
        } else if (recorded_properties_per_instance[max_instances][m] > 0 || m < 2) {
            // Print all probabilities that are larger than 0, and always print the probabilities for state 0 and 1
            std::string amplitude = std::get<1>(recorded_properties[m]);
            std::replace(amplitude.begin(), amplitude.end(), '2', '1');
            measure_result.insert({amplitude, recorded_properties_per_instance[max_instances][m]});
        }
    }
    return measure_result;
}

void StochasticNoiseSimulator::runStochSimulationForId(unsigned int                                stochRun,
                                                       int                                         nQubits,
                                                       std::vector<double>&                        recordedPropertiesStorage,
                                                       std::vector<std::tuple<long, std::string>>& recordedPropertiesList,
                                                       std::map<std::string, int>&                 classicalMeasurementsMap,
                                                       unsigned long long                          localSeed) {
    std::mt19937_64                        generator(localSeed);
    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);

    const unsigned long numberOfRuns = stochasticRuns / max_instances + (stochRun < stochasticRuns % max_instances ? 1 : 0);
    const int           approx_mod   = std::ceil(static_cast<double>(qc->getNops()) / (step_number + 1));

    //printf("Running %d times and using the dd at %p, using the cn object at %p\n", numberOfRuns, (void *) &localDD, (void *) &localDD->cn);
    for (unsigned long current_run = 0; current_run < numberOfRuns; current_run++) {
        const auto t1 = std::chrono::steady_clock::now();

        std::unique_ptr<StochasticNoiseSimulatorDDPackage> localDD = std::make_unique<StochasticNoiseSimulatorDDPackage>(getNumberOfQubits());

        std::map<unsigned int, bool> classic_values;

        unsigned int op_count     = 0;
        unsigned int approx_count = 0;

        dd::mEdge identityDD = localDD->makeIdent(nQubits);
        localDD->incRef(identityDD);
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
                dd::mEdge    dd_op{};
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
                    dd_op    = dd::getDD(classic_op->getOperation(), localDD);
                    targets  = classic_op->getOperation()->getTargets();
                    controls = classic_op->getOperation()->getControls();
                    if (!execute_op) {
                        // applyNoiseOperation(targets.front(), controls, identityDD, (std::unique_ptr<dd::Package> &) localDD, localRootEdge, generator, dist, line, identityDD);
                        continue;
                    }
                } else {
                    targets  = op->getTargets();
                    controls = op->getControls();

                    if (targets.size() == 1 && controls.empty()) {
                        dd_op = localDD->stochasticNoiseOperationCache.lookup(op->getType(), targets.front());
                        if (dd_op.p == nullptr) {
                            dd_op = dd::getDD(op.get(), localDD);
                            localDD->stochasticNoiseOperationCache.insert(op->getType(), targets.front(), dd_op);
                        }
                    } else {
                        dd_op = dd::getDD(op.get(), localDD);
                    }
                }

                std::vector usedQubits = targets;
                for (auto control: controls) {
                    usedQubits.push_back(control.qubit);
                }

                if (sequentialApplyNoise) {
                    auto tmp0 = localDD->multiply(dd_op, localRootEdge);
                    localDD->incRef(tmp0);
                    localDD->decRef(localRootEdge);
                    localRootEdge = tmp0;
                    dd_op         = identityDD;
                    for (auto gate_noise_type: gateNoiseEffects) {
                        std::vector<dd::noiseOperations> tmp_gate_noise_types = {gate_noise_type};
                        applyNoiseOperation(usedQubits, dd_op, localDD, localRootEdge, generator, dist, identityDD, tmp_gate_noise_types);
                    }
                } else {
                    applyNoiseOperation(usedQubits, dd_op, localDD, localRootEdge, generator, dist, identityDD, gateNoiseEffects);
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

void StochasticNoiseSimulator::applyNoiseOperation(const std::vector<dd::Qubit>& usedQubits, dd::mEdge dd_op, const std::unique_ptr<StochasticNoiseSimulatorDDPackage>& localDD,
                                                   dd::vEdge& localRootEdge, std::mt19937_64& generator, std::uniform_real_distribution<dd::fp>& dist,
                                                   const dd::mEdge& identityDD, const std::vector<dd::noiseOperations>& noiseOperation) {
    bool multiQubitOperation = usedQubits.size() > 1;

    for (auto& target: usedQubits) {
        auto operation = generateNoiseOperation(localDD, dd_op, target, noiseOperation, generator, dist, false, multiQubitOperation);
        auto tmp       = localDD->multiply(operation, localRootEdge);

        if (dd::ComplexNumbers::mag2(tmp.w) < dist(generator)) {
            operation = generateNoiseOperation(localDD, dd_op, target, noiseOperation, generator, dist, true, multiQubitOperation);
            tmp       = localDD->multiply(operation, localRootEdge);
        }

        if (tmp.w != dd::Complex::one) {
            tmp.w = dd::Complex::one;
        }
        localDD->incRef(tmp);
        localDD->decRef(localRootEdge);
        localRootEdge = tmp;

        // I only need to apply the operations once
        dd_op = identityDD;
    }
}

dd::mEdge StochasticNoiseSimulator::generateNoiseOperation(const std::unique_ptr<StochasticNoiseSimulatorDDPackage>& localDD, dd::mEdge dd_operation,
                                                           signed char target, const std::vector<dd::noiseOperations>& noiseOperation, std::mt19937_64& generator,
                                                           std::uniform_real_distribution<dd::fp>& distribution, bool amplitudeDamping,
                                                           bool multiQubitOperation) {
    qc::OpType effect;
    for (const auto& noise_type: noiseOperation) {
        if (noise_type != dd::noiseOperations::amplitudeDamping) {
            effect = returnNoiseOperation(noise_type, distribution(generator), multiQubitOperation);
        } else {
            if (amplitudeDamping) {
                effect = qc::ATrue;
            } else {
                effect = qc::AFalse;
            }
        }
        switch (effect) {
            case (dd::I): {
                continue;
            }
            case (qc::ATrue): {
                auto tmp_op = localDD->stochasticNoiseOperationCache.lookup(multiQubitOperation ? qc::multiATrue : qc::ATrue, target);
                if (tmp_op.p == nullptr) {
                    tmp_op = localDD->makeGateDD(multiQubitOperation ? ampDampingTrueMulti : ampDampingTrue, getNumberOfQubits(), target);
                    localDD->stochasticNoiseOperationCache.insert(multiQubitOperation ? qc::multiATrue : qc::ATrue, target, tmp_op);
                }

                dd_operation = localDD->multiply(tmp_op, dd_operation);
                break;
            }
            case (qc::AFalse): {
                auto tmp_op = localDD->stochasticNoiseOperationCache.lookup(multiQubitOperation ? qc::multiAFalse : qc::AFalse, target);
                if (tmp_op.p == nullptr) {
                    tmp_op = localDD->makeGateDD(multiQubitOperation ? ampDampingFalseMulti : ampDampingFalse, getNumberOfQubits(), target);
                    localDD->stochasticNoiseOperationCache.insert(multiQubitOperation ? qc::multiAFalse : qc::AFalse, target, tmp_op);
                }
                dd_operation = localDD->multiply(tmp_op, dd_operation);
                break;
            }
            case (qc::X): {
                auto tmp_op = localDD->stochasticNoiseOperationCache.lookup(effect, target);
                if (tmp_op.p == nullptr) {
                    tmp_op = localDD->makeGateDD(dd::Xmat, getNumberOfQubits(), target);
                    localDD->stochasticNoiseOperationCache.insert(effect, target, tmp_op);
                }
                dd_operation = localDD->multiply(tmp_op, dd_operation);
                break;
            }
            case (qc::Y): {
                auto tmp_op = localDD->stochasticNoiseOperationCache.lookup(effect, target);
                if (tmp_op.p == nullptr) {
                    tmp_op = localDD->makeGateDD(dd::Ymat, getNumberOfQubits(), target);
                    localDD->stochasticNoiseOperationCache.insert(effect, target, tmp_op);
                }
                dd_operation = localDD->multiply(tmp_op, dd_operation);
                break;
            }
            case (qc::Z): {
                auto tmp_op = localDD->stochasticNoiseOperationCache.lookup(effect, target);
                if (tmp_op.p == nullptr) {
                    tmp_op = localDD->makeGateDD(dd::Zmat, getNumberOfQubits(), target);
                    localDD->stochasticNoiseOperationCache.insert(effect, target, tmp_op);
                }
                dd_operation = localDD->multiply(tmp_op, dd_operation);
                break;
            }
            default: {
                throw std::runtime_error("Unknown noise operation '" + std::to_string(effect) + "'\n");
            }
        }
    }
    return dd_operation;
}

qc::OpType StochasticNoiseSimulator::returnNoiseOperation(dd::noiseOperations noiseOperation, double prob, bool multi_qubit_noise) const {
    switch (noiseOperation) {
        case dd::noiseOperations::depolarization: {
            if (prob >= 3 * (multi_qubit_noise ? noiseProbabilityMulti : noiseProbability) / 4) {
                return qc::I;
            } else if (prob < (multi_qubit_noise ? noiseProbabilityMulti : noiseProbability) / 4) {
                return qc::X;
            } else if (prob < (multi_qubit_noise ? noiseProbabilityMulti : noiseProbability) / 2) {
                return qc::Y;
            } else {
                return qc::Z;
            }
        }
        case dd::noiseOperations::phaseFlip: {
            if (prob > (multi_qubit_noise ? noiseProbabilityMulti : noiseProbability)) {
                return dd::I;
            } else {
                return dd::Z;
            }
        }
        default:
            throw std::runtime_error(std::string{"Unknown noise effect '"} + std::to_string(noiseOperation) + "'");
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
                    recorded_properties.emplace_back(std::make_tuple(m, intToString(m)));
                }
            } else {
                recorded_properties.emplace_back(std::make_tuple(std::stoi(subString), intToString(std::stoi(subString))));
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
            recorded_properties.emplace_back(std::make_tuple(m, intToString(m)));
        }
    } else {
        recorded_properties.emplace_back(std::make_tuple(std::stoi(subString), intToString(std::stoi(subString))));
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
void StochasticNoiseSimulator::setMeasuredQubitToZero(signed char& at, dd::vEdge& e, std::unique_ptr<StochasticNoiseSimulatorDDPackage>& localDD) const {
    auto f = dd::mEdge::one;

    for (std::size_t p = 0; p < getNumberOfQubits(); p++) {
        if (static_cast<dd::Qubit>(p) == at) {
            f = localDD->makeDDNode(static_cast<dd::Qubit>(p), std::array{f, f, dd::mEdge::zero, dd::mEdge::zero});
            //                f = dd->makeDDNode(static_cast<dd::Qubit>(p), std::array{f, dd::mEdge::zero, dd::mEdge::zero, dd::mEdge::zero});
        } else {
            f = localDD->makeDDNode(static_cast<dd::Qubit>(p), std::array{f, dd::mEdge::zero, dd::mEdge::zero, f});
        }
    }
    auto tmp = localDD->multiply(f, e);
    localDD->incRef(tmp);
    localDD->decRef(e);
    e = tmp;
}
