#include "StochasticNoiseSimulator.hpp"

#include <queue>
#include <stdexcept>

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

            auto dd_op = op->getDD(dd);
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
    const unsigned short n_qubits = qc->getNqubits();

    //Ceiling[(Log[estimatesProp] + Log[(2/confidence)])/(2*errorBound^2)]
    if (stochastic_runs == 0) {
        stochastic_runs = std::ceil((std::log(recorded_properties.size()) + std::log(2 / stoch_confidence)) / (2 * stoch_error_margin * stoch_error_margin));
    }
    //std::clog << "Conducting perfect run...\n";
    const auto t1_perfect = std::chrono::steady_clock::now();
    perfect_simulation_run();
    const auto t2_perfect = std::chrono::steady_clock::now();
    perfect_run_time      = std::chrono::duration<float>(t2_perfect - t1_perfect).count();

    dd::Edge noiseless_root_edge = root_edge;

    // Generate a vector for each instance + 1. the final vector stores the average of all runs and is calculated
    // after the runs have finished
    for (unsigned int i = 0; i < max_instances + 1; i++) {
        recorded_properties_per_instance.emplace_back(std::vector<double>(recorded_properties.size(), 0));
    }
    //std::clog << "Conducting " << stochastic_runs << " runs using " << max_instances << " cores...\n";
    std::vector<std::thread> threadArray;
    // The stochastic runs are applied in parallel
    //std::clog << "Starting " << max_instances << " threads\n";
    const auto t1_stoch = std::chrono::steady_clock::now();
    for (unsigned int runID = 0; runID < max_instances; runID++) {
        threadArray.emplace_back(&StochasticNoiseSimulator::runStochSimulationForId,
                                 this,
                                 runID,
                                 n_qubits,
                                 noiseless_root_edge,
                                 std::ref(recorded_properties_per_instance[runID]),
                                 std::ref(recorded_properties),
                                 static_cast<unsigned long long>(mt()));
    }
    // wait for threads to finish
    for (auto& thread: threadArray) {
        thread.join();
    }
    const auto t2_stoch = std::chrono::steady_clock::now();
    stoch_run_time      = std::chrono::duration<float>(t2_stoch - t1_stoch).count();
    //std::clog <<"Calculating amplitudes from all runs...\n";
    for (unsigned long j = 0; j < recorded_properties.size(); j++) {
        for (unsigned int i = 0; i < max_instances; i++) {
            recorded_properties_per_instance[max_instances][j] += recorded_properties_per_instance[i][j];
        }
        recorded_properties_per_instance[max_instances][j] /= stochastic_runs;
    }

    //std::clog << "Probabilities are ... (probabilities < 0.001 are omitted)\n";
    std::map<std::string, double> measure_result;
    for (unsigned long m = 0; m < recorded_properties.size(); m++) {
        if (std::get<0>(recorded_properties[m]) == -3) {
            mean_stoch_time = recorded_properties_per_instance[max_instances][m];
        } else if (std::get<0>(recorded_properties[m]) == -2) {
            approximation_runs = recorded_properties_per_instance[max_instances][m];
        } else if (std::get<0>(recorded_properties[m]) == -1) {
            final_fidelity = recorded_properties_per_instance[max_instances][m];
        } else if (recorded_properties_per_instance[max_instances][m] > 0.001 || m < 2) {
            std::string amplitude = std::get<1>(recorded_properties[m]);
            std::replace(amplitude.begin(), amplitude.end(), '2', '1');
            measure_result.insert({amplitude, recorded_properties_per_instance[max_instances][m]});
        }
    }
    return measure_result;
}

dd::Package::vEdge StochasticNoiseSimulator::RemoveNodesInPackage(std::unique_ptr<dd::Package>& localDD, dd::Package::vEdge e, std::map<dd::Package::vNode*, dd::Package::vEdge>& dag_edges) {
    if (e.isTerminal()) {
        return e;
    }

    const auto it = dag_edges.find(e.p);
    if (it != dag_edges.end()) {
        dd::Package::vEdge r = it->second;
        if (r.w.approximatelyZero()) {
            return dd::Package::vEdge::zero;
        }
        dd::Complex c = localDD->cn.getTemporary();
        dd::ComplexNumbers::mul(c, e.w, r.w);
        r.w = localDD->cn.lookup(c);
        return r;
    }

    std::array<dd::Package::vEdge, dd::RADIX> edges{
            RemoveNodesInPackage(localDD, e.p->e.at(0), dag_edges),
            RemoveNodesInPackage(localDD, e.p->e.at(1), dag_edges)};

    dd::Package::vEdge r = localDD->makeDDNode(e.p->v, edges, false);
    dag_edges[e.p]       = r;
    dd::Complex c        = localDD->cn.getCached();
    dd::ComplexNumbers::mul(c, e.w, r.w);
    r.w = localDD->cn.lookup(c);
    return r;
}

double StochasticNoiseSimulator::ApproximateEdgeByFidelity(std::unique_ptr<dd::Package>& localDD, dd::Package::vEdge& edge, double targetFidelity, bool allLevels, bool removeNodes) {
    std::queue<dd::Package::vNode*>       q;
    std::map<dd::Package::vNode*, dd::fp> probsMone;

    probsMone[edge.p] = dd::ComplexNumbers::mag2(edge.w);

    q.push(edge.p);

    while (!q.empty()) {
        dd::Package::vNode* ptr = q.front();
        q.pop();
        const dd::fp parent_prob = probsMone[ptr];

        if (ptr->e.at(0).w != dd::Complex::zero) {
            if (probsMone.find(ptr->e.at(0).p) == probsMone.end()) {
                q.push(ptr->e.at(0).p);
                probsMone[ptr->e.at(0).p] = 0;
            }
            probsMone[ptr->e.at(0).p] = probsMone.at(ptr->e.at(0).p) + parent_prob * dd::ComplexNumbers::mag2(ptr->e.at(0).w);
        }

        if (ptr->e.at(1).w != dd::Complex::zero) {
            if (probsMone.find(ptr->e.at(1).p) == probsMone.end()) {
                q.push(ptr->e.at(1).p);
                probsMone[ptr->e.at(1).p] = 0;
            }
            probsMone[ptr->e.at(1).p] = probsMone.at(ptr->e.at(1).p) + parent_prob * dd::ComplexNumbers::mag2(ptr->e.at(1).w);
        }
    }

    std::vector<int> nodes(getNumberOfQubits(), 0);

    std::vector<std::priority_queue<std::pair<double, dd::Package::vNode*>, std::vector<std::pair<double, dd::Package::vNode*>>>> qq(
            getNumberOfQubits());

    for (auto& it: probsMone) {
        if (it.first->v < 0) {
            continue; // ignore the terminal node which has v == -1
        }
        nodes.at(it.first->v)++;
        qq.at(it.first->v).emplace(1 - it.second, it.first);
    }

    probsMone.clear();
    std::vector<dd::Package::vNode*> nodes_to_remove;

    int max_remove = 0;
    for (int i = 0; i < getNumberOfQubits(); i++) {
        double                           sum    = 0.0;
        int                              remove = 0;
        std::vector<dd::Package::vNode*> tmp;

        while (!qq.at(i).empty()) {
            auto p = qq.at(i).top();
            qq.at(i).pop();
            sum += 1 - p.first;
            if (sum < 1 - targetFidelity) {
                remove++;
                if (allLevels) {
                    nodes_to_remove.push_back(p.second);
                } else {
                    tmp.push_back(p.second);
                }
            } else {
                break;
            }
        }
        if (!allLevels && remove * i > max_remove) {
            max_remove      = remove * i;
            nodes_to_remove = tmp;
        }
    }

    std::map<dd::Package::vNode*, dd::Package::vEdge> dag_edges;
    for (auto& it: nodes_to_remove) {
        dag_edges[it] = dd::Package::vEdge::zero;
    }

    dd::Package::vEdge newEdge = RemoveNodesInPackage(localDD, edge, dag_edges);
    assert(!std::isnan(dd::CTEntry::val(edge.w.r)));
    assert(!std::isnan(dd::CTEntry::val(edge.w.i)));
    dd::Complex c = localDD->cn.getCached(std::sqrt(dd::ComplexNumbers::mag2(newEdge.w)), 0);
    dd::ComplexNumbers::div(c, newEdge.w, c);
    newEdge.w = localDD->cn.lookup(c);

    dd::fp fidelity = 0;
    if (edge.p->v == newEdge.p->v) {
        fidelity = localDD->fidelity(edge, newEdge);
    }

    if (removeNodes) {
        localDD->decRef(edge);
        localDD->incRef(newEdge);
        edge = newEdge;
    }
    return fidelity;
}

void StochasticNoiseSimulator::runStochSimulationForId(unsigned int                                stochRun,
                                                       int                                         n_qubits,
                                                       dd::Package::vEdge                          rootEdgePerfectRun,
                                                       std::vector<double>&                        recordedPropertiesStorage,
                                                       std::vector<std::tuple<long, std::string>>& recordedPropertiesList,
                                                       unsigned long long                          local_seed) {
    std::mt19937_64                        generator(local_seed);
    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);

    const unsigned long numberOfRuns = stochastic_runs / max_instances + (stochRun < stochastic_runs % max_instances ? 1 : 0);
    const int           approx_mod   = std::ceil(static_cast<double>(qc->getNops()) / (step_number + 1));

    //printf("Running %d times and using the dd at %p, using the cn object at %p\n", numberOfRuns, (void *) &localDD, (void *) &localDD->cn);
    for (unsigned long current_run = 0; current_run < numberOfRuns; current_run++) {
        const auto t1 = std::chrono::steady_clock::now();

        std::unique_ptr<dd::Package> localDD = std::make_unique<dd::Package>(getNumberOfQubits());
        //dd::NoiseOperationTable<dd::Package::mEdge> noiseOperationTable(getNumberOfQubits());

        dd::Package::mEdge identity_DD = localDD->makeIdent(n_qubits);
        localDD->incRef(identity_DD);
        dd::Package::vEdge local_root_edge = localDD->makeZeroState(n_qubits);
        localDD->incRef(local_root_edge);

        std::map<unsigned int, bool> classic_values;

        unsigned int op_count     = 0;
        unsigned int approx_count = 0;

        for (auto& op: *qc) {
            if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
                if (auto* nu_op = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                    if (nu_op->getName()[0] == 'M') {
                        auto quantum = nu_op->getTargets();
                        auto classic = nu_op->getClassics();

                        assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                        for (unsigned int i = 0; i < quantum.size(); ++i) {
                            char result;
                            local_root_edge = MeasureOneCollapsingConcurrent(quantum.at(i), localDD, local_root_edge, generator, &result);
                            assert(result == '0' || result == '1');
                            classic_values[classic.at(i)] = (result == '1');
                        }
                    } else if (nu_op->getName()[0] == 'R' && nu_op->getName()[1] == 's' && nu_op->getName()[2] == 't') {
                        // Reset qubit
                        throw std::runtime_error("Warning: Reset is currently not supported");
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
                localDD->garbageCollect(false);
            } else {
                dd::Package::mEdge dd_op{};
                qc::Targets        targets;
                dd::Controls       controls;
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
                    dd_op    = classic_op->getOperation()->getDD(localDD);
                    targets  = classic_op->getOperation()->getTargets();
                    controls = classic_op->getOperation()->getControls();
                    if (!execute_op) {
                        // applyNoiseOperation(targets.front(), controls, identity_DD, (std::unique_ptr<dd::Package> &) localDD, local_root_edge, generator, dist, line, identity_DD);
                        continue;
                    }
                } else {
                    dd_op    = op->getDD(localDD);
                    targets  = op->getTargets();
                    controls = op->getControls();
                }

                applyNoiseOperation(targets, controls, dd_op, localDD, local_root_edge, generator, dist, identity_DD);
                if (step_fidelity < 1 && (op_count + 1) % approx_mod == 0) {
                    ApproximateEdgeByFidelity(localDD, local_root_edge, step_fidelity, false, true);
                    approx_count++;
                }
                //localDD->garbageCollect(false);
            }
            op_count++;
        }
        localDD->decRef(local_root_edge);
        const auto t2 = std::chrono::steady_clock::now();

        for (unsigned long i = 0; i < recordedPropertiesStorage.size(); i++) {
            if (std::get<0>(recordedPropertiesList[i]) == -3) {
                recordedPropertiesStorage[i] += std::chrono::duration<float>(t2 - t1).count();
            } else if (std::get<0>(recordedPropertiesList[i]) == -2) {
                recordedPropertiesStorage[i] += approx_count;
            } else if (std::get<0>(recordedPropertiesList[i]) == -1) {
                recordedPropertiesStorage[i] += localDD->fidelity(local_root_edge, rootEdgePerfectRun);
            } else {
                // extract amplitude for state
                const auto basisVector = std::get<1>(recordedPropertiesList[i]);
                auto       amplitude   = localDD->getValueByPath(local_root_edge, basisVector);
                auto       prob        = amplitude.r * amplitude.r + amplitude.i * amplitude.i;
                recordedPropertiesStorage[i] += prob;
            }
        }
    }
}

void StochasticNoiseSimulator::applyNoiseOperation(const qc::Targets&                      targets,
                                                   const dd::Controls&                     control_qubits,
                                                   dd::Package::mEdge                      dd_op,
                                                   std::unique_ptr<dd::Package>&           localDD,
                                                   dd::Package::vEdge&                     local_root_edge,
                                                   std::mt19937_64&                        generator,
                                                   std::uniform_real_distribution<dd::fp>& dist,
                                                   dd::Package::mEdge                      identityDD) {
    // TODO: Is this a meaningful way to handle multi-target operations?
    for (auto& target: targets) {
        auto operation = generateNoiseOperation(false, target, generator, dist, dd_op, localDD);
        auto tmp       = localDD->multiply(operation, local_root_edge);

        if (dd::ComplexNumbers::mag2(tmp.w) < dist(generator)) {
            operation = generateNoiseOperation(true, target, generator, dist, dd_op, localDD);
            tmp       = localDD->multiply(operation, local_root_edge);
        }

        if (tmp.w != dd::Complex::one) {
            tmp.w = dd::Complex::one;
        }
        localDD->incRef(tmp);
        localDD->decRef(local_root_edge);
        local_root_edge = tmp;
    }
    // Apply noise to control qubits
    for (auto control: control_qubits) {
        auto operation = generateNoiseOperation(false, control.qubit, generator, dist, identityDD, localDD);
        auto tmp       = localDD->multiply(operation, local_root_edge);
        if (dd::ComplexNumbers::mag2(tmp.w) < dist(generator)) {
            operation = generateNoiseOperation(true, control.qubit, generator, dist, identityDD, localDD);
            tmp       = localDD->multiply(operation, local_root_edge);
        }
        if (tmp.w != dd::Complex::one) {
            tmp.w = dd::Complex::one;
        }
        localDD->incRef(tmp);
        localDD->decRef(local_root_edge);
        local_root_edge = tmp;
    }
}

dd::Package::mEdge StochasticNoiseSimulator::generateNoiseOperation(bool                                    amplitudeDamping,
                                                                    dd::Qubit                               target,
                                                                    std::mt19937_64&                        engine,
                                                                    std::uniform_real_distribution<dd::fp>& distribution,
                                                                    dd::Package::mEdge                      dd_operation,
                                                                    std::unique_ptr<dd::Package>&           localDD) {
    dd::NoiseOperationKind effect;

    for (const auto& noise_type: gate_noise_types) {
        if (noise_type != 'A') {
            effect = ReturnNoiseOperation(noise_type, distribution(engine));
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
                    auto tmp_op = localDD->noiseOperationTable.lookup(getNumberOfQubits(), dd::ATrue, target);
                    if (tmp_op.p == nullptr) {
                        tmp_op = localDD->makeGateDD(dd::GateMatrix({dd::complex_zero, sqrt_amplitude_damping_probability, dd::complex_zero, dd::complex_zero}), getNumberOfQubits(), target);
                        localDD->noiseOperationTable.insert(dd::ATrue, target, tmp_op);
                    }
                    dd_operation = localDD->multiply(tmp_op, dd_operation);
                    break;
                }
                case (dd::AFalse): {
                    auto tmp_op = localDD->noiseOperationTable.lookup(getNumberOfQubits(), dd::AFalse, target);
                    if (tmp_op.p == nullptr) {
                        tmp_op = localDD->makeGateDD(dd::GateMatrix({dd::complex_one, dd::complex_zero, dd::complex_zero, one_minus_sqrt_amplitude_damping_probability}), getNumberOfQubits(), target);
                        localDD->noiseOperationTable.insert(dd::AFalse, target, tmp_op);
                    }
                    dd_operation = localDD->multiply(tmp_op, dd_operation);
                    break;
                }
                case (dd::X): {
                    dd_operation = localDD->multiply(localDD->makeGateDD(dd::Xmat, getNumberOfQubits(), target), dd_operation);
                    break;
                }
                case (dd::Y): {
                    dd_operation = localDD->multiply(localDD->makeGateDD(dd::Ymat, getNumberOfQubits(), target), dd_operation);
                    break;
                }
                case (dd::Z): {
                    dd_operation = localDD->multiply(localDD->makeGateDD(dd::Zmat, getNumberOfQubits(), target), dd_operation);
                    break;
                }
                default: {
                    throw std::runtime_error("Unknown noise operation '" + std::to_string(effect) + "'\n");
                }
            }
        }
    }
    return dd_operation;
}

dd::Package::vEdge StochasticNoiseSimulator::MeasureOneCollapsingConcurrent(unsigned short                      index,
                                                                            const std::unique_ptr<dd::Package>& localDD,
                                                                            dd::Package::vEdge                  local_root_edge,
                                                                            std::mt19937_64&                    generator,
                                                                            char*                               result,
                                                                            bool                                assume_probability_normalization) {
    //todo merge the function with MeasureOneCollapsing
    std::map<dd::Package::vNode*, dd::fp> probsMone;
    std::set<dd::Package::vNode*>         visited_nodes2;
    std::queue<dd::Package::vNode*>       q;

    probsMone[local_root_edge.p] = dd::ComplexNumbers::mag2(local_root_edge.w);
    visited_nodes2.insert(local_root_edge.p);
    q.push(local_root_edge.p);

    while (q.front()->v != index) {
        dd::Package::vNode* ptr = q.front();
        q.pop();
        double prob = probsMone[ptr];

        if (!ptr->e.at(0).w.approximatelyZero()) {
            const dd::fp tmp1 = prob * dd::ComplexNumbers::mag2(ptr->e.at(0).w);

            if (visited_nodes2.find(ptr->e.at(0).p) != visited_nodes2.end()) {
                probsMone[ptr->e.at(0).p] = probsMone[ptr->e.at(0).p] + tmp1;
            } else {
                probsMone[ptr->e.at(0).p] = tmp1;
                visited_nodes2.insert(ptr->e.at(0).p);
                q.push(ptr->e.at(0).p);
            }
        }

        if (!ptr->e.at(1).w.approximatelyZero()) {
            const dd::fp tmp1 = prob * dd::ComplexNumbers::mag2(ptr->e.at(1).w);

            if (visited_nodes2.find(ptr->e.at(1).p) != visited_nodes2.end()) {
                probsMone[ptr->e.at(1).p] = probsMone[ptr->e.at(1).p] + tmp1;
            } else {
                probsMone[ptr->e.at(1).p] = tmp1;
                visited_nodes2.insert(ptr->e.at(1).p);
                q.push(ptr->e.at(1).p);
            }
        }
    }

    dd::fp pzero{0}, pone{0};

    if (assume_probability_normalization) {
        while (!q.empty()) {
            dd::Package::vNode* ptr = q.front();
            q.pop();

            if (!ptr->e.at(0).w.approximatelyZero()) {
                pzero += probsMone[ptr] * dd::ComplexNumbers::mag2(ptr->e.at(0).w);
            }

            if (!ptr->e.at(1).w.approximatelyZero()) {
                pone += probsMone[ptr] * dd::ComplexNumbers::mag2(ptr->e.at(1).w);
            }
        }
    } else {
        std::unordered_map<dd::Package::vNode*, double> probs;
        assign_probs(root_edge, probs);

        while (!q.empty()) {
            dd::Package::vNode* ptr = q.front();
            q.pop();

            if (!ptr->e.at(0).w.approximatelyZero()) {
                pzero += probsMone[ptr] * probs[ptr->e.at(0).p] * dd::ComplexNumbers::mag2(ptr->e.at(0).w);
            }

            if (!ptr->e.at(1).w.approximatelyZero()) {
                pone += probsMone[ptr] * probs[ptr->e.at(1).p] * dd::ComplexNumbers::mag2(ptr->e.at(1).w);
            }
        }
    }

    if (std::abs(pzero + pone - 1) > epsilon) {
        throw std::runtime_error("Numerical instability occurred during measurement: |alpha|^2 + |beta|^2 = " + std::to_string(pzero) + " + " + std::to_string(pone) + " = " + std::to_string(pzero + pone) + ", but should be 1!");
    }

    const dd::fp sum = pzero + pone;

    //std::pair<fp, fp> probs = std::make_pair(pzero, pone);
    dd::GateMatrix measure_m{
            dd::complex_zero, dd::complex_zero,
            dd::complex_zero, dd::complex_zero};

    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);

    dd::fp n = dist(generator);
    dd::fp norm_factor;

    if (n < pzero / sum) {
        measure_m[0] = dd::complex_one;
        norm_factor  = pzero;
        *result      = '0';
    } else {
        measure_m[3] = dd::complex_one;
        norm_factor  = pone;
        *result      = '1';
    }
    dd::Edge m_gate = localDD->makeGateDD(measure_m, getNumberOfQubits(), index);

    dd::Edge e = localDD->multiply(m_gate, local_root_edge);

    dd::Complex c = localDD->cn.getTemporary(std::sqrt(1 / norm_factor), 0);
    dd::ComplexNumbers::mul(c, e.w, c);
    e.w = localDD->cn.lookup(c);
    localDD->incRef(e);
    localDD->decRef(local_root_edge);
    local_root_edge = e;

    return local_root_edge;
}

dd::NoiseOperationKind StochasticNoiseSimulator::ReturnNoiseOperation(char i, double prob) const {
    //    if (forcedResult.size() > currentResult) {
    //        auto tmp = forcedResult[currentResult];
    //        currentResult += 1;
    //        return tmp;
    //    }
    switch (i) {
        case 'D': {
            if (prob >= 3 * noise_probability / 4) {
                //                printf("T");
                return dd::I;
            } else if (prob < noise_probability / 4) {
                //                printf("X");
                return dd::X;
            } else if (prob < noise_probability / 2) {
                //                printf("Y");
                return dd::Y;
            } else { //if (prob < 3 * noise_probability / 4) {
                     //                printf("Z");
                return dd::Z;
            }
        }
        case 'A': {
            return dd::AFalse;
        }
        case 'P': {
            if (prob > noise_probability) {
                //                printf("B");
                return dd::I;
            } else {
                //                printf("P");
                return dd::Z; // A phase flip is represented by a Z operation
            }
        }
        default:
            throw std::runtime_error(std::string{"Unknown noise effect '"} + i + "'");
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
