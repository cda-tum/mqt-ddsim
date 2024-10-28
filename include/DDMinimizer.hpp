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
   * @brief Changes the order of qubits in a QuantumComputation to heuristically
   * optimize for short running times of the DD-simulator.
   * @detail Computes an initialLayout for the QuantumComputation based on a
   * heuristic to optimize the running time of the DD-simulator. After that, the
   * initialLayout is applied, i.e. the qubits in the QuantumComputation are
   * re-ordered such that the resulting QuantumComputation's initialLayout is
   * the identity again. The current implementation is based on patterns found
   * in the controlled gates.
   * @param param qc is the QuantumComputation to optimize the layout for
   */
  static void optimizeInputPermutation(qc::QuantumComputation& qc);


 /**
   * @details First collects operation indices of controlled operation for
   * patterns (s. makeDataStructure). Then, based on the pattern of the
   * controlled gates, the layout is adjusted. If no pattern is found, the
   * control based permutation is created.
   * @param qc is the QuantumComputation to optimize the layout for
   * @return the qc::Permutation The computed permutation to be used as the
   * initialLayout for a QuantumComputation
   */
  static qc::Permutation createGateBasedPermutation(qc::QuantumComputation& qc); 

private:
  //The data structure for the pattern analysis of controlled gates: 
  /**
   *The ladder x_c and c_x describe for three qubits the following controlled gates (c: control
    qubit, x: target qubit):

    * c_x: c | 0  1  2
           x | 1  2  3

    * x_c: c | 1  2  3
           x | 0  1  2
   */
  std::map<std::pair<Qubit, Qubit>, int> xCMap;
  std::map<std::pair<Qubit, Qubit>, int> cXMap;

  /**
    *The ladders c_l, c_r, x_l, x_r consist of several steps, hence the vector of maps. 
     They describe for three qubits the following controlled gates (c: control
     qubit, x: target qubit):
    * c_l_1: c | 0  0  0  and  c_l_2: c | 1  1  and  c_l_3: c | 2
             x | 1  2  3              x | 2  3              x | 3

    * c_r_1: c | 3  3  3  and  c_r_2: c | 2  2  and  c_r_3: c | 1
             x | 0  1  2              x | 0  1              x | 0

    * x_l_1: c | 1  2  3  and  x_l_2: c | 2  3  and  x_l_3: c | 3
             x | 0  0  0              x | 1  1              x | 2

    * x_r_1: c | 0  1  2  and  x_r_2: c | 0  1  and  x_r_3: c | 0
             x | 3  3  3              x | 2  2              x | 1
   */
  std::vector<std::map<std::pair<Qubit, Qubit>, int>> cLMap;
  std::vector<std::map<std::pair<Qubit, Qubit>, int>> cHMap;
  std::vector<std::map<std::pair<Qubit, Qubit>, int>> xLMap;
  std::vector<std::map<std::pair<Qubit, Qubit>, int>> xHMap;

  

  // Helper functions for createGateBasedPermutation
  /**
  * @brief creates a data structure for the pattern analysis of controlled gates
  * @details the data structure consists of two maps:
  * 1. map: string of ladder (step) name to control and target bit to the index
    of the operation
  * 2. map: string of ladder name to vector: max index of operation for each
    step or the c_x or x_c ladder
  * for c_l and x_l position 0 in the vector marks the line of c(x) at 0 -> we
    count the left most as the first
  * for c_r and x_r position 0 in the vector marks the line of c(x) at bits - 1
    -> we count theright most as the first
  *
  * The ladder (steps) describe the following controlled gates (c: control
    qubit, x: target qubit): e.g. for three qubits
        * c_x: c | 0  1  2
               x | 1  2  3

        * x_c: c | 1  2  3
               x | 0  1  2

        * c_l_1: c | 0  0  0  and  c_l_2: c | 1  1  and  c_l_3: c | 2
                 x | 1  2  3              x | 2  3              x | 3

        * c_r_1: c | 3  3  3  and  c_r_2: c | 2  2  and  c_r_3: c | 1
                 x | 0  1  2              x | 0  1              x | 0

        * x_l_1: c | 1  2  3  and  x_l_2: c | 2  3  and  x_l_3: c | 3
                 x | 0  0  0              x | 1  1              x | 2

        * x_r_1: c | 0  1  2  and  x_r_2: c | 0  1  and  x_r_3: c | 0
                 x | 3  3  3              x | 2  2              x | 1
  */

  /** 
    * @brief initializes the data structure for the pattern analysis of controlled gates
    * @details The function sets the qubits of the xCMap, cXMap, cLMap, cHMap, xLMap, xHMap depending on the number of qubits in the QuantumComputation
    * @param bits is the number of qubits in the QuantumComputation
  */
  void initializeDataStructure(std::size_t bits);


  /**
   * @brief Helper function to find the maximum Instruction index of the coplete patterns
   * @details Returns the max index if the pattern is complete, otherwise -1
   * @param map of the pattern map
   * @return the maximum index
   */
  static int findMaxIndex(const std::map<std::pair<Qubit, Qubit>, int>& map);

  // Functions to analyze the pattern of the controlled gates
  static bool isFullLadder(const std::vector<int>& vec);
  static std::size_t getStairCount(const std::vector<int>& vec);
  static int getLadderPosition(const std::vector<int>& vec, int ladder);

  // Functions to adjust the layout based on the pattern of the controlled
  // gates:

  /**
   * @brief Helper function to reverse the layout (q: qubit, l: layer)
   * @details q | 0  1  2  3  turns to q | 0  1  2  3
   *          l | 0  1  2  3           l | 3  2  1  0
   * @param layout
   * @return the reversed layout
   */
  static std::vector<Qubit> reverseLayout(std::vector<Qubit>& layout);

  /**
   * @brief Helper function to rotate the layout to the right (q: qubit, l:
   * layer)
   * @details q | 0  1  2  3 and 1 stairs turns to q | 0  1  2  3
   *          l | 0  1  2  3                       l | 1  2  3  0
   * @param layout, stairs (number of steps to rotate)
   * @return the rotated layout
   */
  static std::vector<Qubit> rotateRight(std::vector<Qubit> layout,
                                        std::size_t stairs);

  /**
   * @brief Helper function to rotate the layout to the left (q: qubit, l:
   * layer)
   * @details q | 0  1  2  3 and 2 stairs turns to q | 0  1  2  3
   *          l | 0  1  2  3                       l | 3  0  1  2
   * @param layout, stairs (number of steps to rotate)
   * @return the rotated layout
   */
  static std::vector<Qubit> rotateLeft(std::vector<Qubit> layout,
                                       std::size_t stairs);

  /**
   * @brief creates a Heuristic based initialLayout for the QuantumComputation.
     This implementation is based on which qubits are controlled by whichqubits
   * @details The function creates a map of each qubit to all the qubits it
   controls.
   * Based on the control to target relationship, a weight for each qubit is
   calculated.
   * The qubits are then sorted based on the weight in increasing order,
   * each controlling qubit is placed after its targets
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
