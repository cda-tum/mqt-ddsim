#include "DeterministicNoiseSimulator.hpp"
#include "nlohmann/json.hpp"

#include "gtest/gtest.h"
#include <memory>

using namespace dd::literals;

std::unique_ptr<qc::QuantumComputation> detGetAdder4Circuit() {
    // circuit taken from https://github.com/pnnl/qasmbench
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->x(0);
    quantumComputation->x(1);
    quantumComputation->h(3);
    quantumComputation->x(3, 2_pc);
    quantumComputation->t(0);
    quantumComputation->t(1);
    quantumComputation->t(2);
    quantumComputation->tdag(3);
    quantumComputation->x(1, 0_pc);
    quantumComputation->x(3, 2_pc);
    quantumComputation->x(0, 3_pc);
    quantumComputation->x(2, 1_pc);
    quantumComputation->x(1, 0_pc);
    quantumComputation->x(3, 2_pc);
    quantumComputation->tdag(0);
    quantumComputation->tdag(1);
    quantumComputation->tdag(2);
    quantumComputation->t(3);
    quantumComputation->x(1, 0_pc);
    quantumComputation->x(3, 2_pc);
    quantumComputation->s(3);
    quantumComputation->x(0, 3_pc);
    quantumComputation->h(3);
    return quantumComputation;
}

TEST(DeterministicNoiseSimTest, MeasurementOne) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(2, dd::Controls{dd::Control{0}}, 1, qc::X);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, 0, qc::Measure);

    {
        auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, 5);
        EXPECT_THROW(ddsim->DeterministicSimulate();, std::invalid_argument);
    }

    {
        auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, 1);
        EXPECT_THROW(ddsim->DeterministicSimulate();, std::invalid_argument);
    }
}

TEST(DeterministicNoiseSimTest, TestingBarrierGate) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->x(0);
    quantumComputation->barrier(0);
    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("A"), 0, 0, 1);
    auto m     = ddsim->DeterministicSimulate();

    ASSERT_EQ(ddsim->getNumberOfOps(), 2);
    ASSERT_EQ(m.find("10")->second, 1);
    std::cout << ddsim->getName() << "\n";
}

TEST(DeterministicNoiseSimTest, TestingResetGate) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(2, std::vector<dd::Qubit>(0), qc::Reset);
    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation);

    EXPECT_THROW(ddsim->DeterministicSimulate(), std::runtime_error);
}

TEST(DeterministicNoiseSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
    quantumComputation->emplace_back<qc::StandardOperation>(2, 0, qc::X);
    auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("A"), 0, 0, 1);
    auto m     = ddsim->DeterministicSimulate();

    ASSERT_EQ(ddsim->getNumberOfOps(), 1);
    ASSERT_EQ(m.find("10")->second, 1);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPDApplySequential) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("APD"), 0.01, std::optional<double>{}, 2, true);

    auto m = ddsim->DeterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0000")->second, 0.0969332192741, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 0.0907888041538, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 0.0141409660985, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 0.0238203475524, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 0.0235097990017, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 0.0244576087400, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 0.0116282811276, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 0.1731941264570, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.4145855071998, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 0.0138062113213, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 0.0184033482066, tolerance);
    EXPECT_NEAR(m.find("1100")->second, 0.0242454336917, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 0.0262779844799, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 0.0239296920989, tolerance);
    EXPECT_NEAR(m.find("1111")->second, 0.0110373166627, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4Track_D) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("D"), 0.01, std::optional<double>{}, 2);

    auto m = ddsim->DeterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0000")->second, 0.0332328704931, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 0.0328434857577, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 0.0129643065735, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 0.0683938280189, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.7370101351171, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 0.0107812802908, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 0.0275086747656, tolerance);
    EXPECT_NEAR(m.find("1100")->second, 0.0117061689898, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 0.0186346925411, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 0.0160082331009, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPD) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("APD"), 0.01, std::optional<double>{}, 2);

    auto m = ddsim->DeterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0000")->second, 0.0969332192741, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 0.0907888041538, tolerance);
    EXPECT_NEAR(m.find("0010")->second, 0.0141409660985, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 0.0238203475524, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 0.0235097990017, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 0.0244576087400, tolerance);
    EXPECT_NEAR(m.find("0111")->second, 0.0116282811276, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 0.1731941264570, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.4145855071998, tolerance);
    EXPECT_NEAR(m.find("1010")->second, 0.0138062113213, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 0.0184033482066, tolerance);
    EXPECT_NEAR(m.find("1100")->second, 0.0242454336917, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 0.0262779844799, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 0.0239296920989, tolerance);
    EXPECT_NEAR(m.find("1111")->second, 0.0110373166627, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAP) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("AP"), 0.001, std::optional<double>{}, 2);

    auto m = ddsim->DeterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    double tolerance = 1e-10;
    EXPECT_NEAR(m.find("1000")->second, 0.03008702498522842, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.9364832248561167, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateRunWithBadParameters) {
    auto quantumComputation = detGetAdder4Circuit();
    EXPECT_THROW(std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("APD"), 0.3, std::optional<double>{}, 2), std::runtime_error);
    EXPECT_THROW(std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("APK"), 0.001, std::optional<double>{}, 2), std::runtime_error);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPDCustomProb) {
    auto quantumComputation = detGetAdder4Circuit();
    auto ddsim              = std::make_unique<DeterministicNoiseSimulator<>>(quantumComputation, std::string("APD"), 0.01, 0.02, 1);

    auto m = ddsim->DeterministicSimulate();
    std::cout << std::setw(2) << nlohmann::json(m) << "\n";

    double tolerance = 1e-10;
    EXPECT_NEAR(m.find("0000")->second, 0.0616548044047, tolerance);
    EXPECT_NEAR(m.find("0001")->second, 0.0570878674208, tolerance);
    EXPECT_NEAR(m.find("0100")->second, 0.0155601736851, tolerance);
    EXPECT_NEAR(m.find("0101")->second, 0.0157508473593, tolerance);
    EXPECT_NEAR(m.find("0110")->second, 0.0301651684817, tolerance);
    EXPECT_NEAR(m.find("1000")->second, 0.1487734834937, tolerance);
    EXPECT_NEAR(m.find("1001")->second, 0.5519250213313, tolerance);
    EXPECT_NEAR(m.find("1011")->second, 0.0132640682125, tolerance);
    EXPECT_NEAR(m.find("1100")->second, 0.0166178042857, tolerance);
    EXPECT_NEAR(m.find("1101")->second, 0.0187340765889, tolerance);
    EXPECT_NEAR(m.find("1110")->second, 0.0301853251959, tolerance);
}
