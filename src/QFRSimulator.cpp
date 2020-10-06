#include "QFRSimulator.hpp"

void QFRSimulator::Simulate() {
    const unsigned short n_qubits = qc->getNqubits();

    std::array<short, qc::MAX_QUBITS> line{};
    line.fill(qc::LINE_DEFAULT);

    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);

    unsigned long op_num = 0;

    std::map<int, bool> classic_values;

    for (auto& op : *qc) {
        if (op->isNonUnitaryOperation()) {
            if (auto *nu_op = dynamic_cast<qc::NonUnitaryOperation *>(op.get())) {
                if (nu_op->getName()[0] == 'M') { // Measure starts with 'M', quite hacky though
                    auto quantum = nu_op->getControls();
                    auto classic = nu_op->getTargets();

                    if (quantum.size() != classic.size()) {
                        std::cerr << "[ERROR] Measurement: Sizes of quantum and classic register mismatch.\n";
                        std::exit(1);
                    }

                    for (unsigned int i = 0; i < quantum.size(); ++i) {
                        auto result = MeasureOneCollapsing(quantum[i].qubit);
                        assert(result == '0' || result == '1');
                        classic_values[classic[i]] = result == '1';
                    }
                } else {
                    std::cerr << "[ERROR] Unsupported non-unitary functionality." << std::endl;
                    std::exit(1);
                }
            } else {
                std::cerr << "[ERROR] Dynamic cast to NonUnitaryOperation failed." << std::endl;
                std::exit(1);
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

                    std::cout << "expected " << expected_value << " and actual value was " << actual_value << "\n";

                    if (actual_value != expected_value) {
                        continue;
                    }
                } else {
                    std::cerr << "[ERROR] Dynamic cast to ClassicControlledOperation failed." << std::endl;
                    std::exit(1);
                }
            }
            //std::clog << "[INFO] op " << op_num++ << " is " << op->getName()
            //          << " #controls=" << op->getControls().size()
            //          << " statesize=" << dd->size(root_edge) << "\n";
            auto dd_op = op->getDD(dd, line);
            auto tmp = dd->multiply(dd_op, root_edge);
            dd->incRef(tmp);
            dd->decRef(root_edge);
            root_edge = tmp;
            dd->garbageCollect();
        }
    }
}