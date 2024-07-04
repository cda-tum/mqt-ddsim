#include "DDMinimizer.hpp"

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"

#include <random>
#include <utility>
#include <vector>
#include <chrono> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>

namespace qc {

/***
 * Public Methods
 ***/
static void optimizeInputPermutation(qc::QuantumComputation& qc){
    qc::Permutation perm = qc::DDMinimizer::createControlBasedPermutation(qc);
    qc.initialLayout = perm;
    qc::CircuitOptimizer::elidePermutations(qc); 
}

qc::Permutation DDMinimizer::createControlBasedPermutation(qc::QuantumComputation& qc){
    
    std::map<Qubit, std::set<Qubit>> controlToTargets = createControlToTargetMap(qc);
    for (const auto& pair : controlToTargets) {
        std::cout << "Control Qubit " << pair.first << " targets: ";
        for (const auto& target : pair.second) {
            std::cout << target << " ";
        }
        std::cout << "\n";
    }
    
    std::size_t bits = qc.getNqubits();
    std::vector<Qubit> logicalQubits(bits);
    std::vector<Qubit> physicalQubits(bits);
    std::map<Qubit, int> qubitWeights;
    for (Qubit i = 0; i < bits; ++i) {
          physicalQubits[i] = i;
          logicalQubits[i] = i;
          qubitWeights.insert({i, 1});
    }

    //trying the approach of  control > target
    int weight = 1;
    for (const auto& pair : controlToTargets) {
    weight = qubitWeights[pair.first];

    for (const auto& target : pair.second) {
        weight = std::max(qubitWeights[target], weight+1);
    }
    weight++;
    qubitWeights[pair.first] = weight;
    }

    sort(physicalQubits.begin(), physicalQubits.end(), [&qubitWeights](const Qubit& a, const Qubit& b) {
        auto weightA = qubitWeights.find(a) != qubitWeights.end() ? qubitWeights.at(a) : 0;
        auto weightB = qubitWeights.find(b) != qubitWeights.end() ? qubitWeights.at(b) : 0;
        return weightA < weightB;
    });   
    
    qc::Permutation perm;

    for (std::size_t m = 0; m < bits; m++) {
               perm[logicalQubits[m]] = physicalQubits[m];
          }
    return perm;

}


 std::vector<qc::Permutation> DDMinimizer::createAllPermutations(qc::QuantumComputation& qc) {
     std::size_t bits = qc.getNqubits();
     std::size_t permutation_count = qc::DDMinimizer::factorial(bits);

     // vector with all possible Permutations
     std::vector<qc::Permutation> permutations;
     qc::Permutation perm;

     // prepare generation of all permutations
     // create vectors that can be shuffeled
     std::vector<Qubit> logicalQubits(bits);
     std::vector<Qubit> physicalQubits(bits);

     //vector that contains all the layouts;
     std::vector<std::vector<Qubit>> layouts;
     
     for (std::size_t i = 0; i < bits; ++i) {
          physicalQubits[i] = i;
          logicalQubits[i] = i;
     }

     std::sort(physicalQubits.begin(), physicalQubits.end());
     do {
          layouts.push_back(physicalQubits);
     } while (std::next_permutation(physicalQubits.begin(), physicalQubits.end()));


     //create all layouts
     //loop over all permutations
     for(std::size_t i = 0; i < permutation_count; i++){
          //loop over all logical bits and create one layout
          for (std::size_t m = 0; m < bits; m++) {
               perm[logicalQubits[m]] = layouts[i][m];
          }
          permutations.push_back(perm);
     } 
     
     return permutations;
}

std::size_t DDMinimizer::factorial(std::size_t n) {
    std::size_t result = 1;
    for (std::size_t i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

std::string DDMinimizer::readFileIntoString(const std::string& filePath) {
    // Open the file
    std::ifstream fileStream(filePath);

    // Check if the file was opened successfully
    if (!fileStream.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::stringstream buffer;
    std::string line;
        while (std::getline(fileStream, line)) {
        // Remove comments and trim the line if necessary
        std::size_t commentPos = line.find('//');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        // Remove leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        if (!line.empty()) {
          /*if (!replaced && line.find("OPENQASM 2.0;") != std::string::npos) {
                line = "OPENQASM 3.0;";
                replaced = true;
            }*/
            buffer << line << '\n';
        }
    }

    return buffer.str();
}



std::map<Qubit, std::set<Qubit>> DDMinimizer::createControlToTargetMap(qc::QuantumComputation& qc) {
    std::map<Qubit, std::set<Qubit>> controlToTargets;

    for (const auto& op : qc.ops) {
        if(!op->isStandardOperation()){
            continue;
        }
        Controls controls = op->getControls();
        std::set<Qubit> targets = {op->getTargets().begin(), op->getTargets().end()};

        /*for (const auto& target : op->getTargets()) {
        // Assuming there's a way to convert Qubit to int, e.g., target.toInt()
            targetInts.insert(target.toInt());
        }*/
        for (const auto& control : controls) {
            if (controlToTargets.find(control.qubit) == controlToTargets.end()) {
            // If the control does not exist in the map, add it with the current set of targetInts
                controlToTargets[control.qubit] = targets;
            } else {
            // If the control exists, insert the new targets into the existing set
                controlToTargets[control.qubit].insert(targets.begin(), targets.end());
            }
        }
    }
    return controlToTargets;
}

}// namespace qc