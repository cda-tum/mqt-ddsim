#include "ShorSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */

TEST(ShorSimTest, Factorize15Test) {
    ShorSimulator ddsim(15, 2, 3ull);
    ddsim.Simulate();

    ASSERT_EQ(ddsim.getFactors().first, 3);
    ASSERT_EQ(ddsim.getFactors().second, 5);
}

TEST(ShorSimTest, Factorize15NegTest) {
    ShorSimulator ddsim(15, 2, 1ull);
    ddsim.Simulate();

    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
}


TEST(ShorSimTest, Factorize55Test) {
    ShorSimulator ddsim(55, 2, 3ull);
    ddsim.Simulate();

    ASSERT_EQ(ddsim.getFactors().first, 11);
    ASSERT_EQ(ddsim.getFactors().second, 5);
}