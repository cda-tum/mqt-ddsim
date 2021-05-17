#include "CircuitSimulator.hpp"
#include "GroverSimulator.hpp"
#include "ShorFastSimulator.hpp"
#include "ShorSimulator.hpp"
#include "Simulator.hpp"
#include "algorithms/Entanglement.hpp"
#include "algorithms/Grover.hpp"
#include "algorithms/QFT.hpp"
#include "nlohmann/json.hpp"

#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

namespace nl = nlohmann;

int main(int argc, char** argv) {
    namespace po = boost::program_options;
    // variables initialized by boost program_options default values
    unsigned long long seed;
    unsigned int       shots;

    unsigned int                         approx_steps;
    double                               step_fidelity;
    ApproximationInfo::ApproximationWhen approx_when;

    po::options_description description("JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options");
    // clang-format off
    description.add_options()
        ("help,h", "produce help message")
        ("seed", po::value<>(&seed)->default_value(0), "seed for random number generator (default zero is possibly directly used as seed!)")
        ("shots", po::value<>(&shots)->default_value(0), "number of measurements (if the algorithm does not contain non-unitary gates, weak simulation is used)")
        ("pv", "display the state vector")
        ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
        ("pm", "print measurement results")
        ("verbose", "Causes some simulators to print additional information to STDERR")
        ("simulate_file", po::value<std::string>(), "simulate a quantum circuit given by file (detection by the file extension)")
        ("simulate_qft", po::value<unsigned int>(), "simulate Quantum Fourier Transform for given number of qubits")
        ("simulate_ghz", po::value<unsigned int>(), "simulate state preparation of GHZ state for given number of qubits")
        ("step_fidelity", po::value<>(&step_fidelity)->default_value(1.0), "target fidelity for each approximation run (>=1 = disable approximation)")
        ("steps", po::value<>(&approx_steps)->default_value(1), "number of approximation steps")
        ("approx_when", po::value<>(&approx_when)->default_value(ApproximationInfo::FidelityDriven), "approximation method ('fidelity' (default) or 'memory'")
        ("approx_state", "do excessive approximation runs at the end of the simulation to see how the quantum state behaves")
        ("simulate_grover", po::value<unsigned int>(), "simulate Grover's search for given number of qubits with random oracle")
        ("simulate_grover_emulated", po::value<unsigned int>(), "simulate Grover's search for given number of qubits with random oracle and emulation")
        ("simulate_grover_oracle_emulated", po::value<std::string>(), "simulate Grover's search for given number of qubits with given oracle and emulation")
        ("simulate_shor", po::value<unsigned int>(), "simulate Shor's algorithm factoring this number")
        ("simulate_shor_coprime", po::value<unsigned int>()->default_value(0), "coprime number to use with Shor's algorithm (zero randomly generates a coprime)")
        ("simulate_shor_no_emulation", "Force Shor simulator to do modular exponentiation instead of using emulation (you'll usually want emulation)")
        ("simulate_fast_shor", po::value<unsigned int>(), "simulate Shor's algorithm factoring this number with intermediate measurements")
        ("simulate_fast_shor_coprime", po::value<unsigned int>()->default_value(0),"coprime number to use with Shor's algorithm (zero randomly generates a coprime)");
    // clang-format on
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
        ddsim                   = std::make_unique<CircuitSimulator>(std::move(quantumComputation), approx_info, seed);
    } else if (vm.count("simulate_qft")) {
        const unsigned int n_qubits = vm["simulate_qft"].as<unsigned int>();
        quantumComputation          = std::make_unique<qc::QFT>(n_qubits);
        ddsim                       = std::make_unique<CircuitSimulator>(std::move(quantumComputation), approx_info, seed);
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
        ddsim                       = std::make_unique<CircuitSimulator>(std::move(quantumComputation), approx_info, seed);
    } else if (vm.count("simulate_grover_emulated")) {
        ddsim = std::make_unique<GroverSimulator>(vm["simulate_grover_emulated"].as<unsigned int>(), seed);
    } else if (vm.count("simulate_grover_oracle_emulated")) {
        ddsim = std::make_unique<GroverSimulator>(vm["simulate_grover_oracle_emulated"].as<std::string>(), seed);
    } else if (vm.count("simulate_ghz")) {
        const unsigned int n_qubits = vm["simulate_ghz"].as<unsigned int>();
        quantumComputation          = std::make_unique<qc::Entanglement>(n_qubits);
        ddsim                       = std::make_unique<CircuitSimulator>(std::move(quantumComputation), approx_info, seed);
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

    nl::json output_obj;

    if (vm.count("pm")) {
        output_obj["measurement_results"] = m;
    }

    if (vm.count("pv")) {
        output_obj["state_vector"] = ddsim->getVectorPair();
    }

    if (vm.count("ps")) {
        output_obj["statistics"] = {
                {"simulation_time", duration_simulation.count()},
                {"benchmark", ddsim->getName()},
                {"n_qubits", +ddsim->getNumberOfQubits()},
                {"applied_gates", ddsim->getNumberOfOps()},
                {"max_nodes", ddsim->getMaxNodeCount()},
                {"shots", shots},
                {"distinct_results", m.size()},
                {"seed", ddsim->getSeed()},
        };

        for (const auto& item: ddsim->AdditionalStatistics()) {
            output_obj["statistics"][item.first] = item.second;
        }
    }

    std::cout << std::setw(2) << output_obj << std::endl;
}
