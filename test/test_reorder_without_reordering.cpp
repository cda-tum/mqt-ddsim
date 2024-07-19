#include "DDMinimizer.hpp"
#include "CircuitSimulator.hpp"
#include "CircuitOptimizer.hpp"
#include "QuantumComputation.hpp"
#include "operations/OpType.hpp"
#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <memory>
#include <algorithm>
#include <tuple>
#include <map>
#include <algorithm>
#include <cctype>

using namespace qc;
using namespace std;

std::string emptyfile = " ";


TEST(ReorderWithoutReorderingTest, longRun) {
     qc::DDMinimizer::runOverNight();
}

TEST(ReorderWithoutReorderingTest, runAll) {
     qc::DDMinimizer::parseOptions(true, true, true, emptyfile, 0);
}

TEST(ReorderWithoutReorderingTest, reorderAllByGates) {
     qc::DDMinimizer::parseOptions(true, false, false, emptyfile, 0);
}

  TEST(ReorderWithoutReorderingTest, reorderAllByControls) {
    qc::DDMinimizer::parseOptions(false, true, false, emptyfile, 0);
}

  TEST(ReorderWithoutReorderingTest, reorderAllByAll) {
     qc::DDMinimizer::parseOptions(false, false, true, emptyfile, 0);
}

 TEST(ReorderWithoutReorderingTest, grover) {
     qc::DDMinimizer::parseOptions(false, false, true, "grover_", 5);
}       

 TEST(ReorderWithoutReorderingTest, assumption) {
     qc::DDMinimizer::parseOptions(false, false, true, "aa", 0);
}  