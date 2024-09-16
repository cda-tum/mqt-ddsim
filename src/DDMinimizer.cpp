#include "DDMinimizer.hpp"

#include "circuit_optimizer/CircuitOptimizer.hpp"
#include "ir/Permutation.hpp"
#include "ir/QuantumComputation.hpp"
#include "ir/operations/Control.hpp"
#include "ir/operations/OpType.hpp"

#include <cstddef>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

using namespace qc;
using namespace std;

namespace qc {

/***
 * Public Methods
 ***/

void optimizeInputPermutation(qc::QuantumComputation qc) {
  // create, set and apply the heuristics based permutation
  const qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);
  qc.initialLayout = perm;
  qc::CircuitOptimizer::elidePermutations(qc);
}

qc::Permutation
DDMinimizer::createGateBasedPermutation(qc::QuantumComputation& qc) {

  // create the data structure to store the indices in the pattern maps as well
  // as the max indices of the ladders
  std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>> maps =
      DDMinimizer::makeDataStructure(qc).first;
  std::map<std::string, std::vector<int>> indices =
      DDMinimizer::makeDataStructure(qc).second;
  const std::size_t bits = qc.getNqubits();

  // iterate over all the ops and mark the index of the found x-c pairs in the
  // map.
  int instructionIndex = 0;
  bool found = false;
  for (const auto& op : qc) {
    if (!op->isStandardOperation() || op->getType() == Z) {
      continue;
    }
    const Controls controls = op->getControls();
    const std::vector<Qubit> targets = {op->getTargets().begin(),
                                        op->getTargets().end()};

    for (const auto& control : controls) {
      for (const auto& target : targets) {
        for (auto& map : maps) {
          auto it = map.second.find({control.qubit, target});
          if (it != map.second.end()) {
            it->second = instructionIndex;
            found = true;
          }
        }
      }
    }
    instructionIndex++;
  }
  if (!found) {
    return qc.initialLayout;
  }

  // iterate over all the maps and find the max index of the found x-c pairs
  for (const auto& map : maps) {
    int max = -1;
    for (const auto& pair : map.second) {
      if (pair.second == -1) {
        max = -1;
        break;
      }
      if (pair.second > max) {
        max = pair.second;
      }
    }
    std::string map_name = map.first.substr(0, 3);
    auto it = indices.find(map_name);

    if (it != indices.end()) {
      if (map_name[2] == 'r') {
        const auto column = static_cast<std::size_t>(map.first[4] - '0');
        it->second[bits - 1 - column] = max;
      } else if (map_name[2] == 'l') {
        const auto column = static_cast<std::size_t>(map.first[4] - '0');
        it->second[column] = max;
      } else {
        it->second[0] = max;
      }
    }
  }

  // create the permutation based on the order of max index in the complete maps
  std::vector<Qubit> layout(bits);
  std::iota(layout.begin(), layout.end(), 0);

  auto cX = indices.find("cX");
  auto xC = indices.find("xC");
  auto cL = indices.find("cL");
  auto xL = indices.find("xL");
  auto cR = indices.find("cR");
  auto xR = indices.find("xR");

  const int prio_cR =
      DDMinimizer::getLadderPosition(cR->second, xC->second[0]);
  const int prio_xL =
      DDMinimizer::getLadderPosition(xL->second, xC->second[0]);
  const int stairs_cR = DDMinimizer::getStairCount(cR->second);
  const int stairs_xL = DDMinimizer::getStairCount(xL->second);
  const int prio_cL =
      DDMinimizer::getLadderPosition(cL->second, cX->second[0]);
  const int prio_xR =
      DDMinimizer::getLadderPosition(xR->second, cX->second[0]);
  const int stairs_cL = DDMinimizer::getStairCount(cL->second);
  const int stairs_xR = DDMinimizer::getStairCount(xR->second);

  // complete case checkins and adjust the layout
  if ((cX->second[0] != -1) &&
      (xC->second[0] == -1 || cX->second[0] < xC->second[0])) {

    if ((prio_cR == 0 && DDMinimizer::isFull(cR->second)) ||
        (prio_xL == 0 && DDMinimizer::isFull(xL->second))) {
    } else if (prio_cR == 0 && stairs_cR > 0) {
      layout = DDMinimizer::reverseLayout(layout);
      layout = DDMinimizer::rotateRight(layout, stairs_cR);
    } else if (prio_xL == 0 && stairs_xL > 0) {
      layout = DDMinimizer::reverseLayout(layout);
      layout = DDMinimizer::rotateLeft(layout, stairs_xL);
    } else if (prio_cR > 0 || prio_xL > 0 ||
               (prio_cR == 0 && prio_xL == 0)) {
      layout = DDMinimizer::reverseLayout(layout);
    }
  } else if ((xC->second[0] != -1) &&
             (cX->second[0] == -1 || cX->second[0] > xC->second[0])) {

    if ((prio_cL == 0 && DDMinimizer::isFull(cL->second)) ||
        (prio_xR == 0 && DDMinimizer::isFull(xR->second))) {
      layout = DDMinimizer::reverseLayout(layout);
    }

    else if (prio_cL == 0 && stairs_cL > 0) {
      layout = DDMinimizer::rotateLeft(layout, stairs_cL);
    } else if (prio_xR == 0 && stairs_xR > 0) {
      layout = DDMinimizer::rotateRight(layout, stairs_xR);
    }
  } else if ((DDMinimizer::isFull(xR->second) ||
              DDMinimizer::isFull(cL->second))) {
    layout = DDMinimizer::reverseLayout(layout);
  } else {
    return DDMinimizer::createControlBasedPermutation(qc);
  }

  // transform layout into permutation
  qc::Permutation perm;
  std::vector<Qubit> layer(bits);
  for (qc::Qubit i = 0; i < bits; i++) {
    layer[i] = i;
  }
  for (qc::Qubit i = 0; i < bits; i++) {
    perm[layer[i]] = layout[i];
  }
  return perm;
}

// Helper function to create the data structure for the pattern analysis of
// controlled gates
std::pair<std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>>,
          std::map<std::string, std::vector<int>>>
DDMinimizer::makeDataStructure(qc::QuantumComputation& qc) {
  const std::size_t bits = qc.getNqubits();

  // map string of cX, xC, cL, xL, cR, xR to map of control and target bit
  // to the index of the gate
  std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>> maps;

  // string of cX, xC, cL, xL, cR, xR to vector of max index of gate
  // for cL and xL position 0 in the vector marks the line of c(x) at 0 -> we
  // count the left most as the first for cR and xR position 0 in the vector
  // marks the line of c(x) at bits - 1 -> we count the right most as the first
  std::map<std::string, std::vector<int>> indices;

  // create x-c ladder
  const std::size_t max = bits - 1;

  // maps for the x-c and c-x ladder with pair of control qubit and target qubit
  // to index
  std::map<pair<Qubit, Qubit>, int> x_c_map;
  std::map<pair<Qubit, Qubit>, int> cX_map;
  for (size_t i = 0; i < max; i++) {
    x_c_map.insert({{i + 1, i}, -1});
    cX_map.insert({{i, i + 1}, -1});
  }
  // save complete maps of ladder and for max index evaluation
  maps.insert({"xC", x_c_map});
  indices.insert({"xC", std::vector<int>(1, 0)});
  maps.insert({"cX", cX_map});
  indices.insert({"cX", std::vector<int>(1, 0)});

  // create c-l and x-l ladder
  for (size_t i = 0; i < max; i++) {
    std::map<pair<Qubit, Qubit>, int> xL_map;
    std::map<pair<Qubit, Qubit>, int> cL_map;
    // create the steps of the ladder
    for (size_t j = 0; j < bits; j++) {
      if (i < j) {
        xL_map.insert({{j, i}, -1});
        cL_map.insert({{i, j}, -1});
      }
    }
    // save the steps in the maps
    maps.insert({"xL_" + std::to_string(i), xL_map});
    maps.insert({"cL_" + std::to_string(i), cL_map});
  }
  // save the complete ladder for max index evaluation
  indices.insert({"xL", std::vector<int>(bits - 1, 0)});
  indices.insert({"cL", std::vector<int>(bits - 1, 0)});

  // create c-r and x-r ladder
  for (size_t i = max; i > 0; i--) {
    std::map<pair<Qubit, Qubit>, int> xR_map;
    std::map<pair<Qubit, Qubit>, int> cR_map;
    // create the steps of the ladder
    for (size_t j = 0; j < bits; j++) {
      if (i > j) {
        xR_map.insert({{j, i}, -1});
        cR_map.insert({{i, j}, -1});
      }
    }
    // save the steps in the maps
    maps.insert({"xR_" + std::to_string(i), xR_map});
    maps.insert({"cR_" + std::to_string(i), cR_map});
  }
  // save the complete ladder for max index evaluation
  indices.insert({"xR", std::vector<int>(bits - 1, 0)});
  indices.insert({"cR", std::vector<int>(bits - 1, 0)});

  return std::make_pair(maps, indices);
}

// Helper function to check if the vector of a ladder step is full, meaning each
// gate appeared in the circuit
bool DDMinimizer::isFull(const std::vector<int>& vec) {
  std::size_t countNegativeOne = 0;
  for (const int value : vec) {
    if (value == -1) {
      countNegativeOne++;
    }
  }
  if (countNegativeOne == 0) {
    return true;
  }
  return false;
}

// Helper function to get the number of complete stairs in a ladder
int DDMinimizer::getStairCount(const std::vector<int>& vec) {
  int count = 0;
  for (const int value : vec) {
    if (value != -1) {
      count++;
    } else {
      return count;
    }
  }
  return count;
}

// Helper function to get the position of a ladder in comparison to other steps
int DDMinimizer::getLadderPosition(const std::vector<int>& vec, int ladder) {
  int count = 0;
  for (size_t i = 0; i < vec.size(); i++) {
    const int value = vec[i];

    if (i == vec.size() - 1 && count != 0) {
      if (value != -1 && value < ladder) {
        count++;
      }
    }
  }
  return count;
}

// Helper functions to manipulate the Layout: reverse, rotate right and left

std::vector<Qubit> DDMinimizer::reverseLayout(std::vector<Qubit> layout) {
  std::vector<Qubit> reversedLayout(layout.size());
  for (std::size_t i = 0; i < layout.size(); i++) {
    reversedLayout[i] = layout[layout.size() - 1 - i];
  }
  return reversedLayout;
}

std::vector<Qubit> DDMinimizer::rotateRight(std::vector<Qubit> layout,
                                            int stairs) {
  const std::vector<Qubit>::size_type size = layout.size();
  std::vector<Qubit> rotatedLayout(size);
  for (int r = 0; r < stairs; ++r) {
    if (!layout.empty()) {
      rotatedLayout[size - static_cast<std::vector<Qubit>::size_type>(r + 1)] =
          layout[static_cast<std::vector<Qubit>::size_type>(r)];
    }
  }
  const std::vector<Qubit>::size_type left =
      size - static_cast<std::vector<Qubit>::size_type>(stairs);
  for (std::vector<Qubit>::size_type r = 0; r < left; ++r) {
    if (!layout.empty()) {
      rotatedLayout[r] =
          layout[r + static_cast<std::vector<Qubit>::size_type>(stairs)];
    }
  }
  return rotatedLayout;
}

std::vector<Qubit> DDMinimizer::rotateLeft(std::vector<Qubit> layout,
                                           int stairs) {
  const std::vector<Qubit>::size_type size = layout.size();
  std::vector<Qubit> rotatedLayout(size);
  for (int r = 0; r < stairs; ++r) {
    if (!layout.empty()) {
      rotatedLayout[static_cast<std::vector<Qubit>::size_type>(r)] =
          layout[size - static_cast<std::vector<Qubit>::size_type>(r + 1)];
    }
  }
  const std::vector<Qubit>::size_type left =
      size - static_cast<std::vector<Qubit>::size_type>(stairs);
  for (std::vector<Qubit>::size_type r = 0; r < left; ++r) {
    if (!layout.empty()) {
      rotatedLayout[r + static_cast<std::vector<Qubit>::size_type>(stairs)] =
          layout[r];
    }
  }
  return rotatedLayout;
}

// Fallback function to create a control based permutation if no pattern is
// found in the controlled gates
qc::Permutation
DDMinimizer::createControlBasedPermutation(qc::QuantumComputation& qc) {
  // create and fill a map of each qubit to all the qubits it controls
  std::map<Qubit, std::set<Qubit>> controlToTargets;

  // iterate over all the ops to mark which qubits are controlled by which
  // qubits
  for (const auto& op : qc) {
    if (!op->isStandardOperation()) {
      continue;
    }
    const Controls controls = op->getControls();
    const std::set<Qubit>& targets = {op->getTargets().begin(),
                                     op->getTargets().end()};

    for (const auto& control : controls) {
      if (controlToTargets.find(control.qubit) == controlToTargets.end()) {
        // If the control does not exist in the map, add it with the current set
        // of targetInts
        controlToTargets[control.qubit] = targets;
      } else {
        // If the control exists, insert the new targets into the existing set
        controlToTargets[control.qubit].insert(targets.begin(), targets.end());
      }
    }
  }

  if (controlToTargets.empty()) {
    return qc.initialLayout;
  }

  // create a weights map for the qubits to evaluate the order of the qubits
  const std::size_t bits = qc.getNqubits();

  std::vector<Qubit> qubit(bits);
  std::vector<Qubit> layer(bits);
  std::map<Qubit, int> qubitWeights;
  for (Qubit i = 0; i < bits; ++i) {
    layer[i] = i;
    qubit[i] = i;
    qubitWeights.insert({i, 1});
  }

  std::set<Qubit> encounteredTargets;
  int weight = 1;
  // compute and anjust the weight of the controlling qubit based on all control
  // to target relations
  for (const auto& pair : controlToTargets) {
    // if the current control qubit is already encountered as a previous target,
    // adjust the weights
    if (encounteredTargets.find(pair.first) != encounteredTargets.end()) {
      qubitWeights = DDMinimizer::adjustWeights(
          qubitWeights, encounteredTargets, pair.first, controlToTargets, 1);
    }
    // get a base line weight for the control qubit as the previous one
    weight = qubitWeights[pair.first];

    // iterate over all the targets that are controlled by the current qubit in
    // current item of the control to target map
    for (const auto& target : pair.second) {
      // save all the encountered targets
      encounteredTargets.insert(target);
      // compute the new weight of the control qubit based on all its targets
      // and the base line
      weight = std::max(qubitWeights[target], weight + 1);
    }
    weight++;
    qubitWeights[pair.first] = weight;
  }

  // sort the layout based on the weights of the qubits
  sort(layer.begin(), layer.end(),
       [&qubitWeights](const Qubit& a, const Qubit& b) {
         auto weightA = qubitWeights.find(a) != qubitWeights.end()
                            ? qubitWeights.at(a)
                            : 0;
         auto weightB = qubitWeights.find(b) != qubitWeights.end()
                            ? qubitWeights.at(b)
                            : 0;
         return weightA < weightB;
       });

  // transform the layout into a permutationq
  qc::Permutation perm;
  for (std::size_t m = 0; m < bits; m++) {
    perm[qubit[m]] = layer[m];
  }
  return perm;
}

// Helper function to adjust the weights of the qubits when a controlling qubit
// was previously a target
std::map<Qubit, int> DDMinimizer::adjustWeights(
    std::map<Qubit, int> qubitWeights, const std::set<Qubit>& targets,
    Qubit ctrl, const std::map<Qubit, std::set<Qubit>>& controlToTargets, 
    int count) {
  // avoid infinite loop
  if (count == static_cast<int>(controlToTargets.size())) {    
    return qubitWeights;
  }
  // recoursively increase all the weights of the control qubits where the
  // current controlling one was a target
  for (const auto& controlPair : controlToTargets) {
    if (controlPair.second.find(ctrl) != controlPair.second.end()) {
      const Qubit control = controlPair.first;
      qubitWeights[controlPair.first]++;
      qubitWeights = DDMinimizer::adjustWeights(qubitWeights, targets, control,
                                                controlToTargets, count + 1);
    }
  }
  return qubitWeights;
}

// Helper function to compute how many permutations there are for a set number
// of qubits
std::size_t DDMinimizer::factorial(std::size_t n) {
  std::size_t result = 1;
  for (std::size_t i = 1; i <= n; ++i) {
    result *= i;
  }
  return result;
}

}; // namespace qc
