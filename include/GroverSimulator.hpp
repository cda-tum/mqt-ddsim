#ifndef DDSIM_GROVERSIMULATOR_HPP
#define DDSIM_GROVERSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

#include <cmath>
#include <utility>

template<class Config = dd::DDPackageConfig>
class GroverSimulator: public Simulator<Config> {
public:
    explicit GroverSimulator(const std::string& oracle_, const std::uint64_t seed_):
        Simulator<Config>(seed_),
        oracle{oracle_.rbegin(), oracle_.rend()},
        nQubits(static_cast<qc::Qubit>(oracle_.length())),
        iterations(calculateIterations(nQubits)) {
    }

    explicit GroverSimulator(const std::string& oracle_):
        oracle{oracle_.rbegin(), oracle_.rend()},
        nQubits(static_cast<qc::Qubit>(oracle_.length())),
        iterations(calculateIterations(nQubits)) {
    }

    explicit GroverSimulator(const qc::Qubit nQubits_, const std::uint64_t seed_):
        Simulator<Config>(seed_),
        nQubits{nQubits_},
        iterations(calculateIterations(nQubits_)) {
        // NOLINTNEXTLINE(misc-const-correctness): Using dist is not const
        std::uniform_int_distribution<int> dist(0, 1); // range is inclusive
        oracle = std::string(nQubits_, '0');
        for (qc::Qubit i = 0; i < nQubits_; i++) {
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

    static std::uint64_t calculateIterations(const qc::Qubit nQubits) {
        // NOLINTNEXTLINE(readability-identifier-naming)
        constexpr long double PI_4 = 0.785398163397448309615660845819875721049292349843776455243L; // dd::PI_4 is of type fp and hence possibly smaller than long double
        if (nQubits <= 3) {
            return 1;
        }
        return static_cast<std::uint64_t>(std::floor(PI_4 * std::pow(2.L, nQubits / 2.0L)));
    }

    [[nodiscard]] std::size_t getNumberOfQubits() const override { return nQubits + 1; };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return 0; };

    [[nodiscard]] std::string getName() const override { return "emulated_grover_" + std::to_string(nQubits); };

    [[nodiscard]] std::string getOracle() const { return {oracle.rbegin(), oracle.rend()}; }

protected:
    std::string oracle; // due to how qubits and std::string are indexed, this is stored in *reversed* order
    qc::Qubit   nQubits;
    qc::Qubit   nAnciallae = 1;
    std::size_t iterations;
};

#endif //DDSIM_GROVERSIMULATOR_HPP
