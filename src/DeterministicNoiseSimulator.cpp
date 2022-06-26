#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN    = dd::ComplexNumbers;
using dEdge = dd::dEdge;
using dNode = dd::dNode;
using mEdge = dd::mEdge;

dEdge DeterministicNoiseSimulator::makeZeroDensityOperator(dd::QubitCount n) {
    auto f = dEdge::one;
    assert((signed char)n == n);
    for (dd::Qubit p = 0; p < (signed char)n; p++) {
        f = dd->makeDDNode(p, std::array{f, dEdge::zero, dEdge::zero, dEdge::zero});
    }
    return f;
}

std::map<std::string, double> DeterministicNoiseSimulator::DeterministicSimulate() {
    const unsigned short         nQubits = qc->getNqubits();
    std::map<unsigned int, bool> classic_values;

    rootEdge = makeZeroDensityOperator(nQubits);
    dd->incRef(rootEdge);

    auto deterministicNoiseFunctionality = dd::DeterministicNoiseFunctionality<DensityMatrixPackage>(
            dd,
            qc->getNqubits(),
            noiseProbSingleQubit,
            noiseProbMultiQubit,
            ampDampingProbSingleQubit,
            ampDampingProbMultiQubit,
            noiseEffects,
            useDensityMatrixType,
            sequentiallyApplyNoise);

    for (auto const& op: *qc) {
        dd->garbageCollect();
        if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
            if (auto* nu_op = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (op->getType() == qc::Measure) {
                    throw std::invalid_argument("Measurements are currently not supported");
                } else if (op->getType() == qc::Reset) {
                    //                    // Reset qubit
                    throw std::runtime_error(std::string{"Unsupported non-unitary functionality: \""} + nu_op->getName() + "\"");
                } else {
                    //Skipping barrier
                    if (op->getType() == qc::Barrier) {
                        continue;
                    }
                    throw std::runtime_error(std::string{"Unsupported non-unitary functionality: \""} + nu_op->getName() + "\"");
                }
            } else {
                throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
            }
            dd->garbageCollect();
        } else {
            if (op->isClassicControlledOperation()) {
                throw std::runtime_error("Classical controlled operations are not supported.");
            }
            auto operation = dd::getDD(op.get(), dd);

            // Applying the operation to the density matrix
            dd->applyOperationToDensity(rootEdge, operation, useDensityMatrixType);

            deterministicNoiseFunctionality.applyNoiseEffects(rootEdge, op);
        }
    }
    return dd->getProbVectorFromDensityMatrix(rootEdge, measurementThreshold);
}
