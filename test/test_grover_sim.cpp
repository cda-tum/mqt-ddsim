#include "GroverSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */

TEST(GroverSimTest, EmulatedOracle14Test) {
    GroverSimulator ddsim(14, 0);
    ddsim.Simulate(1);

    EXPECT_EQ(ddsim.getNumberOfOps(), 0);
    EXPECT_EQ(ddsim.getName(), "emulated_grover_14");
    ASSERT_EQ(ddsim.getPathOfLeastResistance().second.substr(1), ddsim.AdditionalStatistics().at("oracle"));
}

TEST(GroverSimTest, EmulatedFixedOracleTest) {
    GroverSimulator ddsim("0110011", 0);
    ddsim.Simulate(1);

    EXPECT_EQ(ddsim.getNumberOfOps(), 0);
    EXPECT_EQ(ddsim.getName(), "emulated_grover_7");
    ASSERT_EQ(ddsim.getPathOfLeastResistance().second.substr(1), ddsim.AdditionalStatistics().at("oracle"));
}
