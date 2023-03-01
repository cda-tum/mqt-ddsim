#ifndef DDSIM_SHORFASTSIMULATOR_HPP
#define DDSIM_SHORFASTSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

template<class Config = dd::DDPackageConfig>
class ShorFastSimulator: public Simulator<Config> {
    static std::uint64_t modpow(std::uint64_t base, std::uint64_t exp, std::uint64_t modulus) {
        base %= modulus;
        std::uint64_t result = 1ull;
        while (exp > 0) {
            if (exp & 1ull) result = (result * base) % modulus;
            base = (base * base) % modulus;
            exp >>= 1ull;
        }
        return result;
    }

    static std::uint64_t gcd(std::uint64_t a, std::uint64_t b) {
        std::uint64_t c;
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

    void u_a_emulate2(std::uint64_t a);

    void u_a_emulate2_rec(dd::vEdge e);

    [[nodiscard]] std::pair<std::uint32_t, std::uint32_t> post_processing(const std::string& sample) const;

    void ApplyGate(dd::GateMatrix matrix, qc::Qubit target);

    std::vector<unsigned long long>              ts;
    std::vector<std::map<dd::vNode*, dd::vEdge>> nodesOnLevel;

    dd::mEdge addConst(std::uint64_t a);

    dd::mEdge addConstMod(std::uint64_t a);

    dd::mEdge limitTo(std::uint64_t a);

    /// composite number to be factored
    const std::uint32_t compositeN;
    /// coprime number to `compositeN`. Setting this to zero will randomly generate a suitable number
    std::uint32_t     coprimeA;
    const std::size_t requiredBits;
    const std::size_t nQubits;

    std::string                             simResult = "did not start";
    std::pair<std::uint32_t, std::uint32_t> simFactors{0, 0};

    std::size_t numberOfOperations{};

    const bool verbose;

    std::map<dd::vNode*, dd::vEdge> dagEdges;

public:
    ShorFastSimulator(const std::uint32_t compositeNumber, const std::uint32_t coprimeA_, const bool verbose = false):
        Simulator<Config>(), compositeN(compositeNumber), coprimeA(coprimeA_),
        requiredBits(static_cast<std::size_t>(std::ceil(std::log2(compositeNumber)))),
        nQubits(static_cast<std::size_t>(std::ceil(std::log2(compositeN))) + 1),
        verbose(verbose) {
        ts.resize(nQubits);
        nodesOnLevel.resize(nQubits);
    };

    ShorFastSimulator(const std::uint32_t compositeNumber, const std::uint32_t coprimeA_, const std::uint64_t seed, const bool verbose = false):
        Simulator<Config>(seed), compositeN(compositeNumber), coprimeA(coprimeA_),
        requiredBits(static_cast<std::size_t>(std::ceil(std::log2(compositeNumber)))),
        nQubits(static_cast<std::size_t>(std::ceil(std::log2(compositeN))) + 1),
        verbose(verbose) {
        ts.resize(nQubits);
        nodesOnLevel.resize(nQubits);
    };

    std::map<std::string, std::size_t> Simulate(std::size_t shots) override;

    [[nodiscard]] std::string getName() const override {
        return "fast_shor_" + std::to_string(compositeN) + "_" + std::to_string(coprimeA);
    }

    [[nodiscard]] std::size_t getNumberOfQubits() const override {
        return nQubits;
    }

    [[nodiscard]] std::size_t getNumberOfOps() const override {
        return numberOfOperations;
    }

    std::pair<std::uint32_t, std::uint32_t> getFactors() {
        return simFactors;
    }

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"composite_number", std::to_string(compositeN)},
                {"coprime_a", std::to_string(coprimeA)},
                {"sim_result", simResult},
                {"sim_factor1", std::to_string(simFactors.first)},
                {"sim_factor2", std::to_string(simFactors.second)}};
    }
};

#endif //DDSIM_SHORFASTSIMULATOR_HPP
