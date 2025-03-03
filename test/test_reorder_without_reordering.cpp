#include "DDMinimizer.hpp"
#include "ir/Permutation.hpp"
#include "ir/QuantumComputation.hpp"

#include <gtest/gtest.h>
#include <string> // For std::string

using namespace qc;
using namespace std;

TEST(ReorderWithoutReorderingTest, reorderXc) {
  // control -> target
  const std::string testfile = "OPENQASM 2.0;"
                               "include \"qelib1.inc\";"
                               "qreg q[4];"
                               "creg meas[4];"
                               "h q;"
                               "cx q[1],q[0];"
                               "cx q[2],q[1];"
                               "cx q[3],q[2];"
                               "barrier q;"
                               "measure q -> meas;";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 0}, {1, 1}, {2, 2}, {3, 3}}};

  EXPECT_EQ(expectedPerm, perm);
}

TEST(ReorderWithoutReorderingTest, reorderCx) {
  const std::string testfile = "OPENQASM 2.0;"
                               "include \"qelib1.inc\";"
                               "qreg q[4];"
                               "creg meas[4];"
                               "h q;"
                               "cx q[0],q[1];"
                               "cx q[1],q[2];"
                               "cx q[2],q[3];"
                               "barrier q;"
                               "measure q -> meas;";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 3}, {1, 2}, {2, 1}, {3, 0}}};

  EXPECT_EQ(expectedPerm, perm);
}

TEST(ReorderWithoutReorderingTest, reorderXccl) {
  const std::string testfile = "OPENQASM 2.0;"
                               "include \"qelib1.inc\";"
                               "qreg q[4];"
                               "creg meas[4];"
                               "h q;"
                               "cx q[1],q[0];"
                               "cx q[2],q[1];"
                               "cx q[3],q[2];"
                               "cx q[0],q[1];"
                               "cx q[0],q[2];"
                               "cx q[0],q[3];"
                               "barrier q;"
                               "measure q -> meas;";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 1}, {1, 2}, {2, 3}, {3, 0}}};

  EXPECT_EQ(expectedPerm, perm);
}

// failing
TEST(ReorderWithoutReorderingTest, reorderXcxh) {
  const std::string testfile = "OPENQASM 2.0;"
                               "include \"qelib1.inc\";"
                               "qreg q[4];"
                               "creg meas[4];"
                               "h q;"
                               "cx q[1],q[0];"
                               "cx q[2],q[1];"
                               "cx q[3],q[2];"
                               "cx q[0],q[3];"
                               "cx q[1],q[3];"
                               "cx q[2],q[3];"
                               "barrier q;"
                               "measure q -> meas;";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 3}, {1, 0}, {2, 1}, {3, 2}}};

  EXPECT_EQ(expectedPerm, perm);
}

TEST(ReorderWithoutReorderingTest, reorderCxch) {
  const std::string testfile = "OPENQASM 2.0;"
                               "include \"qelib1.inc\";"
                               "qreg q[4];"
                               "creg meas[4];"
                               "h q;"
                               "cx q[0],q[1];"
                               "cx q[1],q[2];"
                               "cx q[2],q[3];"
                               "cx q[1],q[0];"
                               "cx q[2],q[0];"
                               "cx q[3],q[0];"
                               "barrier q;"
                               "measure q -> meas;";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 2}, {1, 1}, {2, 0}, {3, 3}}};

  EXPECT_EQ(expectedPerm, perm);
}

// failing
TEST(ReorderWithoutReorderingTest, reorderCxxl) {
  const std::string testfile = "OPENQASM 2.0;"
                               "include \"qelib1.inc\";"
                               "qreg q[4];"
                               "creg meas[4];"
                               "h q;"
                               "cx q[0],q[1];"
                               "cx q[1],q[2];"
                               "cx q[2],q[3];"
                               "cx q[3],q[0];"
                               "cx q[3],q[1];"
                               "cx q[3],q[2];"
                               "barrier q;"
                               "measure q -> meas;";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 0}, {1, 3}, {2, 2}, {3, 1}}};

  EXPECT_EQ(expectedPerm, perm);
}

TEST(ReorderWithoutReorderingTest, reorderInterlacedQubits) {
  const std::string testfile = "OPENQASM 2.0;"
                               "include \"qelib1.inc\";"
                               "qreg q[4];"
                               "creg meas[4];"
                               "h q;"
                               "cx q[0],q[1];"
                               "cx q[1],q[3];"
                               "cx q[3],q[2];"
                               "barrier q;"
                               "measure q -> meas;";

  auto qc = QuantumComputation::fromQASM(testfile);
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);

  const Permutation expectedPerm = {{{0, 2}, {1, 3}, {2, 1}, {3, 0}}};

  EXPECT_EQ(expectedPerm, perm);
}
