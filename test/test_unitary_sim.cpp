#include "UnitarySimulator.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

using namespace qc::literals;

TEST(UnitarySimTest, ConstructSimpleCircuitSequential) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
  quantumComputation->h(2);
  quantumComputation->ch(2, 1);
  quantumComputation->ch(2, 0);
  UnitarySimulator ddsim(std::move(quantumComputation),
                         UnitarySimulator<>::Mode::Sequential);
  ASSERT_NO_THROW(ddsim.construct());
  const auto& e = ddsim.getConstructedDD();
  EXPECT_TRUE(e.p->e[0].isIdentity());
  EXPECT_TRUE(e.p->e[1].isIdentity());
  auto finalNodes = ddsim.getFinalNodeCount();
  EXPECT_EQ(finalNodes, 4);
  auto maxNodes = ddsim.getMaxNodeCount();
  auto constructionTime = ddsim.getConstructionTime();
  std::cout << "Construction took " << constructionTime
            << "s, requiring a maximum of " << maxNodes << " nodes\n";
}

TEST(UnitarySimTest, ConstructSimpleCircuitRecursive) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
  quantumComputation->h(2);
  quantumComputation->ch(2, 1);
  quantumComputation->ch(2, 0);
  UnitarySimulator ddsim(std::move(quantumComputation),
                         UnitarySimulator<>::Mode::Recursive);
  ASSERT_NO_THROW(ddsim.construct());
  const auto& e = ddsim.getConstructedDD();
  EXPECT_TRUE(e.p->e[0].isIdentity());
  EXPECT_TRUE(e.p->e[1].isIdentity());
  auto finalNodes = ddsim.getFinalNodeCount();
  EXPECT_EQ(finalNodes, 4);
  auto maxNodes = ddsim.getMaxNodeCount();
  auto constructionTime = ddsim.getConstructionTime();
  std::cout << "Construction took " << constructionTime
            << "s, requiring a maximum of " << maxNodes << " nodes\n";
}

TEST(UnitarySimTest, ConstructSimpleCircuitRecursiveWithSeed) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
  quantumComputation->h(2);
  quantumComputation->ch(2, 1);
  quantumComputation->ch(2, 0);
  UnitarySimulator ddsim(std::move(quantumComputation), ApproximationInfo{},
                         1337, UnitarySimulator<>::Mode::Recursive);
  ASSERT_NO_THROW(ddsim.construct());
  const auto& e = ddsim.getConstructedDD();
  EXPECT_TRUE(e.p->e[0].isIdentity());
  EXPECT_TRUE(e.p->e[1].isIdentity());
}

TEST(UnitarySimTest, NonStandardOperation) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(1, 1);
  quantumComputation->h(0);
  quantumComputation->measure(0, 0);
  quantumComputation->barrier(0);
  quantumComputation->h(0);
  quantumComputation->measure(0, 0);

  UnitarySimulator ddsim(std::move(quantumComputation));
  EXPECT_TRUE(ddsim.getMode() == UnitarySimulator<>::Mode::Recursive);
  EXPECT_THROW(ddsim.construct(), std::invalid_argument);
}
