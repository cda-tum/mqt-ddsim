#include <iostream>
#include <memory>
#include <string>
#include <chrono>

#include <boost/program_options.hpp>
#include <algorithms/Grover.hpp>
#include <algorithms/QFT.hpp>
#include <algorithms/Entanglement.hpp>

#include "SimpleSimulator.hpp"

//void increment_numerical_string(std::string& s);

int main(int argc, char** argv) {
    namespace po = boost::program_options;
    po::options_description description("Allowed options");
    description.add_options()
            ("help", "produce help message")
            ("seed", po::value<unsigned long>()->default_value(0), "seed for random number generator")
            ("simulate_file", po::value<std::string>(), "simulate a quantum circuit given by file (detection by the file extension)")
            ("simulate_qft", po::value<unsigned int>(), "simulate Quantum Fourier Transform for given number of qubits")
            ("simulate_grover", po::value<unsigned int>(), "simulate Grover's search for given number of qubits with random oracle")
            ("simulate_ghz", po::value<unsigned int>(), "simulate state preparation of GHZ state for given number of qubits")
		    ("shots", po::value<unsigned int>()->default_value(1), "number of measurements on the final quantum state")
            ("display_vector", "display the state vector")
            ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
            ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << description << "\n";
        return 1;
    }

    std::unique_ptr<qc::QuantumComputation> quantumComputation;

    if (vm.count("simulate_file")) {
    	quantumComputation = std::make_unique<qc::QuantumComputation>();
        const std::string fname = vm["simulate_file"].as<std::string>();
        quantumComputation->import(fname);
    } else if (vm.count("simulate_qft")) {
	    const unsigned int n_qubits = vm["simulate_qft"].as<unsigned int>();
	    quantumComputation = std::make_unique<qc::QFT>(n_qubits);
    } else if (vm.count("simulate_grover")) {
        const unsigned int n_qubits = vm["simulate_grover"].as<unsigned int>();
	    quantumComputation = std::make_unique<qc::Grover>(n_qubits);
    } else if (vm.count("simulate_ghz")) {
	    const unsigned int n_qubits = vm["simulate_ghz"].as<unsigned int>();
	    quantumComputation = std::make_unique<qc::Entanglement>(n_qubits);
    } else {
        std::cout << description << "\n";
        return 1;
    }

    SimpleSimulator ddsim(quantumComputation, vm["seed"].as<unsigned long>());
    auto t1 = std::chrono::high_resolution_clock::now();
    ddsim.Simulate();
    auto t2 = std::chrono::high_resolution_clock::now();
    auto m = ddsim.MeasureAllNonCollapsing(vm["shots"].as<unsigned int>());
    auto t3 = std::chrono::high_resolution_clock::now();

    std::cout << "{\n";

    if (vm["shots"].as<unsigned int>() > 0) {
        std::cout << "  \"measurements\": {";
        bool first = true;
        for(const auto& elem : m)
        {
            if (first) {
                std::cout << "\n    \"" << elem.first << "\": " << elem.second;
                first = false;
            } else {
                std::cout << ",\n    \"" << elem.first << "\": " << elem.second;
            }
        }
        std::cout << "\n  },\n";
    }
    if (vm.count("display_vector")) {
        std::cout << "  \"state_vector\": [";

        bool first_element = true;
        unsigned long long non_zero_entries = 0;
        for(const auto& element : ddsim.getVector()) {
            if (element.r != 0 || element.i != 0) {
                non_zero_entries++;
            }
            if(first_element) {
                std::cout << "\n    " << element;
                first_element = false;
            } else {
                std::cout << ",\n    " << element;
            }
        }
        std::cout << "\n  ],\n";
        std::cout << "  \"non_zero_entries\": " << non_zero_entries << ",\n";
    }


    if (vm.count("ps")) {
        std::chrono::duration<float> duration_simulation = t2-t1;
        std::chrono::duration<float> duration_measurement = t3-t2;
        std::cout << "  \"statistics\": {\n";
        std::cout << "    \"simulation_time\": " << duration_simulation.count() << ",\n";
        std::cout << "    \"measurement_time\": " << duration_measurement.count() << ",\n";
        std::cout << "    \"benchmark\": \"" << ddsim.getName() << "\",\n";
        std::cout << "    \"shots\": " << vm["shots"].as<unsigned int>() << ",\n";
        std::cout << "    \"distinct_results\": " << m.size() << ",\n";
        std::cout << "    \"n_qubits\": " << ddsim.getNumberOfQubits() << ",\n";
        std::cout << "    \"applied_gates\": " << ddsim.getNumberOfOps() << ",\n";
        std::cout << "    \"max_nodes\": " << ddsim.getMaxNodeCount() << ",\n";
        std::cout << "    \"seed\": " << ddsim.getSeed() << "\n";
        std::cout << "  },\n";
    }

    std::cout << "  \"dummy\": 0\n}\n";
}
