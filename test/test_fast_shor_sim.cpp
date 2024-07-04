#include "ShorFastSimulator.hpp"

#include <cstdint>
#include <gtest/gtest.h>

/**
 * These tests may have to be adjusted if something about the random-number
 * generation changes.
 */

TEST(FastShorSimTest, Factorize15BaseTest) {
  ShorFastSimulator ddsim(15, 2, 5ULL, true);

  ASSERT_EQ(ddsim.getNumberOfOps(), 0);
  ASSERT_EQ(ddsim.getName(), "fast_shor_15_2");
  ASSERT_EQ(ddsim.getFactors().first, 0);
  ASSERT_EQ(ddsim.getFactors().second, 0);
  ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "did not start");
  ddsim.simulate(1);

  ASSERT_EQ(ddsim.getFactors().first, 3);
  ASSERT_EQ(ddsim.getFactors().second, 5);
  ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "SUCCESS(3*5)");
}

TEST(FastShorSimTest, Factorize15NoSeedBaseTest) {
  ShorFastSimulator ddsim(15, 2, true);

  ASSERT_EQ(ddsim.getNumberOfOps(), 0);
  ASSERT_EQ(ddsim.getName(), "fast_shor_15_2");
  ASSERT_EQ(ddsim.getFactors().first, 0);
  ASSERT_EQ(ddsim.getFactors().second, 0);
  ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "did not start");
  ddsim.simulate(1);

  ASSERT_EQ(ddsim.getNumberOfQubits(), 5);
}

TEST(FastShorSimTest, Factorize15BaseTestNoFixedCoPrime) {
  ShorFastSimulator ddsim(15, 0, static_cast<std::uint64_t>(1));
  ddsim.simulate(1);

  SUCCEED()
      << "Successfully executed this path. Testing for values is flaky...";
}

TEST(FastShorSimTest, Factorize15NegTest) {
  ShorFastSimulator ddsim(15, 2, static_cast<std::uint64_t>(1));
  ddsim.simulate(1);

  ASSERT_EQ(ddsim.getFactors().first, 0);
  ASSERT_EQ(ddsim.getFactors().second, 0);
  ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "FAILURE");
}

TEST(FastShorSimTest, Factorize55Test) {
  ShorFastSimulator ddsim(55, 2, static_cast<std::uint64_t>(3));
  ddsim.simulate(1);

  ASSERT_EQ(ddsim.getFactors().first, 11);
  ASSERT_EQ(ddsim.getFactors().second, 5);
}

TEST(FastShorSimTest, Factorize221Test) {
  ShorFastSimulator ddsim(221, 2, static_cast<std::uint64_t>(4));
  ddsim.simulate(1);

  ASSERT_EQ(ddsim.getFactors().first, 13);
  ASSERT_EQ(ddsim.getFactors().second, 17);
}
