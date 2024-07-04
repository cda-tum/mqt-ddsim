#include "DDMinimizer.hpp"
#include "CircuitSimulator.hpp"
#include "CircuitOptimizer.hpp"
#include "QuantumComputation.hpp"
#include "operations/OpType.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <memory>
#include <algorithm>

using namespace qc;
using namespace std;

  TEST(ReorderWithoutReorderingTest, reorderByControls) {
    for(int k = 3; k < 7; k++){
       std::cout << "Start Simulation number " + std::to_string(k) + ".......................................................\n";
      //Read in the file into a string
      std::string qasmString;
      try {
          std::string fn = "BenchmarkQuasm/qasm/graphstate_indep_qiskit_" + std::to_string(k) + ".qasm";
          qasmString = DDMinimizer::readFileIntoString(fn);
          //std::cout << qasmString;
      } catch (const std::exception& e) {
          std::cerr << e.what() << "\n";
      }

      //Create a Quantum Computation from the qasm string
      auto qc = QuantumComputation::fromQASM(qasmString);
      std::cout << "Created Quantum Computation from File------------\n";
      qc.print(std::cout);
      qc::Permutation perm = qc::DDMinimizer::createControlBasedPermutation(qc);
      std::cout << "Created Permutation based on controls------------------\n";
      for (const auto& entryy : perm) {
              std::cout << entryy.first << " -> " << entryy.second << "|";
            }
            std::cout << "\n";
            std::cout.flush();

     
      QuantumComputation qcc = qc;
      qcc.initialLayout = perm; 
      qc::CircuitOptimizer::elidePermutations(qcc); 
      auto qcc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qcc));        
      CircuitSimulator ddsim_qcc(std::move(qcc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

      auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qcc));        
      CircuitSimulator ddsim_qc(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
      

      std::chrono::duration<double> duration_qc;
      std::chrono::duration<double> duration_qcc;
      std::size_t size_max_qc = 0;
      std::size_t size_max_qcc = 0;
      std::size_t size_qc = 0;
      std::size_t size_qcc = 0;

      auto start_qc = std::chrono::high_resolution_clock::now();
      ddsim_qc.simulate(1); 
      auto end_qc = std::chrono::high_resolution_clock::now();
      duration_qc = end_qc - start_qc;
      size_max_qc = ddsim_qc.getMaxNodeCount(); 
      size_qc = ddsim_qc.getActiveNodeCount();

      auto start_qcc = std::chrono::high_resolution_clock::now();
      ddsim_qcc.simulate(1); 
      auto end_qcc = std::chrono::high_resolution_clock::now();
      duration_qcc = end_qcc - start_qcc;
      size_max_qcc = ddsim_qcc.getMaxNodeCount();
      size_qcc = ddsim_qcc.getActiveNodeCount();


      std::string graphvizString_qc = ddsim_qc.exportDDtoGraphvizString(true, true, true, true, true);
      std::string graphvizString_qcc = ddsim_qcc.exportDDtoGraphvizString(true, true, true, true, true);

      std::string fname_qc = "BenchmarkQuasm/dot_control/graphstateorigin_" + std::to_string(k) + ".dot";
      std::ofstream out_qc(fname_qc);
      out_qc << graphvizString_qc;
      out_qc.close();
      std::string fname_qcc = "BenchmarkQuasm/dot_control/graphstate_permuted_" + std::to_string(k) + ".dot";
      std::ofstream out_qcc(fname_qcc);
      out_qcc << graphvizString_qcc;
      out_qcc.close();
      
      std::cout << "Identity Duration: " << std::fixed << std::setprecision(6) << duration_qc.count() << " seconds\n";
      std::cout << "Permuted Duration: " << std::fixed << std::setprecision(6) << duration_qcc.count() << " seconds\n";
      std::cout << "Identity Size: " << size_qc << " qubits\n";
      std::cout << "Permuted Size: " << size_qcc << " qubits\n";
      std::cout << "Identity max Size: " << size_max_qc << " qubits\n";
      std::cout << "Permuted max Size: " << size_max_qcc << " qubits\n";
      std::cout.flush();
  }

  }

  TEST(ReorderWithoutReorderingTest, measureAllPermutations) {
  for(int k = 3; k < 7; k++){
       std::cout << "Start Simulation number " + std::to_string(k) + ".......................................................\n";
      //Read in the file into a string
      std::string qasmString;
      try {
          std::string fn = "BenchmarkQuasm/qasm/graphstate_indep_qiskit_" + std::to_string(k) + ".qasm";
          qasmString = DDMinimizer::readFileIntoString(fn);
          //std::cout << qasmString;
      } catch (const std::exception& e) {
          std::cerr << e.what() << "\n";
      }

      //Create a Quantum Computation from the qasm string
      auto qc = QuantumComputation::fromQASM(qasmString);
      std::cout << "Created Quantum Computation from File------------\n";
      qc.print(std::cout);
      std::size_t bits = qc.getNqubits();
      std::cout << "Number of qubits:" << bits << "\n";
      std::size_t permutation_count = qc::DDMinimizer::factorial(bits);
      //Create all Permutations
      std::vector<qc::Permutation> permutations = DDMinimizer::createAllPermutations(qc);
      std::cout << "Created all Permutation layouts------------------\n";
      std::cout << "Number of permutations:" << permutation_count << "\n"; 
      /*Printing permutations
      for (size_t i = 0; i < permutation_count; i++) {
              const Permutation& tm = permutations[i];
              for (const auto& entry : tm) {
                std::cout << entry.first << " -> " << entry.second << "|";
              }
              std::cout << "\n"; 
      }
      */

      //prepare for simulation and benchmarking
      // We build the dd, execute the simulation and measure the execution time and dd size.

      std::vector<std::vector<Qubit>> controlOnTop(permutation_count);

      // save how big the dd were 
      //map of permutation index to pair of size and time
      std::map<size_t, std::vector<std::pair<size_t, std::chrono::duration<double>>>> permutationsBySize;
      std::map<size_t, std::string> permutationToGraphViz;
      //variables for size measurement
      std::size_t min_size = std::numeric_limits<std::size_t>::max();
      std::size_t max_size = 0;
      std::size_t min_size_pos = 0;
      std::size_t max_size_pos = 0;
      std::size_t current_size = 0;
      //variables for execution time measurement
      std::chrono::duration<double> min_time(std::numeric_limits<double>::max());
      std::chrono::duration<double> max_time(std::numeric_limits<double>::lowest());
      std::size_t min_time_pos = 0;
      std::size_t max_time_pos = 0;
      std::chrono::duration<double> duration;

      std::cout << "Starting simulation now, please be patient. This could take a while." << "\n"; 
      std::cout.flush();

      for (size_t i = 0; i < permutation_count; i++) {
        QuantumComputation qcc = qc;
        qcc.initialLayout = permutations[i]; 
        qc::CircuitOptimizer::elidePermutations(qcc); 
        auto qc_unique_ptr = std::make_unique<qc::QuantumComputation>(std::move(qcc));        
        CircuitSimulator ddsim(std::move(qc_unique_ptr), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));

        auto start = std::chrono::high_resolution_clock::now();
        ddsim.simulate(1); 
        auto end = std::chrono::high_resolution_clock::now();

        duration = end - start;
        if (duration < min_time) {
          min_time = duration;
          min_time_pos = i;
        }
        if (duration > max_time) {
            max_time = duration;
            max_time_pos = i;
        }

        current_size = ddsim.getMaxNodeCount(); 
        if (current_size < min_size) {
          min_size = current_size;
          min_size_pos = i;
        }

        if (current_size > max_size) {
          max_size = current_size;
          max_size_pos = i;
        }

        permutationsBySize[current_size].emplace_back(i, duration);

        // Printing the graphviz to file or commandline
        std::string graphvizString = ddsim.exportDDtoGraphvizString(true, true, true, true, true);
        /* print to command line: 
        std::cout << graphvizString << "\n";

        *print to file:
        std::string filename = "dd_reorder_without_reordering_" + std::to_string(i) + ".dot";
        std::ofstream out(filename);
        out << graphvizString;
        out.close();
        */

        //save strings to access later:
        permutationToGraphViz[i] = graphvizString;
      }

      //sort by execution time as well
      for (auto& entry : permutationsBySize) {
      // entry.second is the vector<std::pair<size_t, std::chrono::duration<double>>>
      std::sort(entry.second.begin(), entry.second.end(), 
          [](const std::pair<size_t, std::chrono::duration<double>>& a, const std::pair<size_t, std::chrono::duration<double>>& b) {
              return a.second < b.second; // Compare based on execution time
          });
      }
      
      //Print statistics
      std::cout << "Min time: " << min_time.count() << "s for permutation " << min_time_pos << " --------------------\n";
      const Permutation& tm = permutations[min_time_pos];
      for (const auto& entry : tm) {
        std::cout << entry.first << " -> " << entry.second << "|";
      }
      std::cout << "\n";

      std::cout << "Max time: " << max_time.count() << "s for permutation " << max_time_pos << " ---------------------\n";
      const Permutation& tm2 = permutations[max_time_pos];
      for (const auto& entry : tm2) {
        std::cout << entry.first << " -> " << entry.second << "|";
      }
      std::cout << "\n";

      std::cout << "Min node count: " << min_size << " nodes for permutation " << min_size_pos << " --------------------\n";
      const Permutation& tm3 = permutations[min_size_pos];
      for (const auto& entry : tm3) {
        std::cout << entry.first << " -> " << entry.second << "|";
      }
      std::cout << "\n";
      std::string graphvizString = permutationToGraphViz[min_size_pos];
      std::string fnameMin = "BenchmarkQuasm/dot/graphstate_min_" + std::to_string(k) + ".dot";
      std::ofstream out(fnameMin);
      out << graphvizString;
      out.close();
      
      std::cout << "Max node count: " << max_size << " nodes for permutation " << max_size_pos << " ---------------------\n";
      const Permutation& tm4 = permutations[max_size_pos];
      for (const auto& entry : tm4) {
        std::cout << entry.first << " -> " << entry.second << "|";
      }
      std::cout << "\n";
      std::string graphvizString2 = permutationToGraphViz[max_size_pos];
      std::string fnameMax = "BenchmarkQuasm/dot/graphstate_max_" + std::to_string(k) + ".dot";
      std::ofstream out2(fnameMax);
      out2 << graphvizString2;
      out2.close();

      for (const auto& entry : permutationsBySize) {
      size_t size = entry.first;
      const auto& perm = entry.second;
      size_t amount = perm.size();
      std::cout << "Size: " << size << " has " << amount << " permutations:\n";
        /*for (const auto& permutation : perm) {
            size_t index = permutation.first;
            auto time = permutation.second;
            std::cout << "Index: " << index << ": " << time.count() << ": ";
            //print the actual permutation
            const Permutation& tmm = permutations[index];
            for (const auto& entryy : tmm) {
              std::cout << entryy.first << " -> " << entryy.second << "|";
            }
            std::cout << "\n"; 
        }*/
        std::cout << "\n";
      }
    
    std::cout << "\n";
    std::cout.flush();
  }
}