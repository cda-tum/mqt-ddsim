#include "Definitions.hpp"
#include "Permutation.hpp"
#include "QuantumComputation.hpp"
#include <chrono>


#include <cstddef>
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <tuple>
#include <map>
#include <cctype>
#include <random>
#include <utility>
#include <vector>
#include <sstream>
#include <string>
#include <set>

namespace qc {
class DDMinimizer {






public:
  static void optimizeInputPermutation(qc::QuantumComputation);

  /**
   * @brief creates a vector hopefully the best initial Layout and applies it
   * @param QuantumComputation
   */
  static void parseOptions(bool gateBased,  bool controlBased, bool allPermutations, std::string file, int qubits);
  static void runInputCompariston(int from, int to, bool gateBased,  bool controlBased, bool allPermutations, std::string file);
  static void runLayoutComparison(std::ofstream& out, qc::QuantumComputation& qc, qc::Permutation perm, std::string file, int qubits);
  static void runAllComparisons(std::ofstream& out, qc::QuantumComputation& qc, std::string file, int qubits);


  static qc::Permutation createGateBasedPermutation(qc::QuantumComputation& qc);

   /**
   * @brief creates a possible permutations of the initialLayout based on how to control bits are located
   * @param QuantumComputation
   */
  static qc::Permutation createControlBasedPermutation(qc::QuantumComputation& qc);

  /**
   * @brief creates a vector of all possible permutations of the initialLayout
   * @param QuantumComputation
   */
  static std::vector<qc::Permutation> createAllPermutations(qc::QuantumComputation& qc);

   /**
   * @brief Helper function to compute how many permutations there are for a number of qubits
   * @param number of Bits
   */
  static std::size_t factorial(std::size_t n);

  static std::string readFileIntoString(const std::string& filePath);

  static std::string permToString(Permutation perm);

  static std::string measurementToString(std::vector<bool> code, size_t index, size_t max_nodes, size_t active_nodes, std::chrono::duration<double> time);
  static std::string formatSize_t(size_t t);

}; // class DDMinimizer
} // namespace qc
