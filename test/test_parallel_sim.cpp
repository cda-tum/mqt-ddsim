//
// Created by alexander on 01.07.21.
//
#include "ParallelizationSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>
using namespace dd::literals;

TEST(ParallelSimTest, TrivialParallelCircuit){
    auto quantumComputation = [] {
        auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
        quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::H);
        quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::H);
        quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{2_pc, 1_pc}, 0, qc::X);
        quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::I); // some dummy operations
        quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::I);
        return quantumComputation;
    };

    ParallelizationSimulator ddsim(quantumComputation());
    ddsim.Simulate(1);
}

TEST(ParallelSimTest, Placeholder){

}