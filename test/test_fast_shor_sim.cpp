#include "ShorFastSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */

TEST(FastShorSimTest, Factorize15BaseTest) {
    ShorFastSimulator ddsim(15, 2, 5ull, true);

    ASSERT_EQ(ddsim.getNumberOfOps(), 0);
    ASSERT_EQ(ddsim.getName(), "fast_shor_15_2");
    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
    ASSERT_EQ(ddsim.AdditionalStatistics().at("sim_result"), "did not start");
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getFactors().first, 3);
    ASSERT_EQ(ddsim.getFactors().second, 5);
    ASSERT_EQ(ddsim.AdditionalStatistics().at("sim_result"), "SUCCESS(3*5)");
}

TEST(FastShorSimTest, Factorize15NegTest) {
    ShorFastSimulator ddsim(15, 2, 1ull);
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
    ASSERT_EQ(ddsim.AdditionalStatistics().at("sim_result"), "FAILURE");
}

TEST(FastShorSimTest, Factorize55Test) {
    ShorFastSimulator ddsim(55, 2, 3ull);
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getFactors().first, 11);
    ASSERT_EQ(ddsim.getFactors().second, 5);
}

TEST(FastShorSimTest, Factorize221Test) {
    ShorFastSimulator ddsim(221, 2, 4ull);
    ddsim.Simulate(1);

    ASSERT_EQ(ddsim.getFactors().first, 13);
    ASSERT_EQ(ddsim.getFactors().second, 17);
}