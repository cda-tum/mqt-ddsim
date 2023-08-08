#include "CircuitSimulator.hpp"
#include "algorithms/BernsteinVazirani.hpp"
#include "algorithms/Grover.hpp"

#include <gtest/gtest.h>
#include <memory>

TEST(CircuitSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 1);

    ddsim.simulate(1);

    EXPECT_EQ(ddsim.getMaxNodeCount(), 4);

    auto m = ddsim.measureAll(false);

    ASSERT_EQ("01", m);
}

TEST(CircuitSimTest, SingleOneQubitSingleShot) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 1);

    ddsim.simulate(10);
    ASSERT_EQ("1", ddsim.additionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, SingleOneQubitSingleShot2) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(), 1337);

    ASSERT_EQ(ddsim.getNumberOfOps(), 2);

    ddsim.simulate(10);
    ASSERT_EQ("1", ddsim.additionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, SingleOneQubitMultiShots) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 3);

    ddsim.simulate(10);
    ASSERT_EQ("10", ddsim.additionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, BarrierStatement) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(1);
    quantumComputation->emplace_back<qc::StandardOperation>(1, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(1, std::vector<qc::Qubit>{0}, qc::Barrier);
    quantumComputation->emplace_back<qc::StandardOperation>(1, 0, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo());

    ASSERT_EQ(ddsim.getNumberOfOps(), 3);

    ddsim.simulate(10);
    ASSERT_EQ("1", ddsim.additionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, ClassicControlledOp) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, quantumComputation->getCregs().at("c"), 1);

    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);

    auto m = ddsim.measureAll(false);

    ASSERT_EQ("11", m);
}

TEST(CircuitSimTest, ClassicControlledOpAsNop) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, 0);
    std::unique_ptr<qc::Operation> op(new qc::StandardOperation(2, 1, qc::X));
    quantumComputation->emplace_back<qc::ClassicControlledOperation>(op, quantumComputation->getCregs().at("c"), 0);

    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);

    auto m = ddsim.measureAll(false);

    ASSERT_EQ("01", m);
}

TEST(CircuitSimTest, DestructiveMeasurementAll) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation), 42);
    ddsim.simulate(1);

    const std::vector<dd::ComplexValue> vBefore = ddsim.getVector();
    ASSERT_EQ(vBefore[0], vBefore[1]);
    ASSERT_EQ(vBefore[0], vBefore[2]);
    ASSERT_EQ(vBefore[0], vBefore[3]);

    const std::string m      = ddsim.measureAll(true);
    const auto        vAfter = ddsim.getVector();
    const std::size_t i      = std::stoul(m, nullptr, 2);

    ASSERT_EQ(vAfter[i].r, 1.0);
    ASSERT_EQ(vAfter[i].i, 0.0);
}

TEST(CircuitSimTest, DestructiveMeasurementOne) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 1, qc::H);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);

    const char m      = ddsim.measureOneCollapsing(0);
    const auto vAfter = ddsim.getVector();

    if (m == '0') {
        ASSERT_EQ(vAfter[0], dd::complex_SQRT2_2);
        ASSERT_EQ(vAfter[2], dd::complex_SQRT2_2);
        ASSERT_EQ(vAfter[1], dd::complex_zero);
        ASSERT_EQ(vAfter[3], dd::complex_zero);
    } else if (m == '1') {
        ASSERT_EQ(vAfter[0], dd::complex_zero);
        ASSERT_EQ(vAfter[2], dd::complex_zero);
        ASSERT_EQ(vAfter[1], dd::complex_SQRT2_2);
        ASSERT_EQ(vAfter[3], dd::complex_SQRT2_2);
    } else {
        FAIL() << "Measurement result not in {0,1}!";
    }

    const auto vAfterPairs = ddsim.getVector<std::pair<dd::fp, dd::fp>>();
    const auto vAfterCompl = ddsim.getVector<std::complex<dd::fp>>();

    ASSERT_EQ(vAfterPairs.size(), vAfterCompl.size());
    for (std::size_t i = 0; i < vAfterPairs.size(); i++) {
        ASSERT_EQ(vAfterPairs.at(i).first, vAfterCompl.at(i).real());
        ASSERT_EQ(vAfterPairs.at(i).second, vAfterCompl.at(i).imag());
    }
}

TEST(CircuitSimTest, ApproximateByFidelity) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, qc::Controls{qc::Control{0}, qc::Control{1}}, 2, qc::X);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    std::cout << ddsim.getActiveNodeCount() << "\n";
    ddsim.simulate(1);
    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);

    double const resultingFidelity = ddsim.approximateByFidelity(0.3, false, true, true);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 3);
    ASSERT_DOUBLE_EQ(resultingFidelity, 0.5); //equal up to 4 ULP
}

TEST(CircuitSimTest, ApproximateBySampling) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, qc::Controls{qc::Control{0}, qc::Control{1}}, 2, qc::X);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 6);

    double const resultingFidelity = ddsim.approximateBySampling(1, 0, true, true);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 3);
    ASSERT_LE(resultingFidelity, 0.75); // the least contributing path has .25
}

TEST(CircuitSimTest, ApproximationByMemoryInSimulator) {
    std::unique_ptr<qc::QuantumComputation> quantumComputation = std::make_unique<qc::Grover>(17, 0);
    CircuitSimulator                        ddsim(std::move(quantumComputation), ApproximationInfo(0.3, 1, ApproximationInfo::MemoryDriven));
    ddsim.simulate(1);

    // Memory driven approximation is triggered only on quite large DDs, unsuitable for testing
    // TODO Allow adjusting the limits at runtime?
}

TEST(CircuitSimTest, ApproximationByFidelityInSimulator) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, qc::Controls{qc::Control{0}, qc::Control{1}}, 2, qc::X);

    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::I); // some dummy operations
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::I);

    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(0.3, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);

    ASSERT_EQ(ddsim.getActiveNodeCount(), 3);
    ASSERT_DOUBLE_EQ(std::stod(ddsim.additionalStatistics()["final_fidelity"]), 0.5);
}

TEST(CircuitSimTest, GRCS4x4Test) {
    {
        CircuitSimulator ddsim(std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt"));
        auto             m = ddsim.simulate(100);
        EXPECT_GT(m.size(), 0);
    }
    {
        CircuitSimulator ddsim(std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_1.txt"));
        auto             m = ddsim.simulate(100);
        EXPECT_GT(m.size(), 0);
    }
    {
        CircuitSimulator ddsim(std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_2.txt"));
        auto             m = ddsim.simulate(100);
        EXPECT_GT(m.size(), 0);
    }
}

TEST(CircuitSimTest, TestingProperties) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, qc::Controls{qc::Control{0}, qc::Control{1}}, 2, qc::X);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven), 1);
    ddsim.simulate(1);

    EXPECT_EQ(ddsim.getActiveNodeCount(), 6);
    EXPECT_EQ(ddsim.getMaxMatrixNodeCount(), 0);
    EXPECT_EQ(ddsim.getMatrixActiveNodeCount(), 0);
    EXPECT_EQ(ddsim.countNodesFromRoot(), 7);
    EXPECT_EQ(ddsim.getSeed(), "1");
    EXPECT_EQ(ddsim.additionalStatistics().at("approximation_runs"), "0");
}

TEST(CircuitSimTest, ApproximationTest) {
    // the following creates a state where the first qubit has a <2% probability of being 1
    auto qc = std::make_unique<qc::QuantumComputation>(2);
    qc->h(0);
    qc->ry(1, qc::Control{0}, qc::PI / 8);

    // approximating the state with fidelity 0.98 should allow to eliminate the 1-successor of the first qubit
    CircuitSimulator ddsim(std::move(qc), ApproximationInfo(0.98, 2, ApproximationInfo::FidelityDriven));
    ddsim.simulate(4096);
    const auto vec = ddsim.getVector<std::complex<dd::fp>>();
    EXPECT_EQ(abs(vec[2]), 0);
    EXPECT_EQ(abs(vec[3]), 0);
}

TEST(CircuitSimTest, expectationValueLocalOperators) {
    const std::size_t maxQubits = 3;
    for (std::size_t nrQubits = 1; nrQubits < maxQubits; ++nrQubits) {
        auto             qc = std::make_unique<qc::QuantumComputation>(nrQubits);
        CircuitSimulator ddsim(std::move(qc));
        for (qc::Qubit site = 0; site < nrQubits; ++site) {
            auto xObservable = qc::QuantumComputation(nrQubits);
            xObservable.x(site);
            EXPECT_EQ(ddsim.expectationValue(xObservable), 0);
            auto zObservable = qc::QuantumComputation(nrQubits);
            zObservable.z(site);
            EXPECT_EQ(ddsim.expectationValue(zObservable), 1);
            auto hObservable = qc::QuantumComputation(nrQubits);
            hObservable.h(site);
            EXPECT_EQ(ddsim.expectationValue(hObservable), dd::SQRT2_2);
        }
    }
}

TEST(CircuitSimTest, expectationValueGlobalOperators) {
    const std::size_t maxQubits = 3;
    for (std::size_t nrQubits = 1; nrQubits < maxQubits; ++nrQubits) {
        auto             qc = std::make_unique<qc::QuantumComputation>(nrQubits);
        CircuitSimulator ddsim(std::move(qc));
        auto             xObservable = qc::QuantumComputation(nrQubits);
        auto             zObservable = qc::QuantumComputation(nrQubits);
        auto             hObservable = qc::QuantumComputation(nrQubits);
        for (qc::Qubit site = 0; site < nrQubits; ++site) {
            xObservable.x(site);
            zObservable.z(site);
            hObservable.h(site);
        }
        EXPECT_EQ(ddsim.expectationValue(xObservable), 0);
        EXPECT_EQ(ddsim.expectationValue(zObservable), 1);
        EXPECT_EQ(ddsim.expectationValue(hObservable), std::pow(dd::SQRT2_2, nrQubits));
    }
}

TEST(CircuitSimTest, ToleranceTest) {
    // A small test to make sure that setting and getting the tolerance works
    auto             qc = std::make_unique<qc::QuantumComputation>(2);
    CircuitSimulator ddsim(std::move(qc));
    const auto       tolerance    = ddsim.getTolerance();
    const auto       newTolerance = 0.1;
    ddsim.setTolerance(newTolerance);
    EXPECT_EQ(ddsim.getTolerance(), newTolerance);
    ddsim.setTolerance(tolerance);
    EXPECT_EQ(ddsim.getTolerance(), tolerance);
}

TEST(CircuitSimTest, TooManyQubitsForVectorTest) {
    auto             qc = std::make_unique<qc::QuantumComputation>(61);
    CircuitSimulator ddsim(std::move(qc));
    ddsim.simulate(0);
    EXPECT_THROW({ [[maybe_unused]] auto _ = ddsim.getVector<std::complex<dd::fp>>(); }, std::range_error);
}

TEST(CircuitSimTest, BernsteinVaziraniTest) {
    std::size_t const n       = 3;
    auto              qc      = std::make_unique<qc::BernsteinVazirani>(n, true);
    auto              circSim = std::make_unique<CircuitSimulator<>>(std::move(qc), 23);
    const auto        result  = circSim->simulate(1024U);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(qc->expected), 1024);
}
