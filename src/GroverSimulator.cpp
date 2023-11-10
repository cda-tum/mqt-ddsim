#include "GroverSimulator.hpp"

#include "QuantumComputation.hpp"
#include "dd/FunctionalityConstruction.hpp"

#include <chrono>

template<class Config>
std::map<std::string, std::size_t> GroverSimulator<Config>::simulate(std::size_t shots) {
    // Setup X on the last, Hadamard on all qubits
    qc::QuantumComputation qcSetup(nQubits + nAnciallae);
    qcSetup.x(nQubits);
    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qcSetup.h(i);
    }

    const dd::Edge setupOp{dd::buildFunctionality(&qcSetup, Simulator<Config>::dd)};

    // Build the oracle
    qc::QuantumComputation qcOracle(nQubits + nAnciallae);
    qc::Controls           controls{};
    for (qc::Qubit i = 0; i < nQubits; i++) {
        controls.emplace(i, oracle.at(i) == '1' ? qc::Control::Type::Pos : qc::Control::Type::Neg);
    }
    qcOracle.mcz(controls, nQubits);

    const dd::Edge oracleOp{dd::buildFunctionality(&qcOracle, Simulator<Config>::dd)};

    // Build the diffusion stage.
    qc::QuantumComputation qcDiffusion(nQubits + nAnciallae);

    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qcDiffusion.h(i);
    }
    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qcDiffusion.x(i);
    }

    qcDiffusion.h(nQubits - 1);

    qc::Controls diffControls{};
    for (qc::Qubit j = 0; j < nQubits - 1; ++j) {
        diffControls.emplace(j);
    }
    qcDiffusion.mcx(diffControls, nQubits - 1);

    qcDiffusion.h(nQubits - 1);

    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qcDiffusion.x(i);
    }
    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qcDiffusion.h(i);
    }

    const dd::Edge diffusionOp{dd::buildFunctionality(&qcDiffusion, Simulator<Config>::dd)};

    const dd::Edge fullIteration{Simulator<Config>::dd->multiply(oracleOp, diffusionOp)};
    Simulator<Config>::dd->incRef(fullIteration);

    assert(nQubits + nAnciallae <= std::numeric_limits<dd::Qubit>::max());
    Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(static_cast<dd::Qubit>(nQubits + nAnciallae));
    Simulator<Config>::rootEdge = Simulator<Config>::dd->multiply(setupOp, Simulator<Config>::rootEdge);
    Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);

    std::size_t jPre = 0;

    while ((iterations - jPre) % 8 != 0) {
        //std::clog << "[INFO]  Pre-Iteration " << j_pre+1 << " of " << iterations%8 << " -- size:" << dd->size(rootEdge)  << "\n";
        auto tmp = Simulator<Config>::dd->multiply(fullIteration, Simulator<Config>::rootEdge);
        Simulator<Config>::dd->incRef(tmp);
        Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
        Simulator<Config>::rootEdge = tmp;
        Simulator<Config>::dd->garbageCollect();
        jPre++;
    }

    for (std::size_t j = jPre; j < iterations; j += 8) {
        //std::clog << "[INFO]  Iteration " << j+1 << " of " << iterations << " -- size:" << dd->size(rootEdge)  << "\n";
        auto tmp = Simulator<Config>::dd->multiply(fullIteration, Simulator<Config>::rootEdge);
        for (std::size_t i = 0; i < 7; ++i) {
            tmp = Simulator<Config>::dd->multiply(fullIteration, tmp);
        }
        Simulator<Config>::dd->incRef(tmp);
        Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
        Simulator<Config>::rootEdge = tmp;
        Simulator<Config>::dd->garbageCollect();
    }

    return Simulator<Config>::measureAllNonCollapsing(shots);
}

template class GroverSimulator<dd::DDPackageConfig>;
