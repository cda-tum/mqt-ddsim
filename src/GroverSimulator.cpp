#include "GroverSimulator.hpp"
#include "QuantumComputation.hpp"
#include <chrono>

void GroverSimulator::Simulate() {
    // Setup X on the last, Hadamard on all qubits
    qc::QuantumComputation qc_setup(n_qubits+n_anciallae);
    qc_setup.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, n_qubits, qc::X);
    for (unsigned short i = 0; i < n_qubits; ++i) {
        qc_setup.emplace_back<qc::StandardOperation>(n_qubits + n_anciallae, i, qc::H);
    }
    //qc_setup.print();
    dd::Edge setup_op{qc_setup.buildFunctionality(dd)};


    // Build the oracle
    qc::QuantumComputation qc_oracle(n_qubits+n_anciallae);
    std::vector<qc::Control> controls{};
    for (unsigned short i = 0; i < n_qubits; ++i) {
        controls.emplace_back(i, oracle.at(i) == '1' ? qc::Control::pos : qc::Control::neg);
    }
    qc_oracle.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, controls, n_qubits, qc::Z);
    //qc_oracle.print();
    dd::Edge oracle_op{qc_oracle.buildFunctionality(dd)};

    // Build the diffusion stage.
    qc::QuantumComputation qc_diffusion(n_qubits+n_anciallae);

    for (unsigned short i = 0; i < n_qubits; ++i) {
        qc_diffusion.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, i, qc::H);
    }
    for (unsigned short i = 0; i < n_qubits; ++i) {
        qc_diffusion.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, i, qc::X);
    }

    qc_diffusion.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, n_qubits-1, qc::H);

    std::vector<qc::Control> diff_controls{};
    for (unsigned short j = 0; j < n_qubits-1; ++j) {diff_controls.emplace_back(j);}
    qc_diffusion.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, diff_controls, n_qubits-1);

    qc_diffusion.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, n_qubits-1, qc::H);

    for (unsigned short i = 0; i < n_qubits; ++i) {
        qc_diffusion.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, i, qc::X);
    }
    for (unsigned short i = 0; i < n_qubits; ++i) {
        qc_diffusion.emplace_back<qc::StandardOperation>(n_qubits+n_anciallae, i, qc::H);
    }
    //qc_diffusion.print();
    dd::Edge diffusion_op{qc_diffusion.buildFunctionality(dd)};

    dd::Edge full_iteration{dd->multiply(oracle_op, diffusion_op)};
    dd->incRef(full_iteration);

    root_edge = dd->makeZeroState(n_qubits+n_anciallae);
    root_edge = dd->multiply(setup_op, root_edge);
    dd->incRef(root_edge);

    unsigned int j_pre = 0;

    while((iterations-j_pre) % 8 != 0) {
        //std::clog << "[INFO]  Pre-Iteration " << j_pre+1 << " of " << iterations%8 << " -- size:" << dd->size(root_edge)  << "\n";
        auto tmp = dd->multiply(full_iteration, root_edge);
        dd->incRef(tmp);
        dd->decRef(root_edge);
        root_edge = tmp;
        dd->garbageCollect();
        j_pre++;
    }

    for (unsigned long long j = j_pre; j < iterations; j+=8) {
        //std::clog << "[INFO]  Iteration " << j+1 << " of " << iterations << " -- size:" << dd->size(root_edge)  << "\n";
        auto tmp = dd->multiply(full_iteration, root_edge);
        tmp = dd->multiply(full_iteration, tmp);
        tmp = dd->multiply(full_iteration, tmp);
        tmp = dd->multiply(full_iteration, tmp);
        tmp = dd->multiply(full_iteration, tmp);
        tmp = dd->multiply(full_iteration, tmp);
        tmp = dd->multiply(full_iteration, tmp);
        tmp = dd->multiply(full_iteration, tmp);
        dd->incRef(tmp);
        dd->decRef(root_edge);
        root_edge = tmp;
        dd->garbageCollect();
    }
}
