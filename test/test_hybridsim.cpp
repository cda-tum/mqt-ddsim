#include "HybridSchroedingerFeynmanSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace dd::literals;

TEST(ParallelSimTest, TrivialParallelTest) {
	auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
	quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::H);
	quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::H);
	quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{2_pc, 1_pc}, 0, qc::X);
	quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::I); // some dummy operations
	quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::I);

    HybridSchroedingerFeynmanSimulator ddsim(quantumComputation);
	ddsim.Simulate(1);

	ASSERT_EQ(ddsim.getActiveNodeCount(), 6);
}

TEST(ParallelSimTest, GRCSTest) {
	auto quantumComputation = std::make_unique<qc::QuantumComputation>("circuits/inst_4x4_10_0.txt");
    HybridSchroedingerFeynmanSimulator ddsim_parallel(quantumComputation);
	CircuitSimulator ddsim(quantumComputation);

	ddsim_parallel.Simulate(1);
	ddsim.Simulate(1);

	dd::serialize(ddsim_parallel.root_edge, "result_parallel.dd", true);
	dd::serialize(ddsim.root_edge, "result.dd", true);

	auto dd = std::make_unique<dd::Package>(16);
	auto result = dd->deserialize<dd::Package::vNode>("result_parallel.dd", true);
	auto ref = dd->deserialize<dd::Package::vNode>("result.dd", true);

	bool equal = (result == ref);
	if(!equal) {
        // if edges are not equal -> compare amplitudes
        auto refAmplitudes = dd->getVector(ref);
        auto resultAmplitudes = dd->getVector(result);
        equal = true;
        for (std::size_t i = 0; i < refAmplitudes.size(); ++i) {
            if (std::abs(refAmplitudes[i].first - resultAmplitudes[i].first) > 1e-6 || std::abs(refAmplitudes[i].second - resultAmplitudes[i].second) > 1e-6) {
                equal = false;
                break;
            }
        }
    }
	EXPECT_TRUE(equal);
}
