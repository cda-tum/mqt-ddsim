#include "DDMinimizer.hpp"
#include "CircuitSimulator.hpp"
#include "CircuitOptimizer.hpp"
#include "QuantumComputation.hpp"
#include "operations/OpType.hpp"
#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <memory>
#include <algorithm>
#include <tuple>
#include <map>

using namespace qc;
using namespace std;

  TEST(ReorderWithoutReorderingTest, reorderByControls) {
    std::string fname = "BenchmarkQuasm/results/graphstate_controls.txt";
    std::ofstream out(fname);


    for(int k = 3; k < 7; k++){
      out << "Start Simulation of graphstate_indep_qiskit_" + std::to_string(k) + "__________________________________________________\n";
      
      //Read qasm string from file
      std::string qasmString;
      try {
          std::string fn = "BenchmarkQuasm/qasm/graphstate_indep_qiskit_" + std::to_string(k) + ".qasm";
          qasmString = DDMinimizer::readFileIntoString(fn);
      } catch (const std::exception& e) {
          std::cerr << e.what() << "\n";
      }

      //Create a Quantum Computation from the qasm string
      auto qc = QuantumComputation::fromQASM(qasmString);
      qc.print(out);
      

      //compute the control based permutation of the initial layout
      qc::Permutation perm = qc::DDMinimizer::createControlBasedPermutation(qc);
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

      std::string fname_qc = "BenchmarkQuasm/dot_control/graphstate_origin_" + std::to_string(k) + ".dot";
      std::ofstream out_qc(fname_qc);
      out_qc << graphvizString_qc;
      out_qc.close();
      std::string fname_qcc = "BenchmarkQuasm/dot_control/graphstate_permuted_" + std::to_string(k) + ".dot";
      std::ofstream out_qcc(fname_qcc);
      out_qcc << graphvizString_qcc;
      out_qcc.close();

      out << left << setfill(' ') << setw(30) << "control based Permutation: " << right << setfill('.') << setw(30) << qc::DDMinimizer::permToString(perm) << "\n";
      out << left << setfill(' ') << setw(30) << "Original Permutation: " << right << setfill('.') << setw(30) << qc::DDMinimizer::permToString(qc.initialLayout) << "\n";
      out << std::left << std::setfill(' ') << std::setw(30) << "Identity Duration: " << std::scientific << std::setprecision(4) << duration_qc.count() << "s" << std::right << std::setfill('.') << std::setw(30) << "\n";
      out << std::left << std::setfill(' ') << std::setw(30) << "Permuted Duration: " << std::scientific << std::setprecision(4) << duration_qcc.count() << "s" << std::right << std::setfill('.') << std::setw(30) << "\n";
      out << std::left << std::setfill(' ') << std::setw(30) << "Identity active_nodes: " << qc::DDMinimizer::formatSize_t(size_qc)  << std::right << std::setfill('.') << std::setw(39) << "\n";
      out << std::left << std::setfill(' ') << std::setw(30) << "Permuted active_nodes: " << qc::DDMinimizer::formatSize_t(size_qcc) << std::right << std::setfill('.') << std::setw(39) << "\n";
      out << std::left << std::setfill(' ') << std::setw(30) << "Identity max_nodes: " << qc::DDMinimizer::formatSize_t(size_max_qc)  << std::right << std::setfill('.') << std::setw(39) << "\n";
      out << std::left << std::setfill(' ') << std::setw(30) << "Permuted max_nodes: " << qc::DDMinimizer::formatSize_t(size_max_qcc) << std::right << std::setfill('.') << std::setw(39) << "\n";
      out << "________________________________________________________________________________________________\n\n";

      
  }

    out.close();
  }

  TEST(ReorderWithoutReorderingTest, measureAllPermutations) {
    std::string fname = "BenchmarkQuasm/results/graphstate_all.txt";
    std::ofstream out(fname);

    for(int k = 3; k < 5; k++){
      out << "Start Simulation of graphstate_indep_qiskit_" + std::to_string(k) + "__________________________________________________\n";
      
      //Read qasm string from file
      std::string qasmString;
      try {
          std::string fn = "BenchmarkQuasm/qasm/graphstate_indep_qiskit_" + std::to_string(k) + ".qasm";
          qasmString = DDMinimizer::readFileIntoString(fn);
      } catch (const std::exception& e) {
          std::cerr << e.what() << "\n";
      }

      //Create a Quantum Computation from the qasm string
      auto qc = QuantumComputation::fromQASM(qasmString);
      qc.print(out);
      

      //Save how many qubits the quantum computation has and compute how many permutations of the initial layout exist
      //Create all Permutations of initial layout and save them in the permutations vecotor
      std::size_t bits = qc.getNqubits();
      std::size_t permutation_count = qc::DDMinimizer::factorial(bits);
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
    
      std::chrono::duration<double> min_time(std::numeric_limits<double>::max());
      std::chrono::duration<double> max_time(std::numeric_limits<double>::lowest());
      std::size_t min_time_pos = 0;
      std::size_t max_time_pos = 0;
      std::chrono::duration<double> duration;

      std::map<size_t, std::vector<std::tuple<size_t, size_t, std::chrono::duration<double>>>> perm_by_active_nodes;
      std::map<size_t, std::vector<std::tuple<size_t, size_t, std::chrono::duration<double>>>> perm_by_max_nodes;
      std::map<std::chrono::duration<double>, std::vector<std::tuple<size_t, size_t, size_t>>> perm_by_time;
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

        //Save the time before and after the simulation and run the simulation once
        auto start = std::chrono::high_resolution_clock::now();
        ddsim.simulate(1); 
        auto end = std::chrono::high_resolution_clock::now();

        //collect measruements as in execution time, active nodes, max nodes
        //set min and max values and save positions
        duration = end - start;
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
        [](const std::tuple<size_t, size_t, std::chrono::duration<double>>& a, const std::tuple<size_t, size_t, std::chrono::duration<double>>& b) {
            // First compare by max_nodes
            if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);
            // If max_nodes are equal, then compare by execution time
            return std::get<2>(a) < std::get<2>(b);
        });
      }
      for (auto& entry : perm_by_max_nodes) {
        std::sort(entry.second.begin(), entry.second.end(), 
        [](const std::tuple<size_t, size_t, std::chrono::duration<double>>& a, const std::tuple<size_t, size_t, std::chrono::duration<double>>& b) {
            // First compare by max_nodes
            if (std::get<1>(a) != std::get<1>(b)) return std::get<1>(a) < std::get<1>(b);
            // If max_nodes are equal, then compare by execution time
            return std::get<2>(a) < std::get<2>(b);
        });
      }
      
      //Print statistics of min and max time, active nodes and max nodes with the corresponding permutation
      out << left << setfill(' ') << setw(20) << "Total Permutations: " << permutation_count <<  "\n";
      out << left << setfill(' ') << setw(20) << "Min time: " << std::scientific << std::setprecision(4) << min_time.count() << "s" << right << setfill('.') << setw(30) << " Permutation " << qc::DDMinimizer::formatSize_t(min_time_pos) << ": "<< qc::DDMinimizer::permToString(permutations[min_time_pos]) << "\n";
      out << left << setfill(' ') << setw(20) << "Max time: " << std::scientific << std::setprecision(4) << max_time.count() << "s" << right << setfill('.') << setw(30) << " Permutation " << qc::DDMinimizer::formatSize_t(max_time_pos) << ": "<< qc::DDMinimizer::permToString(permutations[max_time_pos]) << "\n";
      out << left << setfill(' ') << setw(20) << "Min active nodes: "<< qc::DDMinimizer::formatSize_t(min_active_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << qc::DDMinimizer::formatSize_t(min_active_nodes_pos) << ": "<< qc::DDMinimizer::permToString(permutations[min_active_nodes_pos]) << "\n";
      out << left << setfill(' ') << setw(20) << "Min max nodes: "<< qc::DDMinimizer::formatSize_t(min_max_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << qc::DDMinimizer::formatSize_t(min_max_nodes_pos) << ": "<< qc::DDMinimizer::permToString(permutations[min_max_nodes_pos]) << "\n";
      out << left << setfill(' ') << setw(20) << "Max active nodes: "<< qc::DDMinimizer::formatSize_t(max_active_nodes) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << qc::DDMinimizer::formatSize_t(max_active_nodes_pos) << ": "<< qc::DDMinimizer::permToString(permutations[max_active_nodes_pos]) << "\n";
      out << left << setfill(' ') << setw(20) << "Max max nodes: "<< qc::DDMinimizer::formatSize_t(max_active_nodes_pos) << " nodes" << right << setfill('.') << setw(33) << " Permutation " << qc::DDMinimizer::formatSize_t(max_max_nodes_pos) << ": "<< qc::DDMinimizer::permToString(permutations[max_max_nodes_pos]) << "\n";
      out << "________________________________________________________________________________________________\n\n";
      
      
      //write the graphvizStrings of the permutation with minimal and maximal active nodes into files
      std::string graphvizString = permutationToGraphViz[min_active_nodes_pos];
      std::string fnameMin = "BenchmarkQuasm/dot/graphstate_min_" + std::to_string(k) + ".dot";
      std::ofstream out1(fnameMin);
      out1 << graphvizString;
      out1.close();

      std::string graphvizString2 = permutationToGraphViz[max_active_nodes_pos];
      std::string fnameMax = "BenchmarkQuasm/dot/graphstate_max_" + std::to_string(k) + ".dot";
      std::ofstream out2(fnameMax);
      out2 << graphvizString2;
      out2.close();

      size_t index;
      size_t max_nodes = 0;
      size_t active_nodes = 0;
      std::chrono::duration<double> time;

      for (const auto& entry : perm_by_active_nodes) {
      //save the vecotr of all permutation indices (also simulation time and max_nodes) with the same active_nodes count 
      auto& vec = entry.second;
      out << vec.size() << " permutations have " << entry.first << " active nodes.\n";
        //iterate over all the permutations that have the same active_nodes/ are in the vector and print the max_nodes, simulation time and the permutation itself
        for (const auto& permutation : vec) {
            index =  std::get<0>(permutation);
            max_nodes=  std::get<1>(permutation);
            time =  std::get<2>(permutation);
            out << qc::DDMinimizer::measurementToString({true,false,true}, index, max_nodes, active_nodes, time);
            out << qc::DDMinimizer::permToString(permutations[index]) <<"\n";
        }
        out << "\n";
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
          out << qc::DDMinimizer::measurementToString({false,true,true}, index, max_nodes, active_nodes, time );;
          out << qc::DDMinimizer::permToString(permutations[index]) <<"\n";
      }
      out << "\n";
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
        out << qc::DDMinimizer::measurementToString({true,true,false}, index, max_nodes, active_nodes, time );
        out << qc::DDMinimizer::permToString(permutations[index]);
    }
      out << "\n";
    }
    out << "________________________________________________________________________________________________\n\n";
  }

  out.close();

}