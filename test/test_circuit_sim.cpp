#include "CircuitSimulator.hpp"
#include "algorithms/Grover.hpp"

#include <gtest/gtest.h>
#include <memory>

TEST(CircuitSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 1);

    ddsim.Simulate(1);

    auto m = ddsim.MeasureAll(false);

    ASSERT_EQ("01", m);
}

TEST(CircuitSimTest, SingleOneQubitSingleShot) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 1);

    ddsim.Simulate(10);
    ASSERT_EQ("1", ddsim.AdditionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, SingleOneQubitSingleShot2) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 2);

    ddsim.Simulate(10);
    ASSERT_EQ("1", ddsim.AdditionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, SingleOneQubitMultiShots) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 3);

    ddsim.Simulate(10);
    ASSERT_EQ("10", ddsim.AdditionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, ClassicControlledOp) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, quantumComputation->getCregs().at("c"), 1);

    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.Simulate(1);

    auto m = ddsim.MeasureAll(false);

    ASSERT_EQ("11", m);
}

TEST(CircuitSimTest, DestructiveMeasurementAll) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation));
    ddsim.Simulate(1);

    const std::vector<dd::ComplexValue> v_before = ddsim.getVector();
    ASSERT_EQ(v_before[0], v_before[1]);
    ASSERT_EQ(v_before[0], v_before[2]);
    ASSERT_EQ(v_before[0], v_before[3]);

    const std::string m       = ddsim.MeasureAll(true);
    const auto        v_after = ddsim.getVector();
    const int         i       = std::stoi(m, nullptr, 2);

    ASSERT_EQ(v_after[i].r, 1.0);
    ASSERT_EQ(v_after[i].i, 0.0);
}

TEST(CircuitSimTest, DestructiveMeasurementOne) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.Simulate(1);

    const char m       = ddsim.MeasureOneCollapsing(0);
    const auto v_after = ddsim.getVector();

    if (m == '0') {
        ASSERT_EQ(v_after[0], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[2], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[1], dd::complex_zero);
        ASSERT_EQ(v_after[3], dd::complex_zero);
    } else if (m == '1') {
        ASSERT_EQ(v_after[0], dd::complex_zero);
        ASSERT_EQ(v_after[2], dd::complex_zero);
        ASSERT_EQ(v_after[1], dd::complex_SQRT2_2);
        ASSERT_EQ(v_after[3], dd::complex_SQRT2_2);
    } else {
        FAIL() << "Measurement result not in {0,1}!";
    }

    const auto v_after_pairs = ddsim.getVectorPair();
    const auto v_after_compl = ddsim.getVectorComplex();

    assert(v_after_pairs.size() == v_after_compl.size());
    for (std::size_t i = 0; i < v_after_pairs.size(); i++) {
        ASSERT_EQ(v_after_pairs.at(i).first, v_after_compl.at(i).real());
        ASSERT_EQ(v_after_pairs.at(i).second, v_after_compl.at(i).imag());
    }
}

TEST(CircuitSimTest, ApproximateByFidelity) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, dd::Controls{dd::Control{0}, dd::Control{1}}, 2, qc::X);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    std::cout << ddsim.getActiveNodeCount() << "\n";
    ddsim.Simulate(1);
    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);

    double resulting_fidelity = ddsim.ApproximateByFidelity(0.3, false, true, true);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 4);
    ASSERT_DOUBLE_EQ(resulting_fidelity, 0.75); //equal up to 4 ULP
}

TEST(CircuitSimTest, ApproximateBySampling) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, dd::Controls{dd::Control{0}, dd::Control{1}}, 2, qc::X);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);

    double resulting_fidelity = ddsim.ApproximateBySampling(1, 0, true, true);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 3);
    ASSERT_LE(resulting_fidelity, 0.75); // the least contributing path has .25
}

TEST(CircuitSimTest, ApproximationByMemoryInSimulator) {
    std::unique_ptr<qc::QuantumComputation> quantumComputation = std::make_unique<qc::Grover>(17, 0);
    CircuitSimulator                        ddsim(std::move(quantumComputation), ApproximationInfo(0.3, 1, ApproximationInfo::MemoryDriven));
    ddsim.Simulate(1);

    // Memory driven approximation is triggered only on quite large DDs, unsuitable for testing
    // TODO Allow adjusting the limits at runtime?
}

TEST(CircuitSimTest, ApproximationByFidelityInSimulator) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, dd::Controls{dd::Control{0}, dd::Control{1}}, 2, qc::X);

    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::I); // some dummy operations
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::I);

    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(0.3, 1, ApproximationInfo::FidelityDriven));
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 4);
    ASSERT_LE(std::stod(ddsim.AdditionalStatistics()["final_fidelity"]), 0.75); // the least contributing path has .25
}

TEST(CircuitSimTest, GRCS4x4Test) {
    {
        CircuitSimulator ddsim(std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt"));
        auto             m = ddsim.Simulate(100);
        EXPECT_GT(m.size(), 0);
        ddsim.dd->cn.complexTable.printStatistics();
    }
    {
        CircuitSimulator ddsim(std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_1.txt"));
        auto             m = ddsim.Simulate(100);
        EXPECT_GT(m.size(), 0);
        ddsim.dd->cn.complexTable.printStatistics();
    }
    {
        CircuitSimulator ddsim(std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_2.txt"));
        auto             m = ddsim.Simulate(100);
        EXPECT_GT(m.size(), 0);
        ddsim.dd->cn.complexTable.printStatistics();
    }
}