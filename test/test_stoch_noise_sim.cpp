#include "StochasticNoiseSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */
using namespace dd::literals;

std::unique_ptr<qc::QuantumComputation> stochTestsgetAdder4Circuit() {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2_pc, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0_pc, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2_pc, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3_pc, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1_pc, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0_pc, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2_pc, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0_pc, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2_pc, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3_pc, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);

    return quantumComputation;
}

TEST(StochNoiseSimTest, Reset) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(1, std::vector<dd::Qubit>{0}, qc::OpType::Reset);

    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1, 1);
    ddsim.Simulate(1);
    auto m = ddsim.MeasureAll(false);
    ASSERT_EQ("00", m);

    StochasticNoiseSimulator ddsim2(quantumComputation, 1, 1, 2);
    ddsim2.Simulate(1);
    auto m2 = ddsim2.MeasureAll(false);
    ASSERT_EQ("10", m2);
}

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

    std::mt19937_64 gen{};

    char m = ddsim.dd->measureOneCollapsing(ddsim.root_edge, 0, false, gen);

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

//TEST(StochNoiseSimTest, SimulateAdder4TrackFidelityWithNoise) {
//    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1}}, 2, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0}}, 1, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2}}, 3, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3}}, 0, qc::X);
//    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
//    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.01, 30000, 1, 1, "-3-1000");
//    auto                     m = ddsim.StochSimulate();
//
//    std::cout << ddsim.getName() << "\n";
//
//    EXPECT_GE(m.find("1000")->second, 0.13);
//    EXPECT_LE(m.find("1000")->second, 0.17);
//    EXPECT_GE(m.find("1001")->second, 0.53);
//    EXPECT_LE(m.find("1001")->second, 0.57);
//    EXPECT_GE(std::stod(ddsim.AdditionalStatistics().at("final_fidelity")), 0.53);
//    EXPECT_LE(std::stod(ddsim.AdditionalStatistics().at("final_fidelity")), 0.57);
//}

TEST(StochNoiseSimTest, SimulateClassicControlledOpWithError) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
    auto                           classical_register = std::make_pair<unsigned short, unsigned short>(0, 1);
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, classical_register, 1);

    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.02, 1000, 1, 1, "0-3");

    auto m = ddsim.StochSimulate();

    EXPECT_GE(m.find("00")->second, 0.02);
    EXPECT_LE(m.find("00")->second, 0.07);
    EXPECT_GE(m.find("01")->second, 0.45);
    EXPECT_LE(m.find("01")->second, 0.49);
    EXPECT_GE(m.find("10")->second, 0.02);
    EXPECT_LE(m.find("10")->second, 0.06);
    EXPECT_GE(m.find("11")->second, 0.41);
    EXPECT_LE(m.find("11")->second, 0.45);
}

TEST(StochNoiseSimTest, SimulateAdder4WithoutNoise) {
    auto                     quantumComputation = stochTestsgetAdder4Circuit();
    StochasticNoiseSimulator ddsim(quantumComputation, 1, 1);
    ddsim.Simulate(1);
    auto m = ddsim.MeasureAll(false);
    ASSERT_EQ("1001", m);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateError) {
    auto                     quantumComputation = stochTestsgetAdder4Circuit();
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.1, 1000, 1, 1, "0-16");
    auto                     m = ddsim.StochSimulate();

    double tolerance = 0.1;

    EXPECT_LT(std::abs(m.find("0000")->second - 0.25574412296741467), tolerance);
    EXPECT_LT(std::abs(m.find("0001")->second - 0.177720207953642), tolerance);
    EXPECT_LT(std::abs(m.find("0010")->second - 0.06386485600556026), tolerance);
    EXPECT_LT(std::abs(m.find("0011")->second - 0.04438060064535747), tolerance);
    EXPECT_LT(std::abs(m.find("0100")->second - 0.0898482618504159), tolerance);
    EXPECT_LT(std::abs(m.find("0101")->second - 0.062436925904517736), tolerance);
    EXPECT_LT(std::abs(m.find("0110")->second - 0.022981537137908348), tolerance);
    EXPECT_LT(std::abs(m.find("0111")->second - 0.015970195341710985), tolerance);
    EXPECT_LT(std::abs(m.find("1000")->second - 0.08799481366902726), tolerance);
    EXPECT_LT(std::abs(m.find("1001")->second - 0.061149120043750206), tolerance);
    EXPECT_LT(std::abs(m.find("1010")->second - 0.02480081309590326), tolerance);
    EXPECT_LT(std::abs(m.find("1011")->second - 0.017234499727102268), tolerance);
    EXPECT_LT(std::abs(m.find("1100")->second - 0.03505400112419414), tolerance);
    EXPECT_LT(std::abs(m.find("1101")->second - 0.024359601507422463), tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateErrorSelectedProperties) {
    auto                     quantumComputation = stochTestsgetAdder4Circuit();
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.1, 1000, 1, 1, "4,8-15");
    auto                     m         = ddsim.StochSimulate();
    double                   tolerance = 0.1;

    EXPECT_LT(std::abs(m.find("0100")->second - 0.0898482618504159), tolerance);
    EXPECT_LT(std::abs(m.find("1000")->second - 0.08799481366902726), tolerance);
    EXPECT_LT(std::abs(m.find("1001")->second - 0.061149120043750206), tolerance);
    EXPECT_LT(std::abs(m.find("1010")->second - 0.02480081309590326), tolerance);
    EXPECT_LT(std::abs(m.find("1011")->second - 0.017234499727102268), tolerance);
    EXPECT_LT(std::abs(m.find("1100")->second - 0.03505400112419414), tolerance);
    EXPECT_LT(std::abs(m.find("1101")->second - 0.024359601507422463), tolerance);
}

TEST(StochNoiseSimTest, SimulateRunWithBadParameters) {
    auto quantumComputation = stochTestsgetAdder4Circuit();
    EXPECT_THROW(StochasticNoiseSimulator ddsim(quantumComputation, std::string("AP"), 0.3, 1000, 1, 1, "0-1000"), std::runtime_error);
    EXPECT_THROW(StochasticNoiseSimulator ddsim(quantumComputation, std::string("APK"), 0.01, 1000, 1, 1, "0-1000"), std::runtime_error);
    EXPECT_THROW(StochasticNoiseSimulator ddsim(quantumComputation, std::string("AP"), 0.01, -1, 1, 1, "0-1000"), std::runtime_error);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceError) {
    auto                     quantumComputation = stochTestsgetAdder4Circuit();
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("AP"), 0.01, 1000, 1, 1, "0-1000");
    auto                     m         = ddsim.StochSimulate();
    double                   tolerance = 0.1;

    EXPECT_LT(std::abs(m.find("0000")->second - 0.08441970960811902), tolerance);
    EXPECT_LT(std::abs(m.find("0001")->second - 0.0791112059351237), tolerance);
    EXPECT_LT(std::abs(m.find("0100")->second - 0.016558289691361448), tolerance);
    EXPECT_LT(std::abs(m.find("0101")->second - 0.016171421345261474), tolerance);
    EXPECT_LT(std::abs(m.find("0110")->second - 0.02212697987399219), tolerance);
    EXPECT_LT(std::abs(m.find("1000")->second - 0.17460882977464234), tolerance);
    EXPECT_LT(std::abs(m.find("1001")->second - 0.5375349142994105), tolerance);
    EXPECT_LT(std::abs(m.find("1100")->second - 0.014249759740682603), tolerance);
    EXPECT_LT(std::abs(m.find("1101")->second - 0.01403353315543664), tolerance);
    EXPECT_LT(std::abs(m.find("1110")->second - 0.01885716876841646), tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDepolarizationError) {
    auto                     quantumComputation = stochTestsgetAdder4Circuit();
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("D"), 0.01, 1000, 1, 1, "0-1000");
    auto                     m = ddsim.StochSimulate();

    double tolerance = 0.1;

    EXPECT_LT(std::abs(m.find("0000")->second - 0.03323287049319544), tolerance);
    EXPECT_LT(std::abs(m.find("0001")->second - 0.03284348575778577), tolerance);
    EXPECT_LT(std::abs(m.find("0010")->second - 0.01296430657356062), tolerance);
    EXPECT_LT(std::abs(m.find("1000")->second - 0.06839382801894367), tolerance);
    EXPECT_LT(std::abs(m.find("1001")->second - 0.7370101351171158), tolerance);
    EXPECT_LT(std::abs(m.find("1010")->second - 0.010781280290814215), tolerance);
    EXPECT_LT(std::abs(m.find("1011")->second - 0.02750867476569656), tolerance);
    EXPECT_LT(std::abs(m.find("1100")->second - 0.011706168989804651), tolerance);
    EXPECT_LT(std::abs(m.find("1101")->second - 0.01863469254111994), tolerance);
    EXPECT_LT(std::abs(m.find("1110")->second - 0.016008233100915176), tolerance);
}

TEST(StochNoiseSimTest, SimulateAdder4WithNoiseAndApproximation) {
    auto                     quantumComputation = stochTestsgetAdder4Circuit();
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.01, 1000, 1, 0.9, "-3-1000");
    auto                     m = ddsim.StochSimulate();

    double tolerance = 0.1;

    EXPECT_LT(std::abs(m.find("0000")->second - 0.09693321927412533), tolerance);
    EXPECT_LT(std::abs(m.find("0001")->second - 0.09078880415385877), tolerance);
    EXPECT_LT(std::abs(m.find("0010")->second - 0.01414096609854787), tolerance);
    EXPECT_LT(std::abs(m.find("0100")->second - 0.02382034755245074), tolerance);
    EXPECT_LT(std::abs(m.find("0101")->second - 0.023509799001774703), tolerance);
    EXPECT_LT(std::abs(m.find("0110")->second - 0.02445760874001203), tolerance);
    EXPECT_LT(std::abs(m.find("0111")->second - 0.011628281127642115), tolerance);
    EXPECT_LT(std::abs(m.find("1000")->second - 0.1731941264570172), tolerance);
    EXPECT_LT(std::abs(m.find("1001")->second - 0.41458550719988047), tolerance);
    EXPECT_LT(std::abs(m.find("1010")->second - 0.013806211321349706), tolerance);
    EXPECT_LT(std::abs(m.find("1011")->second - 0.01840334820660922), tolerance);
    EXPECT_LT(std::abs(m.find("1100")->second - 0.024245433691737584), tolerance);
    EXPECT_LT(std::abs(m.find("1101")->second - 0.026277984479993615), tolerance);
    EXPECT_LT(std::abs(m.find("1110")->second - 0.023929692098939092), tolerance);
    EXPECT_LT(std::abs(m.find("1111")->second - 0.011037316662706232), tolerance);

    EXPECT_GT(std::stoi(ddsim.AdditionalStatistics().at("approximation_runs")), 0);
}

TEST(StochNoiseSimTest, SimulateAdder4WithDecoherenceAndGateErrorUnoptimizedSim) {
    auto                     quantumComputation = stochTestsgetAdder4Circuit();
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.1, 1000, 1, 1, "0-16");
    ddsim.sequentialApplyNoise = true;
    auto m                     = ddsim.StochSimulate();

    double tolerance = 0.1;

    EXPECT_LT(std::abs(m.find("0000")->second - 0.25574412296741467), tolerance);
    EXPECT_LT(std::abs(m.find("0001")->second - 0.177720207953642), tolerance);
    EXPECT_LT(std::abs(m.find("0010")->second - 0.06386485600556026), tolerance);
    EXPECT_LT(std::abs(m.find("0011")->second - 0.04438060064535747), tolerance);
    EXPECT_LT(std::abs(m.find("0100")->second - 0.0898482618504159), tolerance);
    EXPECT_LT(std::abs(m.find("0101")->second - 0.062436925904517736), tolerance);
    EXPECT_LT(std::abs(m.find("0110")->second - 0.022981537137908348), tolerance);
    EXPECT_LT(std::abs(m.find("0111")->second - 0.015970195341710985), tolerance);
    EXPECT_LT(std::abs(m.find("1000")->second - 0.08799481366902726), tolerance);
    EXPECT_LT(std::abs(m.find("1001")->second - 0.061149120043750206), tolerance);
    EXPECT_LT(std::abs(m.find("1010")->second - 0.02480081309590326), tolerance);
    EXPECT_LT(std::abs(m.find("1011")->second - 0.017234499727102268), tolerance);
    EXPECT_LT(std::abs(m.find("1100")->second - 0.03505400112419414), tolerance);
    EXPECT_LT(std::abs(m.find("1101")->second - 0.024359601507422463), tolerance);
}

TEST(StochNoiseSimTest, ParseProperties) {
    auto                     quantumComputation = stochTestsgetAdder4Circuit();
    StochasticNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.1, 1000, 1, 1, "0, 6, 1");
    auto                     m = ddsim.StochSimulate();

    double tolerance = 0.1;

    EXPECT_LT(std::abs(m.find("0000")->second - 0.25574412296741467), tolerance);
    EXPECT_LT(std::abs(m.find("0110")->second - 0.022981537137908348), tolerance);
    EXPECT_LT(std::abs(m.find("0001")->second - 0.177720207953642), tolerance);

    EXPECT_EQ(m.size(), 3);
}