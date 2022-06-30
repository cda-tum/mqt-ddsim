#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN    = dd::ComplexNumbers;
using dEdge = dd::dEdge;
using dNode = dd::dNode;
using mEdge = dd::mEdge;

template<class DDPackage>
std::map<std::string, double> DeterministicNoiseSimulator<DDPackage>::DeterministicSimulate() {
    std::map<unsigned int, bool> classicValues;

    rootEdge = Simulator<DDPackage>::dd->makeZeroDensityOperator(this->dd->qubits());
    Simulator<DDPackage>::dd->incRef(rootEdge);

    auto deterministicNoiseFunctionality = dd::DeterministicNoiseFunctionality<DensityMatrixPackage>(
            Simulator<DDPackage>::dd,
            qc->getNqubits(),
            noiseProbSingleQubit,
            noiseProbMultiQubit,
            ampDampingProbSingleQubit,
            ampDampingProbMultiQubit,
            noiseEffects,
            useDensityMatrixType,
            sequentiallyApplyNoise);

    for (auto const& op: *qc) {
        Simulator<DDPackage>::dd->garbageCollect();
        if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
            if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                //Skipping barrier
                if (op->getType() == qc::Barrier) {
                    continue;
                }
                throw std::runtime_error(std::string{"Unsupported non-unitary functionality: \""} + nuOp->getName() + "\"");
            } else {
                throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
            }
        } else {
            if (op->isClassicControlledOperation()) {
                throw std::runtime_error("Classical controlled operations are not supported.");
            }
            auto operation = dd::getDD(op.get(), Simulator<DDPackage>::dd);

            // Applying the operation to the density matrix
            Simulator<DDPackage>::dd->applyOperationToDensity(rootEdge, operation, useDensityMatrixType);

            deterministicNoiseFunctionality.applyNoiseEffects(rootEdge, op);
        }
    }
    return Simulator<DDPackage>::dd->getProbVectorFromDensityMatrix(rootEdge, measurementThreshold);
}

template<class DDPackage>
std::map<std::string, std::size_t> DeterministicNoiseSimulator<DDPackage>::sampleFromProbabilityMap(const std::map<std::string, dd::fp>& resultProbabilityMap, unsigned int shots) {
    // Create probability distribution from measure probabilities
    std::vector<dd::fp> weights;
    for (auto& res: resultProbabilityMap) {
        weights.push_back(res.second);
    }
    std::discrete_distribution<> d(weights.begin(), weights.end());

    //Sample n shots elements from the prob distribution
    std::map<std::size_t, std::size_t>     results;
    std::uniform_real_distribution<dd::fp> dist(0.0L, 1.0L);
    for (size_t n = 0; n < shots; ++n) {
        ++results[d(this->mt)];
    }

    // Create the final map containing the measurement results and the corresponding shots
    std::map<std::string, std::size_t> resultShotsMap;
    for (auto& res: results) {
        resultShotsMap.insert({std::next(resultProbabilityMap.begin(), static_cast<long>(res.first))->first, res.second});
    }

    return resultShotsMap;
}

template class DeterministicNoiseSimulator<DensityMatrixPackage>;
