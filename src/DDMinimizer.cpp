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
#include <numeric>



using namespace qc;
using namespace std;

namespace qc {

/***
 * Public Methods
 ***/

void DDMinimizer::runOverNight(){
    std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> data = {
        /*{"ae_indep_qiskit_2.qasm", {}},
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
        {"wstate_indep_qiskit_6.qasm", {}},*/
        /*{"aa_0.qasm", {}},
        {"aa_1.qasm", {}},
        {"aa_2.qasm", {}},
        {"aa_3.qasm", {}},
        {"aa_4.qasm", {}},
        {"aa_5.qasm", {}},
        {"aa_6.qasm", {}},
        {"aa_7.qasm", {}},
        {"aa_8.qasm", {}},
        {"aa_9.qasm", {}},
        {"aa_10.qasm", {}},
        {"aa_11.qasm", {}},
        {"aa_12.qasm", {}},
        {"aa_13.qasm", {}},
        {"aa_14.qasm", {}},
        {"aa_15.qasm", {}},
        {"aa_16.qasm", {}},
        {"aa_17.qasm", {}},
        {"aa_18.qasm", {}},
        {"aa_19.qasm", {}},*/
        {"aaa_0.qasm", {}},
        {"aaa_1.qasm", {}},
        {"aaa_2.qasm", {}},
        {"aaa_3.qasm", {}},
        {"aaa_4.qasm", {}},
        {"aaa_5.qasm", {}},
        {"aaa_6.qasm", {}},
        {"aaa_7.qasm", {}},
        {"aaa_8.qasm", {}},
        {"aaa_9.qasm", {}},
        /*{"aaa_10.qasm", {}},
        {"aaa_11.qasm", {}},*/
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

    std::cout << "\n Start final run for measurement!  \n";
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

            //std::chrono::duration<long double> min_res = std::chrono::duration<long double> (resControl +  entry.second.second) / 2.0;

            std::chrono::duration<long double> min_res = std::min({resControl, entry.second.second});

            auto qcc = QuantumComputation::fromQASM(qasmString);
            std::map<std::size_t, std::chrono::duration<long double>> resAll = DDMinimizer::measureAll(qcc); 

            data[entry.first] = std::make_pair(DDMinimizer::makeAverage(resAll, entry.second.first), min_res);   
}
    return data;
}

std::map<std::size_t, std::chrono::duration<long double>> DDMinimizer::makeAverage(std::map<std::size_t, std::chrono::duration<long double>> res1, std::map<std::size_t, std::chrono::duration<long double>> res2){
    std::map<std::size_t, std::chrono::duration<long double>> min_time;

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


                //min_time[key] = std::chrono::duration<long double> (duration1 + duration2) / 2.0;

                min_time[key] = std::min({duration1, duration2});
            }
        }
        return min_time;
}

std::chrono::duration<long double> DDMinimizer::measureControl(qc::QuantumComputation& qc){
    auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qc));        
    CircuitSimulator ddsim(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    
    std::chrono::duration<long double> duration_1;
    std::chrono::duration<long double> duration_2;
    std::chrono::duration<long double> duration_3;
    std::chrono::duration<long double> duration_4;

    ddsim.simulate(1); 
    ddsim.simulate(1); 
    auto start1_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end1_qc = std::chrono::high_resolution_clock::now();
    auto start2_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end2_qc = std::chrono::high_resolution_clock::now();
    auto start3_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end3_qc = std::chrono::high_resolution_clock::now();
    auto start4_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end4_qc = std::chrono::high_resolution_clock::now();
    
    duration_1 = std::chrono::duration_cast<std::chrono::duration<long double>> (end1_qc - start1_qc);
    duration_2 = std::chrono::duration_cast<std::chrono::duration<long double>> (end2_qc - start2_qc);
    duration_3 = std::chrono::duration_cast<std::chrono::duration<long double>> (end3_qc - start3_qc);
    duration_4 = std::chrono::duration_cast<std::chrono::duration<long double>> (end4_qc - start4_qc);

    //return std::chrono::duration_cast<std::chrono::duration<long double>> (duration_1 + duration_2 + duration_3 + duration_4) / 4.0;

    return std::min({duration_1, duration_2, duration_3, duration_4});
}


std::map<std::size_t, std::chrono::duration<long double>> DDMinimizer::measureAll(qc::QuantumComputation& qc){
    std::map<std::size_t, std::chrono::duration<long double>> res;
    std::size_t bits = qc.getNqubits();    
    std::size_t permutation_count = DDMinimizer::factorial(bits);
    std::vector<qc::Permutation> permutations = DDMinimizer::createAllPermutations(qc);

    std::chrono::duration<long double> duration_1;
    std::chrono::duration<long double> duration_2;
    std::chrono::duration<long double> duration_3;
    std::chrono::duration<long double> duration_4;

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
    auto start1_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end1_qc = std::chrono::high_resolution_clock::now();
    auto start2_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end2_qc = std::chrono::high_resolution_clock::now();
    auto start3_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end3_qc = std::chrono::high_resolution_clock::now();
    auto start4_qc = std::chrono::high_resolution_clock::now();
    ddsim.simulate(1); 
    auto end4_qc = std::chrono::high_resolution_clock::now();
    
    duration_1 = std::chrono::duration_cast<std::chrono::duration<long double>> (end1_qc - start1_qc);
    duration_2 = std::chrono::duration_cast<std::chrono::duration<long double>> (end2_qc - start2_qc);
    duration_3 = std::chrono::duration_cast<std::chrono::duration<long double>> (end3_qc - start3_qc);
    duration_4 = std::chrono::duration_cast<std::chrono::duration<long double>> (end4_qc - start4_qc);

    //res[i]  = std::chrono::duration_cast<std::chrono::duration<long double>> (duration_1 + duration_2 + duration_3 + duration_4) / 4.0;

    res[i]  = std::min({duration_1, duration_2, duration_3, duration_4});

    }
    return res;
}

void DDMinimizer::printResults(std::map<std::string, std::pair<std::map<std::size_t, std::chrono::duration<long double>>, std::chrono::duration<long double>>> data){
    std::map<std::string, std::pair<int, int>> fileNames = {
    /*{"ae_indep_qiskit_", {2, 6}},
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
    {"wstate_indep_qiskit_", {5, 6}}*/
    //{"aa_", {0,19}},
    {"aaa_", {0, 9}},
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
    std::chrono::duration<long double> duration_qc = std::chrono::duration<long double>::zero();
    std::chrono::duration<long double> duration_qcc = std::chrono::duration<long double>::zero();   
    
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
        std::cout << "Key not fstopound in map\n";
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
    /*for (const auto& permutation : vec) {
        index =  std::get<0>(permutation);
        max_nodes=  std::get<1>(permutation);
        time =  std::get<2>(permutation);
        sum_time += time;
        out << DDMinimizer::measurementToString({true,false,true}, index, max_nodes, active_nodes, time);
        out << DDMinimizer::permToString(permutations[index]) <<"\n";
        }
    out << "\n...........................\n";*/
    out << "Average time: " << std::scientific << std::setprecision(4) << (sum_time / vec.size()).count() << "s\n\n";
    sum_time = std::chrono::duration<long double>(0);
    }

    out << "________________________________________________________________________________________________\n\n";
    for (const auto& entry : perm_by_max_nodes) {
        //save the vecotr of all permutation indices (also simulation time and active_nodes) with the same max_nodes count 
        const auto& vec = entry.second;
        out << vec.size() << " permutations have " << entry.first << " max nodes.\n";
        //iterate over all the permutations that have the same max_nodes/ are in the vector and print the active_nodes, simulation time and the permutation itself
        /*for (const auto& permutation : vec) {
            index = std::get<0>(permutation);
            active_nodes = std::get<1>(permutation);
            time = std::get<2>(permutation);
            sum_time += time;
            out << DDMinimizer::measurementToString({false,true,true}, index, max_nodes, active_nodes, time );;
            out << DDMinimizer::permToString(permutations[index]) <<"\n";
        }
        out << "\n...........................\n";*/
        out << "Average time: " << std::scientific << std::setprecision(4) << (sum_time / vec.size()).count() << "s\n\n";
        sum_time = std::chrono::duration<long double>(0);
    }
    int max = 20;
    out << "________________________________________________________________________________________________\n\n";
    for (const auto& entry : perm_by_time) {
        time = entry.first; 
        const auto& perm = entry.second;
        //Each time should only have one entry. I think
        for (const auto& permutation : perm) {
        if(max <= 0) {
            break;
        }
        max --;
        out << "execution time of: " << std::scientific << std::setprecision(4) << time.count() << "s: ";
        index = std::get<0>(permutation);
        active_nodes = std::get<1>(permutation);
        max_nodes = std::get<2>(permutation);
        out << DDMinimizer::measurementToString({true, true, false}, index, max_nodes, active_nodes, time);
        out << DDMinimizer::permToString(permutations[index]) << "\n";
    }
    }
    out << "________________________________________________________________________________________________\n\n";
}

























void optimizeInputPermutation(qc::QuantumComputation qc){
    qc::Permutation perm = DDMinimizer::createControlBasedPermutation(qc);
    qc.initialLayout = perm;
    qc::CircuitOptimizer::elidePermutations(qc); 
}

void DDMinimizer::parseOptions(bool gateBased,  bool controlBased, bool allPermutations, std::string file, int qubits){
    std::map<std::string, std::pair<int, int>> fileNames = {
    {"aa_", {0, 19}},
    {"aaa_", {0, 11}},
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
            qc::Permutation perm = DDMinimizer::createGateBasedPermutation(qc);
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

qc::Permutation DDMinimizer::createGateBasedPermutation(qc::QuantumComputation& qc){
    std::map<std::string, std::map<std::pair<Qubit, Qubit>, int>> maps = DDMinimizer::makeDataStructure(qc).first;
    std::map<std::string, std::vector<int>> indices = DDMinimizer::makeDataStructure(qc).second; 
    std::size_t bits = qc.getNqubits();

    std::cout << "Created Data structure_____________________________________________________________\n";
        // Print the contents of maps
    std::cout << "Contents of maps:\n";
    for (const auto& map : maps) {
        std::cout << map.first << ":\n";
        for (const auto& pair : map.second) {
            std::cout << "  (" << pair.first.first << ", " << pair.first.second << ") -> " << pair.second << "\n";
        }
    }

    // Print the contents of indices
    std::cout << "Contents of indices:\n";
    for (const auto& index : indices) {
        std::cout << index.first << ": ";
        for (const auto& val : index.second) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }


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
    std::cout << "Map after index marking_____________________________________________________________\n";
    for (const auto& map : maps) {
        std::cout << map.first << ":\n";
        for (const auto& pair : map.second) {
            std::cout << "  (" << pair.first.first << ", " << pair.first.second << ") -> " << pair.second << "\n";
        }
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
    std::cout << "Indices after filling_____________________________________________________________\n";
    for (const auto& index : indices) {
        std::cout << index.first << ": ";
        for (const auto& val : index.second) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Create Layout_____________________________________________________________\n";
    //create the permutation based on the order of max index in the complete maps
    std::vector<Qubit> layout(bits);
    std::iota(layout.begin(), layout.end(), 0);

    auto c_x = indices.find("c_x");
    auto x_c = indices.find("x_c");
    auto c_l = indices.find("c_l");
    auto x_l = indices.find("x_l");
    auto c_r = indices.find("c_r");
    auto x_r = indices.find("x_r");
    std::cout << "c_x->second[0]: " << c_x->second[0] << "\n";
    std::cout << "x_c->second[0]: " << x_c->second[0] << "\n";


    int prio_c_r = DDMinimizer::getLadderPosition(c_r->second, x_c->second[0]);
    int prio_x_l = DDMinimizer::getLadderPosition(x_l->second, x_c->second[0]);
    int stairs_c_r = DDMinimizer::getStairCount(c_r->second);
    int stairs_x_l = DDMinimizer::getStairCount(x_l->second);
    int prio_c_l = DDMinimizer::getLadderPosition(c_l->second, c_x->second[0]);
    int prio_x_r = DDMinimizer::getLadderPosition(x_r->second, c_x->second[0]);
    int stairs_c_l = DDMinimizer::getStairCount(c_l->second);
    int stairs_x_r = DDMinimizer::getStairCount(x_r->second);


    if(c_x->second[0] < x_c->second[0]){
        std::cout << "Case c_x < x_c\n";
        std::cout << "prio_c_r: " << prio_c_r << "\n";
        std::cout << "prio_x_l: " << prio_x_l << "\n";
        std::cout << "stairs_c_r: " << stairs_c_r << "\n";
        std::cout << "stairs_x_l: " << stairs_x_l << "\n";


        if(prio_c_r > 0 || prio_x_l > 0) {
            std::cout << "Case prio_c_r > 0 || prio_x_l > 0 -> reverse Layout\n";
            layout = DDMinimizer::reverseLayout(layout);
        }
        else if (prio_c_r == 0 && stairs_c_r > 0){
            std::cout << "Case prio_c_r == 0 && stairs_c_r > 0 -> Rotate Right stairs_c_r times\n";
            for(int i = 0; i < stairs_c_r; i++){
                layout = DDMinimizer::rotateRight(layout);
            }
        }
        else if (prio_x_l == 0 && stairs_x_l > 0){
            std::cout << "Case prio_x_l == 0 && stairs_x_l > 0 -> Rotate Left stairs_x_l times\n";
            for(int i = 0; i < stairs_x_l; i++){
                layout = DDMinimizer::rotateLeft(layout);
            }
        }
    }
    else if(c_x->second[0] > x_c->second[0]){
        std::cout << "Case c_x > x_c\n";
        std::cout << "prio_c_l: " << prio_c_l << "\n";
        std::cout << "prio_x_r: " << prio_x_r << "\n";
        std::cout << "stairs_c_l: " << stairs_c_l << "\n";
        std::cout << "stairs_x_r: " << stairs_x_r << "\n";


        if(prio_c_l > 0 || prio_x_r > 0) {
            std::cout << "Case prio_c_l > 0 || prio_x_r > 0 -> identity\n";
            //do nothing cause identity is what we need anyway
        }
        else if (prio_c_r == 0 && stairs_c_l > 0){
            std::cout << "Case prio_c_l == 0 && stairs_c_l > 0 -> reverse and rotate Right stairs_c_l times \n";
            layout = DDMinimizer::reverseLayout(layout);
            for(int i = 0; i < stairs_c_r; i++){
                layout = DDMinimizer::rotateRight(layout);
            }
        }
        else if (prio_x_l == 0 && stairs_x_r > 0){
            std::cout << "Case prio_x_r == 0 && stairs_x_r > 0 -> reverse and rotate Left stairts_x_r times \n";
            layout = DDMinimizer::reverseLayout(layout);
            for(int i = 0; i < stairs_x_l; i++){
                layout = DDMinimizer::rotateLeft(layout);
            }
        }
    }

    else {
        std::cout << "Case c_x = x_c\n";
        if(DDMinimizer::isFull(x_r->second) || DDMinimizer::isFull(c_l->second)){
            std::cout << "Case x_r and c_l are full -> reverse Layout\n";
            layout = DDMinimizer::reverseLayout(layout);
        }

        else {
           std::cout << "Case x_r and c_l are not full -> Control based return\n";
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
    std::cout << "Permutation: " << DDMinimizer::permToString(perm) << "\n";
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
        physicalQubits[i] = static_cast<unsigned int>(i);
        logicalQubits[i] = static_cast<unsigned int>(i);
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