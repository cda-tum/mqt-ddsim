#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN    = dd::ComplexNumbers;
using dEdge = dd::dEdge;
using dNode = dd::dNode;
using mEdge = dd::mEdge;

template<class DDPackage>
std::map<std::string, double> DeterministicNoiseSimulator<DDPackage>::DeterministicSimulate() {
    std::map<unsigned int, bool> classicValues;

    rootEdge = Simulator<DDPackage>::dd->makeZeroDensityOperator();
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

template class DeterministicNoiseSimulator<DensityMatrixPackage>;
