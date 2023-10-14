#include "DeterministicNoiseSimulator.hpp"
#include "StochasticNoiseSimulator.hpp"
#include "cxxopts.hpp"
#include "nlohmann/json.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

namespace nl = nlohmann;

int main(int argc, char** argv) { // NOLINT(bugprone-exception-escape)
    cxxopts::Options options("MQT DDSIM", "see for more information https://www.cda.cit.tum.de/");
    // clang-format off
    options.add_options()
        ("h,help", "produce help message")
        ("seed", "seed for random number generator (default zero is possibly directly used as seed!)", cxxopts::value<std::size_t>()->default_value("0"))
        ("pm", "print measurements")
        ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
        ("verbose", "Causes some simulators to print additional information to STDERR")

        ("simulate_file", "simulate a quantum circuit given by file (detection by the file extension)", cxxopts::value<std::string>())
        ("step_fidelity", "target fidelity for each approximation run (>=1 = disable approximation)", cxxopts::value<double>()->default_value("1.0"))
        ("steps", "number of approximation steps", cxxopts::value<unsigned int>()->default_value("1"))
        // Parameters for noise aware simulation
        ("noise_effects", "Noise effects (A (=amplitude damping),D (=depolarization),P (=phase flip)) in the form of a character string describing the noise effects", cxxopts::value<std::string>()->default_value("APD"))
        ("noise_prob", "Probability for applying noise.", cxxopts::value<double>()->default_value("0.001"))
        ("noise_prob_t1", "Probability for applying amplitude damping noise (default:2 x noise_prob)", cxxopts::value<double>())
        ("noise_prob_multi", "Noise factor for multi qubit operations", cxxopts::value<double>()->default_value("2"))
        ("unoptimized_sim", "Use unoptimized scheme for stochastic/deterministic noise-aware simulation")
        ("stoch_runs", "Number of stochastic runs. When the value is 0, the deterministic simulator is started. ", cxxopts::value<std::size_t>()->default_value("0"))
        ("properties", R"(Comma separated list of tracked amplitudes, when conducting a stochastic simulation. The "-" operator can be used to specify a range.)", cxxopts::value<std::string>()->default_value("0-100"))

    ; // end arguments list
    // clang-format on
    auto vm = options.parse(argc, argv);

    if (vm.count("help") > 0) {
        std::cout << options.help();
        std::exit(0);
    }

    std::unique_ptr<qc::QuantumComputation> quantumComputation;

    if (vm.count("simulate_file") > 0) {
        const std::string fname = vm["simulate_file"].as<std::string>();
        quantumComputation      = std::make_unique<qc::QuantumComputation>(fname);
    } else {
        std::cerr << "Did not find anything to simulate. See help below.\n"
                  << options.help() << "\n";
        std::exit(1);
    }

    if (quantumComputation && quantumComputation->getNqubits() > 100) {
        std::clog << "[WARNING] Quantum computation contains quite many qubits. You're jumping into the deep end.\n";
    }

    std::optional<double> noiseProbT1{};
    if (vm.count("noise_prob_t1") > 0) {
        noiseProbT1 = vm["noise_prob_t1"].as<double>();
    }

    if (vm["stoch_runs"].as<std::size_t>() > 0) {
        // Using stochastic simulator
        auto ddsim = std::make_unique<StochasticNoiseSimulator<>>(std::move(quantumComputation),
                                                                  vm["noise_effects"].as<std::string>(),
                                                                  vm["noise_prob"].as<double>(),
                                                                  noiseProbT1,
                                                                  vm["noise_prob_multi"].as<double>(),
                                                                  vm["stoch_runs"].as<size_t>(),
                                                                  vm["properties"].as<std::string>(),
                                                                  vm.count("unoptimized_sim"),
                                                                  vm["steps"].as<unsigned int>(),
                                                                  vm["step_fidelity"].as<double>(),
                                                                  vm["seed"].as<std::size_t>());

        auto t1 = std::chrono::steady_clock::now();

        const std::map<std::string, double> measurementResults = ddsim->stochSimulate();

        auto t2 = std::chrono::steady_clock::now();

        const std::chrono::duration<float> durationSimulation = t2 - t1;

        nl::json outputObj;

        if (vm.count("ps") > 0) {
            outputObj["statistics"] = {
                    {"simulation_time", durationSimulation.count()},
                    {"benchmark", ddsim->getName()},
                    {"n_qubits", +ddsim->getNumberOfQubits()},
                    {"applied_gates", ddsim->getNumberOfOps()},
                    {"max_nodes", ddsim->getMaxNodeCount()},
                    {"max_matrix_nodes", ddsim->getMaxMatrixNodeCount()},
                    {"seed", ddsim->getSeed()},
            };

            for (const auto& [key, value]: ddsim->additionalStatistics()) {
                outputObj["statistics"][key] = value;
            }
        }

        if (vm.count("pm") > 0) {
            outputObj["measurement_results"] = measurementResults;
        }

        std::cout << std::setw(2) << outputObj << std::endl;

    } else if (vm["stoch_runs"].as<std::size_t>() == 0) {
        // Using deterministic simulator
        auto ddsim = std::make_unique<DeterministicNoiseSimulator<>>(std::move(quantumComputation), vm["noise_effects"].as<std::string>(),
                                                                     vm["noise_prob"].as<double>(),
                                                                     noiseProbT1,
                                                                     vm["noise_prob_multi"].as<double>(),
                                                                     vm.count("unoptimized_sim"), vm["seed"].as<std::size_t>());

        auto t1 = std::chrono::steady_clock::now();

        const auto measurementResults = ddsim->deterministicSimulate();

        auto t2 = std::chrono::steady_clock::now();

        const std::chrono::duration<float> durationSimulation = t2 - t1;

        nl::json outputObj;

        if (vm.count("ps") > 0) {
            outputObj["statistics"] = {
                    {"simulation_time", durationSimulation.count()},
                    {"benchmark", ddsim->getName()},
                    {"n_qubits", ddsim->getNumberOfQubits()},
                    {"applied_gates", ddsim->getNumberOfOps()},
                    {"max_matrix_nodes", ddsim->getMaxMatrixNodeCount()},
                    {"active_matrix_nodes", ddsim->getMatrixActiveNodeCount()},
                    {"seed", ddsim->getSeed()},
                    {"active_nodes", ddsim->getActiveNodeCount()},
            };

            for (const auto& item: ddsim->additionalStatistics()) {
                outputObj["statistics"][item.first] = item.second;
            }
        }

        if (vm.count("pm") > 0) {
            outputObj["measurement_results"] = measurementResults;
        }
        std::cout << std::setw(2) << outputObj << "\n";
    }
}
