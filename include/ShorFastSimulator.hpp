#ifndef DDSIM_SHORFASTSIMULATOR_HPP
#define DDSIM_SHORFASTSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

template<class Config = dd::DDPackageConfig>
class ShorFastSimulator: public Simulator<Config> {
    static std::uint64_t modpow(std::uint64_t base, std::uint64_t exp, std::uint64_t modulus) {
        base %= modulus;
        std::uint64_t result = 1ULL;
        while (exp > 0) {
            if ((exp & 1ULL) > 0) {
                result = (result * base) % modulus;
            }
            base = (base * base) % modulus;
            exp >>= 1ULL;
        }
        return result;
    }

    static std::uint64_t gcd(std::uint64_t a, std::uint64_t b) {
        while (a != 0) {
            const std::uint64_t c = a;
            a                     = b % a;
            b                     = c;
        }
        return b;
    }

    static double cosine(double fac, double div) {
        return std::cos((dd::PI * fac) / div);
    }

    static double sine(double fac, double div) {
        return std::sin((dd::PI * fac) / div);
    }

    void uAEmulate2(std::uint64_t a);

    void uAEmulate2Rec(dd::vEdge e);

    [[nodiscard]] std::pair<std::uint32_t, std::uint32_t> postProcessing(const std::string& sample) const;

    void applyGate(dd::GateMatrix matrix, dd::Qubit target);

    std::vector<std::map<dd::vNode*, dd::vEdge>> nodesOnLevel;

    dd::mEdge addConst(std::uint64_t a);

    dd::mEdge addConstMod(std::uint64_t a);

    dd::mEdge limitTo(std::uint64_t a);

    /// composite number to be factored
    std::uint32_t compositeN;
    /// coprime number to `compositeN`. Setting this to zero will randomly generate a suitable number
    std::uint32_t coprimeA;
    std::size_t   requiredBits;
    std::size_t   nQubits;

    std::string                             simResult = "did not start";
    std::pair<std::uint32_t, std::uint32_t> simFactors{0, 0};

    std::size_t numberOfOperations{};

    bool verbose;

    std::map<dd::vNode*, dd::vEdge> dagEdges;

public:
    ShorFastSimulator(const std::uint32_t compositeNumber, const std::uint32_t coprimeA_, const bool verbose_ = false):
        Simulator<Config>(), compositeN(compositeNumber), coprimeA(coprimeA_),
        requiredBits(static_cast<std::size_t>(std::ceil(std::log2(compositeNumber)))),
        nQubits(static_cast<std::size_t>(std::ceil(std::log2(compositeN))) + 1),
        verbose(verbose_) {
        nodesOnLevel.resize(nQubits);
    };

    ShorFastSimulator(const std::uint32_t compositeNumber, const std::uint32_t coprimeA_, const std::uint64_t seed_, const bool verbose_ = false):
        Simulator<Config>(seed_), compositeN(compositeNumber), coprimeA(coprimeA_),
        requiredBits(static_cast<std::size_t>(std::ceil(std::log2(compositeNumber)))),
        nQubits(static_cast<std::size_t>(std::ceil(std::log2(compositeN))) + 1),
        verbose(verbose_) {
        nodesOnLevel.resize(nQubits);
    };

    std::map<std::string, std::size_t> simulate(std::size_t shots) override;

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

    std::map<std::string, std::string> additionalStatistics() override {
        return {
                {"composite_number", std::to_string(compositeN)},
                {"coprime_a", std::to_string(coprimeA)},
                {"sim_result", simResult},
                {"sim_factor1", std::to_string(simFactors.first)},
                {"sim_factor2", std::to_string(simFactors.second)}};
    }
};

#endif //DDSIM_SHORFASTSIMULATOR_HPP
