#include "StochasticNoiseSimulator.hpp"

#include "gtest/gtest.h"
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */
using namespace qc::literals;

std::unique_ptr<qc::QuantumComputation> stochGetAdder4Circuit() {
    // circuit taken from https://github.com/pnnl/qasmbench
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4, 4);
    quantumComputation->x(0);
    quantumComputation->x(1);
    quantumComputation->h(3);
    quantumComputation->cx(2, 3);
    quantumComputation->t(0);
    quantumComputation->t(1);
    quantumComputation->t(2);
    quantumComputation->tdg(3);
    quantumComputation->cx(0, 1);
    quantumComputation->cx(2, 3);
    quantumComputation->cx(3, 0);
    quantumComputation->cx(1, 2);
    quantumComputation->cx(0, 1);
    quantumComputation->cx(2, 3);
    quantumComputation->tdg(0);
    quantumComputation->tdg(1);
    quantumComputation->tdg(2);
    quantumComputation->t(3);
    quantumComputation->cx(0, 1);
    quantumComputation->cx(2, 3);
    quantumComputation->s(3);
    quantumComputation->cx(3, 0);
    quantumComputation->h(3);

    quantumComputation->measure(0, 0);
    quantumComputation->measure(1, 1);
    quantumComputation->measure(2, 2);
    quantumComputation->measure(3, 3);
    return quantumComputation;
}

TEST(StochNoiseSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->x(0);
    quantumComputation->measure(0, 0);
    quantumComputation->measure(1, 1);
    StochasticNoiseSimulator ddsim(std::move(quantumComputation));

    ASSERT_EQ(ddsim.getNumberOfOps(), 3);

    const auto m = ddsim.simulate(1);

    ASSERT_EQ(static_cast<double>(m.find("01")->second), 1);
}

TEST(StochNoiseSimTest, ResetOp) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(1, 1);
    quantumComputation->x(0);
    quantumComputation->reset(0);
    quantumComputation->measure(0, 0);

    StochasticNoiseSimulator ddsim(std::move(quantumComputation));

    const auto m = ddsim.simulate(1);

    ASSERT_EQ(static_cast<double>(m.find("0")->second), 1);
}

TEST(StochNoiseSimTest, ApproximateByFidelity) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), ApproximationInfo{0.8, 1, ApproximationInfo::FidelityDriven}, "APD", 0.1);

    const auto m = ddsim.simulate(1000);

    double const tolerance = 50;

    EXPECT_NEAR(static_cast<double>(m.find("0000")->second), 255, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1000")->second), 177, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0100")->second), 63, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1100")->second), 44, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0010")->second), 89, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1010")->second), 62, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0110")->second), 22, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1110")->second), 15, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0001")->second), 87, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1001")->second), 61, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0101")->second), 24, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1101")->second), 17, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0011")->second), 35, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1011")->second), 24, tolerance);
}

TEST(StochNoiseSimTest, Reordering) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3, 3);
    quantumComputation->h(0);
    quantumComputation->h(1);
    quantumComputation->barrier({0, 1, 2});
    quantumComputation->mcx({0, 1}, 2);

    StochasticNoiseSimulator ddsim(std::move(quantumComputation));

    ddsim.simulate(1);
}

TEST(StochNoiseSimTest, SimulateClassicControlledOpWithError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->x(0);
    quantumComputation->measure(0, 0);
    quantumComputation->h(0);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(1, qc::X));
    auto                           classicalRegister = std::pair<std::size_t, std::size_t>(0, 1);
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, classicalRegister, 1);

    for (qc::Qubit i = 0; i < 2; i++) {
        quantumComputation->measure(i, i);
    }

    StochasticNoiseSimulator ddsim(std::move(quantumComputation), "APD", 0.02);

    const auto m = ddsim.simulate(1000);

    double const tolerance = 50;
    EXPECT_NEAR(static_cast<double>(m.find("00")->second), 49, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("01")->second), 45, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("10")->second), 469, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("11")->second), 435, tolerance);
}

TEST(StochNoiseSimTest, CheckQubitOrder) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4, 4);
    quantumComputation->x(0);

    for (qc::Qubit i = 0; i < 4; i++) {
        quantumComputation->measure(i, i);
    }

    StochasticNoiseSimulator ddsim(std::move(quantumComputation), {}, 42U, "APD", 0.02);

    const auto m = ddsim.simulate(1000);

    double const tolerance = 50;
    EXPECT_NEAR(static_cast<double>(m.find("0001")->second), 1000, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithoutNoise) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), {}, 42U, "APD", 0);

    const auto m = ddsim.simulate(1000);

    ASSERT_EQ(static_cast<double>(m.find("1001")->second), 1000);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateError) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), {}, 42U, "APD", 0.1);

    const auto m = ddsim.simulate(1000);

    double const tolerance = 50;

    EXPECT_NEAR(static_cast<double>(m.find("0000")->second), 255, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1000")->second), 177, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0100")->second), 63, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1100")->second), 44, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0010")->second), 89, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1010")->second), 62, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0110")->second), 22, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1110")->second), 15, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0001")->second), 87, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1001")->second), 61, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0101")->second), 24, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1101")->second), 17, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0011")->second), 35, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1011")->second), 24, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateErrorSelectedProperties) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), {}, 42U, "APD", 0.1);

    auto         m         = ddsim.simulate(1000);
    double const tolerance = 50;

    EXPECT_NEAR(static_cast<double>(m.find("0000")->second), 211, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1000")->second), 146, tolerance);
}

TEST(StochNoiseSimTest, SimulateRunWithBadParameters) {
    EXPECT_THROW(const StochasticNoiseSimulator ddsim(stochGetAdder4Circuit(), "AP", 0.3), std::runtime_error);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceError) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), {}, 42U, std::string("AP"), 0.01);

    auto         m         = ddsim.simulate(1000);
    double const tolerance = 50;

    EXPECT_NEAR(static_cast<double>(m.find("0000")->second), 84, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1000")->second), 79, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0010")->second), 16, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1010")->second), 16, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0110")->second), 22, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0001")->second), 174, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1001")->second), 537, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0011")->second), 14, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1011")->second), 14, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0111")->second), 18, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDepolarizationError) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), {}, 42U, "D", 0.01);

    const auto m = ddsim.simulate(1000);

    double const tolerance = 50;

    EXPECT_NEAR(static_cast<double>(m.find("0000")->second), 33, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1000")->second), 32, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0100")->second), 12, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0001")->second), 68, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1001")->second), 737, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0101")->second), 10, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1101")->second), 27, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0011")->second), 11, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1011")->second), 18, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0111")->second), 16, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithNoiseAndApproximation) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), ApproximationInfo{0.9, 1, ApproximationInfo::FidelityDriven}, 42U, "APD", 0.01);

    const auto m = ddsim.simulate(1000);

    size_t const tolerance = 50;

    EXPECT_NEAR(static_cast<double>(m.find("0000")->second), 96, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1000")->second), 90, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0100")->second), 14, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0010")->second), 23, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1010")->second), 23, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0110")->second), 24, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1110")->second), 11, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0001")->second), 173, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1001")->second), 414, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0101")->second), 13, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1101")->second), 18, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0011")->second), 24, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1011")->second), 26, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0111")->second), 23, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1111")->second), 11, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateErrorUnoptimizedSim) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), {}, 42U, "APD", 0.1);

    const auto m = ddsim.simulate(1000);

    const size_t tolerance = 50;

    EXPECT_NEAR(static_cast<double>(m.find("0000")->second), 255, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1000")->second), 177, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0100")->second), 63, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1100")->second), 44, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0010")->second), 89, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1010")->second), 62, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0110")->second), 22, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1110")->second), 15, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0001")->second), 87, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1001")->second), 61, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0101")->second), 24, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1101")->second), 17, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("0011")->second), 35, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("1011")->second), 24, tolerance);

    EXPECT_EQ(ddsim.getMaxMatrixNodeCount(), 0);
    EXPECT_EQ(ddsim.getMatrixActiveNodeCount(), 0);
    EXPECT_EQ(ddsim.countNodesFromRoot(), 0);
    auto statistics = ddsim.additionalStatistics();
    EXPECT_NEAR(static_cast<double>(m.find("1011")->second), 24, tolerance);

    EXPECT_EQ(std::stoi(ddsim.additionalStatistics().at("approximation_runs")), 0);

    EXPECT_NE(statistics.find("approximation_runs"), statistics.end());
    EXPECT_NE(statistics.find("stoch_wall_time"), statistics.end());
    EXPECT_NE(statistics.find("threads"), statistics.end());
}

TEST(StochNoiseSimTest, TestingBarrierGate) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->x(0);
    quantumComputation->h(1);
    quantumComputation->t(1);
    quantumComputation->barrier({0, 1});
    quantumComputation->h(1);
    quantumComputation->h(0);
    quantumComputation->measure(0, 0);
    quantumComputation->measure(1, 1);

    StochasticNoiseSimulator ddsim(std::move(quantumComputation), {}, 42U, "APD", 0.02);

    const auto m = ddsim.simulate(1000);

    double const tolerance = 50;
    EXPECT_NEAR(static_cast<double>(m.find("00")->second), 416, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("10")->second), 102, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("01")->second), 385, tolerance);
    EXPECT_NEAR(static_cast<double>(m.find("11")->second), 95, tolerance);
}

TEST(StochNoiseSimTest, TestingWithErrorProbZero) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), "APD", 0);

    const auto m = ddsim.simulate(1000);

    EXPECT_EQ(static_cast<double>(m.find("1001")->second), 1000);
}

TEST(StochNoiseSimTest, TestingWithEmpthyNoiseTypes) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string(""), 0.1);

    const auto m = ddsim.simulate(1000);
    EXPECT_EQ(static_cast<double>(m.find("1001")->second), 1000);
}

TEST(StochNoiseSimTest, TestingSimulatorFunctionality) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string(""), 0.1);

    const auto m = ddsim.simulate(1000);

    EXPECT_EQ(ddsim.getNumberOfQubits(), 4);
    EXPECT_EQ(ddsim.getActiveNodeCount(), 0);
    EXPECT_EQ(ddsim.getMaxNodeCount(), 0);
    EXPECT_EQ(ddsim.getMaxMatrixNodeCount(), 0);
    EXPECT_EQ(ddsim.getMatrixActiveNodeCount(), 0);
    EXPECT_EQ(ddsim.countNodesFromRoot(), 0);
    std::cout << ddsim.getName() << "\n";
}
