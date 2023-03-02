#ifndef DDSIM_GROVERSIMULATOR_HPP
#define DDSIM_GROVERSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

#include <cmath>
#include <utility>

template<class Config = dd::DDPackageConfig>
class GroverSimulator: public Simulator<Config> {
public:
    explicit GroverSimulator(const std::string& oracle_, const unsigned long long seed_):
        Simulator<Config>(seed_),
        oracle{oracle_.rbegin(), oracle_.rend()},
        n_qubits(static_cast<qc::Qubit>(oracle_.length())),
        iterations(CalculateIterations(n_qubits)) {
    }

    explicit GroverSimulator(const std::string& oracle_):
        oracle{oracle_.rbegin(), oracle_.rend()},
        n_qubits(static_cast<qc::Qubit>(oracle_.length())),
        iterations(CalculateIterations(n_qubits)) {
    }

    explicit GroverSimulator(const qc::Qubit n_qubits_, const unsigned long long seed_):
        Simulator<Config>(seed_),
        n_qubits{n_qubits_},
        iterations(CalculateIterations(n_qubits_)) {
        std::uniform_int_distribution<int> dist(0, 1); // range is inclusive
        oracle = std::string(n_qubits_, '0');
        for (qc::Qubit i = 0; i < n_qubits_; i++) {
            if (dist(Simulator<Config>::mt) == 1) {
                oracle[i] = '1';
            }
        }
    }

    std::map<std::string, std::size_t> simulate(std::size_t shots) override;

    std::map<std::string, std::string> additionalStatistics() override {
        return {
                {"oracle", std::string(oracle.rbegin(), oracle.rend())},
                {"iterations", std::to_string(iterations)},
        };
    }

    static unsigned long long CalculateIterations(const qc::Qubit n_qubits) {
        constexpr long double PI_4 = 0.785398163397448309615660845819875721049292349843776455243L; // dd::PI_4 is of type fp and hence possibly smaller than long double
        if (n_qubits <= 3) {
            return 1;
        } else {
            return static_cast<unsigned long long>(std::floor(PI_4 * std::pow(2.L, n_qubits / 2.0L)));
        }
    }

    [[nodiscard]] std::size_t getNumberOfQubits() const override { return n_qubits + 1; };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return 0; };

    [[nodiscard]] std::string getName() const override { return "emulated_grover_" + std::to_string(n_qubits); };

    [[nodiscard]] std::string getOracle() const { return {oracle.rbegin(), oracle.rend()}; }

protected:
    std::string       oracle; // due to how qubits and std::string are indexed, this is stored in *reversed* order
    const qc::Qubit   n_qubits;
    const qc::Qubit   n_anciallae = 1;
    const std::size_t iterations;
};

#endif //DDSIM_GROVERSIMULATOR_HPP
