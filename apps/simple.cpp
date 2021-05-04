#include "CircuitSimulator.hpp"
#include "GroverSimulator.hpp"
#include "HybridSchroedingerFeynmanSimulator.hpp"
#include "ShorFastSimulator.hpp"
#include "ShorSimulator.hpp"
#include "Simulator.hpp"

#include <algorithms/Entanglement.hpp>
#include <algorithms/Grover.hpp>
#include <algorithms/QFT.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv) {
    namespace po = boost::program_options;
    // variables initialized by boost program_options default values
    unsigned long long seed;
    unsigned int       shots;
    unsigned int                             nthreads;
    HybridSchroedingerFeynmanSimulator::Mode mode = HybridSchroedingerFeynmanSimulator::Mode::Amplitude;

    unsigned int                         approx_steps;
    double                               step_fidelity;
    ApproximationInfo::ApproximationWhen approx_when;

    po::options_description description("JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options");
    description.add_options()("help,h", "produce help message")("seed", po::value<>(&seed)->default_value(0),
                                                                "seed for random number generator (default zero is possibly directly used as seed!)")("shots", po::value<>(&shots)->default_value(0), "number of measurements (if the algorithm does not contain non-unitary gates, weak simulation is used)")("display_vector", "display the state vector")("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")("verbose", "Causes some simulators to print additional information to STDERR")("benchmark", "print simulation stats in a single CSV style line (overrides --ps and suppresses most other output, please don't rely on the format across versions)")("nthreads", po::value<>(&nthreads)->default_value(2), "#threads used for parallel execution")("simulate_file", po::value<std::string>(), "simulate a quantum circuit given by file (detection by the file extension)")("simulate_file_hybrid", po::value<std::string>(), "simulate a quantum circuit given by file (detection by the file extension) using the hybrid Schrodinger-Feynman simulator")("hybrid_mode", po::value<std::string>(), "mode used for hybrid Schrodinger-Feynman simulation (*amplitude*, dd)")("simulate_qft", po::value<unsigned int>(), "simulate Quantum Fourier Transform for given number of qubits")("simulate_ghz", po::value<unsigned int>(), "simulate state preparation of GHZ state for given number of qubits")("step_fidelity", po::value<>(&step_fidelity)->default_value(1.0), "target fidelity for each approximation run (>=1 = disable approximation)")("steps", po::value<>(&approx_steps)->default_value(1), "number of approximation steps")("approx_when", po::value<>(&approx_when)->default_value(ApproximationInfo::FidelityDriven), "approximation method ('fidelity' (default) or 'memory'")("approx_state", "do excessive approximation runs at the end of the simulation to see how the quantum state behaves")("simulate_grover", po::value<unsigned int>(), "simulate Grover's search for given number of qubits with random oracle")("simulate_grover_emulated", po::value<unsigned int>(), "simulate Grover's search for given number of qubits with random oracle and emulation")("simulate_grover_oracle_emulated", po::value<std::string>(), "simulate Grover's search for given number of qubits with given oracle and emulation")("simulate_shor", po::value<unsigned int>(), "simulate Shor's algorithm factoring this number")("simulate_shor_coprime", po::value<unsigned int>()->default_value(0), "coprime number to use with Shor's algorithm (zero randomly generates a coprime)")("simulate_shor_no_emulation", "Force Shor simulator to do modular exponentiation instead of using emulation (you'll usually want emulation)")("simulate_fast_shor", po::value<unsigned int>(), "simulate Shor's algorithm factoring this number with intermediate measurements")("simulate_fast_shor_coprime", po::value<unsigned int>()->default_value(0), "coprime number to use with Shor's algorithm (zero randomly generates a coprime)");
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, description), vm);

        if (vm.count("help")) {
            std::cout << description;
            return 0;
        }
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "[ERROR] " << e.what() << "! Try option '--help' for available commandline options.\n";
        std::exit(1);
    }

    std::unique_ptr<qc::QuantumComputation> quantumComputation;
    std::unique_ptr<Simulator>              ddsim{nullptr};
    ApproximationInfo                       approx_info(step_fidelity, approx_steps, approx_when);
    const bool                              verbose = vm.count("verbose") > 0;

    if (vm.count("simulate_file")) {
        const std::string fname = vm["simulate_file"].as<std::string>();
        quantumComputation      = std::make_unique<qc::QuantumComputation>(fname);
        ddsim                   = std::make_unique<CircuitSimulator>(quantumComputation, approx_info, seed);
    } else if (vm.count("simulate_file_hybrid")) {
        const std::string fname = vm["simulate_file_hybrid"].as<std::string>();
        if (vm.count("hybrid_mode")) {
            const std::string mname = vm["hybrid_mode"].as<std::string>();
            if (mname == "amplitude") {
                mode = HybridSchroedingerFeynmanSimulator::Mode::Amplitude;
            } else if (mname == "dd") {
                mode = HybridSchroedingerFeynmanSimulator::Mode::DD;
            }
        }
        quantumComputation = std::make_unique<qc::QuantumComputation>(fname);
        ddsim              = std::make_unique<HybridSchroedingerFeynmanSimulator>(quantumComputation, approx_info, seed, mode, nthreads);
    } else if (vm.count("simulate_qft")) {
        const unsigned int n_qubits = vm["simulate_qft"].as<unsigned int>();
        quantumComputation          = std::make_unique<qc::QFT>(n_qubits);
        ddsim                       = std::make_unique<CircuitSimulator>(quantumComputation, approx_info, seed);
    } else if (vm.count("simulate_fast_shor")) {
        const unsigned int composite_number = vm["simulate_fast_shor"].as<unsigned int>();
        const unsigned int coprime          = vm["simulate_fast_shor_coprime"].as<unsigned int>();
        if (seed == 0) {
            ddsim = std::make_unique<ShorFastSimulator>(composite_number, coprime, verbose);
        } else {
            ddsim = std::make_unique<ShorFastSimulator>(composite_number, coprime, seed, verbose);
        }
    } else if (vm.count("simulate_shor")) {
        const unsigned int composite_number = vm["simulate_shor"].as<unsigned int>();
        const unsigned int coprime          = vm["simulate_shor_coprime"].as<unsigned int>();
        const bool         emulate          = vm.count("simulate_shor_no_emulation") == 0;
        if (seed == 0) {
            ddsim = std::make_unique<ShorSimulator>(composite_number, coprime, emulate, verbose, step_fidelity < 1);
        } else {
            ddsim = std::make_unique<ShorSimulator>(composite_number, coprime, seed, emulate, verbose,
                                                    step_fidelity < 1);
        }
    } else if (vm.count("simulate_grover")) {
        const unsigned int n_qubits = vm["simulate_grover"].as<unsigned int>();
        quantumComputation          = std::make_unique<qc::Grover>(n_qubits, seed);
        ddsim                       = std::make_unique<CircuitSimulator>(quantumComputation, approx_info, seed);
    } else if (vm.count("simulate_grover_emulated")) {
        ddsim = std::make_unique<GroverSimulator>(vm["simulate_grover_emulated"].as<unsigned int>(), seed);
    } else if (vm.count("simulate_grover_oracle_emulated")) {
        ddsim = std::make_unique<GroverSimulator>(vm["simulate_grover_oracle_emulated"].as<std::string>(), seed);
    } else if (vm.count("simulate_ghz")) {
        const unsigned int n_qubits = vm["simulate_ghz"].as<unsigned int>();
        quantumComputation          = std::make_unique<qc::Entanglement>(n_qubits);
        ddsim                       = std::make_unique<CircuitSimulator>(quantumComputation, approx_info, seed);
    } else {
        std::cerr << "Did not find anything to simulate. See help below.\n"
                  << description << "\n";
        std::exit(1);
    }

    if (quantumComputation && quantumComputation->getNqubits() > 100) {
        std::clog << "[WARNING] Quantum computation contains quite a qubits. You're jumping into the deep end.\n";
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto m  = ddsim->Simulate(shots);
    auto t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration_simulation = t2 - t1;

    if (vm.count("approx_state")) {
        // TargetFidelity
        ddsim->ApproximateByFidelity(1 / 100.0, false, false, true);
        ddsim->ApproximateByFidelity(2 / 100.0, false, false, true);
        ddsim->ApproximateByFidelity(3 / 100.0, false, false, true);
        ddsim->ApproximateByFidelity(4 / 100.0, false, false, true);
        for (int i = 6; i <= 95; i += 2) {
            ddsim->ApproximateByFidelity(i / 100.0, false, false, true);
        }
        ddsim->ApproximateByFidelity(96 / 100.0, false, false, true);
        ddsim->ApproximateByFidelity(97 / 100.0, false, false, true);
        ddsim->ApproximateByFidelity(98 / 100.0, false, false, true);
        ddsim->ApproximateByFidelity(99 / 100.0, false, false, true);
        ddsim->ApproximateByFidelity(100 / 100.0, false, false, true);

        // TargetFidelityPerLevel
        ddsim->ApproximateByFidelity(1 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(5 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(10 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(20 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(30 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(40 / 1000.0, true, false, true);
        for (int i = 50; i <= 950; i += 10) {
            ddsim->ApproximateByFidelity(i / 1000.0, true, false, true);
        }
        ddsim->ApproximateByFidelity(960 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(970 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(980 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(985 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(990 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(995 / 1000.0, true, false, true);
        ddsim->ApproximateByFidelity(1000 / 1000.0, true, false, true);

        // Traversal
        for (int i = 1; i < 10; i += 1) {
            ddsim->ApproximateBySampling(i, 0, false, true);
        }
        for (int i = 10; i < 100; i += 10) {
            ddsim->ApproximateBySampling(i, 0, false, true);
        }
        for (int i = 100; i < 1000; i += 100) {
            ddsim->ApproximateBySampling(i, 0, false, true);
        }
        for (int i = 1000; i < 100000; i += 1000) {
            ddsim->ApproximateBySampling(i, 0, false, true);
        }
        for (int i = 100000; i <= 1000000; i += 10000) {
            ddsim->ApproximateBySampling(i, 0, false, true);
        }

        // Traversal+Threshold
        for (int i = 1; i < 10; i += 1) {
            ddsim->ApproximateBySampling(1000000, i, false, true);
        }
        for (int i = 10; i < 100; i += 10) {
            ddsim->ApproximateBySampling(1000000, i, false, true);
        }

        for (int i = 100; i <= 5000; i += 100) {
            ddsim->ApproximateBySampling(1000000, i, false, true);
        }
    }

    if (vm.count("benchmark")) {
        auto more_info = ddsim->AdditionalStatistics();
        std::cout << ddsim->getName() << ", "
                  << ddsim->getNumberOfQubits() << ", "
                  << std::fixed << duration_simulation.count() << std::defaultfloat << ", "
                  << more_info["single_shots"] << ","
                  << more_info["approximation_runs"] << ","
                  << more_info["final_fidelity"] << ", "
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

    if (!m.empty()) {
        std::cout << "  \"measurements\": {";
        bool first_element = true;
        for (const auto& element: m) {
            std::cout << (first_element ? "" : ",") << "\n    \"" << element.first << "\": " << element.second;
            first_element = false;
        }
        std::cout << "\n  },\n";
    }
    if (vm.count("display_vector")) {
        std::cout << "  \"state_vector\": [";

        bool               first_element    = true;
        unsigned long long non_zero_entries = 0;
        for (const auto& element: ddsim->getVector()) {
            if (element.r != 0 || element.i != 0) {
                non_zero_entries++;
                std::cout << (first_element ? "" : ",") << "\n    " << std::showpos << element.r << element.i << "i"
                          << std::noshowpos;
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
                  << "    \"simulation_time\": " << std::fixed << duration_simulation.count() << std::defaultfloat
                  << ",\n"
                  << "    \"benchmark\": \"" << ddsim->getName() << "\",\n"
                  << "    \"shots\": " << shots << ",\n"
                  << "    \"distinct_results\": " << m.size() << ",\n"
                  << "    \"n_qubits\": " << ddsim->getNumberOfQubits() << ",\n"
                  << "    \"applied_gates\": " << ddsim->getNumberOfOps() << ",\n"
                  << "    \"max_nodes\": " << ddsim->getMaxNodeCount() << ",\n";
        for (const auto& item: ddsim->AdditionalStatistics()) {
            std::cout << "    \"" << item.first << "\": \"" << item.second << "\",\n";
        }
        std::cout << "    \"seed\": " << ddsim->getSeed() << "\n"
                  << "  },\n";
    }
    std::cout << "  \"dummy\": 0\n}\n"; // trailing element to make json printout easier
}
