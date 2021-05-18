#include "StochasticNoiseSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */

TEST(StochNoiseSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);

    ASSERT_EQ(ddsim.getNumberOfOps(), 1);

    ddsim.Simulate(1);

    auto m = ddsim.MeasureAll(false);

    ASSERT_EQ("01", m);
}

TEST(StochNoiseSimTest, ClassicControlledOp) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
    auto                           classical_register = std::make_pair<unsigned short, unsigned short>(0, 1);
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, classical_register, 1);

    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);

    auto m = ddsim.MeasureAll(false);

    ASSERT_EQ("11", m);
}

TEST(StochNoiseSimTest, DestructiveMeasurementAll) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);

    const std::vector<dd::ComplexValue> v_before = ddsim.getVector();
    ASSERT_EQ(v_before[0], v_before[1]);
    ASSERT_EQ(v_before[0], v_before[2]);
    ASSERT_EQ(v_before[0], v_before[3]);

    const std::string                   m       = ddsim.MeasureAll(true);
    const std::vector<dd::ComplexValue> v_after = ddsim.getVector();
    const int                           i       = std::stoi(m, nullptr, 2);

    ASSERT_EQ(v_after[i].r, 1.0);
    ASSERT_EQ(v_after[i].i, 0.0);
}

TEST(StochNoiseSimTest, DestructiveMeasurementOne) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);

    const char                          m       = ddsim.MeasureOneCollapsing(0);
    const std::vector<dd::ComplexValue> v_after = ddsim.getVector();

    if (m == '0') {
        ASSERT_EQ(v_after[0], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[2], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[1], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[3], (dd::ComplexValue{0, 0}));
    } else if (m == '1') {
        ASSERT_EQ(v_after[0], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[2], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[1], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[3], dd::complex_SQRT2_2);
    } else {
        FAIL() << "Measurement result not in {0,1}!";
    }
}

TEST(StochNoiseSimTest, DestructiveMeasurementOneArbitraryNormalization) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);

    char            m = 'X';
    std::mt19937_64 gen{};

    ddsim.root_edge = ddsim.MeasureOneCollapsingConcurrent(0, ddsim.dd, ddsim.root_edge, gen, &m, false);

    const std::vector<dd::ComplexValue> v_after = ddsim.getVector();

    for (auto const& e: v_after) {
        std::cout << e << " ";
    }
    std::cout << "\n";

    if (m == '0') {
        ASSERT_EQ(v_after[0], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[2], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[1], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[3], (dd::ComplexValue{0, 0}));
    } else if (m == '1') {
        ASSERT_EQ(v_after[0], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[2], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[1], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[3], dd::complex_SQRT2_2);
    } else {
        FAIL() << "Measurement result not in {0,1}!";
    }
}

TEST(StochNoiseSimTest, ApproximateByFidelity) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, dd::Controls{dd::Control{0}, dd::Control{1}}, 2, qc::X);
    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);

    double resulting_fidelity = ddsim.ApproximateByFidelity(0.3, false, true);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 4);
    ASSERT_DOUBLE_EQ(resulting_fidelity, 0.75); //equal up to 4 ULP
}

TEST(StochNoiseSimTest, ApproximateBySampling) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, dd::Controls{dd::Control{0}, dd::Control{1}}, 2, qc::X);
    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);

    double resulting_fidelity = ddsim.ApproximateBySampling(1, 0, true);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 3);
    ASSERT_LE(resulting_fidelity, 0.75); // the least contributing path has .25
}

TEST(StochNoiseSimTest, Reordering) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(3, std::vector<dd::Qubit>{0, 1, 2}, qc::OpType::Barrier);
    quantumComputation->emplace_back<qc::StandardOperation>(3, dd::Controls{dd::Control{0}, dd::Control{1}}, 2, qc::X);

    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);
}

TEST(StochNoiseSimTest, SimulateAdder4TrackFidelityWithNoise) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.01, 30000, 1, 1, "-3-1000");
    auto                     m = ddsim.StochSimulate();

    std::cout << ddsim.getName() << "\n";

    EXPECT_GE(m.find("1000")->second, 0.14);
    EXPECT_LE(m.find("1000")->second, 0.16);
    EXPECT_GE(m.find("1001")->second, 0.54);
    EXPECT_LE(m.find("1001")->second, 0.56);
    EXPECT_GE(std::stod(ddsim.AdditionalStatistics().at("final_fidelity")), 0.54);
    EXPECT_LE(std::stod(ddsim.AdditionalStatistics().at("final_fidelity")), 0.56);
}

TEST(StochNoiseSimTest, SimulateClassicControlledOpWithError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
    auto                           classical_register = std::make_pair<unsigned short, unsigned short>(0, 1);
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, classical_register, 1);

    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.02, 30000, 1, 1, "-1-1000");
    auto                     m = ddsim.StochSimulate();

    EXPECT_GE(m.find("00")->second, 0.03);
    EXPECT_LE(m.find("00")->second, 0.06);
    EXPECT_GE(m.find("01")->second, 0.46);
    EXPECT_LE(m.find("01")->second, 0.48);
    EXPECT_GE(m.find("10")->second, 0.03);
    EXPECT_LE(m.find("10")->second, 0.05);
    EXPECT_GE(m.find("11")->second, 0.42);
    EXPECT_LE(m.find("11")->second, 0.44);
}

TEST(StochNoiseSimTest, SimulateAdder4WithoutNoise) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);
    auto m = ddsim.MeasureAll(false);
    ASSERT_EQ("1001", m);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.1, 30000, 1, 1, "-3-1000");
    auto                     m = ddsim.StochSimulate();

    EXPECT_GE(m.find("0000")->second, 0.19);
    EXPECT_LE(m.find("0000")->second, 0.23);
    EXPECT_GE(m.find("0001")->second, 0.13);
    EXPECT_LE(m.find("0001")->second, 0.15);
    EXPECT_GE(m.find("0100")->second, 0.08);
    EXPECT_LE(m.find("0100")->second, 0.10);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateErrorSelectedProperties) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(4, std::vector<dd::Qubit>{0, 1, 2}, qc::OpType::Barrier);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.1, 30000, 1, 1, " -3-500,501,502");
    auto                     m = ddsim.StochSimulate();

    EXPECT_GE(m.find("0000")->second, 0.19);
    EXPECT_LE(m.find("0000")->second, 0.23);
    EXPECT_GE(m.find("0001")->second, 0.13);
    EXPECT_LE(m.find("0001")->second, 0.15);
    EXPECT_GE(m.find("0100")->second, 0.08);
    EXPECT_LE(m.find("0100")->second, 0.10);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("AP"), 0.3, 30000, 1, 1, "0-1000");
    auto                     m = ddsim.StochSimulate();

    EXPECT_GE(m.find("0000")->second, 0.78);
    EXPECT_LE(m.find("0000")->second, 0.80);
    EXPECT_GE(m.find("0001")->second, 0.18);
    EXPECT_LE(m.find("0001")->second, 0.20);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDepolarizationError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("D"), 0.01, 30000, 1, 1, "0-1000");
    auto                     m = ddsim.StochSimulate();

    EXPECT_GE(m.find("1001")->second, 0.81);
    EXPECT_LE(m.find("1001")->second, 0.84);
    EXPECT_GE(m.find("0000")->second, 0.01);
    EXPECT_LE(m.find("0000")->second, 0.03);
}

TEST(StochNoiseSimTest, SimulateAdder4WithNoiseAndApproximation) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.01, 30000, 1, 0.9, "-3-1000");
    auto                     m = ddsim.StochSimulate();

    EXPECT_DOUBLE_EQ(std::stod(ddsim.AdditionalStatistics().at("step_fidelity")), 0.9);
    EXPECT_GT(std::stoi(ddsim.AdditionalStatistics().at("approximation_runs")), 0);
}