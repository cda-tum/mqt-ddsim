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
            ("shots", po::value<unsigned int>()->default_value(0), "number of measurements (if the algorithm does not contain non-unitary gates, weak simulation is used)")
            ("display_vector", "display the state vector")
            ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
            ("verbose", "Causes some simulators to print additional information to STDERR")
            ("benchmark", "print simulation stats in a single CSV style line (overrides --ps and suppresses most other output, please don't rely on the format across versions)")

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
            ("noise_effects", po::value<std::string>(),
             "Noise effects (A (=amplitude damping),D (=depolarization),P (=phase flip)) in the form of a character string describing the noise effects (default=\" \")")
            ("noise_prob", po::value<double>(), "Probability for applying noise (default=0.001)")
            ("confidence", po::value<double>(),
             "Confidence in the error bound of the stochastic simulation (default= 0.05)")
            ("error_bound", po::value<double>(), "Error bound of the stochastic simulation (default=0.1)")
            ("stoch_runs", po::value<long>()->default_value(0), "Number of stochastic runs. When the value is 0 the value is calculated using the confidence, error_bound and number of tracked properties. (default = 0)")
            ("properties", po::value<std::string>(), R"(Comma separated list of tracked properties. Note that -1 is the fidelity and "-" can be used to specify a range.  (default="0-1000"))")
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
        ddsim = std::make_unique<QFRSimulator>(quantumComputation,
                                               vm["steps"].as<unsigned int>(), vm["step_fidelity"].as<double>(),
                                               seed);
    } else {
        std::cerr << "Did not find anything to simulate. See help below.\n"
                  << description << "\n";
        return 1;
    }

    if (quantumComputation && quantumComputation->getNqubits() > dd::MAXN) {
        std::cerr << "Quantum computation contains to many qubits (limit is set to " << dd::MAXN << "). See documentation for details.\n";
        std::exit(1);
    }
        if (vm.count("noise_effects")) {
            ddsim->setNoiseEffects((char *) &vm["noise_effects"].as<std::string>()[0]);
    } else {
            ddsim->setNoiseEffects((char *) &std::string("0")[0]);
    }
    if (vm.count("noise_prob")) {
        ddsim->setAmplitudeDampingProbability(vm["noise_prob"].as<double>());
    } else {
        ddsim->setAmplitudeDampingProbability(0.001);
    }

    if (vm.count("confidence")) {
        ddsim->stoch_confidence = vm["confidence"].as<double>();
    }

    if (vm.count("properties")) {
        ddsim->setRecordedProperties((char *) &vm["properties"].as<std::string>()[0]);
    } else {
        ddsim->setRecordedProperties("0-1000");
    }

    if (vm.count("error_bound")) {
        ddsim->stoch_error_margin = vm["error_bound"].as<double>();
    } else {
        ddsim->stoch_error_margin = 0.01;
    }
    ddsim->stochastic_runs = vm["stoch_runs"].as<long>();

    auto t1 = std::chrono::high_resolution_clock::now();
    ddsim->StochSimulate();
    auto t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration_simulation = t2-t1;

    if (vm.count("benchmark")) {
        auto more_info = ddsim->AdditionalStatistics();
        std::cout << ddsim->getName() << ", "
                  << ddsim->getNumberOfQubits() << ", "
                  //<< vm["approximate"].as<float>() << ", "
                  << std::fixed << duration_simulation.count() << std::defaultfloat << ", "
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

    if (vm.count("ps")) {
        std::cout << "  \"statistics\": {\n"
                  << "    \"simulation_time\": " << std::fixed << duration_simulation.count() << std::defaultfloat << ",\n"
                  << "    \"benchmark\": \"" << ddsim->getName() << "\",\n"
                  << "    \"shots\": " << vm["shots"].as<unsigned int>() << ",\n"
//                  << "    \"distinct_results\": " << m.size() << ",\n"
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
