//
// Created by Lukas Burgholzer on 09.08.21.
//

#include "TaskBasedSimulator.hpp"
#include "algorithms/Entanglement.hpp"
#include "algorithms/Grover.hpp"
#include "dd/Export.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace dd::literals;

TEST(TaskBasedSimTest, SimpleCircuitSingleThreaded) {
    auto qc = std::make_unique<qc::QuantumComputation>(2);
    qc->h(1U);
    qc->x(0U, 1_pc);

    // construct simulator and generate sequential contraction plan
    TaskBasedSimulator tbs(std::move(qc), TaskBasedSimulator::Mode::Sequential, 1);

    // simulate circuit
    auto counts = tbs.Simulate(1024);

    EXPECT_TRUE(tbs.dd->getValueByPath(tbs.root_edge, 0).approximatelyEquals({dd::SQRT2_2, 0}));
    EXPECT_TRUE(tbs.dd->getValueByPath(tbs.root_edge, 3).approximatelyEquals({dd::SQRT2_2, 0}));

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

TEST(TaskBasedSimTest, GroverCircuitBracket) {
    std::unique_ptr<qc::QuantumComputation> qc          = std::make_unique<qc::Grover>(4, 12345);
    auto                                    grover      = dynamic_cast<qc::Grover*>(qc.get());
    auto                                    targetValue = grover->targetValue;

    // construct simulator and generate sequential contraction plan
    TaskBasedSimulator tbs(std::move(qc), TaskBasedSimulator::Mode::BracketGrouping3, 1);

    // simulate circuit
    auto counts = tbs.Simulate(4096);

    auto c    = tbs.dd->getValueByPath(tbs.root_edge, targetValue);
    auto prob = c.r * c.r + c.i * c.i;
    EXPECT_GT(prob, 0.9);

    dd::export2Dot(tbs.root_edge, "result_grover.dot", true, true);

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}


TEST(TaskBasedSimTest, GroverCircuitPairwiseGroupingSingleThreaded) {
    std::unique_ptr<qc::QuantumComputation> qc          = std::make_unique<qc::Grover>(4, 12345);
    auto                                    grover      = dynamic_cast<qc::Grover*>(qc.get());
    auto                                    targetValue = grover->targetValue;
    grover->print(std::cout);

    // construct simulator and generate sequential contraction plan
    TaskBasedSimulator tbs(std::move(qc), TaskBasedSimulator::Mode::PairwiseRecursiveGrouping, 1);

    // simulate circuit
    auto counts = tbs.Simulate(4096);

    auto c    = tbs.dd->getValueByPath(tbs.root_edge, targetValue);
    auto prob = c.r * c.r + c.i * c.i;
    EXPECT_GT(prob, 0.9);

    dd::export2Dot(tbs.root_edge, "result_grouping.dot", true, true);

    for (const auto& [state, count]: counts) {
        std::cout << state << ": " << count << std::endl;
    }
}

/*TEST(TaskBasedSimTest, GroverCircuitTest) {
    std::size_t nq   = 2U;
    std::size_t seed = 12345U;

    std::unique_ptr<qc::QuantumComputation> qc1         = std::make_unique<qc::Grover>(nq, seed);
    auto                                    grover      = dynamic_cast<qc::Grover*>(qc1.get());
    auto                                    targetValue = grover->targetValue;

    grover->print(std::cout);

    // construct simulator and generate sequential contraction plan
    TaskBasedSimulator tbs1(std::move(qc1), 1);
    tbs1.generateSequentialContractionPlan();

    // simulate circuit
    auto counts1 = tbs1.Simulate(1);

    auto c    = tbs1.dd->getValueByPath(tbs1.root_edge, targetValue);
    auto prob = c.r * c.r + c.i * c.i;
    EXPECT_GT(prob, 0.9);

    dd::export2Dot(tbs1.root_edge, "result_sequential.dot", true, true);

    std::unique_ptr<qc::QuantumComputation> qc2 = std::make_unique<qc::Grover>(nq, seed);

    // construct simulator and generate sequential contraction plan
    TaskBasedSimulator tbs2(std::move(qc2), 1);
    tbs2.generatePairwiseRecursiveGroupingContractionPlan();

    // simulate circuit
    auto counts2 = tbs2.Simulate(1);

    c    = tbs2.dd->getValueByPath(tbs2.root_edge, targetValue);
    prob = c.r * c.r + c.i * c.i;
    EXPECT_GT(prob, 0.9);

    dd::export2Dot(tbs2.root_edge, "result_grouping.dot", true, true);
}
*/
TEST(TaskBasedSimTest, MWEAccuracy) {
    std::size_t nq = 1U;

    qc::QuantumComputation qc(nq);
    qc.h(0);
    qc.z(0);

    std::cout << qc << std::endl;
    auto dd = std::make_unique<dd::Package>(1);
    //    auto state = dd->makeBasisState(1, {dd::BasisStates::plus});
    auto state = dd->makeZeroState(1);
    std::cout << "---" << std::endl;
    auto state1 = dd->multiply(qc.at(0)->getDD(dd), state);
    std::cout << "---" << std::endl;
    auto result = dd->multiply(qc.at(1)->getDD(dd), state1);

    //    auto result = qc.simulate(state, dd);

    dd::export2Dot(result, "result_mem.dot", true, true, true, true);
    dd::export2Dot(result, "result.dot", true, true);
}
