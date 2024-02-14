#include "DeterministicNoiseSimulator.hpp"
#include "nlohmann/json.hpp"

#include "gtest/gtest.h"
#include <memory>

using namespace qc::literals;

std::unique_ptr<qc::QuantumComputation> detGetAdder4Circuit() {
    // circuit taken from https://github.com/pnnl/qasmbench
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
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
    return quantumComputation;
}

TEST(DeterministicNoiseSimTest, MeasurementOneSeedA) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->h(0);
    quantumComputation->cx(0, 1);
    quantumComputation->measure(0, 0);
    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), 5);
    EXPECT_THROW(ddsim->deterministicSimulate(), std::runtime_error);
}

TEST(DeterministicNoiseSimTest, MeasurementOneSeedB) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
    quantumComputation->h(0);
    quantumComputation->cx(0, 1);
    quantumComputation->measure(0, 0);
    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), 1);
    EXPECT_THROW(ddsim->deterministicSimulate(), std::runtime_error);
}

TEST(DeterministicNoiseSimTest, TestingBarrierGate) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->x(0);
    quantumComputation->barrier(0);
    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("A"), 0, 0, 1);
    auto m     = ddsim->deterministicSimulate();

    ASSERT_EQ(ddsim->getNumberOfOps(), 2);
    ASSERT_EQ(m.find("01")->second, 1);
    std::cout << ddsim->getName() << "\n";
}

TEST(DeterministicNoiseSimTest, TestingResetGate) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->reset(0);
    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation));

    EXPECT_THROW(ddsim->deterministicSimulate(), std::runtime_error);
}

TEST(DeterministicNoiseSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->x(0);
    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("A"), 0, 0, 1);
    auto m     = ddsim->deterministicSimulate();

    ASSERT_EQ(ddsim->getNumberOfOps(), 1);
    ASSERT_EQ(m.find("01")->second, 1);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPDApplySequential) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("APD"), 0.01, std::optional<double>{}, 2, true);

    auto m = ddsim->deterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    const double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0000")->second, 0.0969332192741, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 0.1731941264570, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 0.0238203475524, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 0.0242454336917, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 0.0141409660985, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 0.0138062113213, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 0.0244576087400, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 0.0239296920989, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 0.0907888041538, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.4145855071998, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 0.0235097990017, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 0.0262779844799, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 0.0184033482066, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 0.0116282811276, tolerance);
    EXPECT_NEAR(m.find("1111")->second, 0.0110373166627, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackD) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("D"), 0.01, std::optional<double>{}, 2);

    auto m = ddsim->deterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    const double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0000")->second, 0.0332328704931, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 0.0683938280189, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 0.0117061689898, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 0.0129643065735, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 0.0107812802908, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 0.0160082331009, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 0.0328434857577, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.7370101351171, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 0.0186346925411, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 0.0275086747656, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPD) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("APD"), 0.01, std::optional<double>{}, 2);

    auto m = ddsim->deterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    const double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0000")->second, 0.0969332192741, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 0.1731941264570, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 0.0238203475524, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 0.0242454336917, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 0.0141409660985, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 0.0138062113213, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 0.0244576087400, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 0.0239296920989, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 0.0907888041538, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.4145855071998, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 0.0235097990017, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 0.0262779844799, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 0.0184033482066, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 0.0116282811276, tolerance);
    EXPECT_NEAR(m.find("1111")->second, 0.0110373166627, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAP) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("AP"), 0.001, std::optional<double>{}, 2);

    auto m = ddsim->deterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    const double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0001")->second, 0.03008702498522842, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.9364832248561167, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateRunWithBadParameters) {
    EXPECT_THROW(std::make_unique<DeterministicNoiseSimulator<>>(detGetAdder4Circuit(), std::string("APD"), 0.3, std::optional<double>{}, 2), std::runtime_error);
    EXPECT_THROW(std::make_unique<DeterministicNoiseSimulator<>>(detGetAdder4Circuit(), std::string("APK"), 0.001, std::optional<double>{}, 2), std::runtime_error);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPDCustomProb) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("APD"), 0.01, 0.02, 1);

    auto m = ddsim->deterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    const double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0000")->second, 0.0616548044047, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 0.1487734834937, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 0.0155601736851, tolerance);
    EXPECT_NEAR(m.find("0011")->second, 0.0166178042857, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 0.0301651684817, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 0.0301853251959, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 0.0570878674208, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.5519250213313, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 0.0157508473593, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 0.0187340765889, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 0.0132640682125, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4NoNoise1) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("I"), 0.01, 0.02, 1);

    auto m = ddsim->deterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    const double tolerance = 1e-10;
    EXPECT_NEAR(m.find("1001")->second, 1, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4NoNoise2) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("APD"), 0, 0, 1);

    auto m = ddsim->deterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    const double tolerance = 1e-10;
    EXPECT_NEAR(m.find("1001")->second, 1, tolerance);
}

TEST(DeterministicNoiseSimTest, TestFunctionsOptimized) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("APD"), 0.01, 0.02, 1);
    auto m                  = ddsim->deterministicSimulate();

    EXPECT_EQ(ddsim->getNumberOfQubits(), 4);
    EXPECT_EQ(ddsim->getActiveNodeCount(), 22);
    EXPECT_EQ(ddsim->getMaxNodeCount(), 44);
    EXPECT_EQ(ddsim->getMaxMatrixNodeCount(), 0);
    EXPECT_EQ(ddsim->getMatrixActiveNodeCount(), 0);
    EXPECT_EQ(ddsim->countNodesFromRoot(), 23);
}

TEST(DeterministicNoiseSimTest, sampleFromProbabilityMap1) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);

    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("APD"), 0, 0, 1, true);
    auto m     = ddsim->deterministicSimulate();

    const std::size_t shots        = 100000;
    auto              sampledShots = ddsim->sampleFromProbabilityMap(m, shots);

    double const tolerance = 0.01;
    for (const auto& [state, prob]: m) {
        const auto relFreq = static_cast<dd::fp>(sampledShots.find(state)->second) / shots;
        EXPECT_NEAR(prob, relFreq, tolerance);
    }
}

TEST(DeterministicNoiseSimTest, sampleFromProbabilityMap2) {
    auto quantumComputation = detGetAdder4Circuit();

    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), std::string("AP"), 0.1, 0.2, 1, true);

    const std::size_t shots        = 1000000;
    auto              sampledShots = ddsim->simulate(shots);
    auto              m            = ddsim->deterministicSimulate();

    double const tolerance = 0.01;
    for (const auto& [state, prob]: m) {
        const auto relFreq = static_cast<dd::fp>(sampledShots.find(state)->second) / shots;
        EXPECT_NEAR(prob, relFreq, tolerance);
    }
}
