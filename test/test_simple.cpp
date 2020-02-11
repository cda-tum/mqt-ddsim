#include "SimpleSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

TEST(SimulatorTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
	quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    SimpleSimulator ddsim(quantumComputation, 0);
    ddsim.Simulate();

    auto m = ddsim.MeasureAll(false);

    ASSERT_EQ("10", m);
}


TEST(SimulatorTest, DestructiveMeasurementAll) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    SimpleSimulator ddsim(quantumComputation, 0);
    ddsim.Simulate();


    const std::vector<dd::ComplexValue> v_before = ddsim.getVector();
    ASSERT_EQ(v_before[0], v_before[1]);
    ASSERT_EQ(v_before[0], v_before[2]);
    ASSERT_EQ(v_before[0], v_before[3]);

    const std::string m = ddsim.MeasureAll(true);
    const std::vector<dd::ComplexValue> v_after = ddsim.getVector();
    const int i = std::stoi(m, nullptr, 2);

    ASSERT_EQ(v_after[i].r, 1.0);
    ASSERT_EQ(v_after[i].i, 0.0);
}

TEST(SimulatorTest, DestructiveMeasurementOne) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    SimpleSimulator ddsim(quantumComputation, 0);
    ddsim.Simulate();

    const std::string m = ddsim.MeasureOneCollapsing(0);
    const std::vector<dd::ComplexValue> v_after = ddsim.getVector();

    if(m == "0") {
        ASSERT_EQ(v_after[0], (dd::ComplexValue{dd::SQRT_2, 0}));
        ASSERT_EQ(v_after[1], (dd::ComplexValue{dd::SQRT_2, 0}));
        ASSERT_EQ(v_after[2], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[3], (dd::ComplexValue{0, 0}));
    } else if (m == "1") {
        ASSERT_EQ(v_after[0], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[1], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[2], (dd::ComplexValue{dd::SQRT_2, 0}));
        ASSERT_EQ(v_after[3], (dd::ComplexValue{dd::SQRT_2, 0}));
    } else {
        FAIL() << "Measurement result not in {0,1}!";
    }
}
