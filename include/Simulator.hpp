#ifndef DDSIMULATOR_H
#define DDSIMULATOR_H

#include "Definitions.hpp"
#include "dd/Package.hpp"
#include "operations/OpType.hpp"

#include <algorithm>
#include <array>
#include <complex>
#include <cstddef>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

template<class Config = dd::DDPackageConfig>
class Simulator {
public:
    explicit Simulator(const unsigned long long randomSeed):
        seed(randomSeed), hasFixedSeed(true) {
        mt.seed(randomSeed);
    };

    explicit Simulator():
        seed(0), hasFixedSeed(false) {
        // this is probably overkill but better safe than sorry
        std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> random_data{};
        std::random_device                                                    rd;
        std::generate(std::begin(random_data), std::end(random_data), [&rd]() { return rd(); });
        std::seed_seq seeds(std::begin(random_data), std::end(random_data));
        mt.seed(seeds);
    };

    virtual ~Simulator() = default;

    virtual std::map<std::string, std::size_t> Simulate(std::size_t shots) = 0;

    virtual std::map<std::string, std::string> AdditionalStatistics() { return {}; };

    std::string MeasureAll(bool collapse = false) {
        return dd->measureAll(rootEdge, collapse, mt, epsilon);
    }

    std::map<std::string, std::size_t> MeasureAllNonCollapsing(std::size_t shots) {
        std::map<std::string, std::size_t> results;
        for (std::size_t i = 0; i < shots; i++) {
            const auto m = MeasureAll(false);
            results[m]++;
        }
        return results;
    }

    char MeasureOneCollapsing(const qc::Qubit index, const bool assume_probability_normalization = true) {
        assert(index < getNumberOfQubits());
        return dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(index), assume_probability_normalization, mt, epsilon);
    }

    std::map<std::string, std::size_t> SampleFromAmplitudeVectorInPlace(std::vector<std::complex<dd::fp>>& amplitudes, std::size_t shots);

    [[nodiscard]] std::vector<dd::ComplexValue> getVector() const;

    [[nodiscard]] std::vector<std::pair<dd::fp, dd::fp>> getVectorPair() const;

    [[nodiscard]] std::vector<std::complex<dd::fp>> getVectorComplex() const;

    [[nodiscard]] virtual std::size_t getActiveNodeCount() const { return dd->vUniqueTable.getActiveNodeCount(); }

    [[nodiscard]] virtual std::size_t getMaxNodeCount() const { return dd->vUniqueTable.getMaxActiveNodes(); }

    [[nodiscard]] virtual std::size_t getMaxMatrixNodeCount() const { return dd->mUniqueTable.getMaxActiveNodes(); }

    [[nodiscard]] virtual std::size_t getMatrixActiveNodeCount() const { return dd->mUniqueTable.getActiveNodeCount(); }

    [[nodiscard]] virtual std::size_t countNodesFromRoot() const { return dd->size(rootEdge); }

    [[nodiscard]] std::pair<dd::ComplexValue, std::string> getPathOfLeastResistance() const;

    [[nodiscard]] std::string getSeed() const { return hasFixedSeed ? std::to_string(seed) : "-1"; }

    [[nodiscard]] virtual std::size_t getNumberOfQubits() const = 0;

    [[nodiscard]] virtual std::size_t getNumberOfOps() const = 0;

    [[nodiscard]] virtual std::string getName() const = 0;

    [[nodiscard]] static inline std::string toBinaryString(const std::size_t value, const std::size_t number_of_qubits) {
        std::string binary(number_of_qubits, '0');
        for (std::size_t j = 0; j < number_of_qubits; ++j) {
            if (value & (1 << j))
                binary[j] = '1';
        }
        return binary;
    }

    [[nodiscard]] std::vector<std::priority_queue<std::pair<double, dd::vNode*>, std::vector<std::pair<double, dd::vNode*>>>> GetNodeContributions(const dd::vEdge& edge) const;

    double ApproximateByFidelity(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge& edge, double targetFidelity, bool allLevels, bool removeNodes, bool verbose = false);
    double ApproximateByFidelity(double targetFidelity, bool allLevels, bool removeNodes, bool verbose = false) {
        return ApproximateByFidelity(dd, rootEdge, targetFidelity, allLevels, removeNodes, verbose);
    }

    double ApproximateBySampling(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge& edge, std::size_t nSamples, std::size_t threshold, bool removeNodes, bool verbose = false);
    double ApproximateBySampling(std::size_t nSamples, std::size_t threshold, bool removeNodes, bool verbose = false) {
        return ApproximateBySampling(dd, rootEdge, nSamples, threshold, removeNodes, verbose);
    }

    dd::vEdge static RemoveNodes(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge edge, std::map<dd::vNode*, dd::vEdge>& dag_edges);

    std::unique_ptr<dd::Package<Config>> dd = std::make_unique<dd::Package<Config>>();
    dd::vEdge                            rootEdge{};

protected:
    std::mt19937_64 mt;

    const std::uint64_t seed = 0;
    const bool               hasFixedSeed;
    const dd::fp             epsilon = 0.001;

    static void NextPath(std::string& s);
};

struct StochasticNoiseSimulatorDDPackageConfig: public dd::DDPackageConfig {
    static constexpr std::size_t STOCHASTIC_CACHE_OPS = qc::OpType::OpCount;
};

struct DensityMatrixSimulatorDDPackageConfig: public dd::DDPackageConfig {
    static constexpr std::size_t UT_DM_NBUCKET                 = 65536U;
    static constexpr std::size_t UT_DM_INITIAL_ALLOCATION_SIZE = 4096U;

    static constexpr std::size_t CT_DM_DM_MULT_NBUCKET = 16384U;
    static constexpr std::size_t CT_DM_ADD_NBUCKET     = 16384U;
    static constexpr std::size_t CT_DM_NOISE_NBUCKET   = 4096U;

    static constexpr std::size_t UT_MAT_NBUCKET            = 16384U;
    static constexpr std::size_t CT_MAT_ADD_NBUCKET        = 4096U;
    static constexpr std::size_t CT_VEC_ADD_NBUCKET        = 4096U;
    static constexpr std::size_t CT_MAT_TRANS_NBUCKET      = 4096U;
    static constexpr std::size_t CT_MAT_CONJ_TRANS_NBUCKET = 4096U;

    static constexpr std::size_t CT_MAT_MAT_MULT_NBUCKET        = 1U;
    static constexpr std::size_t CT_MAT_VEC_MULT_NBUCKET        = 1U;
    static constexpr std::size_t UT_VEC_NBUCKET                 = 1U;
    static constexpr std::size_t UT_VEC_INITIAL_ALLOCATION_SIZE = 1U;
    static constexpr std::size_t UT_MAT_INITIAL_ALLOCATION_SIZE = 1U;
    static constexpr std::size_t CT_VEC_KRON_NBUCKET            = 1U;
    static constexpr std::size_t CT_MAT_KRON_NBUCKET            = 1U;
    static constexpr std::size_t CT_VEC_INNER_PROD_NBUCKET      = 1U;
    static constexpr std::size_t STOCHASTIC_CACHE_OPS           = 1U;
};

#endif //DDSIMULATOR_H
