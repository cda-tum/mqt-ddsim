#include <QuantumComputation.hpp>
#include <Simulator.hpp>
#include <QFRSimulator.hpp>

#include <benchmark/benchmark.h>

#include <algorithm>

static void BM_sim_X(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::X);
    QFRSimulator sim{qc};
    for(auto _ : state) {
        sim.Simulate();
    }
    state.SetLabel("X");
}
BENCHMARK(BM_sim_X)->DenseRange(4, 25);

static void BM_sim_H(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::H);
    QFRSimulator sim{qc};
    for(auto _ : state) {
        sim.Simulate();
    }
    state.SetLabel("H");
}
BENCHMARK(BM_sim_H)->DenseRange(4, 25);

static void BM_sim_T(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::T);
    QFRSimulator sim{qc};
    for(auto _ : state) {
        sim.Simulate();
    }
    state.SetLabel("T");
}
BENCHMARK(BM_sim_T)->DenseRange(4, 25);

static void BM_sim_CNOT(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), qc::Control{0}, 1, qc::X);
    QFRSimulator sim{qc};
    for(auto _ : state) {
        sim.Simulate();
    }
    state.SetLabel("CNOT");
}
BENCHMARK(BM_sim_CNOT)->DenseRange(4, 25);

static void BM_sim_TOFFOLI(benchmark::State& state) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    std::vector<qc::Control> controls{qc::Control(0), qc::Control(1)};
    qc->emplace_back<qc::StandardOperation>(state.range(0), controls, 2, qc::X);
    QFRSimulator sim{qc};
    for(auto _ : state) {
        sim.Simulate();
    }
    state.SetLabel("TOFFOLI");
}
BENCHMARK(BM_sim_TOFFOLI)->DenseRange(4, 25);

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
    const unsigned int n_qubits = state.range(0);
    const unsigned int depth = 9;
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(n_qubits);
    // first rotation
    for (int i=0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
    }

    // entangling
    for (int i=0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, qc::Control(i), (i+1)%n_qubits, qc::X);
    }
    // middle part: rotations and entanglement
    for (int d=0; d < depth-1; d++) {
        // mid rotation
        for (int i=0; i < n_qubits; i++) {
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        }
        // entangling
        for (int i=0; i < n_qubits; i++) {
            qc->emplace_back<qc::StandardOperation>(n_qubits, qc::Control(i), (i+1)%n_qubits, qc::X);
        }
    }
    // last rotation
    for (int i=0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
    }
    QFRSimulator sim{qc};
    for(auto _ : state) {
        sim.Simulate();
    }
    state.SetLabel("QCBM");
}

static void BM_extra_QCBM_optimized(benchmark::State& state) {
    const unsigned int n_qubits = state.range(0);
    const unsigned int depth = 9;
    std::unique_ptr<dd::Package> dd = std::make_unique<dd::Package>();


    std::unique_ptr<qc::QuantumComputation> qc_pre = std::make_unique<qc::QuantumComputation>(n_qubits);
    // first rotation
    for (int i=0; i < n_qubits; i++) {
        qc_pre->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
        qc_pre->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
    }


    // entangling
    for (int i=0; i < n_qubits; i++) {
        qc_pre->emplace_back<qc::StandardOperation>(n_qubits, qc::Control(i), (i+1)%n_qubits, qc::X);
    }
    dd::Edge pre{qc_pre->buildFunctionality(dd)};
    dd->incRef(pre);

    // middle part: rotations and entanglement

    // mid rotation
    std::unique_ptr<qc::QuantumComputation> qc_middle = std::make_unique<qc::QuantumComputation>(n_qubits);

    for (int i=0; i < n_qubits; i++) {
        qc_middle->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        qc_middle->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
        qc_middle->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
    }
    // entangling
    for (int i=0; i < n_qubits; i++) {
        qc_middle->emplace_back<qc::StandardOperation>(n_qubits, qc::Control(i), (i+1)%n_qubits, qc::X);
    }
    dd::Edge middle{qc_middle->buildFunctionality(dd)};
    dd->incRef(middle);

    // last rotation
    std::unique_ptr<qc::QuantumComputation> qc_post = std::make_unique<qc::QuantumComputation>(n_qubits);

    for (int i=0; i < n_qubits; i++) {
        qc_post->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        qc_post->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
    }
    dd::Edge post{qc_post->buildFunctionality(dd)};
    dd->incRef(post);

    for(auto _ : state) {
        dd::Edge qstate = dd->makeZeroState(n_qubits);
        qstate = dd->multiply(pre, qstate);

        for (int d=0; d < depth-1; d++) {
            qstate = dd->multiply(middle, qstate);
            dd->garbageCollect();
        }
        qstate = dd->multiply(post, qstate);
        benchmark::DoNotOptimize(qstate);
        dd->garbageCollect();
    }
}

BENCHMARK(BM_extra_QCBM_optimized)->DenseRange(4, 25);