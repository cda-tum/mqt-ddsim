#include "Definitions.hpp"
#include "ir/Permutation.hpp"
#include "ir/QuantumComputation.hpp"

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace qc {
class DDMinimizer {

public:
  /**
   * @brief creates a Heuristic based initialLayout for the QuantumComputation
   and applies it. The current implementation is based on pattern found in the
   controlled gates
   * @param QuantumComputation
  */
  static void optimizeInputPermutation(qc::QuantumComputation);

  /**
   * @brief creates a Heuristic based initialLayout for the QuantumComputation.
            This implementation is based on pattern found in the controlled
   gates
   * @param QuantumComputation
   * @return the qc::Permutation
  */
  static qc::Permutation createGateBasedPermutation(qc::QuantumComputation& qc);

  // Helper functions for createGateBasedPermutation
  /**
  * @brief creates a data structure for the pattern analysis of controlled gates
  * @param QuantumComputation
  * @return a pair of maps:
          1. map string of c_x, x_c, c_l, x_l, c_r, x_r ladder step to control
  and target bit to the index of the gate
          2. map: string of c_x, x_c, c_l, x_l, c_r, x_r ladder to vector of max
  index of gate for the indices: for c_l and x_l position 0 in the vector marks
  the line of c(x) at 0 -> we count the left most as the first for c_r and x_r
  position 0 in the vector marks the line of c(x) at bits - 1 -> we count the
  right most as the first
  */
  static std::pair<
      std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>>,
      std::map<std::string, std::vector<int>>>
  makeDataStructure(qc::QuantumComputation& qc);

  // Functions to analyze the pattern of the controlled gates
  static bool isFull(const std::vector<int>& vec);
  static int getStairCount(const std::vector<int>& vec);
  static int getLadderPosition(const std::vector<int>& vec, int laadder);

  // Functions to adjust the layout based on the pattern of the controlled gates
  static std::vector<Qubit> reverseLayout(std::vector<Qubit> layout);
  static std::vector<Qubit> rotateRight(std::vector<Qubit> layout, int stairs);
  static std::vector<Qubit> rotateLeft(std::vector<Qubit> layout, int stairs);

  /**
   * @brief creates a Heuristic based initialLayout for the QuantumComputation.
            This implementation is based on which qubits are controlled by which
   qubits
   * @param QuantumComputation
   * @return the qc::Permutation
   */
  static qc::Permutation
  createControlBasedPermutation(qc::QuantumComputation& qc);

  // Helper function for createControlBasedPermutation

  /**
   * @brief recursively adjusts the weights of the qubits based on the control
   * and target qubits
   * @param map of qubit to weight, current set of target qubits, current
   * control qubit, overall map of control qubit to target qubits, count of
   * recoursive calls
   * @return adjusted map of qubit to weight
   */
  static std::map<Qubit, int>
  adjustWeights(std::map<Qubit, int> qubitWeights,
                const std::set<Qubit>& targets, Qubit ctrl,
                const std::map<Qubit, std::set<Qubit>>& controlToTargets,
                int count);

  /**
  * @brief creates a vector of all possible permutations of the initialLayout
            used for first implementation of testing all Permutations and
  drawing conclusions
  * @param QuantumComputation
  */
  static std::vector<qc::Permutation>
  createAllPermutations(qc::QuantumComputation& qc);

  /**
   * @brief Helper function to compute how many permutations there are for a set
   *        number of qubits
   * @param number of Bits
   */
  static std::size_t factorial(std::size_t n);

}; // class DDMinimizer
} // namespace qc
