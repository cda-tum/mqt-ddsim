#include "StochasticNoiseSimulator.hpp"

#include <gtest/gtest.h>
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

    auto m = ddsim.simulate(1);

    ASSERT_EQ(m.find("01")->second, 1);
}

TEST(StochNoiseSimTest, ResetOp) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(1, 1);
    quantumComputation->x(0);
    quantumComputation->reset(0);
    quantumComputation->measure(0, 0);

    StochasticNoiseSimulator ddsim(std::move(quantumComputation));

    auto m = ddsim.simulate(1);

    ASSERT_EQ(m.find("0")->second, 1);
}

//TEST(StochNoiseSimTest, ClassicControlledOp) {
//    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
//    quantumComputation->x(0);
//    quantumComputation->measure(0, 0);
//    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
//    auto                           classicalRegister = std::pair<std::size_t, std::size_t>(0, 1);
//    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, classicalRegister, 1);
//
//    StochasticNoiseSimulator ddsim(std::move(quantumComputation), 1, 1);
//
//    ddsim.simulate(1);
//
//    auto m = ddsim.measureAll(false);
//
//    ASSERT_EQ("11", m);
//}

//TEST(StochNoiseSimTest, DestructiveMeasurementAll) {
//    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2,2);
//    quantumComputation->h(0);
//    quantumComputation->h(1);
//    StochasticNoiseSimulator ddsim(std::move(quantumComputation), 1, 1);
//
//    ddsim.simulate(1);
//
//    const auto vBefore = ddsim.getVector();
//    ASSERT_EQ(vBefore[0], vBefore[1]);
//    ASSERT_EQ(vBefore[0], vBefore[2]);
//    ASSERT_EQ(vBefore[0], vBefore[3]);
//
//    const std::string m      = ddsim.measureAll(true);
//    const auto        vAfter = ddsim.getVector();
//    const std::size_t i      = std::stoul(m, nullptr, 2);
//
//    ASSERT_EQ(vAfter[i].real(), 1.0);
//    ASSERT_EQ(vAfter[i].imag(), 0.0);
//}
//
//TEST(StochNoiseSimTest, DestructiveMeasurementOne) {
//    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2,2);
//    quantumComputation->h(0);
//    quantumComputation->h(1);
//    StochasticNoiseSimulator ddsim(std::move(quantumComputation), 1, 1);
//
//    ddsim.simulate(1);
//
//    const char m      = ddsim.measureOneCollapsing(0);
//    const auto vAfter = ddsim.getVector();
//
//    if (m == '0') {
//        ASSERT_EQ(vAfter[0], dd::SQRT2_2);
//        ASSERT_EQ(vAfter[2], dd::SQRT2_2);
//        ASSERT_EQ(vAfter[1], 0.);
//        ASSERT_EQ(vAfter[3], 0.);
//    } else if (m == '1') {
//        ASSERT_EQ(vAfter[0], 0.);
//        ASSERT_EQ(vAfter[2], 0.);
//        ASSERT_EQ(vAfter[1], dd::SQRT2_2);
//        ASSERT_EQ(vAfter[3], dd::SQRT2_2);
//    } else {
//        FAIL() << "Measurement result not in {0,1}!";
//    }
//}
//
//TEST(StochNoiseSimTest, DestructiveMeasurementOneArbitraryNormalization) {
//    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2,2);
//    quantumComputation->h(0);
//    quantumComputation->h(1);
//    StochasticNoiseSimulator ddsim(std::move(quantumComputation), 1, 1);
//
//    ddsim.simulate(1);
//
//    std::mt19937_64 gen{}; // NOLINT(cert-msc51-cpp)
//
//    char const m = ddsim.dd->measureOneCollapsing(ddsim.rootEdge, 0, false, gen);
//
//    const auto vAfter = ddsim.getVector();
//
//    for (auto const& e: vAfter) {
//        std::cout << e << " ";
//    }
//    std::cout << "\n";
//
//    if (m == '0') {
//        ASSERT_EQ(vAfter[0], dd::SQRT2_2);
//        ASSERT_EQ(vAfter[2], dd::SQRT2_2);
//        ASSERT_EQ(vAfter[1], 0.);
//        ASSERT_EQ(vAfter[3], 0.);
//    } else if (m == '1') {
//        ASSERT_EQ(vAfter[0], 0.);
//        ASSERT_EQ(vAfter[2], 0.);
//        ASSERT_EQ(vAfter[1], dd::SQRT2_2);
//        ASSERT_EQ(vAfter[3], dd::SQRT2_2);
//    } else {
//        FAIL() << "Measurement result not in {0,1}!";
//    }
//}

//TEST(StochNoiseSimTest, ApproximateByFidelity) {
//    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
//    quantumComputation->h(0);
//    quantumComputation->h(1);
//    quantumComputation->mcx({0, 1}, 2);
//    StochasticNoiseSimulator ddsim(std::move(quantumComputation), 1, 1, 54);
//
//    ddsim.simulate(1);
//
//    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);
//
//    double const resultingFidelity = ddsim.approximateByFidelity(0.3, false, true);
//
//    ASSERT_EQ(ddsim.getActiveNodeCount(), 3);
//    ASSERT_DOUBLE_EQ(resultingFidelity, 0.5); //equal up to 4 ULP
//}
//
//TEST(StochNoiseSimTest, ApproximateBySampling) {
//    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3,3);
//    quantumComputation->h(0);
//    quantumComputation->h(1);
//    quantumComputation->mcx({0, 1}, 2);
//    StochasticNoiseSimulator ddsim(std::move(quantumComputation), 1, 1);
//
//    ddsim.simulate(1);
//
//    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);
//
//    double const resultingFidelity = ddsim.approximateBySampling(1, 0, true);
//
//    ASSERT_EQ(ddsim.getActiveNodeCount(), 3);
//    ASSERT_LE(resultingFidelity, 0.75); // the least contributing path has .25
//}

TEST(StochNoiseSimTest, Reordering) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3, 3);
    quantumComputation->h(0);
    quantumComputation->h(1);
    quantumComputation->barrier({0, 1, 2});
    quantumComputation->mcx({0, 1}, 2);

    StochasticNoiseSimulator ddsim(std::move(quantumComputation), 1, 1);

    ddsim.simulate(1);
}

TEST(StochNoiseSimTest, SimulateClassicControlledOpWithError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->x(0);
    quantumComputation->measure(0, 0);
    quantumComputation->h(0);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
    auto                           classicalRegister = std::pair<std::size_t, std::size_t>(0, 1);
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, classicalRegister, 1);

    for (qc::Qubit i = 0; i < 2; i++) {
        quantumComputation->measure(i, i);
    }

    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0.02, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    double const tolerance = 50;
    EXPECT_NEAR(m.find("00")->second, 49, tolerance);
    EXPECT_NEAR(m.find("01")->second, 45, tolerance);
    EXPECT_NEAR(m.find("10")->second, 469, tolerance);
    EXPECT_NEAR(m.find("11")->second, 435, tolerance);
}

TEST(StochNoiseSimTest, CheckQubitOrder) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4, 4);
    quantumComputation->x(0);

    for (qc::Qubit i = 0; i < 4; i++) {
        quantumComputation->measure(i, i);
    }

    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0.02, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    double const tolerance = 50;
    EXPECT_NEAR(m.find("0001")->second, 1000, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithoutNoise) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    ASSERT_EQ(m.find("1001")->second, 1000);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateError) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0.1, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    double const tolerance = 50;

    EXPECT_NEAR(m.find("0000")->second, 255, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 177, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 63, tolerance);
    EXPECT_NEAR(m.find("1100")->second, 44, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 89, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 62, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 22, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 15, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 87, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 61, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 24, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 17, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 35, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 24, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateErrorSelectedProperties) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0.1, std::optional<double>{}, 2);

    auto         m         = ddsim.simulate(1000);
    double const tolerance = 50;

    EXPECT_NEAR(m.find("0000")->second, 211, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 146, tolerance);
}

TEST(StochNoiseSimTest, SimulateRunWithBadParameters) {
    EXPECT_THROW(const StochasticNoiseSimulator ddsim(stochGetAdder4Circuit(), std::string("AP"), 0.3, std::optional<double>{}, 2, 1000), std::runtime_error);
    EXPECT_THROW(const StochasticNoiseSimulator ddsim(stochGetAdder4Circuit(), std::string("APK"), 0.01, std::optional<double>{}, 2, 1000), std::runtime_error);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceError) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("AP"), 0.01, std::optional<double>{}, 2);

    auto         m         = ddsim.simulate(1000);
    double const tolerance = 50;

    EXPECT_NEAR(m.find("0000")->second, 84, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 79, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 16, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 16, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 22, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 174, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 537, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 14, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 14, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 18, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDepolarizationError) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("D"), 0.01, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    double const tolerance = 50;

    EXPECT_NEAR(m.find("0000")->second, 33, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 32, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 12, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 68, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 737, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 10, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 27, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 11, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 18, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 16, tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithNoiseAndApproximation) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0.01, std::optional<double>{}, 2, 1, 1, 0.9);

    auto m = ddsim.simulate(1000);

    size_t const tolerance = 50;

    EXPECT_NEAR(m.find("0000")->second, 96, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 90, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 14, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 23, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 23, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 24, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 11, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 173, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 414, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 13, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 18, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 24, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 26, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 23, tolerance);
    EXPECT_NEAR(m.find("1111")->second, 11, tolerance);

    //    EXPECT_GT(std::stoi(ddsim.additionalStatistics().at("approximation_runs")), 0);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateErrorUnoptimizedSim) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0.1, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    const size_t tolerance = 50;

    EXPECT_NEAR(m.find("0000")->second, 255, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 177, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 63, tolerance);
    EXPECT_NEAR(m.find("1100")->second, 44, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 89, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 62, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 22, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 15, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 87, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 61, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 24, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 17, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 35, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 24, tolerance);

    auto tmp0 = ddsim.getSeed();
    EXPECT_EQ(ddsim.getMaxMatrixNodeCount(), 0);
    EXPECT_EQ(ddsim.getMatrixActiveNodeCount(), 0);
    EXPECT_EQ(ddsim.countNodesFromRoot(), 0);
    auto statistics = ddsim.additionalStatistics();
    EXPECT_NEAR(m.find("1011")->second, 24, tolerance);

    EXPECT_EQ(std::stoi(ddsim.additionalStatistics().at("approximation_runs")), 0);

    EXPECT_NE(statistics.find("step_fidelity"), statistics.end());
    EXPECT_NE(statistics.find("approximation_runs"), statistics.end());
    EXPECT_NE(statistics.find("perfect_run_time"), statistics.end());
    EXPECT_NE(statistics.find("mean_stoch_run_time"), statistics.end());
    EXPECT_NE(statistics.find("stoch_wall_time"), statistics.end());
    EXPECT_NE(statistics.find("parallel_instances"), statistics.end());
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

    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0.02, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    double const tolerance = 50;
    EXPECT_NEAR(m.find("00")->second, 416, tolerance);
    EXPECT_NEAR(m.find("10")->second, 102, tolerance);
    EXPECT_NEAR(m.find("01")->second, 385, tolerance);
    EXPECT_NEAR(m.find("11")->second, 95, tolerance);
}

TEST(StochNoiseSimTest, TestingWithErrorProbZero) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string("APD"), 0, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    EXPECT_EQ(m.find("1001")->second, 1000);
}

TEST(StochNoiseSimTest, TestingWithEmpthyNoiseTypes) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string(""), 0.1, std::optional<double>{}, 2);

    auto         m         = ddsim.simulate(1000);
    double const tolerance = 50;

    EXPECT_EQ(m.find("1001")->second, 1000);
}

TEST(StochNoiseSimTest, TestingSimulatorFunctionality) {
    auto                     quantumComputation = stochGetAdder4Circuit();
    StochasticNoiseSimulator ddsim(std::move(quantumComputation), std::string(""), 0.1, std::optional<double>{}, 2);

    auto m = ddsim.simulate(1000);

    EXPECT_EQ(ddsim.getNumberOfQubits(), 4);
    EXPECT_EQ(ddsim.getActiveNodeCount(), 0);
    EXPECT_EQ(ddsim.getMaxNodeCount(), 0);
    EXPECT_EQ(ddsim.getMaxMatrixNodeCount(), 0);
    EXPECT_EQ(ddsim.getMatrixActiveNodeCount(), 0);
    EXPECT_EQ(ddsim.countNodesFromRoot(), 0);
    std::cout << ddsim.getName() << "\n";
}
