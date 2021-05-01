#ifndef DDSIM_SHORFASTSIMULATOR_HPP
#define DDSIM_SHORFASTSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

class ShorFastSimulator: public Simulator {
    static unsigned long long modpow(unsigned long long base, unsigned long long exp, unsigned long long modulus) {
        base %= modulus;
        unsigned long long result = 1ull;
        while (exp > 0) {
            if (exp & 1ull) result = (result * base) % modulus;
            base = (base * base) % modulus;
            exp >>= 1ull;
        }
        return result;
    }

    static unsigned long long gcd(unsigned long long a, unsigned long long b) {
        unsigned long long c;
        while (a != 0) {
            c = a;
            a = b % a;
            b = c;
        }
        return b;
    }

    static double QMDDcos(double fac, double div) {
        return std::cos((dd::PI * fac) / div);
    }

    static double QMDDsin(double fac, double div) {
        return std::sin((dd::PI * fac) / div);
    }

    void u_a_emulate2(unsigned long long a);

    void u_a_emulate2_rec(dd::Package::vEdge e);

    [[nodiscard]] std::pair<unsigned int, unsigned int> post_processing(const std::string& sample) const;

    void ApplyGate(dd::GateMatrix matrix, dd::Qubit target);

    std::vector<unsigned long long>                                ts;
    std::vector<std::map<dd::Package::vNode*, dd::Package::vEdge>> nodesOnLevel;

    dd::Package::mEdge addConst(unsigned long long a);

    dd::Package::mEdge addConstMod(unsigned long long a);

    dd::Package::mEdge limitTo(unsigned long long a);

    /// composite number to be factored
    const unsigned int n;
    /// coprime number to `n`. Setting this to zero will randomly generate a suitable number
    unsigned int         coprime_a;
    const std::size_t    required_bits;
    const dd::QubitCount n_qubits;

    std::string                   sim_result = "did not start";
    std::pair<unsigned, unsigned> sim_factors{0, 0};

    unsigned long number_of_operations{};

    const bool verbose;

    std::map<dd::Package::vNode*, dd::Package::vEdge> dag_edges;

public:
    ShorFastSimulator(int composite_number, int coprime_a, bool verbose = false):
        Simulator(), n(composite_number), coprime_a(coprime_a),
        required_bits(std::ceil(std::log2(composite_number))), n_qubits(std::ceil(std::log2(n)) + 1),
        verbose(verbose) {
        ts.resize(n_qubits);
        nodesOnLevel.resize(n_qubits);
    };

    ShorFastSimulator(int composite_number, int coprime_a, unsigned long long seed, bool verbose = false):
        Simulator(seed), n(composite_number), coprime_a(coprime_a),
        required_bits(std::ceil(std::log2(composite_number))), n_qubits(std::ceil(std::log2(n)) + 1),
        verbose(verbose) {
        ts.resize(n_qubits);
        nodesOnLevel.resize(n_qubits);
    };

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    [[nodiscard]] std::string getName() const override {
        return "fast_shor_" + std::to_string(n) + "_" + std::to_string(coprime_a);
    }

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override {
        return n_qubits;
    }

    [[nodiscard]] std::size_t getNumberOfOps() const override {
        return number_of_operations;
    }

    std::pair<unsigned, unsigned> getFactors() {
        return sim_factors;
    }

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"composite_number", std::to_string(n)},
                {"coprime_a", std::to_string(coprime_a)},
                {"sim_result", sim_result},
                {"sim_factor1", std::to_string(sim_factors.first)},
                {"sim_factor2", std::to_string(sim_factors.second)}};
    }
};

#endif //DDSIM_SHORFASTSIMULATOR_HPP
