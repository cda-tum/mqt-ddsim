#include "GroverSimulator.hpp"

#include "QuantumComputation.hpp"
#include "dd/FunctionalityConstruction.hpp"

#include <chrono>

template<class Config>
std::map<std::string, std::size_t> GroverSimulator<Config>::simulate(std::size_t shots) {
    // Setup X on the last, Hadamard on all qubits
    qc::QuantumComputation qc_setup(nQubits + nAnciallae);
    qc_setup.x(nQubits);
    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qc_setup.h(i);
    }

    dd::Edge setup_op{dd::buildFunctionality(&qc_setup, Simulator<Config>::dd)};

    // Build the oracle
    qc::QuantumComputation qc_oracle(nQubits + nAnciallae);
    qc::Controls           controls{};
    for (qc::Qubit i = 0; i < nQubits; i++) {
        controls.emplace(qc::Control{i, oracle.at(i) == '1' ? qc::Control::Type::Pos : qc::Control::Type::Neg});
    }
    qc_oracle.z(nQubits, controls);

    dd::Edge oracle_op{dd::buildFunctionality(&qc_oracle, Simulator<Config>::dd)};

    // Build the diffusion stage.
    qc::QuantumComputation qc_diffusion(nQubits + nAnciallae);

    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qc_diffusion.h(i);
    }
    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qc_diffusion.x(i);
    }

    qc_diffusion.h(nQubits - 1);

    qc::Controls diff_controls{};
    for (qc::Qubit j = 0; j < nQubits - 1; ++j) {
        diff_controls.emplace(qc::Control{j});
    }
    qc_diffusion.x(nQubits - 1, diff_controls);

    qc_diffusion.h(nQubits - 1);

    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qc_diffusion.x(i);
    }
    for (qc::Qubit i = 0; i < nQubits; ++i) {
        qc_diffusion.h(i);
    }

    dd::Edge diffusion_op{dd::buildFunctionality(&qc_diffusion, Simulator<Config>::dd)};

    dd::Edge full_iteration{Simulator<Config>::dd->multiply(oracle_op, diffusion_op)};
    Simulator<Config>::dd->incRef(full_iteration);

    assert(nQubits + nAnciallae <= std::numeric_limits<dd::QubitCount>::max());
    Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(static_cast<dd::QubitCount>(nQubits + nAnciallae));
    Simulator<Config>::rootEdge = Simulator<Config>::dd->multiply(setup_op, Simulator<Config>::rootEdge);
    Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);

    std::size_t j_pre = 0;

    while ((iterations - j_pre) % 8 != 0) {
        //std::clog << "[INFO]  Pre-Iteration " << j_pre+1 << " of " << iterations%8 << " -- size:" << dd->size(rootEdge)  << "\n";
        auto tmp = Simulator<Config>::dd->multiply(full_iteration, Simulator<Config>::rootEdge);
        Simulator<Config>::dd->incRef(tmp);
        Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
        Simulator<Config>::rootEdge = tmp;
        Simulator<Config>::dd->garbageCollect();
        j_pre++;
    }

    for (std::size_t j = j_pre; j < iterations; j += 8) {
        //std::clog << "[INFO]  Iteration " << j+1 << " of " << iterations << " -- size:" << dd->size(rootEdge)  << "\n";
        auto tmp = Simulator<Config>::dd->multiply(full_iteration, Simulator<Config>::rootEdge);
        for (std::size_t i = 0; i < 7; ++i) {
            tmp = Simulator<Config>::dd->multiply(full_iteration, tmp);
        }
        Simulator<Config>::dd->incRef(tmp);
        Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
        Simulator<Config>::rootEdge = tmp;
        Simulator<Config>::dd->garbageCollect();
    }

    return Simulator<Config>::measureAllNonCollapsing(shots);
}

template class GroverSimulator<dd::DDPackageConfig>;
