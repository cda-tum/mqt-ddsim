#include "CircuitSimulator.hpp"
#include "DDMinimizer.hpp"
#include "ir/Permutation.hpp"
#include "ir/QuantumComputation.hpp"

#include <gtest/gtest.h>
#include <iostream>

using namespace qc;
using namespace std;

void printPermutation(const Permutation& perm, const std::string& name) {
  std::cout << name << ": {";
  for (const auto& [key, value] : perm) {
    std::cout << key << " -> " << value << ", ";
  }
  std::cout << "}" << std::endl;
  std::cout.flush();
}

TEST(ReorderWithoutReorderingTest, xc) {
  // control -> target
  const std::string testfile = "OPENQASM 2.0;\n"
                               "include \"qelib1.inc\";\n"
                               "qreg q[4];\n"
                               "creg meas[4];"
                               "h q[0];\n"
                               "h q[1];\n"
                               "h q[2];\n"
                               "h q[3];\n"
                               "cx q[1],q[0];\n"
                               "cx q[2],q[1];\n"
                               "cx q[3],q[2];\n"
                               "barrier q[0],q[1],q[2],q[3];\n"
                               "measure q[0] -> meas[0];\n"
                               "measure q[1] -> meas[1];\n"
                               "measure q[2] -> meas[2];\n"
                               "measure q[3] -> meas[3];\n";

  auto qc = QuantumComputation::fromQASM(testfile);
  DDMinimizer::optimizeInputPermutation(qc);

  std::size_t bits = 4; // Example number of bits
  std::vector<Qubit> layout = {0, 1, 2, 3};

  // Create the expected permutation
  Permutation expectedPerm;
  for (Qubit i = 0; i < bits; i++) {
    expectedPerm[i] = layout[i];
  }

  printPermutation(expectedPerm, "Expected Permutation");
  printPermutation(qc.initialLayout, "Actual Permutation");

  EXPECT_EQ(expectedPerm, qc.initialLayout);
}

TEST(ReorderWithoutReorderingTest, cx) {
  // TODO
}

TEST(ReorderWithoutReorderingTest, xccl3) {
  // TODO
}

TEST(ReorderWithoutReorderingTest, xcxr3) {
  // TODO
}

TEST(ReorderWithoutReorderingTest, cxcr3) {
  // TODO
}

TEST(ReorderWithoutReorderingTest, cxxl3) {
  // TODO
}

TEST(ReorderWithoutReorderingTest, noPattern) {
  // TODO
}
