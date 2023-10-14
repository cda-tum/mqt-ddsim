#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN = dd::ComplexNumbers;

template<class Config>
dd::SparsePVecStrKeys DeterministicNoiseSimulator<Config>::deterministicSimulate() {
    rootEdge = Simulator<Config>::dd->makeZeroDensityOperator(static_cast<dd::Qubit>(qc->getNqubits()));
    Simulator<Config>::dd->incRef(rootEdge);

    auto deterministicNoiseFunctionality = dd::DeterministicNoiseFunctionality<Config>(
            Simulator<Config>::dd,
            static_cast<dd::Qubit>(qc->getNqubits()),
            noiseProbSingleQubit,
            noiseProbMultiQubit,
            ampDampingProbSingleQubit,
            ampDampingProbMultiQubit,
            noiseEffects,
            useDensityMatrixType,
            sequentiallyApplyNoise);

    for (auto const& op: *qc) {
        Simulator<Config>::dd->garbageCollect();
        if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
            if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                //Skipping barrier
                if (op->getType() == qc::Barrier) {
                    continue;
                }
                throw std::runtime_error(std::string{"Unsupported non-unitary functionality: \""} + nuOp->getName() + "\"");
            }
            throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
        }
        if (op->isClassicControlledOperation()) {
            throw std::runtime_error("Classical controlled operations are not supported.");
        }
        auto operation = dd::getDD(op.get(), Simulator<Config>::dd);

        // Applying the operation to the density matrix
        Simulator<Config>::dd->applyOperationToDensity(rootEdge, operation, useDensityMatrixType);

        deterministicNoiseFunctionality.applyNoiseEffects(rootEdge, op);
    }
    return rootEdge.getSparseProbabilityVectorStrKeys(measurementThreshold);
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
