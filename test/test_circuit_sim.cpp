#include "CircuitSimulator.hpp"
#include "algorithms/BernsteinVazirani.hpp"
#include "algorithms/Grover.hpp"
#include "algorithms/QFT.hpp"
#include "algorithms/QPE.hpp"

#include "gtest/gtest.h"
#include <memory>

TEST(CircuitSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->x(0);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 1);

    ddsim.simulate(1);

    EXPECT_EQ(ddsim.getMaxNodeCount(), 4);

    auto m = ddsim.measureAll(false);

    ASSERT_EQ("01", m);
}

TEST(CircuitSimTest, SingleOneQubitSingleShot) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->h(0);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 1);

    ddsim.simulate(10);
    ASSERT_EQ("1", ddsim.additionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, SingleOneQubitSingleShot2) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->h(0);
    quantumComputation->measure(0, 0);
    std::cout << *quantumComputation << "\n";
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(), 1337);

    ASSERT_EQ(ddsim.getNumberOfOps(), 2);

    ddsim.simulate(10);
    ASSERT_EQ("1", ddsim.additionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, SingleOneQubitMultiShots) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->h(0);
    quantumComputation->measure(0, 0);
    quantumComputation->h(0);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ASSERT_EQ(ddsim.getNumberOfOps(), 3);

    ddsim.simulate(10);
    ASSERT_EQ("10", ddsim.additionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, BarrierStatement) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(1);
    quantumComputation->h(0);
    quantumComputation->barrier(0);
    quantumComputation->h(0);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo());

    ASSERT_EQ(ddsim.getNumberOfOps(), 3);

    ddsim.simulate(10);
    ASSERT_EQ("1", ddsim.additionalStatistics().at("single_shots"));
}

TEST(CircuitSimTest, ClassicControlledOp) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->x(0);
    quantumComputation->measure(0, 0);
    quantumComputation->classicControlled(qc::X, 1U, quantumComputation->getCregs().at("c"));

    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);

    auto m = ddsim.measureAll(false);

    ASSERT_EQ("11", m);
}

TEST(CircuitSimTest, ClassicControlledOpAsNop) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->x(0);
    quantumComputation->measure(0, 0);
    quantumComputation->classicControlled(qc::X, 1U, quantumComputation->getCregs().at("c"), 0);

    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);

    auto m = ddsim.measureAll(false);

    ASSERT_EQ("01", m);
}

TEST(CircuitSimTest, DestructiveMeasurementAll) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->h(0);
    quantumComputation->h(1);
    CircuitSimulator ddsim(std::move(quantumComputation), 42);
    ddsim.simulate(1);

    const auto vBefore = ddsim.getVector();
    ASSERT_EQ(vBefore[0], vBefore[1]);
    ASSERT_EQ(vBefore[0], vBefore[2]);
    ASSERT_EQ(vBefore[0], vBefore[3]);

    const std::string m      = ddsim.measureAll(true);
    const auto        vAfter = ddsim.getVector();
    const std::size_t i      = std::stoul(m, nullptr, 2);

    ASSERT_EQ(vAfter[i].real(), 1.0);
    ASSERT_EQ(vAfter[i].imag(), 0.0);
}

TEST(CircuitSimTest, DestructiveMeasurementOne) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->h(0);
    quantumComputation->h(1);
    CircuitSimulator ddsim(std::move(quantumComputation), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);

    const char m      = ddsim.measureOneCollapsing(0);
    const auto vAfter = ddsim.getVector();

    if (m == '0') {
        ASSERT_EQ(vAfter[0], dd::SQRT2_2);
        ASSERT_EQ(vAfter[2], dd::SQRT2_2);
        ASSERT_EQ(vAfter[1], 0.);
        ASSERT_EQ(vAfter[3], 0.);
    } else if (m == '1') {
        ASSERT_EQ(vAfter[0], 0.);
        ASSERT_EQ(vAfter[2], 0.);
        ASSERT_EQ(vAfter[1], dd::SQRT2_2);
        ASSERT_EQ(vAfter[3], dd::SQRT2_2);
    } else {
        FAIL() << "Measurement result not in {0,1}!";
    }
}

TEST(CircuitSimTest, ApproximateByFidelity) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->h(0);
    quantumComputation->h(1);
    quantumComputation->mcx(qc::Controls{qc::Control{0}, qc::Control{1}}, 2);
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
    quantumComputation->h(0);
    quantumComputation->h(1);
    quantumComputation->mcx(qc::Controls{qc::Control{0}, qc::Control{1}}, 2);
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
    quantumComputation->h(0);
    quantumComputation->h(1);
    quantumComputation->mcx(qc::Controls{qc::Control{0}, qc::Control{1}}, 2);

    quantumComputation->i(1); // some dummy operations
    quantumComputation->i(1);

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
    quantumComputation->h(0);
    quantumComputation->h(1);
    quantumComputation->mcx(qc::Controls{qc::Control{0}, qc::Control{1}}, 2);
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
    qc->cry(qc::PI / 8, 0, 1);

    // approximating the state with fidelity 0.98 should allow to eliminate the 1-successor of the first qubit
    CircuitSimulator ddsim(std::move(qc), ApproximationInfo(0.98, 2, ApproximationInfo::FidelityDriven));
    ddsim.simulate(4096);
    const auto vec = ddsim.getVector();
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
    EXPECT_THROW({ [[maybe_unused]] auto _ = ddsim.getVector(); }, std::range_error);
}

TEST(CircuitSimTest, BernsteinVaziraniDynamicTest) {
    std::size_t const n        = 3;
    auto              qc       = std::make_unique<qc::BernsteinVazirani>(n, true);
    const auto        expected = qc->expected; // qc will be undefined after move
    auto              circSim  = std::make_unique<CircuitSimulator<>>(std::move(qc), 23);
    const auto        result   = circSim->simulate(1024U);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(expected), 1024);
}

TEST(CircuitSimTest, QPEDynamicTest) {
    std::size_t const n       = 3;
    auto              qc      = std::make_unique<qc::QPE>(n, true, true);
    auto              circSim = std::make_unique<CircuitSimulator<>>(std::move(qc), 23);
    const auto        result  = circSim->simulate(1024U);
    EXPECT_GE(result.size(), 1);
}

TEST(CircuitSimTest, QFTDynamicTest) {
    std::size_t const n       = 3;
    auto              qc      = std::make_unique<qc::QFT>(n, true, true);
    auto              circSim = std::make_unique<CircuitSimulator<>>(std::move(qc), 23);
    const auto        result  = circSim->simulate(1024U);
    EXPECT_GE(result.size(), 1);
}
