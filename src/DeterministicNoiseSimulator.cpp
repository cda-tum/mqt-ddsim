#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN = dd::ComplexNumbers;

template<class Config>
std::map<std::string, std::size_t> DeterministicNoiseSimulator<Config>::deterministicSimulate(std::size_t shots) {
    rootEdge = Simulator<Config>::dd->makeZeroDensityOperator(static_cast<dd::QubitCount>(qc->getNqubits()));
    Simulator<Config>::dd->incRef(rootEdge);

    auto deterministicNoiseFunctionality = dd::DeterministicNoiseFunctionality<Config>(
            Simulator<Config>::dd,
            static_cast<dd::QubitCount>(qc->getNqubits()),
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
    return measureAllNonCollapsing2(shots);
}

template class DeterministicNoiseSimulator<DensityMatrixSimulatorDDPackageConfig>;
