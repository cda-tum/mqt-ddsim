#include "CircuitSimulator.hpp"
#include "DeterministicNoiseSimulator.hpp"
#include "StochasticNoiseSimulator.hpp"
#include "ir/QuantumComputation.hpp"
#include "qasm3/Importer.hpp"

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cxxopts.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <utility>

namespace nl = nlohmann;

int main(int argc, char** argv) { // NOLINT(bugprone-exception-escape)
  cxxopts::Options options(
      "MQT DDSIM", "see for more information https://www.cda.cit.tum.de/");
  // clang-format off
    options.add_options()
        ("h,help", "produce help message")
        ("seed", "seed for random number generator (default zero is possibly directly used as seed!)", cxxopts::value<std::size_t>()->default_value("0"))
        ("pm", "print measurements")
        ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
        ("verbose", "Causes some simulators to print additional information to STDERR")
        ("simulate_file", "simulate a quantum circuit given by an OpenQASM file", cxxopts::value<std::string>())
        ("step_fidelity", "target fidelity for each approximation run (>=1 = disable approximation)", cxxopts::value<double>()->default_value("1.0"))
        ("steps", "number of approximation steps", cxxopts::value<unsigned int>()->default_value("1"))
        // Parameters for noise aware simulation
        ("noise_effects", "Noise effects (A (=amplitude damping),D (=depolarization),P (=phase flip)) in the form of a character string describing the noise effects", cxxopts::value<std::string>()->default_value("APD"))
        ("noise_prob", "Probability for applying noise.", cxxopts::value<double>()->default_value("0.001"))
        ("noise_prob_t1", "Probability for applying amplitude damping noise (default:2 x noise_prob)", cxxopts::value<double>())
        ("noise_prob_multi", "Noise factor for multi qubit operations", cxxopts::value<double>()->default_value("2"))
        ("use_density_matrix_simulator", "Set this flag to use the density matrix simulator. Per default the stochastic simulator is used")
        ("shots", "Specify the number of shots that shall be generated", cxxopts::value<std::size_t>()->default_value("0"))

    ; // end arguments list
  // clang-format on
  auto vm = options.parse(argc, argv);

  if (vm.count("help") > 0) {
    std::cout << options.help();
    return 0;
  }

  std::unique_ptr<qc::QuantumComputation> quantumComputation;

  if (vm.count("simulate_file") > 0) {
    const std::string fname = vm["simulate_file"].as<std::string>();
    quantumComputation = std::make_unique<qc::QuantumComputation>(
        qasm3::Importer::importf(fname));
  } else {
    std::cerr << "Did not find anything to simulate. See help below.\n"
              << options.help() << "\n";
    return 1;
  }

  if (quantumComputation && quantumComputation->getNqubits() > 100) {
    std::clog << "[WARNING] Quantum computation contains quite many qubits. "
                 "You're jumping into the deep end.\n";
  }

  std::optional<double> noiseProbT1{};
  if (vm.count("noise_prob_t1") > 0) {
    noiseProbT1 = vm["noise_prob_t1"].as<double>();
  }

  if (vm.count("use_density_matrix_simulator") == 0) {
    const auto approxSteps = vm["steps"].as<unsigned int>();
    const auto stepFidelity = vm["step_fidelity"].as<double>();
    const ApproximationInfo approxInfo{stepFidelity, approxSteps,
                                       ApproximationInfo::FidelityDriven};

    // Using stochastic simulator
    auto ddsim = std::make_unique<StochasticNoiseSimulator>(
        std::move(quantumComputation), approxInfo, vm["seed"].as<std::size_t>(),
        vm["noise_effects"].as<std::string>(), vm["noise_prob"].as<double>(),
        noiseProbT1, vm["noise_prob_multi"].as<double>());

    auto t1 = std::chrono::steady_clock::now();

    const auto measurementResults = ddsim->simulate(vm["shots"].as<size_t>());

    auto t2 = std::chrono::steady_clock::now();

    const std::chrono::duration<float> durationSimulation = t2 - t1;

    nl::basic_json outputObj;

    if (vm.count("ps") > 0) {
      outputObj["statistics"] = {
          {"simulation_time", durationSimulation.count()},
          {"benchmark", ddsim->getName()},
          {"n_qubits", +ddsim->getNumberOfQubits()},
          {"applied_gates", ddsim->getNumberOfOps()},
          {"seed", ddsim->getSeed()},
      };

      for (const auto& [key, value] : ddsim->additionalStatistics()) {
        outputObj["statistics"][key] = value;
      }
    }

    if (vm.count("pm") > 0) {
      outputObj["measurement_results"] = measurementResults;
    }

    std::cout << std::setw(2) << outputObj << "\n";

  } else if (vm.count("use_density_matrix_simulator") > 0) {
    // Using deterministic simulator
    auto ddsim = std::make_unique<DeterministicNoiseSimulator>(
        std::move(quantumComputation), ApproximationInfo{},
        vm["seed"].as<std::size_t>(), vm["noise_effects"].as<std::string>(),
        vm["noise_prob"].as<double>(), noiseProbT1,
        vm["noise_prob_multi"].as<double>());

    auto t1 = std::chrono::steady_clock::now();

    const auto measurementResults = ddsim->simulate(vm["shots"].as<size_t>());

    auto t2 = std::chrono::steady_clock::now();

    const std::chrono::duration<float> durationSimulation = t2 - t1;

    nl::basic_json outputObj;

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

      for (const auto& item : ddsim->additionalStatistics()) {
        outputObj["statistics"][item.first] = item.second;
      }
    }

    if (vm.count("pm") > 0) {
      outputObj["measurement_results"] = measurementResults;
    }
    std::cout << std::setw(2) << outputObj << "\n";
  }
}
