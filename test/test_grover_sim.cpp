#include "GroverSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */

TEST(GroverSimTest, EmulatedOracle14Test) {
    GroverSimulator ddsim(14, 0);
    ddsim.Simulate();

    ASSERT_EQ(ddsim.getPathOfLeastResistance().second.substr(1), ddsim.AdditionalStatistics().at("oracle"));
}