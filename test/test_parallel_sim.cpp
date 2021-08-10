//
// Created by alexander on 01.07.21.
//
#include "ParallelizationSimulator.hpp"
#include "GroverSimulator.hpp"
#include "algorithms/Grover.hpp"

#include <algorithms/Entanglement.hpp>
#include <algorithms/QFT.hpp>
#include <gtest/gtest.h>
#include <memory>
using namespace dd::literals;

class ParallelSimTestParameterized : public ::testing::TestWithParam<int>{
protected:
    ParallelizationSimulator testingSimulator(std::unique_ptr<qc::QuantumComputation>&& qc);

};

TEST_P(ParallelSimTestParameterized, Grover){
    int nqubits = GetParam();

    std::unique_ptr<qc::QuantumComputation> grover = std::make_unique<qc::Grover>(nqubits,1);
    ParallelizationSimulator ddsim(std::move(grover));
    std::unique_ptr<qc::QuantumComputation> grover2 = std::make_unique<qc::Grover>(nqubits,1);
    CircuitSimulator ddsim1(std::move(grover2));
    ddsim.Simulate(1);
    ddsim1.Simulate(1);


}

TEST_P(ParallelSimTestParameterized, QFT){
    int nqubits = GetParam();
    std::unique_ptr<qc::QuantumComputation> qft = std::make_unique<qc::QFT>(nqubits);
    ParallelizationSimulator ddsim(std::move(qft));
    ddsim.Simulate(1);
}

TEST_P(ParallelSimTestParameterized, Entanglement){
    int nqubits = GetParam();
    std::unique_ptr<qc::QuantumComputation> ghz = std::make_unique<qc::Entanglement>(nqubits);
    ParallelizationSimulator ddsim(std::move(ghz));
    ddsim.Simulate(1);
}

INSTANTIATE_TEST_CASE_P(ParallelSimTest,ParallelSimTestParameterized, ::testing::Range(12,13));


/*TEST(ParallelSimTest, TrivialParallelCircuit){
    auto quantumComputation = [] {
        auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
        quantumComputation->h(2);
        quantumComputation->h(1);
        quantumComputation->x(0, dd::Controls{1_pc, 2_pc});
        quantumComputation->i(1); // some dummy operations
        quantumComputation->i(1);
        return quantumComputation;
    };

    ParallelizationSimulator ddsim(quantumComputation());
    ddsim.Simulate(1);
}

TEST(ParallelSimTest, Grover) {
    dd::QubitCount                          nq     = 4;
    std::unique_ptr<qc::QuantumComputation> grover = std::make_unique<qc::Grover>(nq);
    ParallelizationSimulator                ddsim(std::move(grover));
    ddsim.Simulate(1);
}
*/
