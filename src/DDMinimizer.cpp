#include "DDMinimizer.hpp"

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "operations/OpType.hpp"

#include <vector>
#include <map>
#include <string>
#include <utility>
#include <set>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <numeric>

using namespace qc;
using namespace std;

namespace qc {

/***
 * Public Methods
 ***/
void optimizeInputPermutation(qc::QuantumComputation qc){
    qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);
    qc.initialLayout = perm;
    qc::CircuitOptimizer::elidePermutations(qc); 
}

qc::Permutation DDMinimizer::createGateBasedPermutation(qc::QuantumComputation& qc){
    std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>> maps = DDMinimizer::makeDataStructure(qc).first;
    std::map<std::string, std::vector<int>> indices = DDMinimizer::makeDataStructure(qc).second; 
    std::size_t bits = qc.getNqubits();

    //iterate over all the ops and mark the index of the found x-c pairs in the map.
    int instruction_index = 0;
    for (const auto& op : qc.ops) {
        if(!op->isStandardOperation()){
            continue;
        }
        Controls controls = op->getControls();
        std::vector<Qubit> targets = {op->getTargets().begin(), op->getTargets().end()};

        for (const auto& control : controls) {
            for(const auto& target : targets){
                for(auto& map : maps){
                    auto it = map.second.find({control.qubit, target});
                    if (it != map.second.end()) {
                        it->second = instruction_index; 
                    }
                }
            }
        }
        instruction_index++;
    }
  
    //std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>> maps 
    //std::map<std::string, std::vector<int>> indices 
    //iterate over all the maps and find the max index of the found x-c pairs
    for(const auto& map : maps){
        int max = -1;
        for(const auto& pair : map.second){
            if(pair.second == -1){
                max = -1;
                break;
            }
            else if (pair.second > max){
                max = pair.second;
            }
        }
        std::string map_name = map.first.substr(0, 3);
        auto it = indices.find(map_name);

        if(it != indices.end()){
            if(map_name[2] == 'r'){
                std::size_t column = static_cast<std::size_t>(map.first[4] - '0');
                it->second[bits - 1 - column] = max;
            }
            else if (map_name[2] == 'l'){
                std::size_t column = static_cast<std::size_t>(map.first[4] - '0');
                it->second[column] = max;
            }
            else {
                it->second[0] = max;
            }
        }
    }
    

    //create the permutation based on the order of max index in the complete maps
    std::vector<Qubit> layout(bits);
    std::iota(layout.begin(), layout.end(), 0);

    auto c_x = indices.find("c_x");
    auto x_c = indices.find("x_c");
    auto c_l = indices.find("c_l");
    auto x_l = indices.find("x_l");
    auto c_r = indices.find("c_r");
    auto x_r = indices.find("x_r");


    int prio_c_r = DDMinimizer::getLadderPosition(c_r->second, x_c->second[0]);
    int prio_x_l = DDMinimizer::getLadderPosition(x_l->second, x_c->second[0]);
    int stairs_c_r = DDMinimizer::getStairCount(c_r->second);
    int stairs_x_l = DDMinimizer::getStairCount(x_l->second);
    int prio_c_l = DDMinimizer::getLadderPosition(c_l->second, c_x->second[0]);
    int prio_x_r = DDMinimizer::getLadderPosition(x_r->second, c_x->second[0]);
    int stairs_c_l = DDMinimizer::getStairCount(c_l->second);
    int stairs_x_r = DDMinimizer::getStairCount(x_r->second);


    if(c_x->second[0] < x_c->second[0]){

        if(prio_c_r > 0 || prio_x_l > 0) {
            layout = DDMinimizer::reverseLayout(layout);
        }
        else if (prio_c_r == 0 && stairs_c_r > 0){
            for(int i = 0; i < stairs_c_r; i++){
                layout = DDMinimizer::rotateRight(layout);
            }
        }
        else if (prio_x_l == 0 && stairs_x_l > 0){
            for(int i = 0; i < stairs_x_l; i++){
                layout = DDMinimizer::rotateLeft(layout);
            }
        }
    }
    else if(c_x->second[0] > x_c->second[0]){

        if(prio_c_l > 0 || prio_x_r > 0) {
            //do nothing cause identity is what we need anyway
        }
        else if (prio_c_r == 0 && stairs_c_l > 0){
            layout = DDMinimizer::reverseLayout(layout);
            for(int i = 0; i < stairs_c_r; i++){
                layout = DDMinimizer::rotateRight(layout);
            }
        }
        else if (prio_x_l == 0 && stairs_x_r > 0){
            layout = DDMinimizer::reverseLayout(layout);
            for(int i = 0; i < stairs_x_l; i++){
                layout = DDMinimizer::rotateLeft(layout);
            }
        }
    }

    else {
        if(DDMinimizer::isFull(x_r->second) || DDMinimizer::isFull(c_l->second)){
            layout = DDMinimizer::reverseLayout(layout);
        }

        else {
           return DDMinimizer::createControlBasedPermutation(qc);
        }
    }

    //transform layout into permutation
    qc::Permutation perm;
    std::vector<Qubit> physicalQubits(bits);
    for (qc::Qubit i = 0; i < bits; i++) {
          physicalQubits[i] = i;
    }
    for (qc::Qubit i = 0; i < bits; i++) {
        perm[physicalQubits[i]] = layout[i];
    }
    return perm;
}  

bool DDMinimizer::isFull(std::vector<int> vec){
   std::size_t countNegativeOne = 0;
    std::size_t totalEntries = vec.size();

    for (int value : vec) {
        if (value == -1) {
            countNegativeOne++;
        }
    }

    if (countNegativeOne < totalEntries / 2) {
        return true;    
    }

    return false;
}


int DDMinimizer::getStairCount(std::vector<int> vec){
   int count = 0;
    for (int value : vec) {
        if (value != -1) {
            count++;
        }
        else {
            return count;
        }
    }
    return count;
}

int DDMinimizer::getLadderPosition(std::vector<int> vec, int ladder){
   int count = 0;
    for (int value : vec) {
        if (value != -1 && value < ladder) {
            count++;
        }
    }
    return count;
}

std::vector<Qubit> DDMinimizer::reverseLayout(std::vector<Qubit> layout){
    std::vector<Qubit> reversed_layout(layout.size());
    for (std::size_t i = 0; i < layout.size(); i++) {
        reversed_layout[layout[i]] = static_cast<Qubit>(i);
    }
    return reversed_layout;
}

std::vector<Qubit> DDMinimizer::rotateRight(std::vector<Qubit> layout){
    std::vector<Qubit> rotated_layout(layout.size());
    rotated_layout[0] = layout[layout.size() - 1];

    for (std::size_t i = 1; i < layout.size(); ++i) {
        rotated_layout[i] = layout[i - 1];
    }

    return rotated_layout;
}

std::vector<Qubit> DDMinimizer::rotateLeft(std::vector<Qubit> layout){
    std::vector<Qubit> rotated_layout(layout.size());
    rotated_layout[layout.size() - 1] = layout[0];
    for (std::size_t i = 0; i < layout.size() - 1; i++) {
        rotated_layout[i] = layout[i + 1];
    }
    return rotated_layout;
}

std::pair<std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>>,std::map<std::string, std::vector<int>>> DDMinimizer::makeDataStructure(qc::QuantumComputation& qc){
    std::size_t bits = qc.getNqubits();
    std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>> maps; 
    //string of c_x, x_c, c_l, x_l, c_r, x_r to vector of max index of gate
    //for c_l and x_l position 0 in the vector marks the line of c(x) at 0 -> we count the left most as the first
    //for c_r and x_r position 0 in the vector marks the line of c(x) at bits - 1 -> we count the right most as the first
    std::map<std::string, std::vector<int>> indices; 

    //create x-c ladder
    std::map<pair<Qubit, Qubit>, int> x_c_map;
    for(size_t i = 0; i < bits; i++){
        x_c_map.insert({{i+1, i}, -1});
    }
    maps.insert({"x_c", x_c_map});
    indices.insert({"x_c", std::vector<int>(1, 0)});   

    //create c-x ladder
    std::map<pair<Qubit, Qubit>, int> c_x_map; //c -> x
    for(size_t i = 0; i < bits; i++){
        c_x_map.insert({{i, i+1}, -1});
    }
    maps.insert({"c_x", c_x_map});
    indices.insert({"c_x", std::vector<int>(1, 0)});

    std::size_t max = bits - 1;
    for(size_t i = 0; i < max; i++){   
        std::map<pair<Qubit, Qubit>, int> x_l_map;
        std::map<pair<Qubit, Qubit>, int> c_l_map;
        for(size_t j = 0; j < bits; j++){
            if(i<j){
                x_l_map.insert({{i, j}, -1});
                x_l_map.insert({{j, i}, -1});
            }
        }
        maps.insert({"x_l_" + std::to_string(i), x_l_map});
        maps.insert({"c_l_" + std::to_string(i), c_l_map});
        
    }
    indices.insert({"x_l", std::vector<int>(bits - 1, 0)});
    indices.insert({"c_l", std::vector<int>(bits - 1, 0)});

    for(size_t i = max; i > 0; i--){   
        std::map<pair<Qubit, Qubit>, int> x_r_map;
        std::map<pair<Qubit, Qubit>, int> c_r_map;
        for(size_t j = 0; j < bits; j++){
            if(i>j){
                x_r_map.insert({{i, j}, -1});
                x_r_map.insert({{j, i}, -1});
            }
        }
        maps.insert({"x_r_" + std::to_string(i), x_r_map});
        maps.insert({"c_r_" + std::to_string(i), c_r_map});
        
    }
    indices.insert({"x_r", std::vector<int>(bits - 1, 0)});
    indices.insert({"c_r", std::vector<int>(bits - 1, 0)});
    return std::make_pair(maps, indices);
}


qc::Permutation DDMinimizer::createControlBasedPermutation(qc::QuantumComputation& qc){
    std::map<Qubit, std::set<Qubit>> controlToTargets;

    for (const auto& op : qc.ops) {
        if(!op->isStandardOperation()){
            continue;
        }
        Controls controls = op->getControls();
        std::set<Qubit> targets = {op->getTargets().begin(), op->getTargets().end()};

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

std::size_t DDMinimizer::factorial(std::size_t n) {
    std::size_t result = 1;
    for (std::size_t i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}


};// namespace qc