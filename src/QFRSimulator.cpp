#include "QFRSimulator.hpp"

std::map<std::string, unsigned int> QFRSimulator::Simulate(unsigned int shots) {
    bool has_nonunitary = false;
    for (auto &op : *qc) {
        if (op->isNonUnitaryOperation()) {
            has_nonunitary = true;
            break;
        }
    }

    if (!has_nonunitary) {
        single_shot();
        return MeasureAllNonCollapsing(shots);
    }

    std::map<std::string, unsigned int> m_counter;

    for (unsigned int i = 0; i < shots; i++) {
        single_shot();
        m_counter[MeasureAll()]++;
    }

    return m_counter;
}

void QFRSimulator::single_shot() {
    const unsigned short n_qubits = qc->getNqubits();

    std::array<short, qc::MAX_QUBITS> line{};
    line.fill(qc::LINE_DEFAULT);

    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);

    unsigned long op_num = 0;
    std::map<int, bool> classic_values;

    const int approx_mod = std::ceil(static_cast<double>(qc->getNops()) / (step_number + 1));

    for (auto &op : *qc) {
        if (op->isNonUnitaryOperation()) {
            if (auto *nu_op = dynamic_cast<qc::NonUnitaryOperation *>(op.get())) {
                if (op->getType() == qc::Measure) {
                    auto quantum = nu_op->getControls();
                    auto classic = nu_op->getTargets();

                    if (quantum.size() != classic.size()) {
                        throw std::runtime_error("Measurement: Sizes of quantum and classic register mismatch.");
                    }

                    for (unsigned int i = 0; i < quantum.size(); ++i) {
                        auto result = MeasureOneCollapsing(quantum[i].qubit);
                        assert(result == '0' || result == '1');
                        classic_values[classic[i]] = result == '1';
                    }
                } else if (op->getType() == qc::Barrier) {
                    continue;
                } else {
                    throw std::runtime_error("Unsupported non-unitary functionality.");
                }
            } else {
                throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
            }
            dd->garbageCollect();
        } else {
            if (op->isClassicControlledOperation()) {
                if (auto *cc_op = dynamic_cast<qc::ClassicControlledOperation *>(op.get())) {
                    const auto start_index = static_cast<unsigned short>(cc_op->getParameter().at(0));
                    const auto length = static_cast<unsigned short>(cc_op->getParameter().at(1));
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
                    throw std::runtime_error(" Dynamic cast to ClassicControlledOperation failed.");
                }
            }
            const auto pre_op_size = dd->size(root_edge);
            /*std::clog << "[INFO] op " << op_num << " is " << op->getName()
                      << " #controls=" << op->getControls().size()
                      << " statesize=" << dd->size(root_edge) << "\n";//*/

            assert(dd->is_globally_consistent_dd(root_edge));
            auto dd_op = op->getDD(dd, line);
            auto tmp = dd->multiply(dd_op, root_edge);
            dd->incRef(tmp);
            dd->decRef(root_edge);
            root_edge = tmp;

            if (step_fidelity < 1.0) {
                if ((op_num + 1) % approx_mod == 0 && approximation_runs < step_number) {
                    [[maybe_unused]] const unsigned int size_before = dd->size(root_edge);
                    const double ap_fid = ApproximateByFidelity(step_fidelity, true);
                    approximation_runs++;
                    final_fidelity *= ap_fid;
                    /*std::clog << "[INFO] Appromation run finished. "
                              << "op_num=" << op_num
                              << "; previous size=" << size_before
                              << "; attained fidelity=" << ap_fid
                              << "; global fidelity=" << final_fidelity
                              << "; #runs=" << approximation_runs
                              << "\n";//*/
                }
            }
            dd->garbageCollect();
        }
        op_num++;
    }
}

std::map<std::string, double> QFRSimulator::StochSimulate() {
    const unsigned short n_qubits = qc->getNqubits();

    //Ceiling[(Log[estimatesProp] + Log[(2/confidence)])/(2*errorBound^2)]
    if (stochastic_runs == 0) {
        stochastic_runs = std::ceil((std::log(recorded_properties.size()) + std::log(2 / stoch_confidence)) / (2L * stoch_error_margin * stoch_error_margin));
    }
    printf("Conducting perfect run ...\n");

    Simulate(1);
    dd::Edge noiseless_root_edge = root_edge;

    // Generate a vector for each instance + 1. the final vector stores the average of all runs and is calculated
    // after the runs have finished
    for (int i = 0; i <= max_instances; i++) {
        recorded_properties_per_instance.emplace_back(std::vector<double>(recorded_properties.size(), 0));
    }

    printf("Conducting %lu runs using %d cores ...\n", stochastic_runs, max_instances);
    std::thread threadArray[MAXIMUM_NUMBER_OF_CORES];
    // The stochastic runs are applied in parallel
    printf("Starting %d threads\n", max_instances);
    for (int runID = 0; runID < max_instances; runID++) {
        threadArray[runID] = std::thread(&QFRSimulator::runStochSimulationForId, this, runID, n_qubits, noiseless_root_edge, std::ref(recorded_properties_per_instance[runID]), std::ref(recorded_properties));
    }
    // wait for threads to finish
    for (int runID = 0; runID < max_instances; runID++) {
        threadArray[runID].join();
    }

//    printf("Conducting %lu using 1 core ...\n", stochastic_runs);
//    for (int runID = 0; runID < max_instances; runID++) {
//        runStochSimulationForId(runID, n_qubits, noiseless_root_edge, recorded_properties_per_instance[runID], recorded_properties);
////        break;
//    }

    printf("Calculating amplitudes from all runs ...\n");
    for (unsigned long j = 0; j < recorded_properties.size(); j++) {
        for (int i = 0; i < max_instances; i++) {
            recorded_properties_per_instance[max_instances][j] += recorded_properties_per_instance[i][j];
        }
        recorded_properties_per_instance[max_instances][j] = recorded_properties_per_instance[max_instances][j] / (double) stochastic_runs;
    }

    printf("Probabilities are ... (probabilities < 0.001 are omitted)\n");
    std::map<std::string, double> measure_result = {};
    for (unsigned long m = 0; m < recorded_properties.size(); m++) {
        if (std::get<0>(recorded_properties[m]) == -1) {
            std::cout << "fidelity=" << recorded_properties_per_instance[max_instances][m] << "\n";
            measure_result.insert({"fidelity", recorded_properties_per_instance[max_instances][m]});
        } else {
            if (recorded_properties_per_instance[max_instances][m] > 0.001 || m < 2) {
                auto amplitude_min = recorded_properties_per_instance[max_instances][m] - stoch_error_margin;
                auto amplitude_max = recorded_properties_per_instance[max_instances][m] + stoch_error_margin;
                if (amplitude_min < 0) amplitude_min = 0;
                if (amplitude_max > 1) amplitude_max = 1;

                std::string amplitude = std::get<1>(recorded_properties[m]);
                std::replace(amplitude.begin(), amplitude.end(), '2', '1');
                std::cout << "state=|" << amplitude << "> proba=" << recorded_properties_per_instance[max_instances][m];
//                          << "\terror=" << stoch_error_margin << " (" << amplitude_min << "--" << amplitude_max << ")";
                std::cout << "\n" << std::flush;
                measure_result.insert({amplitude, recorded_properties_per_instance[max_instances][m]});
            }
        }
    }
    return measure_result;
}

void
QFRSimulator::runStochSimulationForId(int stochRun, int n_qubits, dd::Edge rootEdgePerfectRun, std::vector<double> &recordedPropertiesStorage, std::vector<std::tuple<long, std::string>> &recordedPropertiesList) {
    std::uniform_real_distribution<fp> dist(0.0, 1.0L);
    std::default_random_engine generator(seed + std::rand()); //Additional rand to increase entropy

    std::array<short, qc::MAX_QUBITS> line{};
    line.fill(qc::LINE_DEFAULT);
    std::unique_ptr<dd::Package> localDD = std::make_unique<dd::Package>();
    line[0] = 2;
    auto identity_DD = localDD->makeGateDD(qc::Imat, n_qubits, line);
    localDD->incRef(identity_DD);
    line[0] = -1;

    unsigned long numberOfRuns;
    numberOfRuns = std::floor(stochastic_runs / max_instances);
    if (stochRun < stochastic_runs % max_instances) {
        numberOfRuns += 1;
    }
    dd::Edge local_root_edge{};
    //printf("Running %d times and using the dd at %p, using the cn object at %p\n", numberOfRuns, (void *) &localDD, (void *) &localDD->cn);
    for (unsigned long current_run = 0; current_run < numberOfRuns; current_run++) {
//        printf("Current run id: %lu\n", current_run);
        std::map<unsigned int, bool> classic_values;
        local_root_edge = localDD->makeZeroState(n_qubits);
        localDD->incRef(local_root_edge);
        for (auto &op : *qc) {
            if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
                if (auto *nu_op = dynamic_cast<qc::NonUnitaryOperation *>(op.get())) {
                    if (nu_op->getName()[0] == 'M') {
                        auto quantum = nu_op->getControls();
                        auto classic = nu_op->getTargets();

                        if (quantum.size() != classic.size()) {
                            std::cerr << "[ERROR] Measurement: Sizes of quantum and classic register mismatch.\n";
                            std::exit(1);
                        }

                        for (unsigned int i = 0; i < quantum.size(); ++i) {
                            char result;
                            local_root_edge = MeasureOneCollapsingConcurrent(quantum[i].qubit, (std::unique_ptr<dd::Package> &) localDD, local_root_edge, generator, &result);
                            assert(result == '0' || result == '1');
                            classic_values[classic[i]] = result == '1';
                        }
                    } else if (nu_op->getName()[0] == 'R' && nu_op->getName()[1] == 's' && nu_op->getName()[2] == 't') {
                        // Reset qubit
                        printf("Warning: Reset is currently not supported");
                        continue;
                    } else {
                        //Skipping barrier
                        if (op->getType() == 27) {
                            continue;
                        }
                        std::cerr << "[ERROR] Unsupported non-unitary functionality." << std::endl;
                        std::exit(1);
                    }
                } else {
                    std::cerr << "[ERROR] Dynamic cast to NonUnitaryOperation failed." << std::endl;
                    std::exit(1);
                }
                localDD->garbageCollect();
            } else {
                dd::Edge dd_op = {};
                std::vector<unsigned short> targets;
                std::vector<qc::Control> controls;
                if (op->isClassicControlledOperation()) {
                    // Check if the operation is controlled by a classical register
                    auto *classic_op = dynamic_cast<qc::ClassicControlledOperation *>(op.get());
                    bool conditionTrue = true;
                    auto expValue = classic_op->getExpectedValue();

                    for (unsigned int i = classic_op->getControlRegister().first;
                         i < classic_op->getControlRegister().second; i++) {
                        if (classic_values[i] != (expValue % 2)) {
                            conditionTrue = false;
                            break;
                        } else {
                            expValue = expValue >> 1u;
                        }
                    }
                    dd_op = classic_op->getOperation()->getDD((std::unique_ptr<dd::Package> &) localDD, line);
                    targets = classic_op->getOperation()->getTargets();
                    assert(targets.size() == 1);
                    controls = classic_op->getOperation()->getControls();
                    if (!conditionTrue) {
//                        applyNoiseOperation(targets.front(), controls, identity_DD, (std::unique_ptr<dd::Package> &) localDD, local_root_edge, generator, dist, line, identity_DD);
                        continue;
                    }
                } else {
                    dd_op = op->getDD((std::unique_ptr<dd::Package> &) localDD, line);
                    targets = op->getTargets();
                    assert(targets.size() == 1);
                    controls = op->getControls();
                }
//                dd_op = op->getDD((std::unique_ptr<dd::Package> &) localDD, line);
//                auto tmp = localDD->multiply(dd_op, local_root_edge);
//                localDD->incRef(tmp);
//                localDD->decRef(local_root_edge);
//                local_root_edge = tmp;
//                for (int iter = 0; gate_noise[iter] != 0; iter++) {
//                    gateNoisev2[0] = gate_noise[iter];
//                    applyNoiseOperation(targets.front(), controls, identity_DD, (std::unique_ptr<dd::Package> &) localDD, local_root_edge, generator, dist, line, identity_DD);
//                }
                applyNoiseOperation(targets.front(), controls, dd_op, (std::unique_ptr<dd::Package> &) localDD, local_root_edge, generator, dist, line, identity_DD);
            }
//            printf("current run %lu, operation nr %lu\n", current_run, operation_nr);
//            operation_nr += 1;
//            if(operation_nr <= 10 && current_run == 0){
//                localDD->printVector(local_root_edge);
//                localDD->export2Dot(local_root_edge, "/home/user/Documents/drawing/afterToNormalization");
//                printf("\n");
//            }
        }
//        printf("Final state for run %lu\n", current_run);
//        printf("\n");
//        localDD->printVector(local_root_edge);
//        printf("\n");
//        if(current_run == 9){
//            printf("\n");
//        }
        for (unsigned long i = 0; i < recordedPropertiesStorage.size(); i++) {
            if (std::get<0>(recordedPropertiesList[i]) < 0) {
                recordedPropertiesStorage[i] += localDD->fidelity(local_root_edge, rootEdgePerfectRun);
            } else {
                // extract amplitude for state
                const auto basisVector = std::get<1>(recordedPropertiesList[i]);
                auto amplitude = localDD->getValueByPath(local_root_edge, basisVector);
                auto prob = amplitude.r * amplitude.r + amplitude.i * amplitude.i;
                recordedPropertiesStorage[i] += prob;
            }
        }
//        localDD->decRef(local_root_edge);
//        dd::NodePtr available_nodes = localDD->nodeAvail;
//        long available_nodes_count = 0;
//        while (available_nodes != nullptr){
//            available_nodes = available_nodes->next;
//            available_nodes_count++;
//        }
//        printf("Available nodes before garbage collect: %ld\n", available_nodes_count);
        try {
            localDD->garbageCollect(false);
        } catch (...) {
            printf("Warning an exception occurred in the garbageCollect. Generating a new dd package and continuing\n");
            localDD.reset();
            localDD = std::make_unique<dd::Package>();
        }
//        available_nodes = localDD->nodeAvail;
//        available_nodes_count = 0;
//        while (available_nodes != nullptr){
//            available_nodes = available_nodes->next;
//            available_nodes_count++;
//        }
//        printf("Available nodes after garbage collect: %ld\n", available_nodes_count);
//        printf("Nodes after simulation and garbage count %llu\n", localDD->node_allocations);
//        printf("\n");
//        dd::NodePtr node_pointer = localDD->firstAvail;
//        while (node_pointer != nullptr){
//            available_nodes = localDD->nodeAvail;
//            while(available_nodes != node_pointer && available_nodes != nullptr){
//                available_nodes = available_nodes->next;
//            }
//            if (available_nodes == nullptr){
//                printf("Did not find the current node\n");
//                printf("E0 %f r %f i, E1 %f r %f i , E2 %f r %f i, E3 %f r %f i\n", CN::val(node_pointer->e[0].w.r), CN::val(node_pointer->e[0].w.i),
//                CN::val(node_pointer->e[1].w.r), CN::val(node_pointer->e[1].w.i),
//                CN::val(node_pointer->e[2].w.r), CN::val(node_pointer->e[2].w.i),
//                CN::val(node_pointer->e[3].w.r), CN::val(node_pointer->e[3].w.i));
//                if (CN::val(node_pointer->e[0].w.r) != 1 && CN::val(node_pointer->e[0].w.i) == 0 &&
//                    CN::val(node_pointer->e[1].w.r) == 0 && CN::val(node_pointer->e[1].w.i) == 0 &&
//                    CN::val(node_pointer->e[2].w.r) == 0 && CN::val(node_pointer->e[2].w.i) == 0 &&
//                    CN::val(node_pointer->e[3].w.r) != 1 && CN::val(node_pointer->e[3].w.i) == 0) {
//                    printf("Found you \n");
//                }
//            } else {
//                printf("Found the current node!\n");
//            }
//            node_pointer = node_pointer->persistent_chain;
//        }
    }
//    printf("operation lookups: %lu, operation hit %lu, SuccessProb: %f\n", localDD->operationLook, localDD->operationCThit, (double) localDD->operationCThit / localDD->operationLook);

// Deleting the pointer
    localDD.reset();
}

void QFRSimulator::applyNoiseOperation(unsigned short targets, std::vector<qc::Control> &controls, dd::Edge dd_op, std::unique_ptr<dd::Package> &localDD, dd::Edge &local_root_edge, std::default_random_engine &generator,
                                       std::uniform_real_distribution<fp> &dist, std::array<short, qc::MAX_QUBITS> &line, dd::Edge identityDD) {
    auto operation = generateNoiseOperation(false, targets, generator, dist, dd_op, (std::unique_ptr<dd::Package> &) localDD, line);
    auto tmp = localDD->multiply(operation, local_root_edge);
    if (dd::ComplexNumbers::mag2(tmp.w) < dist(generator)) {
        operation = generateNoiseOperation(true, targets, generator, dist, dd_op, (std::unique_ptr<dd::Package> &) localDD, line);
        tmp = localDD->multiply(operation, local_root_edge);
    }
    if (tmp.w != CN::ONE) {
        tmp.w = CN::ONE;
    }
    localDD->incRef(tmp);
    localDD->decRef(local_root_edge);
    local_root_edge = tmp;
    // Apply noise to control qubits
    for (auto control: controls) {
        operation = generateNoiseOperation(false, control.qubit, generator, dist, identityDD, (std::unique_ptr<dd::Package> &) localDD, line);
        tmp = localDD->multiply(operation, local_root_edge);
//                    root_edge_weight = dd::ComplexNumbers::mag2(tmp.w);
        if (dd::ComplexNumbers::mag2(tmp.w) < dist(generator)) {
            operation = generateNoiseOperation(true, control.qubit, generator, dist, identityDD, (std::unique_ptr<dd::Package> &) localDD, line);
            tmp = localDD->multiply(operation, local_root_edge);
        }
        if (tmp.w != CN::ONE) {
            tmp.w = CN::ONE;
        }
        localDD->incRef(tmp);
        localDD->decRef(local_root_edge);
        local_root_edge = tmp;
    }
}


dd::Edge
QFRSimulator::generateNoiseOperation(const bool &amplitudeDamping, const unsigned short &target, std::default_random_engine &engine, std::uniform_real_distribution<fp> &distribution, dd::Edge dd_operation, std::unique_ptr<dd::Package> &localDD,
                                     std::array<short, qc::MAX_QUBITS> &line) {
    dd::CTkind effect;
    line[target] = 2;

    for (int noiseIterator = 0; gate_noise[noiseIterator] != '\0'; noiseIterator++) {
        if (gate_noise[noiseIterator] != 'A') {
            effect = ReturnNoiseOperation(gate_noise[noiseIterator], distribution(engine));
        } else {
            if (amplitudeDamping) {
                effect = dd::ATrue;
            } else {
                effect = dd::AFalse;
            }
        }
        if (effect != dd::I) {
            switch (effect) {
                case (dd::ATrue): {
                    auto tmp_op = localDD->OperationLookup(dd::ATrue, line.data(), getNumberOfQubits());
                    if (tmp_op.p == nullptr) {
                        tmp_op = localDD->makeGateDD(qc::GateMatrix({qc::complex_zero, sqrt_amplitude_damping_probability, qc::complex_zero, qc::complex_zero}), getNumberOfQubits(), line);
                        localDD->OperationInsert(dd::ATrue, line.data(), tmp_op, getNumberOfQubits());
                    }
                    dd_operation = localDD->multiply(tmp_op, dd_operation);
                    break;
                }
                case (dd::AFalse): {
                    auto tmp_op = localDD->OperationLookup(dd::AFalse, line.data(), getNumberOfQubits());
                    if (tmp_op.p == nullptr) {
                        tmp_op = localDD->makeGateDD(qc::GateMatrix({qc::complex_one, qc::complex_zero, qc::complex_zero, one_minus_sqrt_amplitude_damping_probability}), getNumberOfQubits(), line);
                        localDD->OperationInsert(dd::AFalse, line.data(), tmp_op, getNumberOfQubits());
                    }
                    dd_operation = localDD->multiply(tmp_op, dd_operation);
                    break;
                }
                case (dd::X): {
                    dd_operation = localDD->multiply(localDD->makeGateDD(qc::Xmat, getNumberOfQubits(), line), dd_operation);
                    break;
                }
                case (dd::Y): {
                    dd_operation = localDD->multiply(localDD->makeGateDD(qc::Ymat, getNumberOfQubits(), line), dd_operation);
                    break;
                }
                case (dd::Z): {
                    dd_operation = localDD->multiply(localDD->makeGateDD(qc::Zmat, getNumberOfQubits(), line), dd_operation);
                    break;
                }
                default: {
                    printf("Error: Unknown noise operation");
                    assert(false);
                }
            }
        }
    }
    line[target] = -1;
    return dd_operation;
}
