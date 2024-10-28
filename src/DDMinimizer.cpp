#include "DDMinimizer.hpp"

#include "Definitions.hpp"
#include "circuit_optimizer/CircuitOptimizer.hpp"
#include "ir/Permutation.hpp"
#include "ir/QuantumComputation.hpp"
#include "ir/operations/Control.hpp"
#include "ir/operations/OpType.hpp"

#include <algorithm>
#include <cstddef>
#include <map>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

using namespace qc;
using namespace std;

namespace qc {

/***
 * Public Methods
 ***/

void DDMinimizer::optimizeInputPermutation(qc::QuantumComputation& qc) {
  // create, set and apply the heuristics based permutation
  const qc::Permutation& perm = DDMinimizer::createGateBasedPermutation(qc);
  qc.initialLayout = perm;
  qc::CircuitOptimizer::elidePermutations(qc);
}

qc::Permutation
DDMinimizer::createGateBasedPermutation(qc::QuantumComputation& qc) {
  // create the data structure to store the indices in the pattern maps as well
  // as the max indices of the ladders

  const std::size_t bits = qc.getNqubits();

  // initialize the maps with the control and target qubits of the pattern
  qc::DDMinimizer minimizer;
  minimizer.initializeDataStructure(bits);

  // iterate over all the ops and mark the index of the found x-c pairs in the
  // map.
  int instructionIndex = 0;
  bool found = false;
  for (const auto& op : qc) {
    if (!op->isStandardOperation() || op->getType() == Z) {
      continue;
    }

    for (const auto& control : op->getControls()) {
      for (const auto& target : op->getTargets()) {
        auto itxC = minimizer.xCMap.find({control.qubit, target});
        if (itxC != minimizer.xCMap.end()) {
          itxC->second = instructionIndex;
          found = true;
        }
        auto itcX = minimizer.cXMap.find({control.qubit, target});
        if (itcX != minimizer.cXMap.end()) {
          itcX->second = instructionIndex;
          found = true;
        }
        for (size_t i = 0; i < bits - 1; i++) {
          auto itcL = minimizer.cLMap[i].find({control.qubit, target});
          if (itcL != minimizer.cLMap[i].end()) {
            itcL->second = instructionIndex;
            found = true;
          }
          auto itcH = minimizer.cHMap[i].find({control.qubit, target});
          if (itcH != minimizer.cHMap[i].end()) {
            itcH->second = instructionIndex;
            found = true;
          }
          auto itxL = minimizer.xLMap[i].find({control.qubit, target});
          if (itxL != minimizer.xLMap[i].end()) {
            itxL->second = instructionIndex;
            found = true;
          }
          auto itxH = minimizer.xHMap[i].find({control.qubit, target});
          if (itxH != minimizer.xHMap[i].end()) {
            itxH->second = instructionIndex;
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
  int cXIndex = 0;
  int xCIndex = 0;
  std::vector<int> cLIndex(bits - 1, 0);
  std::vector<int> cHIndex(bits - 1, 0);
  std::vector<int> xLIndex(bits - 1, 0);
  std::vector<int> xHIndex(bits - 1, 0);

  cXIndex = DDMinimizer::findMaxIndex(minimizer.cXMap);
  xCIndex = DDMinimizer::findMaxIndex(minimizer.xCMap);
  for (size_t i = 0; i < bits - 1; i++) {
    cLIndex[i] = DDMinimizer::findMaxIndex(minimizer.cLMap[i]);
    cHIndex[i] = DDMinimizer::findMaxIndex(minimizer.cHMap[i]);
    xLIndex[i] = DDMinimizer::findMaxIndex(minimizer.xLMap[i]);
    xHIndex[i] = DDMinimizer::findMaxIndex(minimizer.xHMap[i]);
  }

  // create the permutation based on the order of max index in the complete maps
  std::vector<Qubit> layout(bits);
  std::iota(layout.begin(), layout.end(), 0);

  const int prioCh = DDMinimizer::getLadderPosition(cHIndex, xCIndex);
  const int prioXl = DDMinimizer::getLadderPosition(xLIndex, xCIndex);
  const std::size_t stairsCh = DDMinimizer::getStairCount(cHIndex);
  const std::size_t stairsXl = DDMinimizer::getStairCount(xLIndex);
  const int prioCl = DDMinimizer::getLadderPosition(cLIndex, cXIndex);
  const int prioXh = DDMinimizer::getLadderPosition(xHIndex, cXIndex);
  const std::size_t stairsCl = DDMinimizer::getStairCount(cLIndex);
  const std::size_t stairsXh = DDMinimizer::getStairCount(xHIndex);

  // complete case checkins and adjust the layout
  // reverse of  q | 0  1  2  3  turns to q | 0  1  2  3
  //             l | 0  1  2  3           l | 3  2  1  0

  if ((cXIndex != -1) && (xCIndex == -1 || cXIndex < xCIndex)) {

    if (prioCh == 0 && stairsCh > 0) {
      std::reverse(layout.begin(), layout.end());
      layout = DDMinimizer::rotateLeft(layout, stairsCh);
    } else if (prioXl == 0 && stairsXl > 0) {
      std::reverse(layout.begin(), layout.end());
      layout = DDMinimizer::rotateRight(layout, stairsXl);
    } else if (prioCh > 0 || prioXl > 0 || (prioCh == 0 && prioXl == 0)) {
      std::reverse(layout.begin(), layout.end());
    }
  } else if ((xCIndex != -1) && (cXIndex == -1 || cXIndex > xCIndex)) {

    if ((prioCl == 0 && DDMinimizer::isFullLadder(cLIndex)) ||
        (prioXh == 0 && DDMinimizer::isFullLadder(xHIndex))) {
      std::reverse(layout.begin(), layout.end());
    }

    else if (prioCl == 0 && stairsCl > 0) {
      layout = DDMinimizer::rotateRight(layout, stairsCl);
    } else if (prioXh == 0 && stairsXh > 0) {
      layout = DDMinimizer::rotateLeft(layout, stairsXh);
    }
  } else if ((DDMinimizer::isFullLadder(xHIndex) ||
              DDMinimizer::isFullLadder(cLIndex))) {
    std::reverse(layout.begin(), layout.end());
  } else {
    // in case no full pattern was identified, call fallback function to
    // determine ordering based on singular controlled operations
    return DDMinimizer::createControlBasedPermutation(qc);
  }

  // transform layout into permutation
  // Permutation is std::map<Qubit, Qubit>
  qc::Permutation perm;
  for (qc::Qubit i = 0; i < bits; i++) {
    perm[i] = layout[i];
  }
  return perm;
}

void DDMinimizer::initializeDataStructure(std::size_t bits) {
  xCMap.clear();
  cXMap.clear();
  cLMap.resize(bits - 1);
  cHMap.resize(bits - 1);
  xLMap.resize(bits - 1);
  xHMap.resize(bits - 1);

  const std::size_t max = bits - 1;
  // create x-c ladder
  for (size_t i = 0; i < max; i++) {
    xCMap.insert({{i + 1, i}, -1});
    cXMap.insert({{i, i + 1}, -1});
  }

  // create c-l and x-l ladder
  for (size_t i = 0; i < max; i++) {
    for (size_t j = 0; j < bits; j++) {
      if (i < j) {
        xLMap[i].insert({{j, i}, -1});
        cLMap[i].insert({{i, j}, -1});
      }
    }
  }

  // create c-h and x-h ladder
  for (size_t i = max; i > 0; i--) {
    for (size_t j = 0; j < bits; j++) {
      if (i > j) {
        xHMap[bits - i - 1].insert({{j, i}, -1});
        cHMap[bits - i - 1].insert({{i, j}, -1});
      }
    }
  }
}

int DDMinimizer::findMaxIndex(
    const std::map<std::pair<Qubit, Qubit>, int>& map) {
  int maxIndex = -1; // Initialize to the smallest possible value

  for (const auto& entry : map) {
    // in case the index of -1 is encountered, this indicates that the overall
    // pattern is not completely present in the circuit. Therefore we set the
    // overall maxIndex to -1. With that we are able to filter out incomplete
    // patterns as they do not impact the order of the permutation.
    if (entry.second == -1) {
      maxIndex = -1;
      break;
    }
    if (entry.second > maxIndex) {
      maxIndex = entry.second;
    }
  }

  return maxIndex;
}

// Helper function to check if the vector of a ladder step is full, meaning each
// gate appeared in the circuit
bool DDMinimizer::isFullLadder(const std::vector<int>& vec) {
  return std::all_of(vec.begin(), vec.end(),
                     [](int value) { return value != -1; });
}

// Helper function to get the number of complete stairs in a ladder
std::size_t DDMinimizer::getStairCount(const std::vector<int>& vec) {
  std::size_t count = 0;
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

// Helper functions to manipulate the Layout: rotate right and left

std::vector<Qubit> DDMinimizer::rotateRight(std::vector<Qubit> layout,
                                            std::size_t stairs) {
  const std::size_t size = layout.size();
  std::vector<Qubit> rotatedLayout(size);
  for (std::size_t r = 0; r < stairs; ++r) {
    if (!layout.empty()) {
      rotatedLayout[size - (r + 1)] = layout[r];
    }
  }
  const std::size_t left = size - stairs;
  for (std::size_t r = 0; r < left; ++r) {
    if (!layout.empty()) {
      rotatedLayout[r] = layout[r + stairs];
    }
  }
  return rotatedLayout;
}

std::vector<Qubit> DDMinimizer::rotateLeft(std::vector<Qubit> layout,
                                           std::size_t stairs) {
  const std::size_t size = layout.size();
  std::vector<Qubit> rotatedLayout(size);
  for (std::size_t r = 0; r < stairs; ++r) {
    if (!layout.empty()) {
      rotatedLayout[r] = layout[size - (r + 1)];
    }
  }
  const std::size_t left = size - stairs;
  for (std::size_t r = 0; r < left; ++r) {
    if (!layout.empty()) {
      rotatedLayout[r + stairs] = layout[r];
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
  // recursively increase all the weights of the control qubits where the
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

}; // namespace qc
