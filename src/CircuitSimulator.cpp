#include "CircuitSimulator.hpp"

#include "CircuitOptimizer.hpp"
#include "dd/Export.hpp"
#include "dd/FunctionalityConstruction.hpp"
#include "dd/Operations.hpp"

template<class Config>
std::map<std::string, std::size_t> CircuitSimulator<Config>::simulate(std::size_t shots) {
    bool hasNonmeasurementNonUnitary = false;
    bool hasMeasurements             = false;
    bool measurementsLast            = true;

    std::map<std::size_t, std::size_t> measurementMap;

    for (auto& op: *qc) {
        if (op->isClassicControlledOperation() || (op->isNonUnitaryOperation() && op->getType() != qc::Measure && op->getType() != qc::Barrier)) {
            hasNonmeasurementNonUnitary = true;
        }
        if (op->getType() == qc::Measure) {
            auto* nonUnitaryOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get());
            if (nonUnitaryOp == nullptr) {
                throw std::runtime_error("Op with type Measurement could not be casted to NonUnitaryOperation");
            }
            hasMeasurements = true;

            const auto& quantum = nonUnitaryOp->getTargets();
            const auto& classic = nonUnitaryOp->getClassics();

            if (quantum.size() != classic.size()) {
                throw std::runtime_error("Measurement: Sizes of quantum and classic register mismatch.");
            }

            for (unsigned int i = 0; i < quantum.size(); ++i) {
                measurementMap[quantum.at(i)] = classic.at(i);
            }
        }

        if (hasMeasurements && op->isUnitary()) {
            measurementsLast = false;
        }
    }

    // easiest case: all gates are unitary --> simulate once and sample away on all qubits
    if (!hasNonmeasurementNonUnitary && !hasMeasurements) {
        singleShot(false);
        return Simulator<Config>::measureAllNonCollapsing(shots);
    }

    // single shot is enough, but the sampling should only return actually measured qubits
    if (!hasNonmeasurementNonUnitary && measurementsLast) {
        singleShot(true);
        std::map<std::string, std::size_t> measurementCounter;
        const auto                         qubits = qc->getNqubits();
        const auto                         cbits  = qc->getNcbits();

        // MeasureAllNonCollapsing returns a map from measurement over all qubits to the number of occurrences
        for (const auto& [bit_string, count]: Simulator<Config>::measureAllNonCollapsing(shots)) {
            std::string resultString(qc->getNcbits(), '0');

            for (auto const& [qubit_index, bitIndex]: measurementMap) {
                resultString[cbits - bitIndex - 1] = bit_string[qubits - qubit_index - 1];
            }

            measurementCounter[resultString] += count;
        }

        return measurementCounter;
    }

    // there are nonunitaries (or intermediate measurement_map) and we have to actually do multiple single_shots :(
    std::map<std::string, std::size_t> measurementCounter;

    for (unsigned int i = 0; i < shots; i++) {
        const auto result = singleShot(false);
        const auto cbits  = qc->getNcbits();

        std::string resultString(qc->getNcbits(), '0');

        // result is a map from the cbit index to the Boolean value
        for (const auto& [bitIndex, value]: result) {
            resultString[cbits - bitIndex - 1] = value ? '1' : '0';
        }
        measurementCounter[resultString]++;
    }
    return measurementCounter;
}

template<class Config>
dd::fp CircuitSimulator<Config>::expectationValue(const qc::QuantumComputation& observable) {
    // simulate the circuit to get the state vector
    singleShot(true);

    // construct the DD for the observable
    const auto observableDD = dd::buildFunctionality(&observable, Simulator<Config>::dd);

    // calculate the expectation value
    return Simulator<Config>::dd->expectationValue(observableDD, Simulator<Config>::rootEdge);
}

template<class Config>
std::map<std::size_t, bool> CircuitSimulator<Config>::singleShot(const bool ignoreNonUnitaries) {
    singleShots++;
    const auto nQubits = qc->getNqubits();

    Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(static_cast<dd::Qubit>(nQubits));
    Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);

    std::size_t                 opNum = 0;
    std::map<std::size_t, bool> classicValues;

    const auto approxMod = static_cast<std::size_t>(std::ceil(static_cast<double>(qc->getNops()) / (static_cast<double>(approximationInfo.stepNumber + 1))));

    for (auto& op: *qc) {
        if (op->isNonUnitaryOperation()) {
            if (ignoreNonUnitaries) {
                continue;
            }
            if (auto* nonUnitaryOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (op->getType() == qc::Measure) {
                    auto quantum = nonUnitaryOp->getTargets();
                    auto classic = nonUnitaryOp->getClassics();

                    assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                    for (std::size_t i = 0; i < quantum.size(); ++i) {
                        auto result = Simulator<Config>::measureOneCollapsing(quantum.at(i));
                        assert(result == '0' || result == '1');
                        classicValues[classic.at(i)] = (result == '1');
                    }

                } else if (nonUnitaryOp->getType() == qc::Reset) {
                    const auto& qubits = nonUnitaryOp->getTargets();
                    for (const auto& qubit: qubits) {
                        auto bit = Simulator<Config>::dd->measureOneCollapsing(Simulator<Config>::rootEdge, static_cast<dd::Qubit>(qubit), true, Simulator<Config>::mt);
                        // apply an X operation whenever the measured result is one
                        if (bit == '1') {
                            const auto x   = qc::StandardOperation(qc->getNqubits(), qubit, qc::X);
                            auto       tmp = Simulator<Config>::dd->multiply(dd::getDD(&x, Simulator<Config>::dd), Simulator<Config>::rootEdge);
                            Simulator<Config>::dd->incRef(tmp);
                            Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
                            Simulator<Config>::rootEdge = tmp;
                            Simulator<Config>::dd->garbageCollect();
                        }
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
                    const auto   startIndex    = static_cast<std::uint16_t>(classicallyControlledOp->getParameter().at(0));
                    const auto   length        = static_cast<std::uint16_t>(classicallyControlledOp->getParameter().at(1));
                    const auto   expectedValue = classicallyControlledOp->getExpectedValue();
                    unsigned int actualValue   = 0;
                    for (std::size_t i = 0; i < length; i++) {
                        actualValue |= (classicValues[startIndex + i] ? 1U : 0U) << i;
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

            auto ddOp = dd::getDD(op.get(), Simulator<Config>::dd);
            auto tmp  = Simulator<Config>::dd->multiply(ddOp, Simulator<Config>::rootEdge);
            Simulator<Config>::dd->incRef(tmp);
            Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
            Simulator<Config>::rootEdge = tmp;

            if (approximationInfo.stepNumber > 0 && approximationInfo.stepFidelity < 1.0) {
                if (approximationInfo.strategy == ApproximationInfo::FidelityDriven && (opNum + 1) % approxMod == 0 &&
                    approximationRuns < approximationInfo.stepNumber) {
                    [[maybe_unused]] const auto sizeBefore = Simulator<Config>::rootEdge.size();
                    const auto                  apFid      = Simulator<Config>::approximateByFidelity(approximationInfo.stepFidelity, false, true);
                    approximationRuns++;
                    finalFidelity *= static_cast<long double>(apFid);
                    /*std::clog << "[INFO] Fidelity-driven ApproximationInfo run finished. "
                              << "op_num=" << op_num
                              << "; previous size=" << size_before
                              << "; attained fidelity=" << ap_fid
                              << "; global fidelity=" << final_fidelity
                              << "; #runs=" << approximation_runs
                              << "\n";//*/
                } else if (approximationInfo.strategy == ApproximationInfo::MemoryDriven) {
                    [[maybe_unused]] const auto sizeBefore = Simulator<Config>::rootEdge.size();
                    if (Simulator<Config>::dd->template getUniqueTable<dd::vNode>().possiblyNeedsCollection()) {
                        const auto apFid = Simulator<Config>::approximateByFidelity(approximationInfo.stepFidelity, false, true);
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
