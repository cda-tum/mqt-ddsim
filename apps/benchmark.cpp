#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "Simulator.hpp"

#include <algorithm>
#include <benchmark/benchmark.h>

auto min_estimator = [](const std::vector<double>& v) -> double {
    return *(std::min_element(std::begin(v), std::end(v)));
};

static void BM_sim_X(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::X);
    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        sim.Simulate(1);
    }
    state.SetLabel("X");
}

BENCHMARK(BM_sim_X)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator);

static void BM_sim_H(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::H);
    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        sim.Simulate(1);
    }
    state.SetLabel("H");
}

BENCHMARK(BM_sim_H)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator);

static void BM_sim_T(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::T);
    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        sim.Simulate(1);
    }
    state.SetLabel("T");
}

BENCHMARK(BM_sim_T)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator);

static void BM_sim_CNOT(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), dd::Control{0}, 1, qc::X);
    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        sim.Simulate(1);
    }
    state.SetLabel("CNOT");
}

BENCHMARK(BM_sim_CNOT)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator);

static void BM_sim_TOFFOLI(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    dd::Controls                            controls{dd::Control{0}, dd::Control{1}};
    qc->emplace_back<qc::StandardOperation>(state.range(0), controls, 2, qc::X);
    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        sim.Simulate(1);
    }
    state.SetLabel("Toffoli");
}

BENCHMARK(BM_sim_TOFFOLI)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator);

/**
 * The following is translated from the other benchmarks at Roger-luo/quantum-benchmarks and does the following:
 * 1. Rotate a bit
 * 2. Entangle all qubits
 * 3a. Rotate a bit more
 * 3b. Entangle all qubits and repeat 3 a+b until we have entangled the qubits `depth` times)
 * 4. One last rotation part
 * 5. Then simulate and measure time
 *
 * @param state contains number of qubits
 */
static void BM_sim_QCBM(benchmark::State& state) {
    const unsigned int                      n_qubits = state.range(0);
    const unsigned int                      depth    = 9;
    std::unique_ptr<qc::QuantumComputation> qc       = std::make_unique<qc::QuantumComputation>(n_qubits);
    // first rotation
    for (unsigned int i = 0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
    }

    // entangling
    for (unsigned int i = 0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, dd::Control{static_cast<dd::Qubit>(i)}, (i + 1) % n_qubits,
                                                qc::X);
    }
    // middle part: rotations and entanglement
    for (unsigned int d = 0; d < depth - 1; d++) {
        // mid rotation
        for (unsigned int i = 0; i < n_qubits; i++) {
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        }
        // entangling
        for (unsigned int i = 0; i < n_qubits; i++) {
            qc->emplace_back<qc::StandardOperation>(n_qubits, dd::Control{static_cast<dd::Qubit>(i)},
                                                    (i + 1) % n_qubits, qc::X);
        }
    }
    // last rotation
    for (unsigned int i = 0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
    }
    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        sim.Simulate(1);
    }
    state.SetLabel("QCBM");
}

BENCHMARK(BM_sim_QCBM)->DenseRange(4, 21)->ComputeStatistics("min",
                                                             min_estimator); // on our compute server, the 22 qubit instance takes more than 48h

static void BM_extra_QCBM_optimized(benchmark::State& state) {
    const unsigned int           n_qubits = state.range(0);
    const unsigned int           depth    = 9;
    std::unique_ptr<dd::Package> dd       = std::make_unique<dd::Package>();

    std::unique_ptr<qc::QuantumComputation> qc_pre = std::make_unique<qc::QuantumComputation>(n_qubits);
    // first rotation
    for (unsigned int i = 0; i < n_qubits; i++) {
        qc_pre->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
        qc_pre->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
    }
    dd::Edge pre{qc_pre->buildFunctionality(dd)};
    dd->incRef(pre);
    std::cout << "pre=" << dd->size(pre) << "  ";

    // entangling
    std::unique_ptr<qc::QuantumComputation> qc_entangle = std::make_unique<qc::QuantumComputation>(n_qubits);
    for (unsigned int i = 0; i < n_qubits; i++) {
        qc_entangle->emplace_back<qc::StandardOperation>(n_qubits, dd::Control{static_cast<dd::Qubit>(i)},
                                                         (i + 1) % n_qubits, qc::X);
    }
    dd::Edge entangle{qc_entangle->buildFunctionality(dd)};
    dd->incRef(entangle);
    std::cout << "entangle=" << dd->size(entangle) << "  ";

    // mid rotation
    std::unique_ptr<qc::QuantumComputation> qc_middle = std::make_unique<qc::QuantumComputation>(n_qubits);
    for (unsigned int i = 0; i < n_qubits; i++) {
        qc_middle->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        qc_middle->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
        qc_middle->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
    }
    dd::Edge middle{qc_middle->buildFunctionality(dd)};
    dd->incRef(middle);
    std::cout << "middle=" << dd->size(middle) << "  ";

    // last rotation
    std::unique_ptr<qc::QuantumComputation> qc_post = std::make_unique<qc::QuantumComputation>(n_qubits);
    for (unsigned int i = 0; i < n_qubits; i++) {
        qc_post->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        qc_post->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
    }
    dd::Edge post{qc_post->buildFunctionality(dd)};
    dd->incRef(post);
    std::cout << "post=" << dd->size(post) << "\n";

    for (auto _: state) {
        dd::Edge qstate = dd->makeZeroState(n_qubits);
        qstate          = dd->multiply(pre, qstate);
        qstate          = dd->multiply(entangle, qstate);

        for (unsigned int d = 0; d < depth - 1; d++) {
            qstate = dd->multiply(middle, qstate);
            qstate = dd->multiply(entangle, qstate);
            dd->garbageCollect();
        }
        qstate = dd->multiply(post, qstate);
        dd->garbageCollect();
    }
    state.SetLabel("QCBM preprocessed");
}

BENCHMARK(BM_extra_QCBM_optimized)->DenseRange(4, 21)->ComputeStatistics("min", min_estimator);