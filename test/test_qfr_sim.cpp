#include "QFRSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

TEST(QFRSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
	quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    QFRSimulator ddsim(quantumComputation);
    ddsim.Simulate();

    auto m = ddsim.MeasureAll(false);

    ASSERT_EQ("01", m);
}

TEST(QFRSimTest, ClassicControlledOp) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    std::vector<unsigned short> qubit_to_measure = {0};
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, qubit_to_measure, qubit_to_measure);
    std::unique_ptr<qc::Operation> op (new qc::StandardOperation(2, 1, qc::X));
    auto classical_register = std::make_pair<unsigned short, unsigned short>(0,1);
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, classical_register, 1);

    QFRSimulator ddsim(quantumComputation);
    ddsim.Simulate();

    auto m = ddsim.MeasureAll(false);

    ASSERT_EQ("11", m);
}


TEST(QFRSimTest, DestructiveMeasurementAll) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    QFRSimulator ddsim(quantumComputation);
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

TEST(QFRSimTest, DestructiveMeasurementOne) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    QFRSimulator ddsim(quantumComputation);
    ddsim.Simulate();

    const char m = ddsim.MeasureOneCollapsing(0);
    const std::vector<dd::ComplexValue> v_after = ddsim.getVector();

    if(m == '0') {
        ASSERT_EQ(v_after[0], (dd::ComplexValue{dd::SQRT_2, 0}));
        ASSERT_EQ(v_after[2], (dd::ComplexValue{dd::SQRT_2, 0}));
        ASSERT_EQ(v_after[1], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[3], (dd::ComplexValue{0, 0}));
    } else if (m == '1') {
        ASSERT_EQ(v_after[0], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[2], (dd::ComplexValue{0, 0}));
        ASSERT_EQ(v_after[1], (dd::ComplexValue{dd::SQRT_2, 0}));
        ASSERT_EQ(v_after[3], (dd::ComplexValue{dd::SQRT_2, 0}));
    } else {
        FAIL() << "Measurement result not in {0,1}!";
    }
}
