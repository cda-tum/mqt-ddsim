#include "CircuitSimulator.hpp"
#include "HybridSchrodingerFeynmanSimulator.hpp"
#include "ir/QuantumComputation.hpp"

#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

using namespace qc::literals;

TEST(HybridSimTest, TrivialParallelDD) {
  auto quantumComputation = [] {
    auto qc = std::make_unique<qc::QuantumComputation>(4);
    qc->h(2);
    qc->h(1);
    qc->mcx({2, 1}, 0);
    qc->i(1); // some dummy operations
    qc->i(1);
    return qc;
  };

  HybridSchrodingerFeynmanSimulator ddsim(
      quantumComputation(), HybridSchrodingerFeynmanSimulator<>::Mode::DD);

  auto resultDD = ddsim.simulate(8192);
  for (const auto& entry : resultDD) {
    std::cout << "resultDD[" << entry.first << "] = " << entry.second << "\n";
  }

  ASSERT_EQ(ddsim.getActiveNodeCount(), 6);
  ASSERT_EQ(resultDD.size(), 4);
  auto it = resultDD.find("0000");
  ASSERT_TRUE(it != resultDD.end());
  EXPECT_NEAR(static_cast<double>(it->second), 2048, 128);
  it = resultDD.find("0010");
  ASSERT_TRUE(it != resultDD.end());
  EXPECT_NEAR(static_cast<double>(it->second), 2048, 128);
  it = resultDD.find("0100");
  ASSERT_TRUE(it != resultDD.end());
  EXPECT_NEAR(static_cast<double>(it->second), 2048, 128);
  it = resultDD.find("0111");
  ASSERT_TRUE(it != resultDD.end());
  EXPECT_NEAR(static_cast<double>(it->second), 2048, 128);
}

TEST(HybridSimTest, TrivialParallelAmplitude) {
  auto quantumComputation = [] {
    auto qc = std::make_unique<qc::QuantumComputation>(4);
    qc->h(2);
    qc->h(1);
    qc->mcx({2, 1}, 0);
    qc->i(1); // some dummy operations
    qc->i(1);
    return qc;
  };

  HybridSchrodingerFeynmanSimulator ddsim(
      quantumComputation(),
      HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude);

  auto resultAmp = ddsim.simulate(8192);
  for (const auto& entry : resultAmp) {
    std::cout << "resultAmp[" << entry.first << "] = " << entry.second << "\n";
  }

  ASSERT_EQ(resultAmp.size(), 4);
  auto it = resultAmp.find("0000");
  ASSERT_TRUE(it != resultAmp.end());
  EXPECT_NEAR(static_cast<double>(it->second), 2048, 128);
  it = resultAmp.find("0010");
  ASSERT_TRUE(it != resultAmp.end());
  EXPECT_NEAR(static_cast<double>(it->second), 2048, 128);
  it = resultAmp.find("0100");
  ASSERT_TRUE(it != resultAmp.end());
  EXPECT_NEAR(static_cast<double>(it->second), 2048, 128);
  it = resultAmp.find("0111");
  ASSERT_TRUE(it != resultAmp.end());
  EXPECT_NEAR(static_cast<double>(it->second), 2048, 128);
}

TEST(HybridSimTest, NonStandardOperation) {
  auto quantumComputation = std::make_unique<qc::QuantumComputation>(1, 1);
  quantumComputation->h(0);
  quantumComputation->measure(0, 0);
  quantumComputation->barrier(0);
  quantumComputation->h(0);
  quantumComputation->measure(0, 0);

  HybridSchrodingerFeynmanSimulator ddsim(std::move(quantumComputation));
  EXPECT_THROW(ddsim.simulate(0), std::invalid_argument);
}

TEST(HybridSimTest, TooManyQubitsForVectorTest) {
  auto qc = std::make_unique<qc::QuantumComputation>(61);
  const HybridSchrodingerFeynmanSimulator<> ddsim(
      std::move(qc), ApproximationInfo{},
      HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude);
  EXPECT_THROW(
      { [[maybe_unused]] auto _ = ddsim.getVectorFromHybridSimulation(); },
      std::range_error);
}

TEST(HybridSimTest, RegressionTestDDModeUnevenChunks) {
  // The circuit below has 3 two-qubit gates.
  // Thus HSF simulation creates 8 decisions.
  // Setting the number of threads to 3 results in the following chunks: 3|3|2
  // The result of the simulation should be |10>.

  auto qc = std::make_unique<qc::QuantumComputation>(2U);
  qc->x(0);
  qc->cx(0, 1);
  qc->cx(1, 0);
  qc->cx(0, 1);

  const auto mode = HybridSchrodingerFeynmanSimulator<>::Mode::DD;
  const auto nthreads = 3U;

  HybridSchrodingerFeynmanSimulator ddsim(std::move(qc), ApproximationInfo{},
                                          mode, nthreads);
  const auto result = ddsim.simulate(1024U);
  EXPECT_EQ(result.size(), 1U);
  EXPECT_EQ(result.begin()->first, "10");
  EXPECT_EQ(result.begin()->second, 1024U);
}

TEST(HybridSimTest, RegressionTestAmplitudeModeUnevenChunks) {
  // The circuit below has 3 two-qubit gates.
  // Thus HSF simulation creates 8 decisions.
  // Setting the number of threads to 3 results in the following chunks: 3|3|2
  // The result of the simulation should be |10>.

  auto qc = std::make_unique<qc::QuantumComputation>(2U);
  qc->x(0);
  qc->cx(0, 1);
  qc->cx(1, 0);
  qc->cx(0, 1);

  const auto mode = HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude;
  const auto nthreads = 3U;

  HybridSchrodingerFeynmanSimulator ddsim(std::move(qc), ApproximationInfo{},
                                          mode, nthreads);
  const auto result = ddsim.simulate(1024U);
  EXPECT_EQ(result.size(), 1U);
  EXPECT_EQ(result.begin()->first, "10");
  EXPECT_EQ(result.begin()->second, 1024U);
}

TEST(HybridSimTest, RegressionTestDDModeMoreChunksAsThreads) {
  // The circuit below has 6 two-qubit gates.
  // Thus HSF simulation creates 64 decisions.
  // Due to the maximum number of decisions per thread being 16 in DD mode,
  // setting the number of threads to 2 results in the following chunks:
  // 16|16|16|16 The result of the simulation should be |01>.

  auto qc = std::make_unique<qc::QuantumComputation>(2U);
  qc->x(0);
  qc->cx(0, 1);
  qc->cx(1, 0);
  qc->cx(0, 1);
  qc->cx(0, 1);
  qc->cx(1, 0);
  qc->cx(0, 1);

  const auto mode = HybridSchrodingerFeynmanSimulator<>::Mode::DD;
  const auto nthreads = 2U;

  HybridSchrodingerFeynmanSimulator ddsim(std::move(qc), ApproximationInfo{},
                                          mode, nthreads);
  const auto result = ddsim.simulate(1024U);
  EXPECT_EQ(result.size(), 1U);
  EXPECT_EQ(result.begin()->first, "01");
  EXPECT_EQ(result.begin()->second, 1024U);
}

TEST(HybridSimTest, RegressionTestAmplitudeModeMoreChunksAsThreads) {
  // The circuit below has 8 two-qubit gates.
  // Thus HSF simulation creates 256 decisions.
  // Due to the maximum number of decisions per thread being 64 in Amplitude
  // mode, setting the number of threads to 2 results in the following chunks:
  // 64|64|64|64 The result of the simulation should be |10>.

  auto qc = std::make_unique<qc::QuantumComputation>(2U);
  qc->x(0);
  qc->cx(0, 1);
  qc->cx(1, 0);
  qc->cx(0, 1);
  qc->cx(0, 1);
  qc->cx(1, 0);
  qc->cx(0, 1);
  qc->cx(0, 1);
  qc->cx(1, 0);

  const auto mode = HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude;
  const auto nthreads = 2U;

  HybridSchrodingerFeynmanSimulator ddsim(std::move(qc), ApproximationInfo{},
                                          mode, nthreads);
  const auto result = ddsim.simulate(1024U);
  EXPECT_EQ(result.size(), 1U);
  EXPECT_EQ(result.begin()->first, "10");
  EXPECT_EQ(result.begin()->second, 1024U);
}
