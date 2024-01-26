#include "ShorSimulator.hpp"

#include "gtest/gtest.h"
#include <memory>

/**
 * These tests may have to be adjusted if something about the random-number generation changes.
 */

TEST(ShorSimTest, Factorize15EmulationTest) {
    // add verbose = true for coverage
    ShorSimulator ddsim(15, 2, 3ULL, true, false);

    ASSERT_EQ(ddsim.getNumberOfOps(), 0);
    ASSERT_EQ(ddsim.getName(), "shor_15_2");
    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
    ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "did not start");
    ddsim.simulate(1);

    ASSERT_EQ(ddsim.getFactors().first, 3);
    ASSERT_EQ(ddsim.getFactors().second, 5);
    ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "SUCCESS(3*5)");

    ASSERT_EQ(ddsim.additionalStatistics().at("polr_factor1"), "3");
    ASSERT_EQ(ddsim.additionalStatistics().at("polr_factor2"), "5");
    ASSERT_EQ(ddsim.additionalStatistics().at("polr_result"), "SUCCESS(3*5)");
}

TEST(ShorSimTest, Factorize15EmulationNoSeedTest) {
    // add verbose = true for coverage
    ShorSimulator ddsim(15, 2, true, false);

    ASSERT_EQ(ddsim.getNumberOfOps(), 0);
    ASSERT_EQ(ddsim.getName(), "shor_15_2");
    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
    ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "did not start");
    ddsim.simulate(1);

    ASSERT_EQ(ddsim.additionalStatistics().at("polr_factor1"), "3");
    ASSERT_EQ(ddsim.additionalStatistics().at("polr_factor2"), "5");
    ASSERT_EQ(ddsim.additionalStatistics().at("polr_result"), "SUCCESS(3*5)");
}

TEST(ShorSimTest, Factorize15EmulationNoOptionalParametersTest) {
    // add verbose = true for coverage
    ShorSimulator ddsim(15, 2);

    ASSERT_EQ(ddsim.getNumberOfOps(), 0);
    ASSERT_EQ(ddsim.getName(), "shor_15_2");
    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
    ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "did not start");
    ddsim.simulate(1);

    ASSERT_EQ(ddsim.additionalStatistics().at("polr_factor1"), "3");
    ASSERT_EQ(ddsim.additionalStatistics().at("polr_factor2"), "5");
    ASSERT_EQ(ddsim.additionalStatistics().at("polr_result"), "SUCCESS(3*5)");
}

TEST(ShorSimTest, Factorize15EmulationNoCoprimeParametersTest) {
    // add verbose = true for coverage
    ShorSimulator ddsim(15, 0);

    ASSERT_EQ(ddsim.getNumberOfOps(), 0);
    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
    ASSERT_EQ(ddsim.additionalStatistics().at("coprime_a"), "0");
    ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "did not start");
    ddsim.simulate(1);

    ASSERT_NE(ddsim.additionalStatistics().at("coprime_a"), "0");
}

TEST(ShorSimTest, Factorize15EmulationInvalidCoprimeParametersTest) {
    // add verbose = true for coverage
    ShorSimulator ddsim(15, 3);

    ASSERT_EQ(ddsim.getNumberOfOps(), 0);
    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
    ASSERT_EQ(ddsim.additionalStatistics().at("coprime_a"), "3");
    ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "did not start");
    ddsim.simulate(1);

    ASSERT_NE(ddsim.additionalStatistics().at("coprime_a"), "3");
}

TEST(ShorSimTest, Factorize15NegTest) {
    ShorSimulator ddsim(15, 2, 1ULL);
    ddsim.simulate(1);

    ASSERT_EQ(ddsim.getFactors().first, 0);
    ASSERT_EQ(ddsim.getFactors().second, 0);
    ASSERT_EQ(ddsim.additionalStatistics().at("sim_result"), "FAILURE");

    ASSERT_EQ(ddsim.additionalStatistics().at("polr_factor1"), "3");
    ASSERT_EQ(ddsim.additionalStatistics().at("polr_factor2"), "5");
    ASSERT_EQ(ddsim.additionalStatistics().at("polr_result"), "SUCCESS(3*5)");
}

TEST(ShorSimTest, Factorize55Test) {
    ShorSimulator ddsim(55, 2, 3ULL);
    ddsim.simulate(1);

    ASSERT_EQ(ddsim.getFactors().first, 11);
    ASSERT_EQ(ddsim.getFactors().second, 5);
}

TEST(ShorSimTest, Factorize55ApproximateTest) {
    ShorSimulator ddsim(55, 2, 3ULL, true, true);
    ddsim.simulate(1);

    ASSERT_EQ(ddsim.getFactors().first, 11);
    ASSERT_EQ(ddsim.getFactors().second, 5);
}
