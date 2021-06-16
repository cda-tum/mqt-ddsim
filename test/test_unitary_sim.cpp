#include "UnitarySimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace dd::literals;

TEST(UnitarySimTest, ConstructSimpleCircuitSequential) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2_pc, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2_pc, 0, qc::H);
    UnitarySimulator ddsim(std::move(quantumComputation), UnitarySimulator::Mode::Sequential);
    ASSERT_NO_THROW(ddsim.Construct());
    const auto& e = ddsim.getConstructedDD();
    EXPECT_TRUE(e.p->e[0].p->ident);
    EXPECT_TRUE(e.p->e[1].p->ident);
}

TEST(UnitarySimTest, ConstructSimpleCircuitRecursive) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2_pc, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2_pc, 0, qc::H);
    UnitarySimulator ddsim(std::move(quantumComputation), UnitarySimulator::Mode::Recursive);
    ASSERT_NO_THROW(ddsim.Construct());
    const auto& e = ddsim.getConstructedDD();
    EXPECT_TRUE(e.p->e[0].p->ident);
    EXPECT_TRUE(e.p->e[1].p->ident);
}

TEST(UnitarySimTest, ConstructSimpleCircuitRecursiveWithSeed) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(3);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2_pc, 1, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(3, 2_pc, 0, qc::H);
    UnitarySimulator ddsim(std::move(quantumComputation), ApproximationInfo{}, 1337, UnitarySimulator::Mode::Recursive);
    ASSERT_NO_THROW(ddsim.Construct());
    const auto& e = ddsim.getConstructedDD();
    EXPECT_TRUE(e.p->e[0].p->ident);
    EXPECT_TRUE(e.p->e[1].p->ident);
}

TEST(UnitarySimTest, NonStandardOperation) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(1);
    quantumComputation->emplace_back<qc::StandardOperation>(1, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(1, 0, 0);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(1, 0, qc::Barrier);
    quantumComputation->emplace_back<qc::StandardOperation>(1, 0, qc::H);
    quantumComputation->emplace_back<qc::NonUnitaryOperation>(1, 0, 0);

    UnitarySimulator ddsim(std::move(quantumComputation));
    EXPECT_TRUE(ddsim.getMode() == UnitarySimulator::Mode::Recursive);
    EXPECT_THROW(ddsim.Construct(), std::invalid_argument);
}
