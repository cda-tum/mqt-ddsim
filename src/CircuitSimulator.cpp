#include "CircuitSimulator.hpp"

#include "Operations.hpp"
#include "dd/Export.hpp"

template<class Config>
std::map<std::string, std::size_t> CircuitSimulator<Config>::Simulate(std::size_t shots) {
    bool has_nonmeasurement_nonunitary = false;
    bool has_measurements              = false;
    bool measurements_last             = true;

    std::map<std::size_t, std::size_t> measurement_map;

    for (auto& op: *qc) {
        if (op->isClassicControlledOperation() || (op->isNonUnitaryOperation() && op->getType() != qc::Measure && op->getType() != qc::Barrier)) {
            has_nonmeasurement_nonunitary = true;
        }
        if (op->getType() == qc::Measure) {
            auto nu_op = dynamic_cast<qc::NonUnitaryOperation*>(op.get());
            if (nu_op == nullptr) {
                throw std::runtime_error("Op with type Measurement could not be casted to NonUnitaryOperation");
            }
            has_measurements = true;

            const auto& quantum = nu_op->getTargets();
            const auto& classic = nu_op->getClassics();

            if (quantum.size() != classic.size()) {
                throw std::runtime_error("Measurement: Sizes of quantum and classic register mismatch.");
            }

            for (unsigned int i = 0; i < quantum.size(); ++i) {
                measurement_map[quantum.at(i)] = classic.at(i);
            }
        }
        if (has_measurements && op->isUnitary()) {
            measurements_last = false;
        }
    }

    // easiest case: all gates are unitary --> simulate once and sample away on all qubits
    if (!has_nonmeasurement_nonunitary && !has_measurements) {
        singleShot(false);
        return Simulator<Config>::MeasureAllNonCollapsing(shots);
    }

    // single shot is enough, but the sampling should only return actually measured qubits
    if (!has_nonmeasurement_nonunitary && measurements_last) {
        singleShot(true);
        std::map<std::string, std::size_t> m_counter;
        const auto                         n_qubits = qc->getNqubits();
        const auto                         n_cbits  = qc->getNcbits();

        // MeasureAllNonCollapsing returns a map from measurement over all qubits to the number of occurrences
        for (const auto& item: Simulator<Config>::MeasureAllNonCollapsing(shots)) {
            std::string result_string(qc->getNcbits(), '0');

            for (auto const& m: measurement_map) {
                // m.first is the qubit, m.second the classical bit
                result_string[n_cbits - m.second - 1] = item.first[n_qubits - m.first - 1];
            }

            m_counter[result_string] += item.second;
        }

        return m_counter;
    }

    // there are nonunitaries (or intermediate measurement_map) and we have to actually do multiple single_shots :(
    std::map<std::string, std::size_t> m_counter;

    for (unsigned int i = 0; i < shots; i++) {
        const auto result  = singleShot(false);
        const auto n_cbits = qc->getNcbits();

        std::string result_string(qc->getNcbits(), '0');

        // result is a map from the cbit index to the Boolean value
        for (const auto& r: result) {
            result_string[n_cbits - r.first - 1] = r.second ? '1' : '0';
        }
        m_counter[result_string]++;
    }

    return m_counter;
}

template<class Config>
std::map<std::size_t, bool> CircuitSimulator<Config>::singleShot(const bool ignore_nonunitaries) {
    singleShots++;
    const auto nQubits = qc->getNqubits();

    Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(static_cast<dd::QubitCount>(nQubits));
    Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);

    std::size_t                 opNum = 0;
    std::map<std::size_t, bool> classicValues;

    const auto approxMod = static_cast<std::size_t>(std::ceil(static_cast<double>(qc->getNops()) / (approximationInfo.stepNumber + 1)));

    for (auto& op: *qc) {
        if (op->isNonUnitaryOperation()) {
            if (ignore_nonunitaries) {
                continue;
            }
            if (auto* nonUnitaryOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (op->getType() == qc::Measure) {
                    auto quantum = nonUnitaryOp->getTargets();
                    auto classic = nonUnitaryOp->getClassics();

                    assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                    for (std::size_t i = 0; i < quantum.size(); ++i) {
                        auto result = Simulator<Config>::MeasureOneCollapsing(quantum.at(i));
                        assert(result == '0' || result == '1');
                        classicValues[classic.at(i)] = (result == '1');
                    }
                } else if (op->getType() == qc::Barrier) {
                    continue;
                } else {
                    throw std::runtime_error("Unsupported non-unitary functionality.");
                }
            } else {
                throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
            }
            Simulator<Config>::dd->garbageCollect();
        } else {
            if (op->isClassicControlledOperation()) {
                if (auto* classicallyControlledOp = dynamic_cast<qc::ClassicControlledOperation*>(op.get())) {
                    const auto   startIndex     = static_cast<unsigned short>(classicallyControlledOp->getParameter().at(0));
                    const auto   length         = static_cast<unsigned short>(classicallyControlledOp->getParameter().at(1));
                    const auto   expectedValue  = classicallyControlledOp->getExpectedValue();
                    unsigned int actualValue    = 0;
                    for (std::size_t i = 0; i < length; i++) {
                        actualValue |= (classicValues[startIndex + i] ? 1u : 0u) << i;
                    }

                    //std::clog << "expected " << expected_value << " and actual value was " << actual_value << "\n";

                    if (actualValue != expectedValue) {
                        continue;
                    }
                } else {
                    throw std::runtime_error("Dynamic cast to ClassicControlledOperation failed.");
                }
            }
            /*std::clog << "[INFO] op " << op_num << " is " << op->getName() << " on " << +op->getTargets().at(0)
                      << " #controls=" << op->getControls().size()
                      << " statesize=" << dd->size(rootEdge) << "\n";//*/

            auto ddOp  = dd::getDD(op.get(), Simulator<Config>::dd);
            auto tmp   = Simulator<Config>::dd->multiply(ddOp, Simulator<Config>::rootEdge);
            Simulator<Config>::dd->incRef(tmp);
            Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
            Simulator<Config>::rootEdge = tmp;

            if (approximationInfo.stepNumber > 0 && approximationInfo.stepFidelity < 1.0) {
                if (approximationInfo.approxWhen == ApproximationInfo::FidelityDriven && (opNum + 1) % approxMod == 0 &&
                    approximationRuns < approximationInfo.stepNumber) {
                    [[maybe_unused]] const unsigned int size_before = Simulator<Config>::dd->size(Simulator<Config>::rootEdge);
                    const auto                          apFid       = Simulator<Config>::ApproximateByFidelity(approximationInfo.stepFidelity, false, true);
                    approximationRuns++;
                    finalFidelity *= static_cast<long double>(apFid);
                    /*std::clog << "[INFO] Fidelity-driven ApproximationInfo run finished. "
                              << "op_num=" << op_num
                              << "; previous size=" << size_before
                              << "; attained fidelity=" << ap_fid
                              << "; global fidelity=" << final_fidelity
                              << "; #runs=" << approximation_runs
                              << "\n";//*/
                } else if (approximationInfo.approxWhen == ApproximationInfo::MemoryDriven) {
                    [[maybe_unused]] const unsigned int size_before = Simulator<Config>::dd->size(Simulator<Config>::rootEdge);
                    if (Simulator<Config>::dd->template getUniqueTable<dd::vNode>().possiblyNeedsCollection()) {
                        const auto apFid = Simulator<Config>::ApproximateByFidelity(approximationInfo.stepFidelity, false, true);
                        approximationRuns++;
                        finalFidelity *= static_cast<long double>(apFid);
                        /*std::clog << "[INFO] Memory-driven ApproximationInfo run finished. "
                                  << "; previous size=" << size_before
                                  << "; attained fidelity=" << ap_fid
                                  << "; global fidelity=" << final_fidelity
                                  << "; #runs=" << approximation_runs
                                  << "\n";//*/
                    }
                }
            }
            Simulator<Config>::dd->garbageCollect();
        }
        opNum++;
    }
    return classicValues;
}

template class CircuitSimulator<dd::DDPackageConfig>;
