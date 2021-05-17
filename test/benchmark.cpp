#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "Simulator.hpp"

// clang format wants to put the following include to the top of the file
// clang-format off
#include "benchmark/benchmark.h"
// clang-format on

#include <algorithm>
#include <chrono>

/**
 * The inner loop of the benchmark repeatedly simulates the quantum circuit. To avoid unfair advantages due to caching,
 * after each simulation the underlying decision diagram package is reset. The time it takes to simulate is measured manually
 * to exclude the time it takes to reset.
 */

using namespace dd::literals;

auto min_estimator = [](const std::vector<double>& v) -> double {
    return *(std::min_element(std::begin(v), std::end(v)));
};

static void yardstiq_sim_X(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::X);

    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.Simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    state.SetLabel("X");
}

BENCHMARK(yardstiq_sim_X)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator)->UseManualTime();

static void yardstiq_sim_H(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::H);

    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.Simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    state.SetLabel("H");
}

BENCHMARK(yardstiq_sim_H)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator)->UseManualTime();

static void yardstiq_sim_T(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::T);

    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.Simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    state.SetLabel("T");
}

BENCHMARK(yardstiq_sim_T)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator)->UseManualTime();

static void yardstiq_sim_CNOT(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0_pc, 1, qc::X);

    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.Simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    state.SetLabel("CNOT");
}

BENCHMARK(yardstiq_sim_CNOT)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator)->UseManualTime();

static void yardstiq_sim_TOFFOLI(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), dd::Controls{0_pc, 1_pc}, 2, qc::X);

    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.Simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    state.SetLabel("Toffoli");
}

BENCHMARK(yardstiq_sim_TOFFOLI)->DenseRange(4, 25)->ComputeStatistics("min", min_estimator)->UseManualTime();

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
static void yardstiq_sim_QCBM(benchmark::State& state) {
    const dd::QubitCount n_qubits = state.range(0);
    const std::size_t    depth    = 9;
    auto                 qc       = std::make_unique<qc::QuantumComputation>(n_qubits);
    // first rotation
    for (dd::Qubit i = 0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
    }

    // entangling
    for (dd::Qubit i = 0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, dd::Control{i}, (i + 1) % n_qubits, qc::X);
    }
    // middle part: rotations and entanglement
    for (std::size_t d = 0; d < depth - 1; d++) {
        // mid rotation
        for (dd::Qubit i = 0; i < n_qubits; i++) {
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
            qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        }
        // entangling
        for (dd::Qubit i = 0; i < n_qubits; i++) {
            qc->emplace_back<qc::StandardOperation>(n_qubits, dd::Control{i}, (i + 1) % n_qubits, qc::X);
        }
    }
    // last rotation
    for (dd::Qubit i = 0; i < n_qubits; i++) {
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RZ, 1.0);
        qc->emplace_back<qc::StandardOperation>(n_qubits, i, qc::RX, 1.0);
    }
    CircuitSimulator sim(std::move(qc));
    for (auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.Simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
    state.SetLabel("QCBM");
}

// on our compute server, the 22 qubit instance takes more than 48h
BENCHMARK(yardstiq_sim_QCBM)->DenseRange(4, 18)->ComputeStatistics("min", min_estimator)->UseManualTime();

static void BM_extra_QCBM_optimized(benchmark::State& state) {
    const unsigned int n_qubits = state.range(0);
    const unsigned int depth    = 9;
    auto               dd       = std::make_unique<dd::Package>();

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
        benchmark::DoNotOptimize(qstate);
        dd->garbageCollect();
    }
    state.SetLabel("QCBM preprocessed");
}

BENCHMARK(BM_extra_QCBM_optimized)->DenseRange(4, 18)->ComputeStatistics("min", min_estimator);