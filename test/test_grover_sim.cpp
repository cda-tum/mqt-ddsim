#include "GroverSimulator.hpp"

#include <gtest/gtest.h>

/**
 * These tests may have to be adjusted if something about the random-number
 * generation changes.
 */

TEST(GroverSimTest, EmulatedOracle14Test) {
  GroverSimulator ddsim(14, 0);
  ddsim.simulate(1);

  EXPECT_EQ(ddsim.getNumberOfOps(), 0);
  EXPECT_EQ(ddsim.getName(), "emulated_grover_14");
  EXPECT_EQ(ddsim.getOracle(), ddsim.additionalStatistics().at("oracle"));
  ASSERT_EQ(ddsim.getPathOfLeastResistance().second.substr(1),
            ddsim.additionalStatistics().at("oracle"));
}

TEST(GroverSimTest, EmulatedFixedOracleFixedSeedTest) {
  GroverSimulator ddsim("0110011", 0);
  ddsim.simulate(1);

  EXPECT_EQ(ddsim.getNumberOfOps(), 0);
  EXPECT_EQ(ddsim.getName(), "emulated_grover_7");
  EXPECT_EQ(ddsim.getOracle(), ddsim.additionalStatistics().at("oracle"));
  ASSERT_EQ(ddsim.getPathOfLeastResistance().second.substr(1),
            ddsim.additionalStatistics().at("oracle"));
}

TEST(GroverSimTest, EmulatedFixedOracleTest) {
  GroverSimulator ddsim("0110001");
  ddsim.simulate(1);

  EXPECT_EQ(ddsim.getNumberOfOps(), 0);
  EXPECT_EQ(ddsim.getName(), "emulated_grover_7");
  EXPECT_EQ(ddsim.getOracle(), ddsim.additionalStatistics().at("oracle"));
  ASSERT_EQ(ddsim.getPathOfLeastResistance().second.substr(1),
            ddsim.additionalStatistics().at("oracle"));
}
