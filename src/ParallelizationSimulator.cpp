#include "ParallelizationSimulator.hpp"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <mutex>
#include <functional>
#include <vector>
using namespace dd::literals;

std::map<std::string, std::size_t> ParallelizationSimulator::Simulate(unsigned int shots){
    std::thread first (&ParallelizationSimulator::SimulateMatrixVector, this);
    std::cout << "this is a test run\n";

    first.join();

    std::cout << "now we are done";

    shots++;
    std::map<std::string, std::size_t> m_counter;

    return m_counter;

}

dd::Edge<dd::Package::vNode> ParallelizationSimulator::SimulateMatrixVector() {
    single_shots++;
    const dd::QubitCount n_qubits = qc->getNqubits();

    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);

    std::size_t                 op_num = 0;
    std::map<std::size_t, bool> classic_values;

    //const int approx_mod = std::ceil(static_cast<double>(qc->getNops()) / (approx_info.step_number + 1));

    for (auto& op: *qc) {
        if (op->isNonUnitaryOperation()) {
            /*if(ignore_nonunitaries) {
                continue;
            }*/
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
                    throw std::runtime_error("Unsupported non-unitary functionality.");
                }
            } else {
                throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
            }
            dd->garbageCollect();
        } else {
            if (op->isClassicControlledOperation()) {
                if (auto* cc_op = dynamic_cast<qc::ClassicControlledOperation*>(op.get())) {
                    const auto         start_index    = static_cast<unsigned short>(cc_op->getParameter().at(0));
                    const auto         length         = static_cast<unsigned short>(cc_op->getParameter().at(1));
                    const unsigned int expected_value = cc_op->getExpectedValue();
                    unsigned int       actual_value   = 0;
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
            /*std::clog << "[INFO] op " << op_num << " is " << op->getName() << " on " << op->getTargets().at(0)
                      << " #controls=" << op->getControls().size()
                      << " statesize=" << dd->size(root_edge) << "\n";//*/
            /* hier wird Matrix Vector multiplikation ausgeführt, man betrachtet das ganze gatter und nicht einzelne elemente */
            auto dd_op = op->getDD(dd);
            auto tmp   = dd->multiply(dd_op, root_edge);
            /* die neue root edge wird geupdated */
            dd->incRef(tmp);
            dd->decRef(root_edge);
            root_edge = tmp;
            dd->garbageCollect();
        }
        op_num++;
    }
    return root_edge;
}
