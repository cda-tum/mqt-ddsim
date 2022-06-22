#include "DeterministicNoiseSimulator.hpp"
#include "StochasticNoiseSimulator.hpp"
#include "cxxopts.hpp"
#include "nlohmann/json.hpp"

#include <chrono>
#include <dd/Export.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

namespace nl = nlohmann;

int main(int argc, char** argv) {
    unsigned long long seed;

    cxxopts::Options options("MQT DDSIM", "see for more information https://www.cda.cit.tum.de/");
    // clang-format off
    options.add_options()
        ("h,help", "produce help message")
        ("seed", "seed for random number generator (default zero is possibly directly used as seed!)", cxxopts::value<unsigned long long>()->default_value("0"))
        ("pm", "print measurements")
        ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
        ("verbose", "Causes some simulators to print additional information to STDERR")

        ("simulate_file", "simulate a quantum circuit given by file (detection by the file extension)", cxxopts::value<std::string>())
        ("step_fidelity", "target fidelity for each approximation run (>=1 = disable approximation)", cxxopts::value<double>()->default_value("1.0"))
        ("steps", "number of approximation steps", cxxopts::value<unsigned int>()->default_value("1"))
        // Parameters for noise aware simulation
        ("noise_effects", "Noise effects (A (=amplitude damping),D (=depolarization),P (=phase flip)) in the form of a character string describing the noise effects (default=\"APD\")", cxxopts::value<std::string>()->default_value("APD"))
        ("noise_prob", "Probability for applying noise (default=0.001)", cxxopts::value<double>()->default_value("0.001"))
        ("noise_prob_t1", "Probability for applying amplitude damping noise (default=2 x noise_prob)", cxxopts::value<double>()->default_value("-1"))
        ("noise_prob_multi", "Noise factor for multi qubit operations (default=2)", cxxopts::value<double>()->default_value("2"))
        ("unoptimized_sim", "Use unoptimized scheme for stochastic/deterministic noise-aware simulation")
        ("stoch_runs", "Number of stochastic runs. When the value is 0, the deterministic simulator is started. (default = 0)", cxxopts::value<std::size_t>()->default_value("0"))
        ("properties", R"(Comma separated list of tracked amplitudes. The "-" operator can be used to specify a range.  (default="0-100"))", cxxopts::value<std::string>()->default_value("0-100"))

    ; // end arguments list
    // clang-format on
    auto vm = options.parse(argc, argv);

    if (vm.count("help")) {
        std::cout << options.help();
        std::exit(0);
    }

    std::unique_ptr<qc::QuantumComputation> quantumComputation;

    if (vm.count("simulate_file")) {
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

    if (vm["stoch_runs"].as<long>() > 0) {
        // Using stochastic simulator
        std::unique_ptr<StochasticNoiseSimulator> ddsim = std::make_unique<StochasticNoiseSimulator>(quantumComputation,
                                                                                                     vm["noise_effects"].as<std::string>(),
                                                                                                     vm["noise_prob"].as<double>() vm["step_fidelity"].as<double>(),
                                                                                                     vm["seed"].as<unsigned long long>());

        ddsim->stochasticRuns = vm["stoch_runs"].as<long>();

        auto t1 = std::chrono::steady_clock::now();

        const std::map<std::string, double> measurement_results = ddsim->StochSimulate();

        auto t2 = std::chrono::steady_clock::now();

        std::chrono::duration<float> duration_simulation = t2 - t1;

        nl::json output_obj;

        if (vm.count("ps")) {
            output_obj["statistics"] = {
                    {"simulation_time", duration_simulation.count()},
                    {"benchmark", ddsim->getName()},
                    {"stoch_runs", ddsim->stochasticRuns},
                    {"threads", ddsim->max_instances},
                    {"n_qubits", +ddsim->getNumberOfQubits()},
                    {"applied_gates", ddsim->getNumberOfOps()},
                    {"max_nodes", ddsim->getMaxNodeCount()},
                    {"max_matrix_nodes", ddsim->getMaxMatrixNodeCount()},
                    {"seed", ddsim->getSeed()},
            };

            for (const auto& item: ddsim->AdditionalStatistics()) {
                output_obj["statistics"][item.first] = item.second;
            }
        }

        if (vm.count("pm")) {
            output_obj["measurement_results"] = measurement_results;
        }

        std::cout << std::setw(2) << output_obj << std::endl;
    } else if (vm["stoch_runs"].as<long>() == 0) {
        // Using deterministic simulator
        std::unique_ptr<DeterministicNoiseSimulator> ddsim = std::make_unique<DeterministicNoiseSimulator>(quantumComputation, vm["noise_effects"].as<std::string>(),
                                                                                                           vm["noise_prob"].as<double>(),
                                                                                                           vm["noise_prob_t1"].as<double>(),
                                                                                                           vm["noise_prob_multi"].as<double>(),
                                                                                                           vm.count("unoptimized_sim"), seed);

        auto t1 = std::chrono::steady_clock::now();

        const std::map<std::string, double> measurement_results = ddsim->DeterministicSimulate();

        auto t2 = std::chrono::steady_clock::now();

        std::chrono::duration<float> duration_simulation = t2 - t1;

        nl::json output_obj;

        if (vm.count("ps")) {
            output_obj["statistics"] = {
                    {"simulation_time", duration_simulation.count()},
                    {"benchmark", ddsim->getName()},
                    {"n_qubits", +ddsim->getNumberOfQubits()},
                    {"applied_gates", ddsim->getNumberOfOps()},
                    {"max_matrix_nodes", ddsim->getMaxMatrixNodeCount()},
                    {"active_matrix_nodes", ddsim->getMatrixActiveNodeCount()},
                    {"seed", ddsim->getSeed()},
                    {"active_nodes", ddsim->getActiveNodeCount()},
            };

            for (const auto& item: ddsim->AdditionalStatistics()) {
                output_obj["statistics"][item.first] = item.second;
            }
        }

        if (vm.count("pm")) {
            output_obj["measurement_results"] = measurement_results;
        }
        std::cout << std::setw(2) << output_obj << std::endl;
    }
}
