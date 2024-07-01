#include "DeterministicNoiseSimulator.hpp"
#include "QuantumComputation.hpp"
#include "operations/OpType.hpp"

#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

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

TEST(DeterministicNoiseSimTest, TestingBarrierGate) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
  quantumComputation->x(0);
  quantumComputation->barrier(0);
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("A"), 0, 0, 1);
  double const measurementThreshold = 0.01;
  ddsim->simulate(1);
  auto m = ddsim->rootEdge.getSparseProbabilityVectorStrKeys(
      ddsim->getNumberOfQubits(), measurementThreshold);

  ASSERT_EQ(ddsim->getNumberOfOps(), 2);
  ASSERT_EQ(m.find("01")->second, 1);
  std::cout << ddsim->getName() << "\n";
}

TEST(DeterministicNoiseSimTest, TestingResetGate) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
  quantumComputation->x(0);
  quantumComputation->x(1);
  quantumComputation->reset(0);
  quantumComputation->measure(0, 0);
  quantumComputation->measure(1, 1);

  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("A"), 0, 0, 1);
  auto m = ddsim->simulate(1000);

  ASSERT_EQ(m.find("10")->second, 1000);
}

TEST(DeterministicNoiseSimTest, ClassicControlledOp) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(2, 2);
  quantumComputation->x(0);
  quantumComputation->measure(0, 0);
  quantumComputation->classicControlled(qc::X, 1U, {0, 1});
  quantumComputation->measure(0, 0);
  quantumComputation->measure(1, 1);

  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("A"), 0, 0, 1);
  auto m = ddsim->simulate(1000);

  ASSERT_EQ(m.find("11")->second, 1000);
}

TEST(DeterministicNoiseSimTest, SingleOneQubitGateOnTwoQubitCircuit) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(2);
  quantumComputation->x(0);
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("A"), 0, 0, 1);
  auto m = ddsim->simulate(1000);

  ASSERT_EQ(ddsim->getNumberOfOps(), 1);
  ASSERT_EQ(m.find("01")->second, 1000);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPDWithSimulate) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), ApproximationInfo{}, 17U,
      std::string("APD"), 0.01, std::optional<double>{}, 2);
  auto m = ddsim->simulate(10000);

  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const auto expectedEntries =
      std::array{"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
                 "1000", "1001", "1010", "1011", "1101", "1110", "1111"};
  const auto expectedValues =
      std::array{969, 1731, 238, 242, 141, 138, 244, 239,
                 907, 4145, 235, 262, 184, 116, 110};
  const auto tolerance = 100;
  for (std::size_t i = 0; i < expectedEntries.size(); ++i) {
    if (m.count(expectedEntries.at(i)) == 0) {
      FAIL() << "Expected entry " << expectedEntries.at(i)
             << " not found in result";
    }
    EXPECT_NEAR(static_cast<double>(m.at(expectedEntries.at(i))),
                expectedValues.at(i), tolerance);
  }
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackD) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("D"), 0.01,
      std::optional<double>{}, 2);

  double const measurementThreshold = 0.01;
  ddsim->simulate(1);
  auto m = ddsim->rootEdge.getSparseProbabilityVectorStrKeys(
      ddsim->getNumberOfQubits(), measurementThreshold);

  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const auto expectedEntries =
      std::array{"0000", "0001", "0011", "0100", "0101",
                 "0111", "1000", "1001", "1011", "1101"};
  const auto expectedValues = std::array{
      0.0332328704931, 0.0683938280189, 0.0117061689898, 0.0129643065735,
      0.0107812802908, 0.0160082331009, 0.0328434857577, 0.7370101351171,
      0.0186346925411, 0.0275086747656};
  const auto tolerance = 1e-10;
  for (std::size_t i = 0; i < expectedEntries.size(); ++i) {
    if (m.count(expectedEntries.at(i)) == 0) {
      FAIL() << "Expected entry " << expectedEntries.at(i)
             << " not found in result";
    }
    EXPECT_NEAR(m.at(expectedEntries.at(i)), expectedValues.at(i), tolerance);
  }
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPD) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("APD"), 0.01,
      std::optional<double>{}, 2);

  double const measurementThreshold = 0.01;
  ddsim->simulate(1);
  auto m = ddsim->rootEdge.getSparseProbabilityVectorStrKeys(
      ddsim->getNumberOfQubits(), measurementThreshold);

  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const auto expectedEntries =
      std::array{"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
                 "1000", "1001", "1010", "1011", "1101", "1110", "1111"};
  const auto expectedValues = std::array{
      0.0969332192741, 0.1731941264570, 0.0238203475524, 0.0242454336917,
      0.0141409660985, 0.0138062113213, 0.0244576087400, 0.0239296920989,
      0.0907888041538, 0.4145855071998, 0.0235097990017, 0.0262779844799,
      0.0184033482066, 0.0116282811276, 0.0110373166627};
  const auto tolerance = 1e-10;
  for (std::size_t i = 0; i < expectedEntries.size(); ++i) {
    if (m.count(expectedEntries.at(i)) == 0) {
      FAIL() << "Expected entry " << expectedEntries.at(i)
             << " not found in result";
    }
    EXPECT_NEAR(m.at(expectedEntries.at(i)), expectedValues.at(i), tolerance);
  }
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAP) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("AP"), 0.001,
      std::optional<double>{}, 2);

  double const measurementThreshold = 0.01;
  ddsim->simulate(1);
  auto m = ddsim->rootEdge.getSparseProbabilityVectorStrKeys(
      ddsim->getNumberOfQubits(), measurementThreshold);

  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const auto expectedEntries = std::array{"0001", "1001"};
  const auto expectedValues =
      std::array{0.03008702498522842, 0.9364832248561167};
  const auto tolerance = 1e-10;
  for (std::size_t i = 0; i < expectedEntries.size(); ++i) {
    if (m.count(expectedEntries.at(i)) == 0) {
      FAIL() << "Expected entry " << expectedEntries.at(i)
             << " not found in result";
    }
    EXPECT_NEAR(m.at(expectedEntries.at(i)), expectedValues.at(i), tolerance);
  }
}

TEST(DeterministicNoiseSimTest, SimulateRunWithBadParameters) {
  EXPECT_THROW(std::make_unique<DeterministicNoiseSimulator>(
                   detGetAdder4Circuit(), std::string("APD"), 0.3,
                   std::optional<double>{}, 2),
               std::runtime_error);
  EXPECT_THROW(std::make_unique<DeterministicNoiseSimulator>(
                   detGetAdder4Circuit(), std::string("APK"), 0.001,
                   std::optional<double>{}, 2),
               std::runtime_error);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPDCustomProb) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("APD"), 0.01, 0.02, 1);

  double const measurementThreshold = 0.01;
  ddsim->simulate(1);
  auto m = ddsim->rootEdge.getSparseProbabilityVectorStrKeys(
      ddsim->getNumberOfQubits(), measurementThreshold);

  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const auto expectedEntries =
      std::array{"0000", "0001", "0010", "0011", "0110", "0111",
                 "1000", "1001", "1010", "1011", "1101"};
  const auto expectedValues = std::array{
      0.0616548044047, 0.1487734834937, 0.0155601736851, 0.0166178042857,
      0.0301651684817, 0.0301853251959, 0.0570878674208, 0.5519250213313,
      0.0157508473593, 0.0187340765889, 0.0132640682125};
  const auto tolerance = 1e-10;
  for (std::size_t i = 0; i < expectedEntries.size(); ++i) {
    if (m.count(expectedEntries.at(i)) == 0) {
      FAIL() << "Expected entry " << expectedEntries.at(i)
             << " not found in result";
    }
    EXPECT_NEAR(m.at(expectedEntries.at(i)), expectedValues.at(i), tolerance);
  }
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPDWithShots) {
  auto quantumComputation = detGetAdder4Circuit();

  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("APD"), 0.01, 0.02, 1);

  auto m = ddsim->simulate(10000);
  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const auto expectedEntries = std::array{"0000", "0001", "1000", "1001"};
  const auto expectedValues = std::array{616, 1487, 570, 5519};
  const std::size_t tolerance = 500;
  for (std::size_t i = 0; i < expectedEntries.size(); ++i) {
    if (m.count(expectedEntries.at(i)) == 0) {
      FAIL() << "Expected entry " << expectedEntries.at(i)
             << " not found in result";
    }
    EXPECT_NEAR(static_cast<double>(m.at(expectedEntries.at(i))),
                expectedValues.at(i), tolerance);
  }
}

TEST(DeterministicNoiseSimTest, SimulateAdder4NoNoise1) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("I"), 0.01, 0.02, 1);

  double const measurementThreshold = 0.01;
  ddsim->simulate(1);
  auto m = ddsim->rootEdge.getSparseProbabilityVectorStrKeys(
      ddsim->getNumberOfQubits(), measurementThreshold);

  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const double tolerance = 1e-10;
  if (m.count("1001") == 0) {
    FAIL() << "Expected entry 1001 not found in result";
  }
  EXPECT_NEAR(m.find("1001")->second, 1, tolerance);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4NoNoise2) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("APD"), 0, 0, 1);

  std::size_t const shots = 10000;

  auto m = ddsim->simulate(shots);
  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const double tolerance = 1e-10;
  if (m.count("1001") == 0) {
    FAIL() << "Expected entry 1001 not found in result";
  }
  EXPECT_NEAR(static_cast<double>(m.find("1001")->second),
              static_cast<double>(shots), tolerance);
}

TEST(DeterministicNoiseSimTest, TestFunctionsOptimized) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("APD"), 0.01, 0.02, 1);
  auto m = ddsim->simulate(1000);

  EXPECT_EQ(ddsim->getNumberOfQubits(), 4);
  EXPECT_EQ(ddsim->getActiveNodeCount(), 22);
  EXPECT_EQ(ddsim->getMaxNodeCount(), 44);
  EXPECT_EQ(ddsim->getMaxMatrixNodeCount(), 0);
  EXPECT_EQ(ddsim->getMatrixActiveNodeCount(), 0);
  EXPECT_EQ(ddsim->countNodesFromRoot(), 23);
}

TEST(DeterministicNoiseSimTest, TestSimulateInterface) {
  auto quantumComputation = detGetAdder4Circuit();
  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("APD"), 0.01, 0.02, 1);

  auto m = ddsim->simulate(10000);

  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const auto expectedEntries = std::array{"0000", "0001", "1000", "1001"};
  const auto expectedValues = std::array{616, 1487, 570, 5519};
  const double tolerance = 500;
  for (std::size_t i = 0; i < expectedEntries.size(); ++i) {
    if (m.count(expectedEntries.at(i)) == 0) {
      FAIL() << "Expected entry " << expectedEntries.at(i)
             << " not found in result";
    }
    EXPECT_NEAR(static_cast<double>(m.at(expectedEntries.at(i))),
                expectedValues.at(i), tolerance);
  }
}

TEST(DeterministicNoiseSimTest, TestSimulateInterfaceWithMeasurments) {
  auto quantumComputation = detGetAdder4Circuit();
  quantumComputation->addClassicalRegister(4);
  quantumComputation->measure(0, 0);
  quantumComputation->measure(1, 1);
  quantumComputation->measure(2, 2);
  quantumComputation->measure(3, 3);

  auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
      std::move(quantumComputation), std::string("APD"), 0.01, 0.02, 1);

  auto m = ddsim->simulate(10000);

  std::cout << std::setw(2) << nlohmann::basic_json(m) << "\n";

  const auto expectedEntries = std::array{"0000", "0001", "1000", "1001"};
  const auto expectedValues = std::array{616, 1487, 570, 5519};
  const double tolerance = 500;
  for (std::size_t i = 0; i < expectedEntries.size(); ++i) {
    if (m.count(expectedEntries.at(i)) == 0) {
      FAIL() << "Expected entry " << expectedEntries.at(i)
             << " not found in result";
    }
    EXPECT_NEAR(static_cast<double>(m.at(expectedEntries.at(i))),
                expectedValues.at(i), tolerance);
  }
}
