#include "PathSimulator.hpp"
#include "algorithms/Grover.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/Export.hpp"
#include "ir/QuantumComputation.hpp"
#include "ir/operations/OpType.hpp"

#include <complex>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

using namespace qc::literals;

TEST(TaskBasedSimTest, Configuration) {
  EXPECT_EQ(PathSimulator<>::Configuration::modeToString(
                PathSimulator<>::Configuration::Mode::Sequential),
            "sequential");
  EXPECT_EQ(
      PathSimulator<>::Configuration::modeToString(
          PathSimulator<>::Configuration::Mode::PairwiseRecursiveGrouping),
      "pairwise_recursive");
  EXPECT_EQ(PathSimulator<>::Configuration::modeToString(
                PathSimulator<>::Configuration::Mode::BracketGrouping),
            "bracket");
  EXPECT_EQ(PathSimulator<>::Configuration::modeToString(
                PathSimulator<>::Configuration::Mode::Alternating),
            "alternating");
  EXPECT_EQ(PathSimulator<>::Configuration::modeToString(
                PathSimulator<>::Configuration::Mode::GateCost),
            "gate_cost");
  EXPECT_THROW(
      PathSimulator<>::Configuration::modeToString(
          // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
          PathSimulator<>::Configuration::Mode(32)),
      std::invalid_argument);

  EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("sequential"),
            PathSimulator<>::Configuration::Mode::Sequential);
  EXPECT_EQ(
      PathSimulator<>::Configuration::modeFromString("pairwise_recursive"),
      PathSimulator<>::Configuration::Mode::PairwiseRecursiveGrouping);
  EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("bracket"),
            PathSimulator<>::Configuration::Mode::BracketGrouping);
  EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("alternating"),
            PathSimulator<>::Configuration::Mode::Alternating);
  EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("gate_cost"),
            PathSimulator<>::Configuration::Mode::GateCost);
  EXPECT_THROW(
      PathSimulator<>::Configuration::modeFromString("invalid argument"),
      std::invalid_argument);

  auto config = PathSimulator<>::Configuration{};
  config.seed = 12345U;

  config.mode = PathSimulator<>::Configuration::Mode::BracketGrouping;
  config.bracketSize = 3;
  std::cout << config.toString() << "\n";

  config.mode = PathSimulator<>::Configuration::Mode::Alternating;
  config.startingPoint = 13;
  std::cout << config.toString() << "\n";

  config.mode = PathSimulator<>::Configuration::Mode::GateCost;
  config.startingPoint = 2;
  config.gateCost = {2, 2, 1, 1};
  std::cout << config.toString() << "\n";
}

TEST(TaskBasedSimTest, SimpleCircuit) {
  auto qc = std::make_unique<qc::QuantumComputation>(2);
  qc->h(1U);
  qc->cx(1, 0);

  // construct simulator and generate sequential contraction plan
  PathSimulator tbs(std::move(qc), PathSimulator<>::Configuration());

  // simulate circuit
  auto counts = tbs.simulate(1024);

  EXPECT_NEAR(tbs.rootEdge.getValueByIndex(0).real(), dd::SQRT2_2, 1e-10);
  EXPECT_NEAR(tbs.rootEdge.getValueByIndex(3).real(), dd::SQRT2_2, 1e-10);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, SimpleCircuitArgumentConstructor) {
  auto qc = std::make_unique<qc::QuantumComputation>(2);
  qc->h(1U);
  qc->cx(1, 0);

  // construct simulator and generate sequential contraction plan
  PathSimulator tbs(std::move(qc),
                    PathSimulator<>::Configuration::Mode::Sequential, 2, 0, {},
                    12345U);

  // simulate circuit
  auto counts = tbs.simulate(1024);

  EXPECT_NEAR(tbs.rootEdge.getValueByIndex(0).real(), dd::SQRT2_2, 1e-10);
  EXPECT_NEAR(tbs.rootEdge.getValueByIndex(3).real(), dd::SQRT2_2, 1e-10);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, SimpleCircuitAssumeFalseOrder) {
  auto qc = std::make_unique<qc::QuantumComputation>(2);
  qc->h(1U);
  qc->cx(1, 0);
  PathSimulator tbs(std::move(qc), PathSimulator<>::Configuration());
  // construct simulator and generate sequential contraction plan
  PathSimulator<>::SimulationPath::Components path{};
  path.emplace_back(1, 0);
  path.emplace_back(3, 2);
  tbs.setSimulationPath(path, false);
  // simulate circuit
  auto counts = tbs.simulate(1024);

  EXPECT_NEAR(tbs.rootEdge.getValueByIndex(0).real(), dd::SQRT2_2, 1e-10);
  EXPECT_NEAR(tbs.rootEdge.getValueByIndex(3).real(), dd::SQRT2_2, 1e-10);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, SimpleCircuitBracket) {
  auto qc = std::make_unique<qc::QuantumComputation>(2);
  qc->h(1U);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);

  // construct simulator and generate bracketing contraction plan
  auto config = PathSimulator<>::Configuration{};
  config.mode = PathSimulator<>::Configuration::Mode::BracketGrouping;
  config.bracketSize = 3;
  PathSimulator tbs(std::move(qc), config);

  // simulate circuit
  auto counts = tbs.simulate(1024);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, GroverCircuitBracket) {
  const auto* const expected = "1111";
  const auto targetValue = qc::GroverBitString{expected};
  auto qc = std::make_unique<qc::QuantumComputation>(
      qc::createGrover(4, targetValue));

  // construct simulator and generate bracketing contraction plan
  auto config = PathSimulator<>::Configuration{};
  config.mode = PathSimulator<>::Configuration::Mode::BracketGrouping;
  config.bracketSize = 3;
  PathSimulator tbs(std::move(qc), config);

  // simulate circuit
  auto counts = tbs.simulate(4096);

  const auto target = targetValue.to_ullong() | (1ULL << 4);
  const auto c = tbs.rootEdge.getValueByIndex(target);
  EXPECT_GT(std::norm(c), 0.9);

  dd::export2Dot(tbs.rootEdge, "result_grover.dot", true, true);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, GroverCircuitAlternatingMiddle) {
  const auto* const expected = "1111";
  const auto targetValue = qc::GroverBitString{expected};
  auto qc = std::make_unique<qc::QuantumComputation>(
      qc::createGrover(4, targetValue));

  // construct simulator and generate alternating contraction plan
  auto config = PathSimulator<>::Configuration{};
  config.mode = PathSimulator<>::Configuration::Mode::Alternating;
  PathSimulator tbs(std::move(qc), config);

  // simulate circuit
  auto counts = tbs.simulate(4096);

  const auto target = targetValue.to_ullong() | (1ULL << 4);
  const auto c = tbs.rootEdge.getValueByIndex(target);
  EXPECT_GT(std::norm(c), 0.9);

  dd::export2Dot(tbs.rootEdge, "result_grover.dot", true, true);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << '\n';
  }
}

TEST(TaskBasedSimTest, GroverCircuitPairwiseGrouping) {
  const auto* const expected = "1111";
  const auto targetValue = qc::GroverBitString{expected};
  auto qc = std::make_unique<qc::QuantumComputation>(
      qc::createGrover(4, targetValue));

  // construct simulator and generate pairwise recursive contraction plan
  auto config = PathSimulator<>::Configuration{};
  config.mode = PathSimulator<>::Configuration::Mode::PairwiseRecursiveGrouping;
  PathSimulator tbs(std::move(qc), config);

  // simulate circuit
  auto counts = tbs.simulate(4096);

  const auto target = targetValue.to_ullong() | (1ULL << 4);
  const auto c = tbs.rootEdge.getValueByIndex(target);
  EXPECT_GT(std::norm(c), 0.9);

  dd::export2Dot(tbs.rootEdge, "result_grouping.dot", true, true);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, EmptyCircuit) {
  auto qc = std::make_unique<qc::QuantumComputation>(2);

  // construct simulator and generate bracketing contraction plan
  PathSimulator tbs(std::move(qc));

  // simulate circuit
  const auto shots = 1024U;
  auto counts = tbs.simulate(shots);

  for (const auto& [state, count] : counts) {
    EXPECT_EQ(state, "00");
    EXPECT_EQ(count, shots);
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, SimpleCircuitGatecost) {
  auto qc = std::make_unique<qc::QuantumComputation>(2);
  qc->h(1U);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);

  // construct simulator and generate gatecost contraction plan
  PathSimulator tbs(std::move(qc),
                    PathSimulator<>::Configuration::Mode::GateCost, 2, 2,
                    {1, 1}, 12345U);

  // simulate circuit
  auto counts = tbs.simulate(1024);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, SimpleCircuitGatecostConfigurationObject) {
  auto qc = std::make_unique<qc::QuantumComputation>(2);
  qc->h(1U);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);
  qc->cx(1, 0);

  // construct simulator and generate gatecost contraction plan
  auto config = PathSimulator<>::Configuration{};
  config.mode = PathSimulator<>::Configuration::Mode::GateCost;
  config.startingPoint = 5;
  config.gateCost = {1, 1};
  PathSimulator tbs(std::move(qc), config);

  // simulate circuit
  auto counts = tbs.simulate(1024);

  for (const auto& [state, count] : counts) {
    std::cout << state << ": " << count << "\n";
  }
}

TEST(TaskBasedSimTest, DynamicCircuitSupport) {
  auto qc = std::make_unique<qc::QuantumComputation>(1, 1);
  qc->h(0);
  qc->measure(0, 0);
  qc->classicControlled(qc::X, 0, 0, 1);
  std::cout << *qc << "\n";

  PathSimulator sim(std::move(qc));
  EXPECT_THROW(sim.simulate(1024), std::invalid_argument);
}
