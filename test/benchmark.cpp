#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "dd/FunctionalityConstruction.hpp"

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

using namespace qc::literals;

const auto MIN_ESTIMATOR = [](const std::vector<double>& v) -> double {
    return *(std::min_element(std::begin(v), std::end(v)));
};

static void yardstiqSimX(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0, qc::X);

    CircuitSimulator sim(std::move(qc));
    for ([[maybe_unused]] auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsedSeconds.count());
    }
    state.SetLabel("X");
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-owning-memory)
BENCHMARK(yardstiqSimX)->DenseRange(4, 25)->ComputeStatistics("min", MIN_ESTIMATOR)->UseManualTime();

static void yardstiqSimH(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->h(0);

    CircuitSimulator sim(std::move(qc));
    for ([[maybe_unused]] auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsedSeconds.count());
    }
    state.SetLabel("H");
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-owning-memory)
BENCHMARK(yardstiqSimH)->DenseRange(4, 25)->ComputeStatistics("min", MIN_ESTIMATOR)->UseManualTime();

static void yardstiqSimT(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->t(0);

    CircuitSimulator sim(std::move(qc));
    for ([[maybe_unused]] auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsedSeconds.count());
    }
    state.SetLabel("T");
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-owning-memory)
BENCHMARK(yardstiqSimT)->DenseRange(4, 25)->ComputeStatistics("min", MIN_ESTIMATOR)->UseManualTime();

static void yardstiqSimCnot(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), 0_pc, 1, qc::X);

    CircuitSimulator sim(std::move(qc));
    for ([[maybe_unused]] auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsedSeconds.count());
    }
    state.SetLabel("CNOT");
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-owning-memory)
BENCHMARK(yardstiqSimCnot)->DenseRange(4, 25)->ComputeStatistics("min", MIN_ESTIMATOR)->UseManualTime();

static void yardstiqSimToffoli(benchmark::State& state) {
    auto qc = std::make_unique<qc::QuantumComputation>(state.range(0));
    qc->emplace_back<qc::StandardOperation>(state.range(0), qc::Controls{0_pc, 1_pc}, 2, qc::X);
    qc->x(2, qc::Controls{0_pc, 1_pc});

    CircuitSimulator sim(std::move(qc));
    for ([[maybe_unused]] auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsedSeconds.count());
    }
    state.SetLabel("Toffoli");
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-owning-memory)
BENCHMARK(yardstiqSimToffoli)->DenseRange(4, 25)->ComputeStatistics("min", MIN_ESTIMATOR)->UseManualTime();

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
static void yardstiqSimQcbm(benchmark::State& state) {
    const auto        nQubits = static_cast<dd::QubitCount>(state.range(0));
    const std::size_t depth   = 9;
    auto              qc      = std::make_unique<qc::QuantumComputation>(nQubits);
    // first rotation
    for (qc::Qubit i = 0; i < nQubits; i++) {
        qc->rx(i, 1.0);
        qc->rz(i, 1.0);
    }

    // entangling
    for (qc::Qubit i = 0; i < nQubits; i++) {
        qc->emplace_back<qc::StandardOperation>(nQubits, qc::Control{i}, (i + 1) % nQubits, qc::X);
    }
    // middle part: rotations and entanglement
    for (std::size_t d = 0; d < depth - 1; d++) {
        // mid-rotation
        for (qc::Qubit i = 0; i < nQubits; i++) {
            qc->rz(i, 1.0);
            qc->rx(i, 1.0);
            qc->rz(i, 1.0);
        }
        // entangling
        for (qc::Qubit i = 0; i < nQubits; i++) {
            qc->x((i + 1) % nQubits, qc::Control{i});
        }
    }
    // last rotation
    for (qc::Qubit i = 0; i < nQubits; i++) {
        qc->rz(i, 1.0);
        qc->rx(i, 1.0);
    }
    CircuitSimulator sim(std::move(qc));
    for ([[maybe_unused]] auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsedSeconds.count());
    }
    state.SetLabel("QCBM");
}

// on our compute server, the 22 qubit instance takes more than 48h
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-owning-memory)
BENCHMARK(yardstiqSimQcbm)->DenseRange(4, 18)->ComputeStatistics("min", MIN_ESTIMATOR)->UseManualTime();

static void bmExtraQcbmOptimized(benchmark::State& state) {
    const auto         nQubits = static_cast<dd::QubitCount>(state.range(0));
    const unsigned int depth   = 9;
    auto               dd      = std::make_unique<dd::Package<>>();

    std::unique_ptr<qc::QuantumComputation> qcPre = std::make_unique<qc::QuantumComputation>(nQubits);
    // first rotation
    for (unsigned int i = 0; i < nQubits; i++) {
        qcPre->rx(i, 1.0);
        qcPre->rz(i, 1.0);
    }

    const dd::Edge pre{dd::buildFunctionality(qcPre.get(), dd)};
    dd->incRef(pre);
    std::cout << "pre=" << dd->size(pre) << "  ";

    // entangling
    std::unique_ptr<qc::QuantumComputation> qcEntangle = std::make_unique<qc::QuantumComputation>(nQubits);
    for (unsigned int i = 0; i < nQubits; i++) {
        qcEntangle->x((i + 1) % nQubits, qc::Control{i});
    }

    const dd::Edge entangle{dd::buildFunctionality(qcEntangle.get(), dd)};
    dd->incRef(entangle);
    std::cout << "entangle=" << dd->size(entangle) << "  ";

    // mid-rotation
    std::unique_ptr<qc::QuantumComputation> qcMiddle = std::make_unique<qc::QuantumComputation>(nQubits);
    for (unsigned int i = 0; i < nQubits; i++) {
        qcMiddle->rz(i, 1.0);
        qcMiddle->rx(i, 1.0);
        qcMiddle->rz(i, 1.0);
    }
    const dd::Edge middle{dd::buildFunctionality(qcMiddle.get(), dd)};
    dd->incRef(middle);
    std::cout << "middle=" << dd->size(middle) << "  ";

    // last rotation
    std::unique_ptr<qc::QuantumComputation> qcPost = std::make_unique<qc::QuantumComputation>(nQubits);
    for (unsigned int i = 0; i < nQubits; i++) {
        qcPost->rz(i, 1.0);
        qcPost->rx(i, 1.0);
    }
    const dd::Edge post{dd::buildFunctionality(qcPost.get(), dd)};

    dd->incRef(post);
    std::cout << "post=" << dd->size(post) << "\n";

    for ([[maybe_unused]] auto _: state) {
        dd::Edge qstate = dd->makeZeroState(nQubits);
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

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-owning-memory)
BENCHMARK(bmExtraQcbmOptimized)->DenseRange(4, 18)->ComputeStatistics("min", MIN_ESTIMATOR);

static void bmExtraInst4x4100(benchmark::State& state) {
    auto             qc = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");
    CircuitSimulator sim(std::move(qc));
    for ([[maybe_unused]] auto _: state) {
        const auto start = std::chrono::steady_clock::now();
        sim.simulate(1);
        const auto end = std::chrono::steady_clock::now();
        sim.dd->reset();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsedSeconds.count());
    }
    state.SetLabel("inst4x4-10-0");
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-owning-memory)
BENCHMARK(bmExtraInst4x4100)->ComputeStatistics("min", MIN_ESTIMATOR);
