#include "DDMinimizer.hpp"

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "operations/OpType.hpp"

#include <cstddef>
#include <iostream>
#include <chrono>
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
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <cstdio>


using namespace qc;
using namespace std;

namespace qc {

/***
 * Public Methods
 ***/

void DDMinimizer::runOverNight(){
    std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> data = {
        {"ae_indep_qiskit_2.qasm", {}},
        {"ae_indep_qiskit_3.qasm", {}},
        {"ae_indep_qiskit_4.qasm", {}},
        {"ae_indep_qiskit_5.qasm", {}},
        {"ae_indep_qiskit_6.qasm", {}},
        {"dj_indep_qiskit_2.qasm", {}},
        {"dj_indep_qiskit_3.qasm", {}},
        {"dj_indep_qiskit_4.qasm", {}},
        {"dj_indep_qiskit_5.qasm", {}},
        {"dj_indep_qiskit_6.qasm", {}},
        {"ghz_indep_qiskit_2.qasm", {}},
        {"ghz_indep_qiskit_3.qasm", {}},
        {"ghz_indep_qiskit_4.qasm", {}},
        {"ghz_indep_qiskit_5.qasm", {}},
        {"ghz_indep_qiskit_6.qasm", {}},
        {"graphstate_indep_qiskit_3.qasm", {}},
        {"graphstate_indep_qiskit_4.qasm", {}},
        {"graphstate_indep_qiskit_5.qasm", {}},
        {"graphstate_indep_qiskit_6.qasm", {}},
        {"grover-noancilla_indep_qiskit_2.qasm", {}},
        {"grover-noancilla_indep_qiskit_3.qasm", {}},
        {"grover-noancilla_indep_qiskit_4.qasm", {}},
        {"grover-noancilla_indep_qiskit_5.qasm", {}},
        {"grover-noancilla_indep_qiskit_6.qasm", {}},
        {"grover-v-chain_indep_qiskit_2.qasm", {}},
        {"grover-v-chain_indep_qiskit_3.qasm", {}},
        {"grover-v-chain_indep_qiskit_4.qasm", {}},
        {"grover-v-chain_indep_qiskit_5.qasm", {}},
        {"portfolioqaoa_indep_qiskit_3.qasm", {}},
        {"portfolioqaoa_indep_qiskit_4.qasm", {}},
        {"portfolioqaoa_indep_qiskit_5.qasm", {}},
        {"portfolioqaoa_indep_qiskit_6.qasm", {}},
        {"portfoliovqe_indep_qiskit_3.qasm", {}},
        {"portfoliovqe_indep_qiskit_4.qasm", {}},
        {"portfoliovqe_indep_qiskit_5.qasm", {}},
        {"portfoliovqe_indep_qiskit_6.qasm", {}},
        {"qaoa_indep_qiskit_3.qasm", {}},
        {"qaoa_indep_qiskit_4.qasm", {}},
        {"qaoa_indep_qiskit_5.qasm", {}},
        {"qaoa_indep_qiskit_6.qasm", {}},
        {"qftentangled_indep_qiskit_2.qasm", {}},
        {"qftentangled_indep_qiskit_3.qasm", {}},
        {"qftentangled_indep_qiskit_4.qasm", {}},
        {"qftentangled_indep_qiskit_5.qasm", {}},
        {"qftentangled_indep_qiskit_6.qasm", {}},
        {"qft_indep_qiskit_2.qasm", {}},
        {"qft_indep_qiskit_3.qasm", {}},
        {"qft_indep_qiskit_4.qasm", {}},
        {"qft_indep_qiskit_5.qasm", {}},
        {"qft_indep_qiskit_6.qasm", {}},
        {"qnn_indep_qiskit_2.qasm", {}},
        {"qnn_indep_qiskit_3.qasm", {}},
        {"qnn_indep_qiskit_4.qasm", {}},
        {"qnn_indep_qiskit_5.qasm", {}},
        {"qnn_indep_qiskit_6.qasm", {}},
        {"qpeexact_indep_qiskit_2.qasm", {}},
        {"qpeexact_indep_qiskit_3.qasm", {}},
        {"qpeexact_indep_qiskit_4.qasm", {}},
        {"qpeexact_indep_qiskit_5.qasm", {}},
        {"qpeexact_indep_qiskit_6.qasm", {}},
        {"qpeinexact_indep_qiskit_2.qasm", {}},
        {"qpeinexact_indep_qiskit_3.qasm", {}},
        {"qpeinexact_indep_qiskit_4.qasm", {}},
        {"qpeinexact_indep_qiskit_5.qasm", {}},
        {"qpeinexact_indep_qiskit_6.qasm", {}},
        {"qwalk-noancilla_indep_qiskit_3.qasm", {}},
        {"qwalk-noancilla_indep_qiskit_4.qasm", {}},
        {"qwalk-noancilla_indep_qiskit_5.qasm", {}},
        {"qwalk-noancilla_indep_qiskit_6.qasm", {}},
        {"qwalk-v-chain_indep_qiskit_3.qasm", {}},
        {"qwalk-v-chain_indep_qiskit_5.qasm", {}},
        {"random_indep_qiskit_2.qasm", {}},
        {"random_indep_qiskit_3.qasm", {}},
        {"random_indep_qiskit_4.qasm", {}},
        {"random_indep_qiskit_5.qasm", {}},
        {"random_indep_qiskit_6.qasm", {}},
        {"realamprandom_indep_qiskit_2.qasm", {}},
        {"realamprandom_indep_qiskit_3.qasm", {}},
        {"realamprandom_indep_qiskit_4.qasm", {}},
        {"realamprandom_indep_qiskit_5.qasm", {}},
        {"realamprandom_indep_qiskit_6.qasm", {}},
        {"su2random_indep_qiskit_2.qasm", {}},
        {"su2random_indep_qiskit_3.qasm", {}},
        {"su2random_indep_qiskit_4.qasm", {}},
        {"su2random_indep_qiskit_5.qasm", {}},
        {"su2random_indep_qiskit_6.qasm", {}},
        {"twolocalrandom_indep_qiskit_2.qasm", {}},
        {"twolocalrandom_indep_qiskit_3.qasm", {}},
        {"twolocalrandom_indep_qiskit_4.qasm", {}},
        {"twolocalrandom_indep_qiskit_5.qasm", {}},
        {"twolocalrandom_indep_qiskit_6.qasm", {}},
        {"vqe_indep_qiskit_3.qasm", {}},
        {"vqe_indep_qiskit_4.qasm", {}},
        {"vqe_indep_qiskit_5.qasm", {}},
        {"vqe_indep_qiskit_6.qasm", {}},
        {"wstate_indep_qiskit_2.qasm", {}},
        {"wstate_indep_qiskit_3.qasm", {}},
        {"wstate_indep_qiskit_4.qasm", {}},
        {"wstate_indep_qiskit_5.qasm", {}},
        {"wstate_indep_qiskit_6.qasm", {}},
    };

    std::string input; 

    std::cout << "Let's start!  \n";
    
    data = DDMinimizer::initializeMap(data);
    std::cout << "Initiation done_________________________ \n";
    int run = 0;
    do {
        std::cout << "Type 'stop' to exit (continues automatically after 5 seconds): ";
        std::cout.flush(); // Ensure prompt is displayed immediately

        fd_set set;
        struct timeval timeout;

        // Initialize the file descriptor set
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        // Set timeout to 5.0 seconds
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // Wait for input to become available or timeout
        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
        if (rv == -1) {
            perror("select"); // Error occurred in select()
            break;
        } else if (rv == 0) {
            std::cout << "\nNo input. Continuing...\n";
        } else {
            std::getline(std::cin, input);
            if (input == "stop") {
                break;
            }
            std::cout << "You typed: " << input << std::endl;
        }
        
        std::cout << "Run: " << run << "  _______________________________________________________________________________\n";
        data = DDMinimizer::refineMap(data);
        run++;

    } while (input != "stop");


    DDMinimizer::printResults(data);
    std::cout << "You made it!  \n";
}


std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> DDMinimizer::initializeMap(std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> data){
    std::string qasmString;
    for(const auto& entry : data){
            try {
                qasmString = DDMinimizer::readFileIntoString("BenchmarkQuasm/qasm/" +entry.first);
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
            }    
            
            std::cout << "Measure: " + entry.first +" \n";
            std::cout.flush();
            
            auto qc = QuantumComputation::fromQASM(qasmString);
            qc::Permutation perm = DDMinimizer::createControlBasedPermutation(qc);
            qc.initialLayout = perm; 
            qc::CircuitOptimizer::elidePermutations(qc);
            std::chrono::duration<long double> resControl =  DDMinimizer::measureControl(qc);

            auto qcc = QuantumComputation::fromQASM(qasmString);
            std::map<std::size_t, std::chrono::duration<long double>> resAll = DDMinimizer::measureAll(qcc);

            data[entry.first] = std::make_pair(resAll, resControl);
    }
    return data;    
}

std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> DDMinimizer::refineMap(std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> data){
    std::string qasmString;
    for(const auto& entry : data){
            try {
                qasmString = DDMinimizer::readFileIntoString("BenchmarkQuasm/qasm/" +entry.first);
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
            }    
            
            std::cout << "Measure: " + entry.first +" \n";
            std::cout.flush();
            
            auto qc = QuantumComputation::fromQASM(qasmString);
            qc::Permutation perm = DDMinimizer::createControlBasedPermutation(qc);
            qc.initialLayout = perm; 
            qc::CircuitOptimizer::elidePermutations(qc);
            std::chrono::duration<long double> resControl =  DDMinimizer::measureControl(qc); 
            std::chrono::duration<long double> averageResControl = std::chrono::duration_cast<std::chrono::duration<long double>>(resControl + entry.second.second) / 2.0;

            auto qcc = QuantumComputation::fromQASM(qasmString);
            std::map<std::size_t, std::chrono::duration<long double>> resAll = DDMinimizer::measureAll(qcc); 

            data[entry.first] = std::make_pair(DDMinimizer::makeAverage(resAll, entry.second.first), averageResControl);   
}
    return data;
}

std::map<std::size_t, std::chrono::duration<long double>> DDMinimizer::makeAverage(std::map<std::size_t, std::chrono::duration<long double>> res1, std::map<std::size_t, std::chrono::duration<long double>> res2){
    std::map<std::size_t, std::chrono::duration<long double>> avgRes;
        std::chrono::duration<long double> totalDifference = std::chrono::duration<long double>::zero();

        // Combine keys from both maps
        std::map<std::size_t, std::chrono::duration<long double>> combined = res1;
        combined.insert(res2.begin(), res2.end());

        for (const auto& pair : combined) {
            std::size_t key = pair.first;
            auto it1 = res1.find(key);
            auto it2 = res2.find(key);

            if (it1 != res1.end() && it2 != res2.end()) {
                auto duration1 = it1->second;
                auto duration2 = it2->second;

                avgRes[key] = std::chrono::duration_cast<std::chrono::duration<long double>> (duration1 + duration2) / 2.0;
                totalDifference += std::chrono::duration<long double>(std::abs(duration1.count() - duration2.count()));
            }
        }

        std::cout << "Total Difference: " << std::scientific << std::setprecision(4) << totalDifference.count() << "\n";
        return avgRes;
}

std::chrono::duration<long double> DDMinimizer::measureControl(qc::QuantumComputation& qc){
    auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qc));        
    CircuitSimulator ddsim_qc(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    
    std::chrono::duration<long double> duration_qc;

    ddsim_qc.simulate(1); 
    ddsim_qc.simulate(1); 
    auto start3_qc = std::chrono::high_resolution_clock::now();
    ddsim_qc.simulate(1); 
    auto end3_qc = std::chrono::high_resolution_clock::now();
    auto start4_qc = std::chrono::high_resolution_clock::now();
    ddsim_qc.simulate(1); 
    auto end4_qc = std::chrono::high_resolution_clock::now();
    auto start5_qc = std::chrono::high_resolution_clock::now();
    ddsim_qc.simulate(1); 
    auto end5_qc = std::chrono::high_resolution_clock::now();
    auto start6_qc = std::chrono::high_resolution_clock::now();
    ddsim_qc.simulate(1); 
    auto end6_qc = std::chrono::high_resolution_clock::now();
    
    duration_qc = std::chrono::duration_cast<std::chrono::duration<long double>> (end3_qc - start3_qc + end4_qc - start4_qc + end5_qc - start5_qc + end6_qc - start6_qc) / 4.0;
    return duration_qc;
}

std::map<std::size_t, std::chrono::duration<long double>> DDMinimizer::measureAll(qc::QuantumComputation& qc){
    std::map<std::size_t, std::chrono::duration<long double>> res;
    std::size_t bits = qc.getNqubits();    
    std::size_t permutation_count = DDMinimizer::factorial(bits);
    std::vector<qc::Permutation> permutations = DDMinimizer::createAllPermutations(qc);

    std::chrono::duration<long double> duration;

    /* Simulate the circuit using each permutation
    */
    for (size_t i = 0; i < permutation_count; i++) { 
    QuantumComputation qcc = qc;
    qcc.initialLayout = permutations[i]; 
    qc::CircuitOptimizer::elidePermutations(qcc); 
    auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qcc));        
    CircuitSimulator ddsim(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    ddsim.simulate(1); 
    ddsim.simulate(1); 
    auto start3_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end3_qc = std::chrono::high_resolution_clock::now();
    auto start4_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end4_qc = std::chrono::high_resolution_clock::now();
    auto start5_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end5_qc = std::chrono::high_resolution_clock::now();
    auto start6_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end6_qc = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::duration<long double>> (end3_qc - start3_qc + end4_qc - start4_qc + end5_qc - start5_qc  + end6_qc - start6_qc) / 4.0;
    res[i] = duration;
    }
    return res;
}

void DDMinimizer::printResults(std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> data){
    std::map<std::string, std::pair<int, int>> fileNames = {
    {"ae_indep_qiskit_", {2, 6}},
    {"dj_indep_qiskit_", {2, 6}},
    {"ghz_indep_qiskit_", {2, 6}},
    {"graphstate_indep_qiskit_", {3, 5}},
    {"grover-noancilla_indep_qiskit_", {2, 6}},
    {"grover-v-chain_indep_qiskit_", {2, 6}},
    {"portfolioqaoa_indep_qiskit_", {3, 6}},
    {"portfoliovqe_indep_qiskit_", {3, 6}},
    {"qaoa_indep_qiskit_", {3, 6}},
    {"qftentangled_indep_qiskit_", {2, 6}},
    {"qft_indep_qiskit_", {2, 6}},
    {"qnn_indep_qiskit_", {2, 6}},
    {"qpeexact_indep_qiskit_", {2, 6}},
    {"qpeinexact_indep_qiskit_", {2, 6}},
    {"qwalk-noancilla_indep_qiskit_", {3, 6}},
    {"qwalk-v-chain_indep_qiskit_", {3, 6}},
    {"random_indep_qiskit_", {2, 6}},
    {"realamprandom_indep_qiskit_", {2, 6}},
    {"su2random_indep_qiskit_", {2, 6}},
    {"twolocalrandom_indep_qiskit_", {2, 6}},
    {"vqe_indep_qiskit_", {3, 6}},
    {"wstate_indep_qiskit_", {2, 6}}
    };

    std::string qasmString;

    
    for(const auto& entry : fileNames){
        std::string file = entry.first;
        std::string fname_control = "BenchmarkQuasm/results/" + file+ "controls.txt";
        int from = entry.second.first;
        int to = entry.second.second;
    
        std::ofstream out_control(fname_control);
        for(int k = from; k <= to; k++){
             if(file.compare("random_indep_qiskit_") == 0 && k == 6){
                continue;
            }
            try {
                std::string fn = "BenchmarkQuasm/qasm/" + file + std::to_string(k) + ".qasm";
                qasmString = DDMinimizer::readFileIntoString(fn);
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
            }
            out_control << "Start Simulation of " + file + std::to_string(k) + "__________________________________________________\n";            
            auto qc = QuantumComputation::fromQASM(qasmString);
            auto qcc = QuantumComputation::fromQASM(qasmString);
            out_control << qc;
            qc::Permutation perm = DDMinimizer::createControlBasedPermutation(qc);
            qcc.initialLayout = perm; 
            qc::CircuitOptimizer::elidePermutations(qcc);
            DDMinimizer::finalControl(file + std::to_string(k) + ".qasm", out_control, qc, qcc, perm, data);
        }
        out_control.close();
        DDMinimizer::removeAnsi(fname_control);
    
        std::string fname_all = "BenchmarkQuasm/results/" + file+ "all.txt";
        std::cout << "Start measuring all permutations for " + fname_all + "\n";
        std::cout.flush();
        std::ofstream out_all(fname_all);
        for(int k = from; k <= to && k <= 6; k++){
            if(file.compare("random_indep_qiskit_") == 0 && k == 6){
                continue;
            }
            try {
                std::string fn = "BenchmarkQuasm/qasm/" + file + std::to_string(k) + ".qasm";
                qasmString = DDMinimizer::readFileIntoString(fn);
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
            }
            out_all << "Start Simulation of " + file + std::to_string(k) + "__________________________________________________\n";            
            auto qcc = QuantumComputation::fromQASM(qasmString);
            out_all << qcc;
            DDMinimizer::finalAll(file + std::to_string(k) + ".qasm", out_all, qcc, data, k);
        }
        out_all.close();
        DDMinimizer::removeAnsi(fname_all);
    }     
}

void DDMinimizer::finalControl(std::string name, std::ofstream& out, qc::QuantumComputation& qc, qc::QuantumComputation& qcc, qc::Permutation perm, std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> data){
    out << left << setfill(' ') << setw(30) << "Computed Permutation: " << right << setfill('.') << setw(30) <<DDMinimizer::permToString(perm) << "\n";
    out << left << setfill(' ') << setw(30) << "Original Permutation: " << right << setfill('.') << setw(30) <<DDMinimizer::permToString(qc.initialLayout) << "\n";
    
    auto qcc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qcc));        
    CircuitSimulator ddsim_qcc(std::move(qcc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qc));        
    CircuitSimulator ddsim_qc(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    
    std::size_t size_max_qc = 0;
    std::size_t size_max_qcc = 0;
    std::size_t size_qc = 0;
    std::size_t size_qcc = 0; 
    std::chrono::duration<long double> duration_qc; 
    std::chrono::duration<long double> duration_qcc; 


    
    ddsim_qc.simulate(1);
    ddsim_qcc.simulate(1);

    size_max_qc = ddsim_qc.getMaxNodeCount(); 
    size_qc = ddsim_qc.getActiveNodeCount();
    size_max_qcc = ddsim_qcc.getMaxNodeCount();
    size_qcc = ddsim_qcc.getActiveNodeCount();

    if (data.find(name) != data.end()) {
        // Access the entire value pair
        auto& value_pair = data[name];
        auto& nested_map = value_pair.first;

        if (nested_map.find(0) != nested_map.end()) {

            duration_qc = nested_map[0];
        }
        else {
            std::cout << "Key not found in nested map\n";
        }

        // Access the std::chrono::duration<long double>
        duration_qcc = value_pair.second;
    }
    else {
        std::cout << "Key not found in map\n";
    }

    out << std::left << std::setfill(' ') << std::setw(30) << "Identity Duration: " << std::scientific << std::setprecision(4) << duration_qc.count() << "s" << std::right << std::setfill('.') << std::setw(30) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Permuted Duration: " << std::scientific << std::setprecision(4) << duration_qcc.count() << "s" << std::right << std::setfill('.') << std::setw(30) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Identity active_nodes: " << DDMinimizer::formatSize_t(size_qc)  << std::right << std::setfill('.') << std::setw(39) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Permuted active_nodes: " << DDMinimizer::formatSize_t(size_qcc) << std::right << std::setfill('.') << std::setw(39) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Identity max_nodes: " << DDMinimizer::formatSize_t(size_max_qc)  << std::right << std::setfill('.') << std::setw(39) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Permuted max_nodes: " << DDMinimizer::formatSize_t(size_max_qcc) << std::right << std::setfill('.') << std::setw(39) << "\n";
    out << "________________________________________________________________________________________________\n\n";

}

void DDMinimizer::finalAll(std::string name, std::ofstream& out, qc::QuantumComputation& qc, std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> data, int qubits){
    std::size_t bits = qc.getNqubits();
    std::size_t permutation_count = DDMinimizer::factorial(bits);
    std::vector<qc::Permutation> permutations = DDMinimizer::createAllPermutations(qc);

    std::size_t min_active_nodes = std::numeric_limits<std::size_t>::max();
    std::size_t max_active_nodes = 0;
    std::size_t min_active_nodes_pos = 0;
    std::size_t max_active_nodes_pos = 0;
    std::size_t current_active_nodes = 0;

    std::size_t min_max_nodes = std::numeric_limits<std::size_t>::max();
    std::size_t max_max_nodes = 0;
    std::size_t min_max_nodes_pos = 0;
    std::size_t max_max_nodes_pos = 0;
    std::size_t current_max_nodes = 0;

    std::chrono::duration<long double> min_time(std::numeric_limits<double>::max());
    std::chrono::duration<long double> max_time(std::numeric_limits<double>::lowest());
    std::size_t min_time_pos = 0;
    std::size_t max_time_pos = 0;
    std::chrono::duration<long double> duration;

    std::map<size_t, std::vector<std::tuple<size_t, size_t, std::chrono::duration<long double>>>> perm_by_active_nodes;
    std::map<size_t, std::vector<std::tuple<size_t, size_t, std::chrono::duration<long double>>>> perm_by_max_nodes;
    std::map<std::chrono::duration<long double>, std::vector<std::tuple<size_t, size_t, size_t>>> perm_by_time;
    std::map<size_t, std::string> permutationToGraphViz;

    /* Simulate the circuit using each permutation
    */
    for (size_t i = 0; i < permutation_count; i++) { 
    //Create copy of the Quantum Computation, set the initial layout as the current permutation, apply the permutation to the qubits,
    //create the simulator and with that the decision diagram
    QuantumComputation qcc = qc;
    qcc.initialLayout = permutations[i]; 
    qc::CircuitOptimizer::elidePermutations(qcc); 
    auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qcc));        
    CircuitSimulator ddsim(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    /*std::clock_t start = std::clock();
    ddsim.simulate(1);
    std::clock_t end = std::clock();
    double cpu_time_used = static_cast<double>(end - start) / CLOCKS_PER_SEC;*/

    //Save the time before and after the simulation and run the simulation once
    ddsim.simulate(1);

    if (data.find(name) != data.end()) {
        // Access the entire value pair
        auto& value_pair = data[name];
        auto& nested_map = value_pair.first;

        if (nested_map.find(i) != nested_map.end()) {
            duration = nested_map[i];
        }
        else {
            std::cout << "Key: " + std::to_string(i) + " not found in nested map\n";
             std::cout << "Available keys in the map:" << std::endl;
                for (const auto& pair : data) {
                    std::cout << "'" << pair.first << "'" << std::endl;
                }
        }
    }
    else {
        std::cout << "Key not found in map\n";
    }

    if (duration < min_time) {
        min_time = duration;
        min_time_pos = i;
    }
    if (duration > max_time) {
        max_time = duration;
        max_time_pos = i;
    }
    current_active_nodes = ddsim.getActiveNodeCount(); 
    if  (current_active_nodes < min_active_nodes) {
        min_active_nodes = current_active_nodes;
        min_active_nodes_pos = i;
    }
    if  (current_active_nodes > max_active_nodes) {
        max_active_nodes= current_active_nodes;
        max_active_nodes_pos = i;
    }
    current_max_nodes = ddsim.getMaxNodeCount(); 
    if  (current_max_nodes < min_max_nodes) {
        min_max_nodes = current_max_nodes;
        min_max_nodes_pos = i;
    }
    if  (current_max_nodes > max_max_nodes) {
        max_max_nodes= current_max_nodes;
        max_max_nodes_pos = i;
    }

    //save the permutations with the corresponding measurements in the maps
    //save one by active nodes, one by max nodes and one by time
    perm_by_active_nodes[current_active_nodes].emplace_back(i, current_max_nodes, duration);
    perm_by_max_nodes[current_max_nodes].emplace_back(i, current_active_nodes, duration);
    perm_by_time[duration].emplace_back(i, current_active_nodes, current_max_nodes);

    //generate graphviz string and save it to access it later
    std::string graphvizString = ddsim.exportDDtoGraphvizString(true, true, true, true, true);
    permutationToGraphViz[i] = graphvizString;
    }

    //sort the vector in the maps by the not grouped by nodes count and then by time within the group
    for (auto& entry : perm_by_active_nodes) {
    std::sort(entry.second.begin(), entry.second.end(), 
    [](const std::tuple<size_t, size_t, std::chrono::duration<long double>>& a, const std::tuple<size_t, size_t, std::chrono::duration<long double>>& b) {
        // First compare by max_nodes
        if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);
        // If max_nodes are equal, then compare by execution time
        return std::get<2>(a) < std::get<2>(b);
    });
    }
    for (auto& entry : perm_by_max_nodes) {
    std::sort(entry.second.begin(), entry.second.end(), 
    [](const std::tuple<size_t, size_t, std::chrono::duration<long double>>& a, const std::tuple<size_t, size_t, std::chrono::duration<long double>>& b) {
        // First compare by max_nodes
        if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);
        // If max_nodes are equal, then compare by execution time
        return std::get<2>(a) < std::get<2>(b);
    });
    }
    
    //Print statistics of min and max time, active nodes and max nodes with the corresponding permutation
    out << left << setfill(' ') << setw(20) << "Total Permutations: " << permutation_count <<  "\n";
    out << left << setfill(' ') << setw(20) << "Min time: " << std::scientific << std::setprecision(4) << min_time.count() << "s" << right << setfill('.') << setw(30) << " Permutation " << DDMinimizer::formatSize_t(min_time_pos) << ": "<< DDMinimizer::permToString(permutations[min_time_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Max time: " << std::scientific << std::setprecision(4) << max_time.count() << "s" << right << setfill('.') << setw(30) << " Permutation " << DDMinimizer::formatSize_t(max_time_pos) << ": "<< DDMinimizer::permToString(permutations[max_time_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Min active nodes: "<< DDMinimizer::formatSize_t(min_active_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << DDMinimizer::formatSize_t(min_active_nodes_pos) << ": "<< DDMinimizer::permToString(permutations[min_active_nodes_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Min max nodes: "<< DDMinimizer::formatSize_t(min_max_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << DDMinimizer::formatSize_t(min_max_nodes_pos) << ": "<< DDMinimizer::permToString(permutations[min_max_nodes_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Max active nodes: "<< DDMinimizer::formatSize_t(max_active_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << DDMinimizer::formatSize_t(max_active_nodes_pos) << ": "<< DDMinimizer::permToString(permutations[max_active_nodes_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Max max nodes: "<< DDMinimizer::formatSize_t(max_max_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << DDMinimizer::formatSize_t(max_max_nodes_pos) << ": "<< DDMinimizer::permToString(permutations[max_max_nodes_pos]) << "\n";
    out << "________________________________________________________________________________________________\n\n";
    
    
    //write the graphvizStrings of the permutation with minimal and maximal active nodes into files
    std::string graphvizString = permutationToGraphViz[min_active_nodes_pos];
    std::string fnameMin = "BenchmarkQuasm/dot/" + name + "min_active" + std::to_string(qubits) + ".dot";
    std::ofstream out1(fnameMin);
    out1 << graphvizString;
    out1.close();

    std::string graphvizString2 = permutationToGraphViz[max_active_nodes_pos];
    std::string fnameMax = "BenchmarkQuasm/dot/" + name + "max_active" + std::to_string(qubits) + ".dot";
    std::ofstream out2(fnameMax);
    out2 << graphvizString2;
    out2.close();

    std::string graphvizString3 = permutationToGraphViz[min_max_nodes_pos];
    std::string fnameMinMax = "BenchmarkQuasm/dot/" + name + "min_max" + std::to_string(qubits) + ".dot";
    std::ofstream out3(fnameMinMax);
    out3 << graphvizString3;
    out3.close();

    std::string graphvizString4 = permutationToGraphViz[max_max_nodes_pos];
    std::string fnameMaxMax = "BenchmarkQuasm/dot/" + name + "max_max" + std::to_string(qubits) + ".dot";
    std::ofstream out4(fnameMaxMax);
    out4 << graphvizString4;
    out4.close();

    std::string graphvizString5 = permutationToGraphViz[min_time_pos];
    std::string fnameMinTime = "BenchmarkQuasm/dot/" + name + "min_time" + std::to_string(qubits) + ".dot";
    std::ofstream out5(fnameMinTime);
    out5 << graphvizString5;
    out5.close();

    std::string graphvizString6 = permutationToGraphViz[max_time_pos];
    std::string fnameMaxTime = "BenchmarkQuasm/dot/" + name + "max_time" + std::to_string(qubits) + ".dot";
    std::ofstream out6(fnameMaxTime);
    out6 << graphvizString6;
    out6.close();

    size_t index;
    size_t max_nodes = 0;
    size_t active_nodes = 0;
    std::chrono::duration<long double> time;
    std::chrono::duration<long double> sum_time(0);

    for (const auto& entry : perm_by_active_nodes) {
    //save the vecotr of all permutation indices (also simulation time and max_nodes) with the same active_nodes count 
    auto& vec = entry.second;
    out << vec.size() << " permutations have " << entry.first << " active nodes.\n";
    //iterate over all the permutations that have the same active_nodes/ are in the vector and print the max_nodes, simulation time and the permutation itself
    for (const auto& permutation : vec) {
        index =  std::get<0>(permutation);
        max_nodes=  std::get<1>(permutation);
        time =  std::get<2>(permutation);
        sum_time += time;
        out << DDMinimizer::measurementToString({true,false,true}, index, max_nodes, active_nodes, time);
        out << DDMinimizer::permToString(permutations[index]) <<"\n";
        }
    out << "\n...........................\n";
    out << "Average time: " << std::scientific << std::setprecision(4) << (sum_time / vec.size()).count() << "s\n\n";
    sum_time = std::chrono::duration<long double>(0);
    }
    out << "________________________________________________________________________________________________\n\n";
    for (const auto& entry : perm_by_max_nodes) {
        //save the vecotr of all permutation indices (also simulation time and active_nodes) with the same max_nodes count 
        const auto& vec = entry.second;
        out << vec.size() << " permutations have " << entry.first << " max nodes.\n";
        //iterate over all the permutations that have the same max_nodes/ are in the vector and print the active_nodes, simulation time and the permutation itself
        for (const auto& permutation : vec) {
            index = std::get<0>(permutation);
            active_nodes = std::get<1>(permutation);
            time = std::get<2>(permutation);
            sum_time += time;
            out << DDMinimizer::measurementToString({false,true,true}, index, max_nodes, active_nodes, time );;
            out << DDMinimizer::permToString(permutations[index]) <<"\n";
        }
        out << "\n...........................\n";
        out << "Average time: " << std::scientific << std::setprecision(4) << (sum_time / vec.size()).count() << "s\n\n";
        sum_time = std::chrono::duration<long double>(0);
    }
    out << "________________________________________________________________________________________________\n\n";
    for (const auto& entry : perm_by_time) {
        time = entry.first; 
        const auto& perm = entry.second;
        out << "execution time of: " << std::scientific << std::setprecision(4) << time.count() << "s: ";
        //Each time should only have one entry. I think
        for (const auto& permutation : perm) {
        index = std::get<0>(permutation);
        active_nodes = std::get<1>(permutation);
        max_nodes = std::get<2>(permutation);
        // Assuming measurementToString and permToString functions can handle each permutation's details
        out << DDMinimizer::measurementToString({true, true, false}, index, max_nodes, active_nodes, time);
        out << DDMinimizer::permToString(permutations[index]) << "\n";
    }
        out << "\n";
    }
    out << "________________________________________________________________________________________________\n\n";
}

























void optimizeInputPermutation(qc::QuantumComputation qc){
    /*TODO
    qc::Permutation perm = DDMinimizer::createControlBasedPermutation(qc);
    qc.initialLayout = perm;
    qc::CircuitOptimizer::elidePermutations(qc); 
    */
}

void DDMinimizer::parseOptions(bool gateBased,  bool controlBased, bool allPermutations, std::string file, int qubits){
    std::map<std::string, std::pair<int, int>> fileNames = {
    {"ae_indep_qiskit_", {2, 7}},
    {"dj_indep_qiskit_", {2, 7}},
    {"ghz_indep_qiskit_", {2, 10}},
    {"graphstate_indep_qiskit_", {3, 7}},
    {"grover_", {5, 5}},
    {"grover-noancilla_indep_qiskit_", {2, 7}},
    {"grover-v-chain_indep_qiskit_", {2, 7}},
    {"portfolioqaoa_indep_qiskit_", {3, 7}},
    {"portfoliovqe_indep_qiskit_", {3, 7}},
    {"qaoa_indep_qiskit_", {3, 10}},
    {"qftentangled_indep_qiskit_", {2, 7}},
    {"qft_indep_qiskit_", {2, 10}},
    {"qnn_indep_qiskit_", {2, 7}},
    {"qpeexact_indep_qiskit_", {2, 10}},
    {"qpeinexact_indep_qiskit_", {2, 7}},
    {"qwalk-noancilla_indep_qiskit_", {3, 7}},
    {"qwalk-v-chain_indep_qiskit_", {3, 7}},
    {"random_indep_qiskit_", {2, 7}},
    {"realamprandom_indep_qiskit_", {2, 7}},
    {"su2random_indep_qiskit_", {2, 7}},
    {"twolocalrandom_indep_qiskit_", {2, 7}},
    {"vqe_indep_qiskit_", {3, 10}},
    {"wstate_indep_qiskit_", {2, 7}}
    };

    if(file.compare(" ") != 0){
        std::cout << "file: " + file + "\n";
        auto it = fileNames.find(file);
            if (it != fileNames.end()) {
                std::pair<int, int> minMaxQubits = it->second;
                 if(qubits != 0){
                    if(qubits < minMaxQubits.first || qubits > minMaxQubits.second) {
                         std::cout << std::to_string(qubits) + " not valid for " + file + ". Please check list for available versions again.\n";
                    }
                    else{
                        DDMinimizer::runInputCompariston(qubits, qubits, gateBased, controlBased, allPermutations, file);      
                    }
                }
                else{
                    DDMinimizer::runInputCompariston(minMaxQubits.first, minMaxQubits.second, gateBased, controlBased, allPermutations, file);
                }
            }
            else {
                std::cout << file + " not found. Please check list of available circuits again. \n";
                return;
           }  
    }
    else{
        //run all simulations
        for(const auto& entry : fileNames){
                DDMinimizer::runInputCompariston(entry.second.first, entry.second.second, gateBased, controlBased, allPermutations, entry.first); 
        }
    }
}

void DDMinimizer::runInputCompariston(int from, int to, bool gateBased,  bool controlBased, bool allPermutations, std::string file){
    std::string qasmString;
 
    if(gateBased){
        std::string fname_gate = "BenchmarkQuasm/results/" + file+ "gates.txt";
        std::cout << "Start measuring of gate based permutations for " + fname_gate + "\n"; 
        std::cout.flush();
        std::ofstream out_gate(fname_gate);
        for(int k = from; k <= to; k++){
            if(file.compare("random_indep_qiskit_") == 0 && k == 6){
                continue;
            }
            try {
                std::string fn = "BenchmarkQuasm/qasm/" + file + std::to_string(k) + ".qasm";
                qasmString = DDMinimizer::readFileIntoString(fn);
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
            }

            out_gate << "Start Simulation of " + file + std::to_string(k) + "__________________________________________________\n";            
            auto qc = QuantumComputation::fromQASM(qasmString);
            auto qcc = QuantumComputation::fromQASM(qasmString);
            out_gate << qc;
            qc::Permutation perm = DDMinimizer::createGateBasedPermutation(out_gate, qc);
            qcc.initialLayout = perm; 
            qc::CircuitOptimizer::elidePermutations(qcc);
            DDMinimizer::runLayoutComparison(out_gate, qc, qcc, perm, file, k);
        } 
        out_gate.close();
        DDMinimizer::removeAnsi(fname_gate);
    }


    if(controlBased){ 
        std::string fname_control = "BenchmarkQuasm/results/" + file+ "controls.txt";
        std::cout << "Start measuring of cotrol based permutations for " + fname_control + "\n"; 
        std::cout.flush();
        std::ofstream out_control(fname_control);
        for(int k = from; k <= to; k++){
             if(file.compare("random_indep_qiskit_") == 0 && k == 6){
                continue;
            }
            try {
                std::string fn = "BenchmarkQuasm/qasm/" + file + std::to_string(k) + ".qasm";
                qasmString = DDMinimizer::readFileIntoString(fn);
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
            }
            out_control << "Start Simulation of " + file + std::to_string(k) + "__________________________________________________\n";            
            auto qc = QuantumComputation::fromQASM(qasmString);
            auto qcc = QuantumComputation::fromQASM(qasmString);
            out_control << qc;
            qc::Permutation perm = DDMinimizer::createControlBasedPermutation(qc);
            qcc.initialLayout = perm; 
            qc::CircuitOptimizer::elidePermutations(qcc);
            DDMinimizer::runLayoutComparison(out_control, qc, qcc, perm, file, k);
        }
         out_control.close();
        DDMinimizer::removeAnsi(fname_control);
    } 
    
    if(allPermutations){
        std::string fname_all = "BenchmarkQuasm/results/" + file+ "all.txt";
        std::cout << "Start measuring all permutations for " + fname_all + "\n";
        std::cout.flush();
        std::ofstream out_all(fname_all);
        for(int k = from; k <= to && k <= 6; k++){
            if(file.compare("random_indep_qiskit_") == 0 && k == 6){
                continue;
            }
            try {
                std::string fn = "BenchmarkQuasm/qasm/" + file + std::to_string(k) + ".qasm";
                qasmString = DDMinimizer::readFileIntoString(fn);
            } catch (const std::exception& e) {
                std::cout << e.what() << "\n";
            }
            out_all << "Start Simulation of " + file + std::to_string(k) + "__________________________________________________\n";            
            auto qc = QuantumComputation::fromQASM(qasmString);
            out_all << qc;
            DDMinimizer::runAllComparisons(out_all, qc, file, k);
        }
        out_all.close();
        DDMinimizer::removeAnsi(fname_all);
    } 
}

void DDMinimizer::runLayoutComparison(std::ofstream& out, qc::QuantumComputation& qc, qc::QuantumComputation& qcc, qc::Permutation perm, std::string file,  int qubits){
    out << left << setfill(' ') << setw(30) << "Computed Permutation: " << right << setfill('.') << setw(30) <<DDMinimizer::permToString(perm) << "\n";
    out << left << setfill(' ') << setw(30) << "Original Permutation: " << right << setfill('.') << setw(30) <<DDMinimizer::permToString(qc.initialLayout) << "\n";

    auto qcc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qcc));        
    CircuitSimulator ddsim_qcc(std::move(qcc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qc));        
    CircuitSimulator ddsim_qc(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    

    std::chrono::duration<long double> duration_qc;
    std::chrono::duration<long double> duration_qcc;
    std::size_t size_max_qc = 0;
    std::size_t size_max_qcc = 0;
    std::size_t size_qc = 0;
    std::size_t size_qcc = 0;  

    
    ddsim_qc.simulate(1);
    ddsim_qc.simulate(1);
    auto start3_qc = std::chrono::high_resolution_clock::now();
    ddsim_qc.simulate(1); 
    auto end3_qc = std::chrono::high_resolution_clock::now();
    auto start4_qc = std::chrono::high_resolution_clock::now();
    ddsim_qc.simulate(1); 
    auto end4_qc = std::chrono::high_resolution_clock::now();
    auto start5_qc = std::chrono::high_resolution_clock::now();
    ddsim_qc.simulate(1); 
    auto end5_qc = std::chrono::high_resolution_clock::now();
    auto start6_qc = std::chrono::high_resolution_clock::now();
    ddsim_qc.simulate(1); 
    auto end6_qc = std::chrono::high_resolution_clock::now();

    ddsim_qcc.simulate(1);
    ddsim_qcc.simulate(1);
    auto start3_qcc = std::chrono::high_resolution_clock::now();
    ddsim_qcc.simulate(1); 
    auto end3_qcc = std::chrono::high_resolution_clock::now();
    auto start4_qcc = std::chrono::high_resolution_clock::now();
    ddsim_qcc.simulate(1); 
    auto end4_qcc = std::chrono::high_resolution_clock::now();
    auto start5_qcc = std::chrono::high_resolution_clock::now();
    ddsim_qcc.simulate(1); 
    auto end5_qcc = std::chrono::high_resolution_clock::now();
    auto start6_qcc = std::chrono::high_resolution_clock::now();
    ddsim_qcc.simulate(1); 
    auto end6_qcc = std::chrono::high_resolution_clock::now();
    
   

    duration_qc = (end3_qc - start3_qc + end4_qc - start4_qc + end5_qc - start5_qc + end6_qc - start6_qc) / 4.0;
    size_max_qc = ddsim_qc.getMaxNodeCount(); 
    size_qc = ddsim_qc.getActiveNodeCount();

    duration_qcc = (end3_qcc - start3_qcc + end4_qcc - start4_qcc + end5_qcc - start5_qcc + end6_qcc - start6_qcc) / 4.0;
    size_max_qcc = ddsim_qcc.getMaxNodeCount();
    size_qcc = ddsim_qcc.getActiveNodeCount();

    std::string graphvizString_qc = ddsim_qc.exportDDtoGraphvizString(true, true, true, true, true);
    std::string graphvizString_qcc = ddsim_qcc.exportDDtoGraphvizString(true, true, true, true, true);

    std::string fname_qc = "BenchmarkQuasm/dot_computed/" + file + std::to_string(qubits) + ".dot";
    std::ofstream out_qc(fname_qc);
    out_qc << graphvizString_qc;
    out_qc.close();
    std::string fname_qcc = "BenchmarkQuasm/dot_computed/" + file + std::to_string(qubits) + ".dot";
    std::ofstream out_qcc(fname_qcc); 
    out_qcc << graphvizString_qcc;
    out_qcc.close();

    out << std::left << std::setfill(' ') << std::setw(30) << "Identity Duration: " << std::scientific << std::setprecision(4) << duration_qc.count() << "s" << std::right << std::setfill('.') << std::setw(30) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Permuted Duration: " << std::scientific << std::setprecision(4) << duration_qcc.count() << "s" << std::right << std::setfill('.') << std::setw(30) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Identity active_nodes: " << DDMinimizer::formatSize_t(size_qc)  << std::right << std::setfill('.') << std::setw(39) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Permuted active_nodes: " << DDMinimizer::formatSize_t(size_qcc) << std::right << std::setfill('.') << std::setw(39) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Identity max_nodes: " << DDMinimizer::formatSize_t(size_max_qc)  << std::right << std::setfill('.') << std::setw(39) << "\n";
    out << std::left << std::setfill(' ') << std::setw(30) << "Permuted max_nodes: " << DDMinimizer::formatSize_t(size_max_qcc) << std::right << std::setfill('.') << std::setw(39) << "\n";
    out << "________________________________________________________________________________________________\n\n";
}


void DDMinimizer::runAllComparisons(std::ofstream& out, qc::QuantumComputation& qc, std::string file,  int qubits){
    std::size_t bits = qc.getNqubits();
    std::size_t permutation_count = DDMinimizer::factorial(bits);
    std::vector<qc::Permutation> permutations = DDMinimizer::createAllPermutations(qc);

    /*prepare all variables for simulation and benchmarking:

    measurements for the size of the decision diagram:
    -min and max, as well as the coresponding index/position of the permutation in the permutatin vector for the active and max nodes

    measurement for the simulation:
    -min and max duration of the simulation , as well as the coresponding index/position of the permutation in the permutatin vector

    maps to collect all measurements for one permutation and sort them later by active nodes and max nodes for analysis
    -perm_by_active_nodes:active noes -> (index, max nodes, time)
    -perm_by_max_nodes: max nodes -> (index, active nodes, time)
    -perm_by_time: time -> (index, active nodes, max nodes)

    map to collect all the GraphViz strings of the decision diagramm of the permutation after simulation (end state)
    -permutationToGraphViz: index in permutation vector -> GraphvizString
    */
    std::size_t min_active_nodes = std::numeric_limits<std::size_t>::max();
    std::size_t max_active_nodes = 0;
    std::size_t min_active_nodes_pos = 0;
    std::size_t max_active_nodes_pos = 0;
    std::size_t current_active_nodes = 0;

    std::size_t min_max_nodes = std::numeric_limits<std::size_t>::max();
    std::size_t max_max_nodes = 0;
    std::size_t min_max_nodes_pos = 0;
    std::size_t max_max_nodes_pos = 0;
    std::size_t current_max_nodes = 0;

    std::chrono::duration<long double> min_time(std::numeric_limits<double>::max());
    std::chrono::duration<long double> max_time(std::numeric_limits<double>::lowest());
    std::size_t min_time_pos = 0;
    std::size_t max_time_pos = 0;
    std::chrono::duration<long double> duration;

    std::map<size_t, std::vector<std::tuple<size_t, size_t, std::chrono::duration<long double>>>> perm_by_active_nodes;
    std::map<size_t, std::vector<std::tuple<size_t, size_t, std::chrono::duration<long double>>>> perm_by_max_nodes;
    std::map<std::chrono::duration<long double>, std::vector<std::tuple<size_t, size_t, size_t>>> perm_by_time;
    std::map<size_t, std::string> permutationToGraphViz;

    /* Simulate the circuit using each permutation
    */
    for (size_t i = 0; i < permutation_count; i++) { 
    //Create copy of the Quantum Computation, set the initial layout as the current permutation, apply the permutation to the qubits,
    //create the simulator and with that the decision diagram
    QuantumComputation qcc = qc;
    qcc.initialLayout = permutations[i]; 
    qc::CircuitOptimizer::elidePermutations(qcc); 
    auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qcc));        
    CircuitSimulator ddsim(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

    /*std::clock_t start = std::clock();
    ddsim.simulate(1);
    std::clock_t end = std::clock();
    double cpu_time_used = static_cast<double>(end - start) / CLOCKS_PER_SEC;*/

    //Save the time before and after the simulation and run the simulation once
    ddsim.simulate(1);
    ddsim.simulate(1);
    auto start3_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end3_qc = std::chrono::high_resolution_clock::now();
    auto start4_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end4_qc = std::chrono::high_resolution_clock::now();
    auto start5_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end5_qc = std::chrono::high_resolution_clock::now();
    auto start6_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end6_qc = std::chrono::high_resolution_clock::now();

    //collect measruements as in execution time, active nodes, max nodes
    //set min and max values and save positions
    duration = (end3_qc - start3_qc + end4_qc - start4_qc + end5_qc - start5_qc  + end6_qc - start6_qc) / 4.0;
    if (duration < min_time) {
        min_time = duration;
        min_time_pos = i;
    }
    if (duration > max_time) {
        max_time = duration;
        max_time_pos = i;
    }
    current_active_nodes = ddsim.getActiveNodeCount(); 
    if  (current_active_nodes < min_active_nodes) {
        min_active_nodes = current_active_nodes;
        min_active_nodes_pos = i;
    }
    if  (current_active_nodes > max_active_nodes) {
        max_active_nodes= current_active_nodes;
        max_active_nodes_pos = i;
    }
    current_max_nodes = ddsim.getMaxNodeCount(); 
    if  (current_max_nodes < min_max_nodes) {
        min_max_nodes = current_max_nodes;
        min_max_nodes_pos = i;
    }
    if  (current_max_nodes > max_max_nodes) {
        max_max_nodes= current_max_nodes;
        max_max_nodes_pos = i;
    }

    //save the permutations with the corresponding measurements in the maps
    //save one by active nodes, one by max nodes and one by time
    perm_by_active_nodes[current_active_nodes].emplace_back(i, current_max_nodes, duration);
    perm_by_max_nodes[current_max_nodes].emplace_back(i, current_active_nodes, duration);
    perm_by_time[duration].emplace_back(i, current_active_nodes, current_max_nodes);

    //generate graphviz string and save it to access it later
    std::string graphvizString = ddsim.exportDDtoGraphvizString(true, true, true, true, true);
    permutationToGraphViz[i] = graphvizString;
    }

    //sort the vector in the maps by the not grouped by nodes count and then by time within the group
    for (auto& entry : perm_by_active_nodes) {
    std::sort(entry.second.begin(), entry.second.end(), 
    [](const std::tuple<size_t, size_t, std::chrono::duration<long double>>& a, const std::tuple<size_t, size_t, std::chrono::duration<long double>>& b) {
        // First compare by max_nodes
        if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);
        // If max_nodes are equal, then compare by execution time
        return std::get<2>(a) < std::get<2>(b);
    });
    }
    for (auto& entry : perm_by_max_nodes) {
    std::sort(entry.second.begin(), entry.second.end(), 
    [](const std::tuple<size_t, size_t, std::chrono::duration<long double>>& a, const std::tuple<size_t, size_t, std::chrono::duration<long double>>& b) {
        // First compare by max_nodes
        if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);
        // If max_nodes are equal, then compare by execution time
        return std::get<2>(a) < std::get<2>(b);
    });
    }
    
    //Print statistics of min and max time, active nodes and max nodes with the corresponding permutation
    out << left << setfill(' ') << setw(20) << "Total Permutations: " << permutation_count <<  "\n";
    out << left << setfill(' ') << setw(20) << "Min time: " << std::scientific << std::setprecision(4) << min_time.count() << "s" << right << setfill('.') << setw(30) << " Permutation " << DDMinimizer::formatSize_t(min_time_pos) << ": "<< DDMinimizer::permToString(permutations[min_time_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Max time: " << std::scientific << std::setprecision(4) << max_time.count() << "s" << right << setfill('.') << setw(30) << " Permutation " << DDMinimizer::formatSize_t(max_time_pos) << ": "<< DDMinimizer::permToString(permutations[max_time_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Min active nodes: "<< DDMinimizer::formatSize_t(min_active_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << DDMinimizer::formatSize_t(min_active_nodes_pos) << ": "<< DDMinimizer::permToString(permutations[min_active_nodes_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Min max nodes: "<< DDMinimizer::formatSize_t(min_max_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << DDMinimizer::formatSize_t(min_max_nodes_pos) << ": "<< DDMinimizer::permToString(permutations[min_max_nodes_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Max active nodes: "<< DDMinimizer::formatSize_t(max_active_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << DDMinimizer::formatSize_t(max_active_nodes_pos) << ": "<< DDMinimizer::permToString(permutations[max_active_nodes_pos]) << "\n";
    out << left << setfill(' ') << setw(20) << "Max max nodes: "<< DDMinimizer::formatSize_t(max_max_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << DDMinimizer::formatSize_t(max_max_nodes_pos) << ": "<< DDMinimizer::permToString(permutations[max_max_nodes_pos]) << "\n";
    out << "________________________________________________________________________________________________\n\n";
    
    
    //write the graphvizStrings of the permutation with minimal and maximal active nodes into files
    std::string graphvizString = permutationToGraphViz[min_active_nodes_pos];
    std::string fnameMin = "BenchmarkQuasm/dot/" + file + "min_active" + std::to_string(qubits) + ".dot";
    std::ofstream out1(fnameMin);
    out1 << graphvizString;
    out1.close();

    std::string graphvizString2 = permutationToGraphViz[max_active_nodes_pos];
    std::string fnameMax = "BenchmarkQuasm/dot/" + file + "max_active" + std::to_string(qubits) + ".dot";
    std::ofstream out2(fnameMax);
    out2 << graphvizString2;
    out2.close();

    std::string graphvizString3 = permutationToGraphViz[min_max_nodes_pos];
    std::string fnameMinMax = "BenchmarkQuasm/dot/" + file + "min_max" + std::to_string(qubits) + ".dot";
    std::ofstream out3(fnameMinMax);
    out3 << graphvizString3;
    out3.close();

    std::string graphvizString4 = permutationToGraphViz[max_max_nodes_pos];
    std::string fnameMaxMax = "BenchmarkQuasm/dot/" + file + "max_max" + std::to_string(qubits) + ".dot";
    std::ofstream out4(fnameMaxMax);
    out4 << graphvizString4;
    out4.close();

    size_t index;
    size_t max_nodes = 0;
    size_t active_nodes = 0;
    std::chrono::duration<long double> time;
    std::chrono::duration<long double> sum_time(0);

    for (const auto& entry : perm_by_active_nodes) {
    //save the vecotr of all permutation indices (also simulation time and max_nodes) with the same active_nodes count 
    auto& vec = entry.second;
    out << vec.size() << " permutations have " << entry.first << " active nodes.\n";
    //iterate over all the permutations that have the same active_nodes/ are in the vector and print the max_nodes, simulation time and the permutation itself
    for (const auto& permutation : vec) {
        //index =  std::get<0>(permutation);
        //max_nodes=  std::get<1>(permutation);
        time =  std::get<2>(permutation);
        sum_time += time;
        //out << DDMinimizer::measurementToString({true,false,true}, index, max_nodes, active_nodes, time);
        //out << DDMinimizer::permToString(permutations[index]) <<"\n";
        }
    out << "\n...........................\n";
    out << "Average time: " << std::scientific << std::setprecision(4) << (sum_time / vec.size()).count() << "s\n\n";
    sum_time = std::chrono::duration<long double>(0);
    }
    out << "________________________________________________________________________________________________\n\n";
    for (const auto& entry : perm_by_max_nodes) {
        //save the vecotr of all permutation indices (also simulation time and active_nodes) with the same max_nodes count 
        const auto& vec = entry.second;
        out << vec.size() << " permutations have " << entry.first << " max nodes.\n";
        //iterate over all the permutations that have the same max_nodes/ are in the vector and print the active_nodes, simulation time and the permutation itself
        for (const auto& permutation : vec) {
            //index = std::get<0>(permutation);
            //active_nodes = std::get<1>(permutation);
            time = std::get<2>(permutation);
            sum_time += time;
            //out << DDMinimizer::measurementToString({false,true,true}, index, max_nodes, active_nodes, time );;
            //out << DDMinimizer::permToString(permutations[index]) <<"\n";
        }
        out << "\n...........................\n";
        out << "Average time: " << std::scientific << std::setprecision(4) << (sum_time / vec.size()).count() << "s\n\n";
        sum_time = std::chrono::duration<long double>(0);
    }
    out << "________________________________________________________________________________________________\n\n";
    for (const auto& entry : perm_by_time) {
        time = entry.first; 
        const auto& perm = entry.second;
        out << "execution time of: " << std::scientific << std::setprecision(4) << time.count() << "s: ";
        //Each time should only have one entry. I think
        if (!perm.empty()) { 
        auto& permutation = perm.front(); 
        index = std::get<0>(permutation);
        active_nodes = std::get<1>(permutation);
        max_nodes = std::get<2>(permutation);
        out << DDMinimizer::measurementToString({true,true,false}, index, max_nodes, active_nodes, time );
        out << DDMinimizer::permToString(permutations[index]);
    }
        out << "\n";
    }
    out << "________________________________________________________________________________________________\n\n";
}

qc::Permutation DDMinimizer::createGateBasedPermutation(std::ofstream& out, qc::QuantumComputation& qc){
    return qc.initialLayout;
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

    /*for(int i = 0; i < bits; i++){
        std::cout << "Q: " << i << " w: " << qubitWeights[i] << " | ";
    }
    std::cout << "\n";*/   
    
    qc::Permutation perm;

    for (std::size_t m = 0; m < bits; m++) {
               perm[logicalQubits[m]] = physicalQubits[m];
          }
    return perm;
}


std::vector<qc::Permutation> DDMinimizer::createAllPermutations(qc::QuantumComputation& qc) {
     std::size_t bits = qc.getNqubits();
     std::size_t permutation_count = DDMinimizer::factorial(bits);

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

     std::sort(logicalQubits.begin(), logicalQubits.end());
     do {
          layouts.push_back(logicalQubits);
     } while (std::next_permutation(logicalQubits.begin(), logicalQubits.end()));


     //create all layouts
     //loop over all permutations
     for(std::size_t i = 0; i < permutation_count; i++){
          //loop over all logical bits and create one layout
          for (std::size_t m = 0; m < bits; m++) {
               perm[physicalQubits[m]] = layouts[i][m];
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
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::stringstream buffer;
    std::string line;
        while (std::getline(fileStream, line)) {
            buffer << line << '\n';
    }
    return buffer.str();
}

std::string DDMinimizer::permToString(Permutation perm){
    std::ostringstream oss;
    for (const auto& entry : perm) {
        oss << entry.first << " -> " << entry.second << "|";
    }
    return oss.str();
}

/*
@param code 1: active, 2: max, 3: time
*/
std::string DDMinimizer::measurementToString(std::vector<bool> code, size_t index, size_t max_nodes, size_t active_nodes, std::chrono::duration<long double> time){
    std::ostringstream oss;
    oss << "Index: " << DDMinimizer::formatSize_t(index) <<" | ";

    if(code[0]){
        oss << "Max_nodes: " << DDMinimizer::formatSize_t(max_nodes) << " | ";
    }
    if(code[1]){
        oss << "Active_nodes: " << DDMinimizer::formatSize_t(active_nodes) << " | ";
    }
    if(code[2]){
        oss << "Time: " << std::scientific << std::setprecision(4) << time.count() << "s | ";
    }
    return oss.str();
}

std::string DDMinimizer::formatSize_t(size_t t){
    std::ostringstream oss;
    if(t > 99){
        oss << t;
    }
    else if(t > 9){
        oss << t << " ";
    }
    else{
        oss << t << "  ";
    }
    return oss.str();
}

void DDMinimizer::removeAnsi(std::string filePath){
    // Open input file in read mode
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        return;
    }

    // Read the contents of the file into a string using stringstream
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    inputFile.close();

    const std::regex ansi_escape(R"(\x1B\[[0-?]*[ -/]*[@-~])");
    // Remove ANSI escape codes
    std::string cleanedContents = std::regex_replace(buffer.str(), ansi_escape, "");

    // Open the same file in write mode to overwrite it
    std::ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        std::cout << "Failed to open file for writing: " << filePath << std::endl;
        return;
    }

    // Write the cleaned contents back to the file
    outputFile << cleanedContents;
    outputFile.close();
}



};// namespace qc