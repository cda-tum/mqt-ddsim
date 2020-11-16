#include <iostream>
#include <memory>
#include <string>
#include <chrono>

#include <boost/program_options.hpp>
#include <algorithms/Grover.hpp>
#include <algorithms/QFT.hpp>
#include <algorithms/Entanglement.hpp>


#include "Simulator.hpp"
#include "QFRSimulator.hpp"
#include "GroverSimulator.hpp"
#include "ShorFastSimulator.hpp"
#include "ShorSimulator.hpp"


int main(int argc, char** argv) {
    namespace po = boost::program_options;
    unsigned long long seed;

    po::options_description description("JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options");
    description.add_options()
            ("help,h", "produce help message")
            ("seed", po::value<unsigned long long>(&seed)->default_value(0), "seed for random number generator (default zero is possibly directly used as seed!)")
            ("shots", po::value<unsigned int>()->default_value(0), "number of measurements on the final quantum state")
            ("display_vector", "display the state vector")
            ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
            ("verbose", "Causes some simulators to print additional information to STDERR")
            ("benchmark", "print simulation stats in a single CSV style line (overrides --ps and suppresses most other output, please don't rely on the format)")

            ("simulate_file", po::value<std::string>(), "simulate a quantum circuit given by file (detection by the file extension)")
            ("simulate_qft", po::value<unsigned int>(), "simulate Quantum Fourier Transform for given number of qubits")
            ("simulate_ghz", po::value<unsigned int>(), "simulate state preparation of GHZ state for given number of qubits")
            ("step_fidelity", po::value<double>()->default_value(1.0), "target fidelity for each approximation run (>=1 = disable approximation)")
            ("steps", po::value<unsigned int>()->default_value(1), "number of approximation steps")

            ("simulate_grover", po::value<unsigned int>(), "simulate Grover's search for given number of qubits with random oracle")
            ("simulate_grover_emulated", po::value<unsigned int>(), "simulate Grover's search for given number of qubits with random oracle and emulation")
            ("simulate_grover_oracle_emulated", po::value<std::string>(), "simulate Grover's search for given number of qubits with given oracle and emulation")

            ("simulate_shor", po::value<unsigned int>(), "simulate Shor's algorithm factoring this number")
            ("simulate_shor_coprime", po::value<unsigned int>()->default_value(0), "coprime number to use with Shor's algorithm (zero randomly generates a coprime)")
            ("simulate_shor_no_emulation", "Force Shor simulator to do modular exponentiation instead of using emulation (you'll usually want emulation)")

            ("simulate_fast_shor", po::value<unsigned int>(), "simulate Shor's algorithm factoring this number with intermediate measurements")
            ("simulate_fast_shor_coprime", po::value<unsigned int>()->default_value(0), "coprime number to use with Shor's algorithm (zero randomly generates a coprime)")
            ;
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, description), vm);

        if (vm.count("help")) {
            std::cout << description;
            return 0;
        }
        po::notify(vm);
    } catch (const po::error &e) {
        std::cerr << "[ERROR] " << e.what() << "! Try option '--help' for available commandline options.\n";
        std::exit(1);
    }

    std::unique_ptr<qc::QuantumComputation> quantumComputation;
    std::unique_ptr<Simulator> ddsim{nullptr};

    if (vm.count("simulate_file")) {
        const std::string fname = vm["simulate_file"].as<std::string>();
    	quantumComputation = std::make_unique<qc::QuantumComputation>(fname);
        ddsim = std::make_unique<QFRSimulator>(quantumComputation, vm["steps"].as<unsigned int>(), vm["step_fidelity"].as<double>(), seed);
    } else if (vm.count("simulate_qft")) {
	    const unsigned int n_qubits = vm["simulate_qft"].as<unsigned int>();
	    quantumComputation = std::make_unique<qc::QFT>(n_qubits);
        ddsim = std::make_unique<QFRSimulator>(quantumComputation, vm["steps"].as<unsigned int>(), vm["step_fidelity"].as<double>(), seed);
    } else if (vm.count("simulate_fast_shor")) {
        const unsigned int composite_number = vm["simulate_fast_shor"].as<unsigned int>();
        const unsigned int coprime = vm["simulate_fast_shor_coprime"].as<unsigned int>();
        if (seed == 0) {
            ddsim = std::make_unique<ShorFastSimulator>(composite_number, coprime, vm.count("verbose") > 0);
        } else {
            ddsim = std::make_unique<ShorFastSimulator>(composite_number, coprime, seed, vm.count("verbose") > 0);
        }
    } else if (vm.count("simulate_shor")) {
        const unsigned int composite_number = vm["simulate_shor"].as<unsigned int>();
        const unsigned int coprime = vm["simulate_shor_coprime"].as<unsigned int>();
        if (seed == 0) {
            ddsim = std::make_unique<ShorSimulator>(composite_number, coprime, vm.count("verbose") > 0);
        } else {
            ddsim = std::make_unique<ShorSimulator>(composite_number, coprime, seed, vm.count("verbose") > 0);
        }
    } else if (vm.count("simulate_grover")) {
        const unsigned int n_qubits = vm["simulate_grover"].as<unsigned int>();
        quantumComputation = std::make_unique<qc::Grover>(n_qubits, seed);
        ddsim = std::make_unique<QFRSimulator>(quantumComputation, vm["steps"].as<unsigned int>(), vm["step_fidelity"].as<double>(), seed);
    } else if (vm.count("simulate_grover_emulated")) {
        ddsim = std::make_unique<GroverSimulator>(vm["simulate_grover_emulated"].as<unsigned int>(), seed);
    } else if (vm.count("simulate_grover_oracle_emulated")) {
        ddsim = std::make_unique<GroverSimulator>(vm["simulate_grover_oracle_emulated"].as<std::string>(), seed);
    } else if (vm.count("simulate_ghz")) {
	    const unsigned int n_qubits = vm["simulate_ghz"].as<unsigned int>();
	    quantumComputation = std::make_unique<qc::Entanglement>(n_qubits);
        ddsim = std::make_unique<QFRSimulator>(quantumComputation, vm["steps"].as<unsigned int>(), vm["step_fidelity"].as<double>(), seed);
    } else {
        std::cerr << "Did not find anything to simulate. See help below.\n"
                  << description << "\n";
        return 1;
    }

    if (quantumComputation && quantumComputation->getNqubits() > dd::MAXN) {
        std::cerr << "Quantum computation contains to many qubits (limit is set to " << dd::MAXN << "). See documentation for details.\n";
        std::exit(1);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    ddsim->Simulate();
    auto t2 = std::chrono::high_resolution_clock::now();
    auto m = ddsim->MeasureAllNonCollapsing(vm["shots"].as<unsigned int>());
    auto t3 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration_simulation = t2-t1;
    std::chrono::duration<float> duration_measurement = t3-t2;

    if (vm.count("benchmark")) {
        auto more_info = ddsim->AdditionalStatistics();
        std::cout << ddsim->getName() << ", "
                  << ddsim->getNumberOfQubits() << ", "
                  //<< vm["approximate"].as<float>() << ", "
                  << std::fixed << duration_simulation.count() << std::defaultfloat << ", "
                  << std::fixed << duration_measurement.count() << std::defaultfloat << ", "
                  //<< more_info["approximation_runs"] << ","
                  //<< more_info["final_fidelity"] << ", "
                  << more_info["coprime_a"] << ", "
                  << more_info["sim_result"] << ", "
                  << more_info["polr_result"] << ", "
                  << ddsim->getSeed() << ", "
                  << ddsim->getNumberOfOps() << ", "
                  << ddsim->getMaxNodeCount()
                  << "\n";
        return 0;
    }

    std::cout << "{\n";

    if (vm["shots"].as<unsigned int>() > 0) {
        std::cout << "  \"measurements\": {";
        bool first_element = true;
        for(const auto& element : m)
        {
            std::cout << (first_element ? "" : ",") << "\n    \"" << element.first << "\": " << element.second;
            first_element = false;
        }
        std::cout << "\n  },\n";
    }
    if (vm.count("display_vector")) {
        std::cout << "  \"state_vector\": [";

        bool first_element = true;
        unsigned long long non_zero_entries = 0;
        for(const auto& element : ddsim->getVector()) {
            if (element.r != 0 || element.i != 0) {
                non_zero_entries++;
                std::cout << (first_element ? "" : ",") << "\n    " << std::showpos << element.r << element.i << "i" << std::noshowpos;
            } else {
                std::cout << (first_element ? "" : ",") << "\n    0";
            }
            first_element = false;
        }
        std::cout << "\n  ],\n";
        std::cout << "  \"non_zero_entries\": " << non_zero_entries << ",\n";
    }

    if (vm.count("ps")) {
        std::cout << "  \"statistics\": {\n"
                  << "    \"simulation_time\": " << std::fixed << duration_simulation.count() << std::defaultfloat << ",\n"
                  << "    \"measurement_time\": " << std::fixed << duration_measurement.count() << std::defaultfloat << ",\n"
                  << "    \"benchmark\": \"" << ddsim->getName() << "\",\n"
                  << "    \"shots\": " << vm["shots"].as<unsigned int>() << ",\n"
                  << "    \"distinct_results\": " << m.size() << ",\n"
                  << "    \"n_qubits\": " << ddsim->getNumberOfQubits() << ",\n"
                  << "    \"applied_gates\": " << ddsim->getNumberOfOps() << ",\n"
                  << "    \"max_nodes\": " << ddsim->getMaxNodeCount() << ",\n"
                  ;
        for(const auto& item : ddsim->AdditionalStatistics()) {
            std::cout << "    \"" << item.first << "\": \"" << item.second << "\",\n";
        }
        std::cout << "    \"seed\": " << ddsim->getSeed() << "\n"
                  << "  },\n";
    }
    std::cout << "  \"dummy\": 0\n}\n"; // trailing element to make json printout easier
}
