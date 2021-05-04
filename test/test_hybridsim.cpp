#include "HybridSchrodingerFeynmanSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace dd::literals;

TEST(ParallelSimTest, TrivialParallelTest) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{2_pc, 1_pc}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::I); // some dummy operations
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::I);

    HybridSchrodingerFeynmanSimulator ddsim(quantumComputation, HybridSchrodingerFeynmanSimulator::Mode::DD);
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);

    HybridSchrodingerFeynmanSimulator ddsim2(quantumComputation, HybridSchrodingerFeynmanSimulator::Mode::Amplitude);
    auto                              result = ddsim2.Simulate(8192);

    EXPECT_EQ(result.size(), 4);
    auto it = result.find("0000");
    ASSERT_NE(it, result.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = result.find("0010");
    ASSERT_NE(it, result.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = result.find("0100");
    ASSERT_NE(it, result.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = result.find("1110");
    ASSERT_NE(it, result.end());
    EXPECT_NEAR(it->second, 2048, 128);
}

TEST(ParallelSimTest, GRCSTestDD) {
    auto                              quantumComputation = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");
    HybridSchrodingerFeynmanSimulator ddsim_hybrid_dd(quantumComputation, HybridSchrodingerFeynmanSimulator::Mode::DD);
    CircuitSimulator                  ddsim(quantumComputation);

    ddsim_hybrid_dd.Simulate(1);
    ddsim.Simulate(1);

    dd::serialize(ddsim_hybrid_dd.root_edge, "result_parallel.dd", true);
    dd::serialize(ddsim.root_edge, "result.dd", true);

    auto dd     = std::make_unique<dd::Package>(quantumComputation->getNqubits());
    auto result = dd->deserialize<dd::Package::vNode>("result_parallel.dd", true);
    auto ref    = dd->deserialize<dd::Package::vNode>("result.dd", true);

    bool equal = (result == ref);
    if (!equal) {
        // if edges are not equal -> compare amplitudes
        auto refAmplitudes    = dd->getVector(ref);
        auto resultAmplitudes = dd->getVector(result);
        equal                 = true;
        for (std::size_t i = 0; i < refAmplitudes.size(); ++i) {
            if (std::abs(refAmplitudes[i].first - resultAmplitudes[i].first) > 1e-6 || std::abs(refAmplitudes[i].second - resultAmplitudes[i].second) > 1e-6) {
                equal = false;
                break;
            }
        }
    }
    EXPECT_TRUE(equal);
}

TEST(ParallelSimTest, GRCSTestAmplitudes) {
    auto                              quantumComputation = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");
    HybridSchrodingerFeynmanSimulator ddsim_hybrid_amp(quantumComputation, HybridSchrodingerFeynmanSimulator::Mode::Amplitude);
    CircuitSimulator                  ddsim(quantumComputation);

    ddsim_hybrid_amp.Simulate(0);
    ddsim.Simulate(0);

    // if edges are not equal -> compare amplitudes
    auto  refAmplitudes    = ddsim.getVector();
    auto& resultAmplitudes = ddsim_hybrid_amp.getFinalAmplitudes();
    bool  equal            = true;
    for (std::size_t i = 0; i < refAmplitudes.size(); ++i) {
        if (std::abs(refAmplitudes[i].r - resultAmplitudes[i].r) > 1e-6 || std::abs(refAmplitudes[i].i - resultAmplitudes[i].i) > 1e-6) {
            equal = false;
            break;
        }
    }

    EXPECT_TRUE(equal);
}
