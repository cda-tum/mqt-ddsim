#include "CircuitSimulator.hpp"
#include "GroverSimulator.hpp"
#include "HybridSchrodingerFeynmanSimulator.hpp"
#include "ShorFastSimulator.hpp"
#include "ShorSimulator.hpp"
#include "Simulator.hpp"
#include "algorithms/Entanglement.hpp"
#include "algorithms/Grover.hpp"
#include "algorithms/QFT.hpp"
#include "cxxopts.hpp"
#include "dd/Export.hpp"
#include "nlohmann/json.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace nl = nlohmann;

namespace std {
    template<class T>
    void to_json(nl::json& j, const std::complex<T>& p) { // NOLINT(readability-identifier-naming)
        j = nl::json{p.real(), p.imag()};
    }
    template<class T>
    void from_json(const nl::json& j, std::complex<T>& p) { // NOLINT(readability-identifier-naming)
        p.real(j.at(0));
        p.imag(j.at(1));
    }
} /* namespace std */

int main(int argc, char** argv) { // NOLINT(bugprone-exception-escape)
    cxxopts::Options options("MQT DDSIM", "for more information see https://www.cda.cit.tum.de/");
    // clang-format off
    options.add_options()
        ("h,help", "produce help message")
        ("seed", "seed for random number generator (default zero is possibly directly used as seed!)", cxxopts::value<std::uint64_t>()->default_value("0"))
        ("shots", "number of measurements (if the algorithm does not contain non-unitary gates, weak simulation is used)", cxxopts::value<unsigned int>()->default_value("0"))
        ("pv", "display the state vector")
        ("ps", "print simulation stats (applied gates, sim. time, and maximal size of the DD)")
        ("pm", "print measurement results")
        ("pcomplex", "print additional statistics on complex numbers")
        ("dump_complex", "dump edge weights in final state DD to file", cxxopts::value<std::string>())
        ("verbose", "Causes some simulators to print additional information to STDERR")
        ("simulate_file", "simulate a quantum circuit given by file (detection by the file extension)", cxxopts::value<std::string>())
        ("simulate_file_hybrid", "simulate a quantum circuit given by file (detection by the file extension) using the hybrid Schrodinger-Feynman simulator", cxxopts::value<std::string>())
        ("hybrid_mode", "mode used for hybrid Schrodinger-Feynman simulation (*amplitude*, dd)", cxxopts::value<std::string>())
        ("nthreads", "#threads used for hybrid simulation", cxxopts::value<unsigned int>()->default_value("2"))
        ("simulate_qft", "simulate Quantum Fourier Transform for given number of qubits", cxxopts::value<unsigned int>())
        ("simulate_ghz", "simulate state preparation of GHZ state for given number of qubits", cxxopts::value<unsigned int>())
        ("step_fidelity", "target fidelity for each approximation run (>=1 = disable approximation)", cxxopts::value<double>()->default_value("1.0"))
        ("steps", "number of approximation steps", cxxopts::value<unsigned int>()->default_value("1"))
        ("approx_when", "approximation strategy ('fidelity' (default) or 'memory'", cxxopts::value<std::string>()->default_value("fidelity"))
        ("approx_state", "do excessive approximation runs at the end of the simulation to see how the quantum state behaves")
        ("simulate_grover", "simulate Grover's search for given number of qubits with random oracle", cxxopts::value<unsigned int>())
        ("simulate_grover_emulated", "simulate Grover's search for given number of qubits with random oracle and emulation", cxxopts::value<unsigned int>())
        ("simulate_grover_oracle_emulated", "simulate Grover's search for given number of qubits with given oracle and emulation", cxxopts::value<std::string>())
        ("simulate_shor", "simulate Shor's algorithm factoring this number", cxxopts::value<unsigned int>())
        ("simulate_shor_coprime", "coprime number to use with Shor's algorithm (zero randomly generates a coprime)", cxxopts::value<unsigned int>()->default_value("0"))
        ("simulate_fast_shor", "simulate Shor's algorithm factoring this number with intermediate measurements", cxxopts::value<unsigned int>())
        ("simulate_fast_shor_coprime","coprime number to use with Shor's algorithm (zero randomly generates a coprime)", cxxopts::value<unsigned int>()->default_value("0"));
    // clang-format on

    auto vm = options.parse(argc, argv);
    if (vm.count("help") > 0) {
        std::cout << options.help();
        std::exit(0);
    }

    const auto seed         = vm["seed"].as<std::uint64_t>();
    const auto shots        = vm["shots"].as<unsigned int>();
    const auto nthreads     = vm["nthreads"].as<unsigned int>();
    const auto approxSteps  = vm["steps"].as<unsigned int>();
    const auto stepFidelity = vm["step_fidelity"].as<double>();

    auto mode = HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude;

    const auto strategy = ApproximationInfo::fromString(vm["approx_when"].as<std::string>());

    std::unique_ptr<qc::QuantumComputation>         quantumComputation;
    std::unique_ptr<Simulator<dd::DDPackageConfig>> ddsim{nullptr};
    const ApproximationInfo                         approximationInfo(stepFidelity, approxSteps, strategy);
    const bool                                      verbose = vm.count("verbose") > 0;

    if (vm.count("simulate_file") > 0) {
        const std::string fname = vm["simulate_file"].as<std::string>();
        quantumComputation      = std::make_unique<qc::QuantumComputation>(fname);
        ddsim                   = std::make_unique<CircuitSimulator<dd::DDPackageConfig>>(std::move(quantumComputation), approximationInfo, seed);
    } else if (vm.count("simulate_file_hybrid") > 0) {
        const std::string fname = vm["simulate_file_hybrid"].as<std::string>();
        quantumComputation      = std::make_unique<qc::QuantumComputation>(fname);
        if (vm.count("hybrid_mode") > 0) {
            const std::string mname = vm["hybrid_mode"].as<std::string>();
            if (mname == "amplitude") {
                mode = HybridSchrodingerFeynmanSimulator<dd::DDPackageConfig>::Mode::Amplitude;
            } else if (mname == "dd") {
                mode = HybridSchrodingerFeynmanSimulator<dd::DDPackageConfig>::Mode::DD;
            }
        }
        if (seed != 0) {
            ddsim = std::make_unique<HybridSchrodingerFeynmanSimulator<dd::DDPackageConfig>>(std::move(quantumComputation), approximationInfo, seed, mode, nthreads);
        } else {
            ddsim = std::make_unique<HybridSchrodingerFeynmanSimulator<dd::DDPackageConfig>>(std::move(quantumComputation), mode, nthreads);
        }
    } else if (vm.count("simulate_qft") > 0) {
        const unsigned int nQubits = vm["simulate_qft"].as<unsigned int>();
        quantumComputation         = std::make_unique<qc::QFT>(nQubits);
        ddsim                      = std::make_unique<CircuitSimulator<>>(std::move(quantumComputation), approximationInfo, seed);
    } else if (vm.count("simulate_fast_shor") > 0) {
        const unsigned int compositeNumber = vm["simulate_fast_shor"].as<unsigned int>();
        const unsigned int coprime         = vm["simulate_fast_shor_coprime"].as<unsigned int>();
        if (seed == 0) {
            ddsim = std::make_unique<ShorFastSimulator<dd::DDPackageConfig>>(compositeNumber, coprime, verbose);
        } else {
            ddsim = std::make_unique<ShorFastSimulator<dd::DDPackageConfig>>(compositeNumber, coprime, seed, verbose);
        }
    } else if (vm.count("simulate_shor") > 0) {
        const unsigned int compositeNumber = vm["simulate_shor"].as<unsigned int>();
        const unsigned int coprime         = vm["simulate_shor_coprime"].as<unsigned int>();
        if (seed == 0) {
            ddsim = std::make_unique<ShorSimulator<dd::DDPackageConfig>>(compositeNumber, coprime, verbose, stepFidelity < 1);
        } else {
            ddsim = std::make_unique<ShorSimulator<dd::DDPackageConfig>>(compositeNumber, coprime, seed, verbose, stepFidelity < 1);
        }
    } else if (vm.count("simulate_grover") > 0) {
        const unsigned int nQubits = vm["simulate_grover"].as<unsigned int>();
        quantumComputation         = std::make_unique<qc::Grover>(nQubits, seed);
        ddsim                      = std::make_unique<CircuitSimulator<>>(std::move(quantumComputation), approximationInfo, seed);
    } else if (vm.count("simulate_grover_emulated") > 0) {
        ddsim = std::make_unique<GroverSimulator<dd::DDPackageConfig>>(vm["simulate_grover_emulated"].as<unsigned int>(), seed);
    } else if (vm.count("simulate_grover_oracle_emulated") > 0) {
        ddsim = std::make_unique<GroverSimulator<dd::DDPackageConfig>>(vm["simulate_grover_oracle_emulated"].as<std::string>(), seed);
    } else if (vm.count("simulate_ghz") > 0) {
        const unsigned int nQubits = vm["simulate_ghz"].as<unsigned int>();
        quantumComputation         = std::make_unique<qc::Entanglement>(nQubits);
        ddsim                      = std::make_unique<CircuitSimulator<>>(std::move(quantumComputation), approximationInfo, seed);
    } else {
        std::cerr << "Did not find anything to simulate. See help below.\n"
                  << options.help() << "\n";
        std::exit(1);
    }

    if (ddsim->getNumberOfQubits() > 100) {
        std::clog << "[WARNING] Quantum computation contains quite a few qubits. You're jumping into the deep end.\n";
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto m  = ddsim->simulate(shots);
    auto t2 = std::chrono::high_resolution_clock::now();

    const std::chrono::duration<float> durationSimulation = t2 - t1;

    if (vm.count("approx_state") > 0) {
        // TargetFidelity
        ddsim->approximateByFidelity(1 / 100.0, false, false, true);
        ddsim->approximateByFidelity(2 / 100.0, false, false, true);
        ddsim->approximateByFidelity(3 / 100.0, false, false, true);
        ddsim->approximateByFidelity(4 / 100.0, false, false, true);
        for (int i = 6; i <= 95; i += 2) {
            ddsim->approximateByFidelity(i / 100.0, false, false, true);
        }
        ddsim->approximateByFidelity(96 / 100.0, false, false, true);
        ddsim->approximateByFidelity(97 / 100.0, false, false, true);
        ddsim->approximateByFidelity(98 / 100.0, false, false, true);
        ddsim->approximateByFidelity(99 / 100.0, false, false, true);
        ddsim->approximateByFidelity(100 / 100.0, false, false, true);

        // TargetFidelityPerLevel
        ddsim->approximateByFidelity(1 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(5 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(10 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(20 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(30 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(40 / 1000.0, true, false, true);
        for (int i = 50; i <= 950; i += 10) {
            ddsim->approximateByFidelity(i / 1000.0, true, false, true);
        }
        ddsim->approximateByFidelity(960 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(970 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(980 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(985 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(990 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(995 / 1000.0, true, false, true);
        ddsim->approximateByFidelity(1000 / 1000.0, true, false, true);

        // Traversal
        for (std::size_t i = 1; i < 10; i += 1) {
            ddsim->approximateBySampling(i, 0, false, true);
        }
        for (std::size_t i = 10; i < 100; i += 10) {
            ddsim->approximateBySampling(i, 0, false, true);
        }
        for (std::size_t i = 100; i < 1000; i += 100) {
            ddsim->approximateBySampling(i, 0, false, true);
        }
        for (std::size_t i = 1000; i < 100000; i += 1000) {
            ddsim->approximateBySampling(i, 0, false, true);
        }
        for (std::size_t i = 100000; i <= 1000000; i += 10000) {
            ddsim->approximateBySampling(i, 0, false, true);
        }

        // Traversal+Threshold
        for (std::size_t i = 1; i < 10; i += 1) {
            ddsim->approximateBySampling(1000000, i, false, true);
        }
        for (std::size_t i = 10; i < 100; i += 10) {
            ddsim->approximateBySampling(1000000, i, false, true);
        }

        for (std::size_t i = 100; i <= 5000; i += 100) {
            ddsim->approximateBySampling(1000000, i, false, true);
        }
    }

    nl::json outputObj;

    if (vm.count("pm") > 0) {
        outputObj["measurement_results"] = m;
    }

    if (vm.count("pv") > 0) {
        if (auto* hsfSim = dynamic_cast<HybridSchrodingerFeynmanSimulator<>*>(ddsim.get())) {
            outputObj["state_vector"] = hsfSim->getVectorFromHybridSimulation();
        } else {
            outputObj["state_vector"] = ddsim->getVector();
        }
    }

    if (vm.count("ps") > 0) {
        outputObj["statistics"] = {
                {"simulation_time", durationSimulation.count()},
                {"benchmark", ddsim->getName()},
                {"n_qubits", +ddsim->getNumberOfQubits()},
                {"applied_gates", ddsim->getNumberOfOps()},
                {"max_nodes", ddsim->getMaxNodeCount()},
                {"shots", shots},
                {"distinct_results", m.size()},
                {"seed", ddsim->getSeed()},
        };

        for (const auto& [stat, value]: ddsim->additionalStatistics()) {
            outputObj["statistics"][stat] = value;
        }
    }

    if (vm.count("pcomplex") > 0) {
        outputObj["complex_stats"]["cache_count"] = ddsim->dd->cn.cacheCount();
        outputObj["complex_stats"]["real_count"]  = ddsim->dd->cn.realCount();
    }

    if (vm.count("dump_complex") > 0) {
        auto filename = vm["dump_complex"].as<std::string>();
        auto ostream  = std::fstream(filename, std::fstream::out);
        dd::exportEdgeWeights(ddsim->rootEdge, ostream);
    }

    std::cout << std::setw(2) << outputObj << "\n";
}
