#include "QFRSimulator.hpp"

void QFRSimulator::Simulate() {
    const unsigned short n_qubits = qc->getNqubits();

    std::array<short, qc::MAX_QUBITS> line{};
    line.fill(qc::LINE_DEFAULT);

    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);

    unsigned long op_num = 0;

    std::map<int, bool> classic_values;
    std::map<unsigned short, unsigned short> variable_map;

    for (auto& op : *qc) {
        if (op->isNonUnitaryOperation()) {
            if (auto *nu_op = dynamic_cast<qc::NonUnitaryOperation *>(op.get())) {
                if (nu_op->getName()[0] == 'M') { // Measure starts with 'M', quite hacky though
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
            std::clog << "[INFO] op " << op_num++ << " is " << op->getName()
                      << " #controls=" << op->getControls().size()
                      << " statesize=" << dd->size(root_edge) << "\n";


            auto move_to_top = [this](std::unique_ptr<qc::Operation>& op, std::map<unsigned short, unsigned short>& variable_map){
                if (op->getNcontrols() + op->getNtargets() == 1) {
                    root_edge = dd->exchange(root_edge, op->getTargets().at(0), qc->getNqubits()-1);
                    variable_map.at(op->getTargets().at(0)) = qc->getNqubits()-1;
                    variable_map.at(qc->getNqubits()-1) = op->getTargets().at(0);
                } else if (op->getNcontrols() == 1 && op->getNtargets() == 1) {
                    //TODO: variable_map
                    root_edge = dd->exchange(root_edge, op->getControls().at(0).qubit, qc->getNqubits()-1);
                    root_edge = dd->exchange(root_edge, op->getTargets().at(0), qc->getNqubits()-2);
                } else {
                    // ignore multi-controlled gates
                }
            };

            auto move_to_bottom = [this](std::unique_ptr<qc::Operation>& op, std::map<unsigned short, unsigned short>& variable_map){
                if (op->getNcontrols() + op->getNtargets() == 1) {
                    root_edge = dd->exchange(root_edge, 0, op->getTargets().at(0));
                    //TODO: variable_map
                } else if (op->getNcontrols() == 1 && op->getNtargets() == 1) {
                    root_edge = dd->exchange(root_edge, 0, op->getControls().at(0).qubit);
                    root_edge = dd->exchange(root_edge, 1, op->getTargets().at(0));
                    //TODO: variable_map
                } else {
                    // ignore multi-controlled gates
                }
            };

            auto sifting = [this](std::unique_ptr<qc::Operation>& op, std::map<unsigned short, unsigned short>& variable_map){
                /// Idea: sift only if op affects qubits on lower half of DD
                root_edge = dd->sifting(root_edge, variable_map);
            };

            // decrease ref count before performing reordering since we will loose the original root_edge
            dd->decRef(root_edge);

            //move_to_top(op, variable_map);
            //move_to_bottom(op, variable_map);
            //sifting(op, variable_map);

            auto dd_op = op->getDD(dd, line);
            root_edge = dd->multiply(dd_op, root_edge);
            // increase ref count on new root_edge so that it is not removed during garbage collection
            dd->incRef(root_edge);
            dd->garbageCollect();
        }
    }

    //qc->changePermutation(root_edge, variable_map, line, dd);
}