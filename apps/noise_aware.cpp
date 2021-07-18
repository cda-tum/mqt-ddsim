#include "DeterministicNoiseSimulator.hpp"
#include "StochasticNoiseSimulator.hpp"
#include "nlohmann/json.hpp"

#include <boost/program_options.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

namespace nl = nlohmann;

int main(int argc, char** argv) {
    namespace po = boost::program_options;
    unsigned long long seed;

    po::options_description description("JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options");
    // clang-format off
    description.add_options()
        ("help,h", "produce help message")
        ("seed", po::value<unsigned long long>(&seed)->default_value(0), "seed for random number generator (default zero is possibly directly used as seed!)")
        ("pm", "print measurements")
        ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
        ("verbose", "Causes some simulators to print additional information to STDERR")

        ("simulate_file", po::value<std::string>(), "simulate a quantum circuit given by file (detection by the file extension)")
        ("step_fidelity", po::value<double>()->default_value(1.0), "target fidelity for each approximation run (>=1 = disable approximation)")
        ("steps", po::value<unsigned int>()->default_value(1), "number of approximation steps")

        ("noise_effects", po::value<std::string>()->default_value("APD"), "Noise effects (A (=amplitude damping),D (=depolarization),P (=phase flip)) in the form of a character string describing the noise effects (default=\"APD\")")
        ("noise_prob", po::value<double>()->default_value(0.001), "Probability for applying noise (default=0.001)")
        ("confidence", po::value<double>()->default_value(0.05), "Confidence in the error bound of the stochastic simulation (default= 0.05)")
        ("error_bound", po::value<double>()->default_value(0.1), "Error bound of the stochastic simulation (default=0.1)")
        ("stoch_runs", po::value<long>()->default_value(0), "Number of stochastic runs. When the value is 0 the value is calculated using the confidence, error_bound and number of tracked properties. When the value is -1, the deterministic simulator is started. When the value is -2 the old method for deterministic noise application is used) (default = 0)")
        ("properties", po::value<std::string>()->default_value("-3-1000"), R"(Comma separated list of tracked properties. Note that -1 is the fidelity and "-" can be used to specify a range.  (default="-3-1000"))")
    ;
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

    if (vm.count("simulate_file")) {
        const std::string fname = vm["simulate_file"].as<std::string>();
        quantumComputation      = std::make_unique<qc::QuantumComputation>(fname);
    } else {
        std::cerr << "Did not find anything to simulate. See help below.\n"
                  << description << "\n";
        std::exit(1);
    }

    if (quantumComputation && quantumComputation->getNqubits() > 100) {
        std::clog << "[WARNING] Quantum computation contains quite many qubits. You're jumping into the deep end.\n";
    }

    if (vm["stoch_runs"].as<long>() >= 0) {
        std::unique_ptr<StochasticNoiseSimulator> ddsim = std::make_unique<StochasticNoiseSimulator>(quantumComputation,
                                                                                                     vm["steps"].as<unsigned int>(),
                                                                                                     vm["step_fidelity"].as<double>(),
                                                                                                     seed);

        ddsim->setNoiseEffects(vm["noise_effects"].as<std::string>());
        ddsim->setAmplitudeDampingProbability(vm["noise_prob"].as<double>());
        ddsim->stoch_confidence = vm["confidence"].as<double>();
        ddsim->setRecordedProperties(vm["properties"].as<std::string>());
        ddsim->stoch_error_margin = vm["error_bound"].as<double>();
        ddsim->stochastic_runs    = vm["stoch_runs"].as<long>();

        auto t1 = std::chrono::steady_clock::now();

        const std::map<std::string, double> measurement_results = ddsim->StochSimulate();

        auto t2 = std::chrono::steady_clock::now();

        std::chrono::duration<float> duration_simulation = t2 - t1;

        nl::json output_obj;

        if (vm.count("ps")) {
            output_obj["statistics"] = {
                    {"simulation_time", duration_simulation.count()},
                    {"benchmark", ddsim->getName()},
                    {"stoch_runs", ddsim->stochastic_runs},
                    {"threads", ddsim->max_instances},
                    {"n_qubits", +ddsim->getNumberOfQubits()},
                    {"applied_gates", ddsim->getNumberOfOps()},
                    {"max_nodes", ddsim->getMaxNodeCount()},
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
    } else if (vm["stoch_runs"].as<long>() < 0) {
        std::unique_ptr<DeterministicNoiseSimulator> ddsim = std::make_unique<DeterministicNoiseSimulator>(quantumComputation, seed);

        if (vm["stoch_runs"].as<long>() == -2) {
            ddsim->noiseApplicationWithKrausMatrices = true;
        }

        ddsim->setNoiseEffects(vm["noise_effects"].as<std::string>());
        ddsim->setAmplitudeDampingProbability(vm["noise_prob"].as<double>());

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
                    {"max_nodes", ddsim->getMaxNodeCount()},
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
