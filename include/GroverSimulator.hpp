#ifndef DDSIM_GROVERSIMULATOR_HPP
#define DDSIM_GROVERSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

#include <cmath>
#include <utility>

class GroverSimulator: public Simulator {
public:
    explicit GroverSimulator(const std::string& oracle, const unsigned long long seed):
        Simulator(seed),
        oracle{oracle.rbegin(),
               oracle.rend()},
        n_qubits(oracle.length()),
        iterations(CalculateIterations(
                n_qubits)) {
    }

    explicit GroverSimulator(const std::string& oracle):
        oracle{oracle.rbegin(), oracle.rend()},
        n_qubits(oracle.length()),
        iterations(CalculateIterations(n_qubits)) {
    }

    explicit GroverSimulator(const dd::QubitCount n_qubits, const unsigned long long seed):
        Simulator(seed),
        n_qubits{n_qubits},
        iterations(
                CalculateIterations(
                        n_qubits)) {
        std::uniform_int_distribution<int> dist(0, 1); // range is inclusive
        oracle = std::string(n_qubits, '0');
        for (dd::Qubit i = 0; i < n_qubits; i++) {
            if (dist(mt) == 1) {
                oracle[i] = '1';
            }
        }
    }

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"oracle", std::string(oracle.rbegin(), oracle.rend())},
                {"iterations", std::to_string(iterations)},
        };
    }

    static unsigned long long CalculateIterations(const unsigned short n_qubits) {
        constexpr long double PI_4 = 0.785398163397448309615660845819875721049292349843776455243L; // dd::PI_4 is of type fp and hence possibly smaller than long double
        if (n_qubits <= 3) {
            return 1;
        } else {
            return static_cast<unsigned long long>(std::floor(PI_4 * std::pow(2.L, n_qubits / 2.0L)));
        }
    }

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return n_qubits + 1; };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return 0; };

    [[nodiscard]] std::string getName() const override { return "emulated_grover_" + std::to_string(n_qubits); };

    [[nodiscard]] std::string getOracle() const { return oracle; }

protected:
    std::string              oracle; // due to how qubits and std::string are indexed, this is stored in *reversed* order
    const unsigned short     n_qubits;
    const unsigned short     n_anciallae = 1;
    const unsigned long long iterations;
};

#endif //DDSIM_GROVERSIMULATOR_HPP
