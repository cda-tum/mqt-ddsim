#include "QFRSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */


TEST(StochNoiseSimTest, SimulateAdder4TrackFidelityWithNoise) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{1, qc::Control::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    QFRSimulator ddsim(quantumComputation, std::string("APD"), 0.01, 30000, 1, 1, "-1-1000", 0, 0, 0);
    auto m = ddsim.StochSimulate();

    EXPECT_TRUE((m.find("1000")->second >= 0.14) && (m.find("1000")->second <= 0.16));
    EXPECT_TRUE((m.find("1001")->second >= 0.54) && (m.find("1001")->second <= 0.56));
    EXPECT_TRUE((m.find("fidelity")->second >= 0.54) && (m.find("fidelity")->second <= 0.56));
}


TEST(StochNoiseSimTest, SimulateClassicControlledOpWithError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    std::vector<unsigned short> qubit_to_measure = {0};
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, qubit_to_measure, qubit_to_measure);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
    auto classical_register = std::make_pair<unsigned short, unsigned short>(0, 1);
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, classical_register, 1);

    QFRSimulator ddsim(quantumComputation, std::string("APD"), 0.02, 30000, 1, 1, "0-1000", 0, 0, 0);
    auto m = ddsim.StochSimulate();

    EXPECT_TRUE((m.find("00")->second >= 0.03) && (m.find("00")->second <= 0.06));
    EXPECT_TRUE((m.find("01")->second >= 0.46) && (m.find("01")->second <= 0.48));
    EXPECT_TRUE((m.find("10")->second >= 0.03) && (m.find("10")->second <= 0.05));
    EXPECT_TRUE((m.find("11")->second >= 0.42) && (m.find("11")->second <= 0.44));
}


TEST(StochNoiseSimTest, SimulateAdder4WithoutNoise) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{1, qc::Control::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    QFRSimulator ddsim(quantumComputation, 1, 1, 0, 0, 0);
    ddsim.Simulate(1);
    auto m = ddsim.MeasureAll(false);
    ASSERT_EQ("1001", m);
}


TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{1, qc::Control::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    QFRSimulator ddsim(quantumComputation, std::string("APD"), 0.1, 30000, 1, 1, "-1-1000", 0, 0, 0);
    auto m = ddsim.StochSimulate();

    EXPECT_TRUE((m.find("0000")->second >= 0.19) && (m.find("0000")->second <= 0.23));
    EXPECT_TRUE((m.find("0001")->second >= 0.13) && (m.find("0001")->second <= 0.15));
    EXPECT_TRUE((m.find("0100")->second >= 0.08) && (m.find("0100")->second <= 0.10));
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{1, qc::Control::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    QFRSimulator ddsim(quantumComputation, std::string("AP"), 0.3, 30000, 1, 1, "0-1000", 0, 0, 0);
    auto m = ddsim.StochSimulate();

    EXPECT_TRUE((m.find("0000")->second >= 0.78) && (m.find("0000")->second <= 0.80));
    EXPECT_TRUE((m.find("0001")->second >= 0.18) && (m.find("0001")->second <= 0.20));
}

TEST(StochNoiseSimTest, SimulateAdder4WithDepolarizationError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{1, qc::Control::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{0, qc::Control::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{2, qc::Control::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, std::vector<qc::Control>{qc::Control{3, qc::Control::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    QFRSimulator ddsim(quantumComputation, std::string("D"), 0.01, 30000, 1, 1, "0-1000", 0, 0, 0);
    auto m = ddsim.StochSimulate();

    EXPECT_TRUE((m.find("1001")->second >= 0.81) && (m.find("1001")->second <= 0.84));
    EXPECT_TRUE((m.find("0000")->second >= 0.01) && (m.find("0000")->second <= 0.03));
}