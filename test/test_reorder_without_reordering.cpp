#include "DDMinimizer.hpp"
#include "ir/Permutation.hpp"
#include "ir/QuantumComputation.hpp"

#include <gtest/gtest.h>
#include <string> // For std::string

using namespace qc;
using namespace std;

TEST(ReorderWithoutReorderingTest, reorderXc) {
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
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 0}, {1, 1}, {2, 2}, {3, 3}}};

  EXPECT_EQ(expectedPerm, perm);
}

TEST(ReorderWithoutReorderingTest, reorderCx) {
  const std::string testfile = "OPENQASM 2.0;\n"
                               "include \"qelib1.inc\";\n"
                               "qreg q[4];\n"
                               "creg meas[4];"
                               "h q[0];\n"
                               "h q[1];\n"
                               "h q[2];\n"
                               "h q[3];\n"
                               "cx q[0],q[1];\n"
                               "cx q[1],q[2];\n"
                               "cx q[2],q[3];\n"
                               "barrier q[0],q[1],q[2],q[3];\n"
                               "measure q[0] -> meas[0];\n"
                               "measure q[1] -> meas[1];\n"
                               "measure q[2] -> meas[2];\n"
                               "measure q[3] -> meas[3];\n";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 3}, {1, 2}, {2, 1}, {3, 0}}};

  EXPECT_EQ(expectedPerm, perm);
}

TEST(ReorderWithoutReorderingTest, reorderXccl) {
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
                               "cx q[0],q[1];\n"
                               "cx q[0],q[2];\n"
                               "cx q[0],q[3];\n"
                               "barrier q[0],q[1],q[2],q[3];\n"
                               "measure q[0] -> meas[0];\n"
                               "measure q[1] -> meas[1];\n"
                               "measure q[2] -> meas[2];\n"
                               "measure q[3] -> meas[3];\n";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 1}, {1, 2}, {2, 3}, {3, 0}}};

  EXPECT_EQ(expectedPerm, perm);
}

// failing
TEST(ReorderWithoutReorderingTest, reorderXcxh) {
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
                               "cx q[0],q[3];\n"
                               "cx q[1],q[3];\n"
                               "cx q[2],q[3];\n"
                               "barrier q[0],q[1],q[2],q[3];\n"
                               "measure q[0] -> meas[0];\n"
                               "measure q[1] -> meas[1];\n"
                               "measure q[2] -> meas[2];\n"
                               "measure q[3] -> meas[3];\n";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 3}, {1, 0}, {2, 1}, {3, 2}}};

  EXPECT_EQ(expectedPerm, perm);
}

TEST(ReorderWithoutReorderingTest, reorderCxch) {
  const std::string testfile = "OPENQASM 2.0;\n"
                               "include \"qelib1.inc\";\n"
                               "qreg q[4];\n"
                               "creg meas[4];"
                               "h q[0];\n"
                               "h q[1];\n"
                               "h q[2];\n"
                               "h q[3];\n"
                               "cx q[0],q[1];\n"
                               "cx q[1],q[2];\n"
                               "cx q[2],q[3];\n"
                               "cx q[1],q[0];\n"
                               "cx q[2],q[0];\n"
                               "cx q[3],q[0];\n"
                               "barrier q[0],q[1],q[2],q[3];\n"
                               "measure q[0] -> meas[0];\n"
                               "measure q[1] -> meas[1];\n"
                               "measure q[2] -> meas[2];\n"
                               "measure q[3] -> meas[3];\n";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 2}, {1, 1}, {2, 0}, {3, 3}}};

  EXPECT_EQ(expectedPerm, perm);
}

// failing
TEST(ReorderWithoutReorderingTest, reorderCxxl) {
  const std::string testfile = "OPENQASM 2.0;\n"
                               "include \"qelib1.inc\";\n"
                               "qreg q[4];\n"
                               "creg meas[4];"
                               "h q[0];\n"
                               "h q[1];\n"
                               "h q[2];\n"
                               "h q[3];\n"
                               "cx q[0],q[1];\n"
                               "cx q[1],q[2];\n"
                               "cx q[2],q[3];\n"
                               "cx q[3],q[0];\n"
                               "cx q[3],q[1];\n"
                               "cx q[3],q[2];\n"
                               "barrier q[0],q[1],q[2],q[3];\n"
                               "measure q[0] -> meas[0];\n"
                               "measure q[1] -> meas[1];\n"
                               "measure q[2] -> meas[2];\n"
                               "measure q[3] -> meas[3];\n";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 0}, {1, 3}, {2, 2}, {3, 1}}};

  EXPECT_EQ(expectedPerm, perm);
}

TEST(ReorderWithoutReorderingTest, reorderInterlacedQubits) {
  const std::string testfile = "OPENQASM 2.0;\n"
                               "include \"qelib1.inc\";\n"
                               "qreg q[4];\n"
                               "creg meas[4];"
                               "h q[0];\n"
                               "h q[1];\n"
                               "h q[2];\n"
                               "h q[3];\n"
                               "cx q[0],q[1];\n"
                               "cx q[1],q[3];\n"
                               "cx q[3],q[2];\n"
                               "barrier q[0],q[1],q[2],q[3];\n"
                               "measure q[0] -> meas[0];\n"
                               "measure q[1] -> meas[1];\n"
                               "measure q[2] -> meas[2];\n"
                               "measure q[3] -> meas[3];\n";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 2}, {1, 3}, {2, 1}, {3, 0}}};

  EXPECT_EQ(expectedPerm, perm);
}
