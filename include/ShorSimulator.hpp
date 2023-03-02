#ifndef DDSIM_SHORSIMULATOR_HPP
#define DDSIM_SHORSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

template<class Config = dd::DDPackageConfig>
class ShorSimulator: public Simulator<Config> {
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

    void uA(std::uint64_t a, std::uint32_t n, std::int32_t c);

    void cmultInv(std::uint64_t a, std::uint32_t n, std::int32_t c);

    void cmult(std::uint64_t a, std::uint32_t n, std::int32_t c);

    void modAddPhiInv(std::uint64_t a, std::uint64_t n, std::int32_t c1, std::int32_t c2);

    void modAddPhi(std::uint64_t a, std::uint32_t n, std::int32_t c1, std::int32_t c2);

    void qftInv();

    void qft();

    void addPhiInv(std::uint64_t a, std::int32_t c1, std::int32_t c2);

    void addPhi(std::uint64_t a, std::int32_t c1, std::int32_t c2);

    static std::int32_t inverseMod(std::int32_t a, std::int32_t n);

    void uAEmulate(std::uint64_t a, std::int32_t q);

    void applyGate(dd::GateMatrix matrix, dd::Qubit target, const dd::Controls& controls);

    void applyGate(dd::GateMatrix matrix, dd::Qubit target, dd::Control control);

    void applyGate(dd::GateMatrix matrix, dd::Qubit target);

    std::vector<std::uint64_t> ts;

    dd::mEdge addConst(std::uint64_t a);

    dd::mEdge addConstMod(std::uint64_t a);

    dd::mEdge limitTo(std::uint64_t a);

    [[nodiscard]] std::pair<std::uint32_t, std::uint32_t> postProcessing(const std::string& sample) const;

    /// composite number to be factored
    std::size_t compositeN;
    /// coprime number to `coprimeN`. Setting this to zero will randomly generate a suitable number
    std::size_t coprimeA;
    std::size_t requiredBits;
    std::size_t nQubits{};

    std::string                             simResult = "did not start";
    std::pair<std::uint32_t, std::uint32_t> simFactors{0, 0};

    std::string                             polrResult = "did not start";
    std::pair<std::uint32_t, std::uint32_t> polrFactors{0, 0};

    bool          emulate;
    bool          verbose;
    bool          approximate;
    std::uint64_t approximationRuns{0};
    long double   finalFidelity{1.0L};
    double        stepFidelity{0.9};

    dd::mEdge limitStateVector(dd::vEdge e);

    std::map<dd::vNode*, dd::mEdge> dagEdges;

public:
    ShorSimulator(const std::size_t compositeNumber, const std::size_t coprimeNumber):
        Simulator<Config>(), compositeN(compositeNumber), coprimeA(coprimeNumber),
        requiredBits(static_cast<std::size_t>(std::ceil(std::log2(compositeNumber)))), emulate(true), verbose(false), approximate(false) {
        ts.resize(nQubits);
    };

    ShorSimulator(const std::size_t compositeNumber, const std::size_t coprimeNumber, const std::uint64_t seed_):
        Simulator<Config>(seed_), compositeN(compositeNumber), coprimeA(coprimeNumber),
        requiredBits(static_cast<std::size_t>(std::ceil(std::log2(compositeNumber)))), emulate(true), verbose(false), approximate(false) {
        ts.resize(nQubits);
    };

    ShorSimulator(const std::size_t compositeNumber, const std::size_t coprimeNumber, const bool emulate_, const bool verbose_, const bool approximate_):
        Simulator<Config>(), compositeN(compositeNumber), coprimeA(coprimeNumber),
        requiredBits(static_cast<std::size_t>(std::ceil(std::log2(compositeNumber)))), emulate(emulate_), verbose(verbose_),
        approximate(approximate_) {
        ts.resize(nQubits);
    };

    ShorSimulator(const std::size_t compositeNumber, const std::size_t coprimeNumber, const std::uint64_t seed_, const bool emulate_, const bool verbose_, const bool approximate_):
        Simulator<Config>(seed_),
        compositeN(compositeNumber), coprimeA(coprimeNumber),
        requiredBits(static_cast<std::size_t>(std::ceil(std::log2(compositeNumber)))), emulate(emulate_), verbose(verbose_),
        approximate(approximate_) {
        ts.resize(nQubits);
    };

    std::map<std::string, std::size_t> simulate(std::size_t shots) override;

    [[nodiscard]] std::string getName() const override {
        return "shor_" + std::to_string(compositeN) + "_" + std::to_string(coprimeA);
    }

    [[nodiscard]] std::size_t getNumberOfQubits() const override {
        return nQubits;
    }

    [[nodiscard]] std::size_t getNumberOfOps() const override {
        return 0;
    }

    std::pair<std::uint32_t, std::uint32_t> getFactors() {
        return simFactors;
    }

    std::map<std::string, std::string> additionalStatistics() override {
        return {
                {"composite_number", std::to_string(compositeN)},
                {"coprime_a", std::to_string(coprimeA)},
                {"emulation", (emulate ? "true" : "false")},
                {"sim_result", simResult},
                {"sim_factor1", std::to_string(simFactors.first)},
                {"sim_factor2", std::to_string(simFactors.second)},
                {"polr_result", polrResult},
                {"polr_factor1", std::to_string(polrFactors.first)},
                {"polr_factor2", std::to_string(polrFactors.second)},
                {"approximation_runs", std::to_string(approximationRuns)},
                {"step_fidelity", std::to_string(stepFidelity)},
                {"final_fidelity", std::to_string(finalFidelity)},
        };
    }
};

#endif //DDSIM_SHORSIMULATOR_HPP
