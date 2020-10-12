#ifndef DDSIM_SHORSIMULATOR_HPP
#define DDSIM_SHORSIMULATOR_HPP

#include "Simulator.hpp"
#include "QuantumComputation.hpp"

class ShorSimulator : public Simulator {
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
        return std::cos((qc::PI * fac)/div);
    }

    static double QMDDsin(double fac, double div) {
        return std::sin((qc::PI * fac)/div);
    }

    void u_a(unsigned long long a, int N, int c);
    void cmult_inv(int a, int N, int c);
    void cmult(int a, int N, int c);
    void mod_add_phi_inv(int a, int N, int c1, int c2);
    void mod_add_phi(int a, int N, int c1, int c2);
    void qft_inv();
    void qft();
    void add_phi_inv(int n, int c1, int c2);
    void add_phi(int n, int c1, int c2);
    int inverse_mod(int a, int n);

    void u_a_emulate(unsigned long long a, int q);

    void ApplyGate(qc::GateMatrix matrix);

    unsigned long long ts[dd::MAXN]{};

    dd::Edge addConst(unsigned long long a);
    dd::Edge addConstMod(unsigned long long a);
    dd::Edge limitTo(unsigned long long a);


    std::array<short, qc::MAX_QUBITS> line{};

    /// composite number to be factored
    const unsigned int n;
    /// coprime number to `n`. Setting this to zero will randomly generate a suitable number
    unsigned int coprime_a;
    const unsigned int required_bits;
    unsigned int n_qubits{};
    unsigned int factor1{}, factor2{};

    std::string sim_result = "did not start";

    const bool emulate;
    const bool verbose;

    dd::Edge limitStateVector(dd::Edge e);
    std::map<dd::NodePtr , dd::Edge> dag_edges;

public:
    ShorSimulator(int composite_number, int coprime_a, bool emulate = true, bool verbose = false) :
    Simulator(), n(composite_number), coprime_a(coprime_a), required_bits(std::ceil(std::log2(composite_number))), emulate(emulate), verbose(verbose) {
        line.fill(qc::LINE_DEFAULT);
    };

    ShorSimulator(int composite_number, int coprime_a, unsigned long long seed, bool emulate = true, bool verbose = false) :
    Simulator(seed), n(composite_number), coprime_a(coprime_a), required_bits(std::ceil(std::log2(composite_number))), emulate(emulate), verbose(verbose) {
        line.fill(qc::LINE_DEFAULT);
    };

    virtual ~ShorSimulator();

    void Simulate() override;
    void Reset();

    std::string getName() const override {
        return "shor_"+std::to_string(n)+"_"+std::to_string(coprime_a);
    }

    unsigned short getNumberOfQubits() const override {
        return n_qubits;
    }
    unsigned long getNumberOfOps() const override {
        return 0;
    }

    std::pair<unsigned, unsigned> getFactors() {
        return {factor1, factor2};
    }

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"composite_number",std::to_string(n)},
                {"coprime_a", std::to_string(coprime_a)},
                {"emulation", std::to_string(emulate)},
                {"result", sim_result},
                {"factor1", std::to_string(factor1)},
                {"factor2", std::to_string(factor2)},
        };
    }

};


#endif //DDSIM_SHORSIMULATOR_HPP
