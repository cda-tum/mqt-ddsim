//
// Created by alexander on 01.07.21.
//
#include "ParallelizationSimulator.hpp"
#include "algorithms/Grover.hpp"

#include <gtest/gtest.h>
#include <memory>
using namespace dd::literals;

TEST(ParallelSimTest, TrivialParallelCircuit){
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
