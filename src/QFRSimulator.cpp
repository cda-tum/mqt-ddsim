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
                    auto classic_bit_index = static_cast<short>(cc_op->getParameter().at(0)); // fp -> short *argh*
                    if (!classic_values[classic_bit_index]) {
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