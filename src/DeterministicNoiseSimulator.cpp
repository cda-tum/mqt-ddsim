#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"
#include "dd/Operations.hpp"

using CN = dd::ComplexNumbers;

template<class Config>
void DeterministicNoiseSimulator<Config>::initializeSimulation(const std::size_t nQubits) {
    DeterministicNoiseSimulator<Config>::rootEdge = DeterministicNoiseSimulator<Config>::dd->makeZeroDensityOperator(static_cast<dd::Qubit>(nQubits));
    Simulator<Config>::dd->incRef(DeterministicNoiseSimulator<Config>::rootEdge);
}

template<class Config>
void DeterministicNoiseSimulator<Config>::applyOperationToState(std::unique_ptr<qc::Operation>& op) {
    auto operation = dd::getDD(op.get(), *Simulator<Config>::dd);
    Simulator<Config>::dd->applyOperationToDensity(DeterministicNoiseSimulator<Config>::rootEdge, operation);
    deterministicNoiseFunctionalityObject.applyNoiseEffects(DeterministicNoiseSimulator<Config>::rootEdge, op);
}

template<class Config>
char DeterministicNoiseSimulator<Config>::measure(unsigned int i) {
    return Simulator<Config>::dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(i), Simulator<Config>::mt);
}

template<class Config>
void DeterministicNoiseSimulator<Config>::reset(qc::NonUnitaryOperation* nonUnitaryOp) {
    auto qubits = nonUnitaryOp->getTargets();
    for (const auto& qubit: qubits) {
        auto const result = measure(static_cast<dd::Qubit>(qubits.at(qubit)));
        if (result == '1') {
            const auto x         = qc::StandardOperation(CircuitSimulator<Config>::qc->getNqubits(), qubit, qc::X);
            const auto operation = dd::getDD(&x, *Simulator<Config>::dd);
            rootEdge             = Simulator<Config>::dd->applyOperationToDensity(rootEdge, operation);
        }
    }
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

template class DeterministicNoiseSimulator<dd::DensityMatrixSimulatorDDPackageConfig>;
