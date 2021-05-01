#ifndef DDSIM_SHORSIMULATOR_HPP
#define DDSIM_SHORSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

class ShorSimulator: public Simulator {
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

    void u_a(unsigned long long a, int N, int c);

    void cmult_inv(int a, int N, int c);

    void cmult(int a, int N, int c);

    void mod_add_phi_inv(int a, int N, int c1, int c2);

    void mod_add_phi(int a, int N, int c1, int c2);

    void qft_inv();

    void qft();

    void add_phi_inv(int a, int c1, int c2);

    void add_phi(int a, int c1, int c2);

    static int inverse_mod(int a, int n);

    void u_a_emulate(unsigned long long a, int q);

    void ApplyGate(dd::GateMatrix matrix, dd::Qubit target, const dd::Controls& controls);

    void ApplyGate(dd::GateMatrix matrix, dd::Qubit target, dd::Control control);

    void ApplyGate(dd::GateMatrix matrix, dd::Qubit target);

    std::vector<unsigned long long> ts;

    dd::Package::mEdge addConst(unsigned long long a);

    dd::Package::mEdge addConstMod(unsigned long long a);

    dd::Package::mEdge limitTo(unsigned long long a);

    [[nodiscard]] std::pair<unsigned int, unsigned int> post_processing(const std::string& sample) const;

    /// composite number to be factored
    const unsigned int n;
    /// coprime number to `n`. Setting this to zero will randomly generate a suitable number
    unsigned int       coprime_a;
    const unsigned int required_bits;
    dd::QubitCount     n_qubits{};

    std::string                   sim_result = "did not start";
    std::pair<unsigned, unsigned> sim_factors{0, 0};

    std::string                   polr_result = "did not start";
    std::pair<unsigned, unsigned> polr_factors{0, 0};

    const bool         emulate;
    const bool         verbose;
    const bool         approximate;
    unsigned long long approximation_runs{0};
    long double        final_fidelity{1.0L};
    double             step_fidelity{0.9};

    dd::Package::mEdge limitStateVector(dd::Package::vEdge e);

    std::map<dd::Package::vNode*, dd::Package::mEdge> dag_edges;

public:
    ShorSimulator(int composite_number, int coprime_a):
        Simulator(), n(composite_number), coprime_a(coprime_a),
        required_bits(std::ceil(std::log2(composite_number))), emulate(true), verbose(false), approximate(false) {
        ts.resize(n_qubits);
    };

    ShorSimulator(int composite_number, int coprime_a, unsigned long long seed):
        Simulator(seed), n(composite_number), coprime_a(coprime_a),
        required_bits(std::ceil(std::log2(composite_number))), emulate(true), verbose(false), approximate(false) {
        ts.resize(n_qubits);
    };

    ShorSimulator(int composite_number, int coprime_a, bool emulate, bool verbose, bool approximate):
        Simulator(), n(composite_number), coprime_a(coprime_a),
        required_bits(std::ceil(std::log2(composite_number))), emulate(emulate), verbose(verbose),
        approximate(approximate) {
        ts.resize(n_qubits);
    };

    ShorSimulator(int composite_number, int coprime_a, unsigned long long seed, bool emulate, bool verbose,
                  bool approximate):
        Simulator(seed),
        n(composite_number), coprime_a(coprime_a),
        required_bits(std::ceil(std::log2(composite_number))), emulate(emulate), verbose(verbose),
        approximate(approximate) {
        ts.resize(n_qubits);
    };

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    [[nodiscard]] std::string getName() const override {
        return "shor_" + std::to_string(n) + "_" + std::to_string(coprime_a);
    }

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override {
        return n_qubits;
    }

    [[nodiscard]] std::size_t getNumberOfOps() const override {
        return 0;
    }

    std::pair<unsigned, unsigned> getFactors() {
        return sim_factors;
    }

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"composite_number", std::to_string(n)},
                {"coprime_a", std::to_string(coprime_a)},
                {"emulation", std::to_string(emulate)},
                {"sim_result", sim_result},
                {"sim_factor1", std::to_string(sim_factors.first)},
                {"sim_factor2", std::to_string(sim_factors.second)},
                {"polr_result", polr_result},
                {"polr_factor1", std::to_string(polr_factors.first)},
                {"polr_factor2", std::to_string(polr_factors.second)},
                {"approximation_runs", std::to_string(approximation_runs)},
                {"step_fidelity", std::to_string(step_fidelity)},
                {"final_fidelity", std::to_string(final_fidelity)},
        };
    }
};

#endif //DDSIM_SHORSIMULATOR_HPP
