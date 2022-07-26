#include "PathSimulator.hpp"
#include "algorithms/Entanglement.hpp"
#include "algorithms/Grover.hpp"
#include "dd/Export.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace dd::literals;

TEST(TaskBasedSimTest, Configuration) {
    EXPECT_EQ(PathSimulator<>::Configuration::modeToString(PathSimulator<>::Configuration::Mode::Sequential), "sequential");
    EXPECT_EQ(PathSimulator<>::Configuration::modeToString(PathSimulator<>::Configuration::Mode::PairwiseRecursiveGrouping), "pairwise_recursive");
    EXPECT_EQ(PathSimulator<>::Configuration::modeToString(PathSimulator<>::Configuration::Mode::BracketGrouping), "bracket");
    EXPECT_EQ(PathSimulator<>::Configuration::modeToString(PathSimulator<>::Configuration::Mode::Alternating), "alternating");
    EXPECT_EQ(PathSimulator<>::Configuration::modeToString(PathSimulator<>::Configuration::Mode::Cotengra), "cotengra");

    EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("sequential"), PathSimulator<>::Configuration::Mode::Sequential);
    EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("pairwise_recursive"), PathSimulator<>::Configuration::Mode::PairwiseRecursiveGrouping);
    EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("bracket"), PathSimulator<>::Configuration::Mode::BracketGrouping);
    EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("alternating"), PathSimulator<>::Configuration::Mode::Alternating);
    EXPECT_EQ(PathSimulator<>::Configuration::modeFromString("cotengra"), PathSimulator<>::Configuration::Mode::Cotengra);

    auto config = PathSimulator<>::Configuration{};
    config.seed = 12345U;

    config.mode        = PathSimulator<>::Configuration::Mode::BracketGrouping;
    config.bracketSize = 3;
    std::cout << config.toString() << std::endl;

    config.mode             = PathSimulator<>::Configuration::Mode::Alternating;
    config.alternatingStart = 13;
    std::cout << config.toString() << std::endl;
}

TEST(TaskBasedSimTest, SimpleCircuit) {
    auto qc = std::make_unique<qc::QuantumComputation>(2);
    qc->h(1U);
    qc->x(0U, 1_pc);

    // construct simulator and generate sequential contraction plan
    PathSimulator tbs(std::move(qc), PathSimulator<>::Configuration());

    // simulate circuit
    auto counts = tbs.Simulate(1024);

    EXPECT_TRUE(tbs.dd->getValueByPath(tbs.rootEdge, 0).approximatelyEquals({dd::SQRT2_2, 0}));
    EXPECT_TRUE(tbs.dd->getValueByPath(tbs.rootEdge, 3).approximatelyEquals({dd::SQRT2_2, 0}));

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, SimpleCircuitArgumentConstructor) {
    auto qc = std::make_unique<qc::QuantumComputation>(2);
    qc->h(1U);
    qc->x(0U, 1_pc);

    // construct simulator and generate sequential contraction plan
    PathSimulator tbs(std::move(qc), PathSimulator<>::Configuration::Mode::Sequential, 2, 0, {}, 12345U);

    // simulate circuit
    auto counts = tbs.Simulate(1024);

    EXPECT_TRUE(tbs.dd->getValueByPath(tbs.rootEdge, 0).approximatelyEquals({dd::SQRT2_2, 0}));
    EXPECT_TRUE(tbs.dd->getValueByPath(tbs.rootEdge, 3).approximatelyEquals({dd::SQRT2_2, 0}));

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, SimpleCircuitAssumeFalseOrder) {
    auto qc = std::make_unique<qc::QuantumComputation>(2);
    qc->h(1U);
    qc->x(0U, 1_pc);
    PathSimulator tbs(std::move(qc), PathSimulator<>::Configuration());
    // construct simulator and generate sequential contraction plan
    PathSimulator<>::SimulationPath::Components path{};
    path.emplace_back(1, 0);
    path.emplace_back(3, 2);
    tbs.setSimulationPath(path, false);
    // simulate circuit
    auto counts = tbs.Simulate(1024);

    EXPECT_TRUE(tbs.dd->getValueByPath(tbs.rootEdge, 0).approximatelyEquals({dd::SQRT2_2, 0}));
    EXPECT_TRUE(tbs.dd->getValueByPath(tbs.rootEdge, 3).approximatelyEquals({dd::SQRT2_2, 0}));

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, SimpleCircuitBracket) {
    auto qc = std::make_unique<qc::QuantumComputation>(2);
    qc->h(1U);
    qc->x(0U, 1_pc);
    qc->x(0U, 1_pc);
    qc->x(0U, 1_pc);

    // construct simulator and generate bracketing contraction plan
    auto config        = PathSimulator<>::Configuration{};
    config.mode        = PathSimulator<>::Configuration::Mode::BracketGrouping;
    config.bracketSize = 3;
    PathSimulator tbs(std::move(qc), config);

    // simulate circuit
    auto counts = tbs.Simulate(1024);

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, GroverCircuitBracket) {
    std::unique_ptr<qc::QuantumComputation> qc          = std::make_unique<qc::Grover>(4, 12345);
    auto                                    grover      = dynamic_cast<qc::Grover*>(qc.get());
    auto                                    targetValue = grover->targetValue;

    // construct simulator and generate bracketing contraction plan
    auto config        = PathSimulator<>::Configuration{};
    config.mode        = PathSimulator<>::Configuration::Mode::BracketGrouping;
    config.bracketSize = 3;
    PathSimulator tbs(std::move(qc), config);

    // simulate circuit
    auto counts = tbs.Simulate(4096);

    const auto target = targetValue.to_ullong() | (1ULL << 4);
    auto       c      = tbs.dd->getValueByPath(tbs.rootEdge, target);
    auto       prob   = c.r * c.r + c.i * c.i;
    EXPECT_GT(prob, 0.9);

    dd::export2Dot(tbs.rootEdge, "result_grover.dot", true, true);

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, GroverCircuitAlternatingMiddle) {
    std::unique_ptr<qc::QuantumComputation> qc          = std::make_unique<qc::Grover>(4, 12345);
    auto                                    grover      = dynamic_cast<qc::Grover*>(qc.get());
    auto                                    targetValue = grover->targetValue;

    // construct simulator and generate alternating contraction plan
    auto config = PathSimulator<>::Configuration{};
    config.mode = PathSimulator<>::Configuration::Mode::Alternating;
    PathSimulator tbs(std::move(qc), config);

    // simulate circuit
    auto counts = tbs.Simulate(4096);

    const auto target = targetValue.to_ullong() | (1ULL << 4);
    auto       c      = tbs.dd->getValueByPath(tbs.rootEdge, target);
    auto       prob   = c.r * c.r + c.i * c.i;
    EXPECT_GT(prob, 0.9);

    dd::export2Dot(tbs.rootEdge, "result_grover.dot", true, true);

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, GroverCircuitPairwiseGrouping) {
    std::unique_ptr<qc::QuantumComputation> qc          = std::make_unique<qc::Grover>(4, 12345);
    auto                                    grover      = dynamic_cast<qc::Grover*>(qc.get());
    auto                                    targetValue = grover->targetValue;

    // construct simulator and generate pairwise recursive contraction plan
    auto config = PathSimulator<>::Configuration{};
    config.mode = PathSimulator<>::Configuration::Mode::PairwiseRecursiveGrouping;
    PathSimulator tbs(std::move(qc), config);

    // simulate circuit
    auto counts = tbs.Simulate(4096);

    const auto target = targetValue.to_ullong() | (1ULL << 4);
    auto       c      = tbs.dd->getValueByPath(tbs.rootEdge, target);
    auto       prob   = c.r * c.r + c.i * c.i;
    EXPECT_GT(prob, 0.9);

    dd::export2Dot(tbs.rootEdge, "result_grouping.dot", true, true);

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, EmptyCircuit) {
    auto qc = std::make_unique<qc::QuantumComputation>(2);

    // construct simulator and generate bracketing contraction plan
    PathSimulator tbs(std::move(qc));

    // simulate circuit
    const auto shots  = 1024U;
    auto       counts = tbs.Simulate(shots);

    for (const auto& [state, count]: counts) {
        EXPECT_EQ(state, "00");
        EXPECT_EQ(count, shots);
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, SimpleCircuitGatecost) {
    auto qc = std::make_unique<qc::QuantumComputation>(2);
    qc->h(1U);
    qc->x(0U, 1_pc);
    qc->x(0U, 1_pc);
    qc->x(0U, 1_pc);

    // construct simulator and generate gatecost contraction plan
    PathSimulator tbs(std::move(qc), PathSimulator<>::Configuration::Mode::Gatecost, 2, 2, {1, 1}, 12345U);

    // simulate circuit
    auto counts = tbs.Simulate(1024);

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, SimpleCircuitGatecostConfigurationObject) {
    auto qc = std::make_unique<qc::QuantumComputation>(2);
    qc->h(1U);
    qc->x(0U, 1_pc);
    qc->x(0U, 1_pc);
    qc->x(0U, 1_pc);

    // construct simulator and generate gatecost contraction plan
    auto config = PathSimulator<>::Configuration{};
    config.mode = PathSimulator<>::Configuration::Mode::Gatecost;
    config.alternatingStart = 2;
    config.gateCost = {1, 1};
    PathSimulator tbs(std::move(qc), config);

    // simulate circuit
    auto counts = tbs.Simulate(1024);

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}