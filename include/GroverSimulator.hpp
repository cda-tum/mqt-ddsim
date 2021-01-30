#ifndef DDSIM_GROVERSIMULATOR_HPP
#define DDSIM_GROVERSIMULATOR_HPP

#include "Simulator.hpp"
#include "QuantumComputation.hpp"

#include <utility>
#include <cmath>

class GroverSimulator : public Simulator {
public:
    explicit GroverSimulator(const std::string &oracle, const unsigned long long seed) : Simulator(seed),
                                                                                         oracle{oracle.rbegin(), oracle.rend()},
                                                                                         n_qubits(oracle.length()),
                                                                                         iterations(CalculateIterations(n_qubits)) {
    }

    explicit GroverSimulator(const std::string &oracle) : oracle{oracle.rbegin(), oracle.rend()},
                                                          n_qubits(oracle.length()),
                                                          iterations(CalculateIterations(n_qubits)) {
    }

    explicit GroverSimulator(const unsigned short n_qubits, const unsigned long long seed) : Simulator(seed),
                                                                                             n_qubits{n_qubits},
                                                                                             iterations(CalculateIterations(n_qubits)) {
        std::uniform_int_distribution<int> dist(0, 1);
        oracle = std::string(n_qubits, '0');
        for (unsigned short i = 0; i < n_qubits; i++) {
            if (dist(mt) == 1) {
                oracle[i] = '1';
            }
        }
    }

    std::map<std::string, unsigned int> Simulate(unsigned int shots) override;
    std::map<std::string, double> StochSimulate() override {};

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"oracle",     std::string(oracle.rbegin(), oracle.rend())},
                {"iterations", std::to_string(iterations)},
        };
    }

    static unsigned long long CalculateIterations(const unsigned short n_qubits) {
        constexpr long double PI_4 = 0.785398163397448309615660845819875721049292349843776455243L; // qc::PI_4 is of type fp and hence possibly smaller than long double
        if (n_qubits <= 3) {
            return 1;
        } else {
            return (unsigned long long) std::floor(PI_4 * std::pow(2.L, n_qubits / 2.0L));
        }
    }


    unsigned short getNumberOfQubits() const override { return n_qubits + 1; };

    unsigned long getNumberOfOps() const override { return 0; };

    std::string getName() const override { return "emulated_grover_" + std::to_string(n_qubits); };

    std::string getOracle() const { return oracle; }

protected:
    std::string oracle; // due to how qubits and std::string are indexed, this is stored in *reversed* order
    const unsigned short n_qubits;
    const unsigned short n_anciallae = 1;
    const unsigned long long iterations;
};


#endif //DDSIM_GROVERSIMULATOR_HPP
