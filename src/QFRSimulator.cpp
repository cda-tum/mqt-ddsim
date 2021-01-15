#include "QFRSimulator.hpp"

std::map<std::string, unsigned int> QFRSimulator::Simulate(unsigned int shots) {
    bool has_nonunitary = false;
    for (auto& op : *qc) {
        if (op->isNonUnitaryOperation()) {
            has_nonunitary = true;
            break;
        }
    }

    if(!has_nonunitary) {
        single_shot();
        return MeasureAllNonCollapsing(shots);
    }

    std::map<std::string, unsigned int> m_counter;

    for (unsigned int i=0; i < shots; i++) {
        single_shot();
        m_counter[MeasureAll()]++;
    }

    return m_counter;
}

void QFRSimulator::single_shot() {
    const unsigned short n_qubits = qc->getNqubits();
    const unsigned int reorder_max_nodes = (2u << n_qubits) * 0.9;

    std::array<short, qc::MAX_QUBITS> line{};
    line.fill(qc::LINE_DEFAULT);

    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);

    unsigned long op_num = 0;
    std::map<int, bool> classic_values;

    unsigned int ops_since_reorder = 0;
    std::map<unsigned short, unsigned short> variable_map;
    qc::permutationMap initial_map = qc->initialLayout;

    if(initial_reorder) {
        variable_map = do_initial_reorder(initial_reorder == 1);
    } else {
        for(int i=0; i < n_qubits; i++) {
            variable_map.insert({i,i});
        }
    }

    const int approx_mod = std::ceil(static_cast<double>(qc->getNops()) / (step_number+1));

    for (auto& op : *qc) {
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
                    for(unsigned int i = 0; i < length; i++) {
                        actual_value |= (classic_values[start_index+i]?1u:0u) << i;
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


            if (dynamic_reorder > 0 && pre_op_size > 1000 && pre_op_size < reorder_max_nodes && ops_since_reorder > 3) {
                dd->garbageCollect(true);

                if (dynamic_reorder == 1) {
                    std::tie(root_edge, sifting_min, sifting_max) = dd->sifting(root_edge, variable_map);
                } else if (dynamic_reorder == 2) {
                    move_to_top(op, variable_map);
                } else if (dynamic_reorder == 3) {
                    move_to_bottom(op, variable_map);
                } else {
                    throw std::runtime_error("Unknown dynamic reordering strategy");
                }
                reorder_count++;
                ops_since_reorder = 0;

                [[maybe_unused]]const unsigned int post_reorder_size = dd->size(root_edge);
                auto pprec = std::clog.precision(2);
                /*std::clog << "  Reordering: " << pre_op_size << " -> " << post_reorder_size
                          << "  (" << (static_cast<double>(post_reorder_size) / pre_op_size) << ")"
                          << " [" << dd->node_substitutions << "]"
                          << "\n";//*/
                std::clog.precision(pprec);
            }

            assert(dd->is_globally_consistent_dd(root_edge));
            auto dd_op = op->getDD(dd, line);
            auto tmp = dd->multiply(dd_op, root_edge);
            dd->incRef(tmp);
            dd->decRef(root_edge);
            root_edge = tmp;

            if(step_fidelity < 1.0) {
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
            ops_since_reorder++;
            assert(dd->is_globally_consistent_dd(root_edge));
        }
        op_num++;
    }

    variable_map_postsim = variable_map;
    if (post_reorder == 1) {
        std::tie(root_edge, sifting_min, sifting_max) = dd->sifting(root_edge, variable_map);
        reorder_count++;
    } else if(dynamic_reorder || initial_reorder) {
        qc->changePermutation(root_edge, variable_map, qc->outputPermutation, line, dd);
        reorder_count++;
    }
    variable_map_postrestore = variable_map;
}

qc::permutationMap QFRSimulator::do_initial_reorder(bool use_controls) {
    std::map<unsigned short, unsigned int> qubit_usage;

    for (auto const& op : *qc) {
        if (op->isNonUnitaryOperation()) {
            continue;
        }
        for (auto const& target : op->getTargets()) {
            qubit_usage[target]++;
        }
        if (use_controls) {
            for (auto const &control : op->getControls()) {
                qubit_usage[control.qubit]++;
            }
        }
    }

    std::vector<std::pair<unsigned short, unsigned int>> vpairs;
    vpairs.reserve(qc->getNqubits());

    for (int i = 0; i < qc->getNqubits(); ++i) {
        vpairs.emplace_back(i, qubit_usage[i]);
    }

    std::sort(vpairs.begin(), vpairs.end(), [](auto const& a, auto const& b){return a.second < b.second;});
    qc::permutationMap new_order;

    for (unsigned int i = 0; i < vpairs.size(); ++i) {
        new_order[i] = vpairs[i].first;
    }
    return new_order;
}

void QFRSimulator::move_to_top(std::unique_ptr<qc::Operation>& op, qc::permutationMap& variable_map) {
    if (op->getNcontrols() + op->getNtargets() == 1) {
        root_edge = dd->exchange2(variable_map[op->getTargets().at(0)], qc->getNqubits()-1, variable_map, root_edge);
    } else if (op->getNcontrols() == 1 && op->getNtargets() == 1) {
        root_edge = dd->exchange2(variable_map[op->getControls().at(0).qubit], qc->getNqubits()-1, variable_map, root_edge);
        root_edge = dd->exchange2(variable_map[op->getTargets().at(0)], qc->getNqubits()-2, variable_map, root_edge);
    } else {
        // ignore multi-controlled gates
    }
    assert(dd->unnormalizedNodes == 0);
}

void QFRSimulator::move_to_bottom(std::unique_ptr<qc::Operation>& op, qc::permutationMap& variable_map){
    if (op->getNcontrols() + op->getNtargets() == 1) {
        root_edge = dd->exchange2(0, variable_map[op->getTargets().at(0)], variable_map, root_edge);
    } else if (op->getNcontrols() == 1 && op->getNtargets() == 1) {
        root_edge = dd->exchange2(0, variable_map[op->getControls().at(0).qubit],variable_map, root_edge);
        root_edge = dd->exchange2(1, variable_map[op->getTargets().at(0)],variable_map, root_edge);
    } else {
        // ignore multi-controlled gates
    }
    assert(dd->unnormalizedNodes == 0);
}