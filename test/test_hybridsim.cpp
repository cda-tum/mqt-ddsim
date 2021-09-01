#include "HybridSchrodingerFeynmanSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace dd::literals;

TEST(HybridSimTest, TrivialParallelTest) {
    auto quantumComputation = [] {
        auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
        quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::H);
        quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::H);
        quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{2_pc, 1_pc}, 0, qc::X);
        quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::I); // some dummy operations
        quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::I);
        return quantumComputation;
    };

    HybridSchrodingerFeynmanSimulator ddsim(quantumComputation(), HybridSchrodingerFeynmanSimulator::Mode::DD);
    auto resultDD = ddsim.Simulate(8192);
    for (const auto& entry: resultDD) {
        std::clog << "resultDD[" << entry.first << "] = " << entry.second << "\n";
    }

    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);
    ASSERT_EQ(resultDD.size(), 4);
    auto it = resultDD.find("0000");
    ASSERT_TRUE(it != resultDD.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = resultDD.find("0010");
    ASSERT_TRUE(it != resultDD.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = resultDD.find("0100");
    ASSERT_TRUE(it != resultDD.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = resultDD.find("0111");
    ASSERT_TRUE(it != resultDD.end());
    EXPECT_NEAR(it->second, 2048, 128);

    HybridSchrodingerFeynmanSimulator ddsim2(quantumComputation(), HybridSchrodingerFeynmanSimulator::Mode::Amplitude);

    auto resultAmp = ddsim2.Simulate(8192);

    for (const auto& entry: resultAmp) {
        std::clog << "resultAmp[" << entry.first << "] = " << entry.second << "\n";
    }

    ASSERT_EQ(resultAmp.size(), 4);
    it = resultAmp.find("0000");
    ASSERT_TRUE(it != resultAmp.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = resultAmp.find("0010");
    ASSERT_TRUE(it != resultAmp.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = resultAmp.find("0100");
    ASSERT_TRUE(it != resultAmp.end());
    EXPECT_NEAR(it->second, 2048, 128);
    it = resultAmp.find("1110");
    ASSERT_TRUE(it != resultAmp.end());
    EXPECT_NEAR(it->second, 2048, 128);
}

TEST(HybridSimTest, GRCSTestDD) {
    auto qc1 = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");
    auto qc2 = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");

    HybridSchrodingerFeynmanSimulator ddsim_hybrid_dd(std::move(qc1), HybridSchrodingerFeynmanSimulator::Mode::DD);
    CircuitSimulator                  ddsim(std::move(qc2));

    ddsim_hybrid_dd.Simulate(1);
    ddsim.Simulate(1);

    dd::serialize(ddsim_hybrid_dd.root_edge, "result_parallel.dd", true);
    dd::serialize(ddsim.root_edge, "result.dd", true);

    auto dd     = std::make_unique<dd::Package>(ddsim.getNumberOfQubits());
    auto result = dd->deserialize<dd::Package::vNode>("result_parallel.dd", true);
    auto ref    = dd->deserialize<dd::Package::vNode>("result.dd", true);

    bool equal = (result == ref);
    if (!equal) {
        // if edges are not equal -> compare amplitudes
        auto refAmplitudes    = dd->getVector(ref);
        auto resultAmplitudes = dd->getVector(result);
        equal                 = true;
        for (std::size_t i = 0; i < refAmplitudes.size(); ++i) {
            if (std::abs(refAmplitudes[i].real() - resultAmplitudes[i].real()) > 1e-6 || std::abs(refAmplitudes[i].imag() - resultAmplitudes[i].imag()) > 1e-6) {
                equal = false;
                break;
            }
        }
    }
    EXPECT_TRUE(equal);
}

TEST(HybridSimTest, GRCSTestAmplitudes) {
    auto qc1 = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");
    auto qc2 = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");

    HybridSchrodingerFeynmanSimulator ddsim_hybrid_amp(std::move(qc1), HybridSchrodingerFeynmanSimulator::Mode::Amplitude);
    CircuitSimulator                  ddsim(std::move(qc2));

    ddsim_hybrid_amp.Simulate(0);
    ddsim.Simulate(0);

    // if edges are not equal -> compare amplitudes
    auto  refAmplitudes    = ddsim.getVector();
    auto& resultAmplitudes = ddsim_hybrid_amp.getFinalAmplitudes();
    bool  equal            = true;
    for (std::size_t i = 0; i < refAmplitudes.size(); ++i) {
        if (std::abs(refAmplitudes[i].r - resultAmplitudes[i].real()) > 1e-6 || std::abs(refAmplitudes[i].i - resultAmplitudes[i].imag()) > 1e-6) {
            equal = false;
            break;
        }
    }

    EXPECT_TRUE(equal);
}

TEST(HybridSimTest, GRCSTestFixedSeed) {
    auto qc1 = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");
    auto qc2 = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");

    HybridSchrodingerFeynmanSimulator ddsim_hybrid_amp(std::move(qc1), ApproximationInfo{}, 42);
    EXPECT_TRUE(ddsim_hybrid_amp.getMode() == HybridSchrodingerFeynmanSimulator::Mode::Amplitude);
    CircuitSimulator ddsim(std::move(qc2));

    ddsim_hybrid_amp.Simulate(0);
    ddsim.Simulate(0);

    // if edges are not equal -> compare amplitudes
    auto  refAmplitudes    = ddsim.getVector();
    auto& resultAmplitudes = ddsim_hybrid_amp.getFinalAmplitudes();
    bool  equal            = true;
    for (std::size_t i = 0; i < refAmplitudes.size(); ++i) {
        if (std::abs(refAmplitudes[i].r - resultAmplitudes[i].real()) > 1e-6 || std::abs(refAmplitudes[i].i - resultAmplitudes[i].imag()) > 1e-6) {
            equal = false;
            break;
        }
    }

    EXPECT_TRUE(equal);
}

TEST(HybridSimTest, NonStandardOperation) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(1);
    quantumComputation->emplace_back<qc::StandardOperation>(1, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(1, 0, 0);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(1, 0, qc::Barrier);
    quantumComputation->emplace_back<qc::StandardOperation>(1, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(1, 0, 0);

    HybridSchrodingerFeynmanSimulator ddsim(std::move(quantumComputation));
    EXPECT_THROW(ddsim.Simulate(0), std::invalid_argument);
}
