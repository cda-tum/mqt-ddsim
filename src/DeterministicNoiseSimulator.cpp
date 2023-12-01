#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN = dd::ComplexNumbers;

template<class Config>
std::map<std::string, std::size_t> DeterministicNoiseSimulator<Config>::simulate(std::size_t shots) {
    bool                               hasNonmeasurementNonUnitary = false;
    bool                               hasMeasurements             = false;
    bool                               measurementsLast            = true;
    std::map<std::size_t, std::size_t> measurementMap;

    std::tie(hasNonmeasurementNonUnitary, hasMeasurements, measurementsLast, measurementMap) = DeterministicNoiseSimulator<Config>::analyseCircuit();

    // easiest case: all gates are unitary --> simulate once and sample away on all qubits
    if (!hasNonmeasurementNonUnitary && !hasMeasurements) {
        deterministicSimulate(false);
        return DeterministicNoiseSimulator<Config>::measureAllNonCollapsing2(shots);
    }

    // single shot is enough, but the sampling should only return actually measured qubits
    if (!hasNonmeasurementNonUnitary && measurementsLast) {
        deterministicSimulate(true);
        std::map<std::string, std::size_t> measurementCounter;
        const auto                         qubits = qc->getNqubits();
        const auto                         cbits  = qc->getNcbits();

        // MeasureAllNonCollapsing returns a map from measurement over all qubits to the number of occurrences
        for (const auto& [bit_string, count]: DeterministicNoiseSimulator<Config>::measureAllNonCollapsing2(shots)) {
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
        const auto result = deterministicSimulate(false);
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
std::tuple<bool, bool, bool, std::map<std::size_t, std::size_t>> DeterministicNoiseSimulator<Config>::analyseCircuit() {
    bool                               hasNonmeasurementNonUnitary = false;
    bool                               hasMeasurements             = false;
    bool                               measurementsLast            = true;
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
    return std::tuple<bool, bool, bool, std::map<std::size_t, std::size_t>>{hasNonmeasurementNonUnitary, hasMeasurements, measurementsLast, measurementMap};
}

template<class Config>
std::map<std::size_t, bool> DeterministicNoiseSimulator<Config>::deterministicSimulate(bool ignoreNonUnitaries) {
    rootEdge = Simulator<Config>::dd->makeZeroDensityOperator(static_cast<dd::Qubit>(qc->getNqubits()));
    Simulator<Config>::dd->incRef(rootEdge);
    std::map<std::size_t, bool> classicValues;

    auto deterministicNoiseFunctionality = dd::DeterministicNoiseFunctionality<Config>(
            Simulator<Config>::dd,
            static_cast<dd::Qubit>(qc->getNqubits()),
            noiseProbSingleQubit,
            noiseProbMultiQubit,
            ampDampingProbSingleQubit,
            ampDampingProbMultiQubit,
            noiseEffects);

    for (auto const& op: *qc) {
        Simulator<Config>::dd->garbageCollect();
        if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
            if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (ignoreNonUnitaries) {
                    continue;
                }
                if (nuOp->getType() == qc::Measure) {
                    auto quantum = nuOp->getTargets();
                    auto classic = nuOp->getClassics();

                    assert(quantum.size() == classic.size()); // this should not happen do to check in Simulate

                    for (std::size_t i = 0U; i < quantum.size(); ++i) {
                        char result                = '2';
                        std::tie(rootEdge, result) = Simulator<Config>::dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(quantum.at(i)), Simulator<Config>::mt);
                        assert(result == '0' || result == '1');
                        classicValues[classic.at(i)] = (result == '1');
                    }
                } else if (op->getType() == qc::Barrier) {
                    continue;
                } else if (op->getType() == qc::Reset) {
                    // Reset qubit
                    auto qubits = nuOp->getTargets();
                    for (const auto& qubit: qubits) {
                        char result                = '2';
                        std::tie(rootEdge, result) = Simulator<Config>::dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(qubits.at(qubit)), Simulator<Config>::mt);
                        if (result == '1') {
                            const auto x         = qc::StandardOperation(qc->getNqubits(), qubit, qc::X);
                            const auto operation = dd::getDD(&x, Simulator<Config>::dd);
                            rootEdge             = Simulator<Config>::dd->applyOperationToDensity(rootEdge, operation);
                        }
                    }
                }
            } else {
                throw std::runtime_error(std::string{"Unsupported non-unitary functionality: \""} + nuOp->getName() + "\"");
            }
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
                }
            }

            auto operation = dd::getDD(op.get(), Simulator<Config>::dd);

            // Applying the operation to the density matrix
            Simulator<Config>::dd->applyOperationToDensity(rootEdge, operation);

            deterministicNoiseFunctionality.applyNoiseEffects(rootEdge, op);
        }
    }
    return classicValues;
}

template<class Config>
std::map<std::string, std::size_t> DeterministicNoiseSimulator<Config>::sampleFromProbabilityMap(const dd::SparsePVecStrKeys& resultProbabilityMap, std::size_t shots) {
    std::vector<dd::fp> weights;
    weights.reserve(resultProbabilityMap.size());

    for (const auto& [state, prob]: resultProbabilityMap) {
        weights.emplace_back(prob);
    }
    std::discrete_distribution<std::size_t> d(weights.begin(), weights.end()); // NOLINT(misc-const-correctness) false-positive

    //Sample n shots elements from the prob distribution
    std::map<std::size_t, std::size_t> results; // NOLINT(misc-const-correctness) false-positive
    for (size_t n = 0; n < shots; ++n) {
        ++results[d(Simulator<Config>::mt)];
    }

    // Create the final map containing the measurement results and the corresponding shots
    std::map<std::string, std::size_t> resultShotsMap;

    for (const auto& [state, prob]: results) {
        resultShotsMap.emplace(std::next(resultProbabilityMap.begin(), static_cast<std::int64_t>(state))->first, prob);
    }

    return resultShotsMap;
}

template class DeterministicNoiseSimulator<DensityMatrixSimulatorDDPackageConfig>;
