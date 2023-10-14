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
    explicit Simulator(const std::uint64_t randomSeed):
        seed(randomSeed), hasFixedSeed(true) {
        mt.seed(randomSeed);
    };

    explicit Simulator():
        hasFixedSeed(false) {
        // this is probably overkill but better safe than sorry
        std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> randomData{};
        std::random_device                                                    rd;
        std::generate(std::begin(randomData), std::end(randomData), [&rd]() { return rd(); });
        std::seed_seq seeds(std::begin(randomData), std::end(randomData));
        mt.seed(seeds);
    };

    virtual ~Simulator() = default;

    /**
     * Run the simulation in the (derived) class.
     * @param shots number of shots to take from the final quantum state
     * @return a map from the strings representing basis states to the number of times they have been measured
     */
    virtual std::map<std::string, std::size_t> simulate(std::size_t shots) = 0;

    virtual std::map<std::string, std::string> additionalStatistics() { return {}; };

    std::string measureAll(bool collapse = false) {
        return dd->measureAll(rootEdge, collapse, mt, epsilon);
    }

    std::map<std::string, std::size_t> measureAllNonCollapsing(std::size_t shots) {
        std::map<std::string, std::size_t> results;
        for (std::size_t i = 0; i < shots; i++) {
            const auto m = measureAll(false);
            results[m]++;
        }
        return results;
    }

    char measureOneCollapsing(const qc::Qubit index, const bool assumeProbabilityNormalization = true) {
        assert(index < getNumberOfQubits());
        return dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(index), assumeProbabilityNormalization, mt, epsilon);
    }

    std::map<std::string, std::size_t> sampleFromAmplitudeVectorInPlace(std::vector<std::complex<dd::fp>>& amplitudes, std::size_t shots);

    [[nodiscard]] dd::CVec getVector() const {
        if (getNumberOfQubits() >= 60) {
            // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
            throw std::range_error("getVector only supports less than 60 qubits.");
        }
        return rootEdge.getVector();
    }

    [[nodiscard]] virtual std::size_t getActiveNodeCount() const { return dd->template getUniqueTable<dd::vNode>().getNumActiveEntries(); }

    [[nodiscard]] virtual std::size_t getMaxNodeCount() const { return dd->template getUniqueTable<dd::vNode>().getPeakNumActiveEntries(); }

    [[nodiscard]] virtual std::size_t getMaxMatrixNodeCount() const { return dd->template getUniqueTable<dd::mNode>().getPeakNumActiveEntries(); }

    [[nodiscard]] virtual std::size_t getMatrixActiveNodeCount() const { return dd->template getUniqueTable<dd::mNode>().getNumActiveEntries(); }

    [[nodiscard]] virtual std::size_t countNodesFromRoot() { return rootEdge.size(); }

    [[nodiscard]] std::pair<dd::ComplexValue, std::string> getPathOfLeastResistance() const;

    [[nodiscard]] std::string getSeed() const { return hasFixedSeed ? std::to_string(seed) : "-1"; }

    [[nodiscard]] virtual std::size_t getNumberOfQubits() const = 0;

    [[nodiscard]] virtual std::size_t getNumberOfOps() const = 0;

    [[nodiscard]] virtual std::string getName() const = 0;

    void setTolerance(const dd::fp tolerance) {
        dd->cn.setTolerance(tolerance);
    }
    [[nodiscard]] dd::fp getTolerance() const {
        return dd::RealNumber::eps;
    }

    [[nodiscard]] std::vector<std::priority_queue<std::pair<double, dd::vNode*>, std::vector<std::pair<double, dd::vNode*>>>> getNodeContributions(const dd::vEdge& edge) const;

    double approximateByFidelity(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge& edge, double targetFidelity, bool allLevels, bool actuallyRemoveNodes, bool verbose = false);
    double approximateByFidelity(double targetFidelity, bool allLevels, bool removeNodes, bool verbose = false) {
        return approximateByFidelity(dd, rootEdge, targetFidelity, allLevels, removeNodes, verbose);
    }

    double approximateBySampling(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge& edge, std::size_t nSamples, std::size_t threshold, bool actuallyRemoveNodes, bool verbose = false);
    double approximateBySampling(std::size_t nSamples, std::size_t threshold, bool removeNodes, bool verbose = false) {
        return approximateBySampling(dd, rootEdge, nSamples, threshold, removeNodes, verbose);
    }

    dd::vEdge static removeNodes(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge edge, std::map<dd::vNode*, dd::vEdge>& dagEdges);

    /**
     * @brief Get a GraphViz representation of the currently stored DD.
     * @param colored Whether to output color-coded edge weights or black and white.
     * @param edgeLabels Whether to output edge labels.
     * @param classic Whether to use the classic visualization or a more modern representation.
     * @param memory An alternative representation for nodes that includes detailed memory information.
     * @param formatAsPolar Whether to format the complex numbers as polar or cartesian coordinates.
     * @returns A Graphviz program representing the current DD
     */
    std::string exportDDtoGraphvizString(bool colored    = true,
                                         bool edgeLabels = false, bool classic = false, bool memory = false, bool formatAsPolar = true);

    /**
     * @brief Write a GraphViz representation of the currently stored DD to a file.
     * @param filename The name of the file to write to.
     * @param colored Whether to output color-coded edge weights or black and white.
     * @param edgeLabels Whether to output edge labels.
     * @param classic Whether to use the classic visualization or a more modern representation.
     * @param memory An alternative representation for nodes that includes detailed memory information.
     * @param formatAsPolar Whether to format the complex numbers as polar or cartesian coordinates.
     */
    void exportDDtoGraphvizFile(const std::string& filename, bool colored = true,
                                bool edgeLabels = false, bool classic = false, bool memory = false, bool formatAsPolar = true);

    std::unique_ptr<dd::Package<Config>> dd = std::make_unique<dd::Package<Config>>();
    dd::vEdge                            rootEdge{};

protected:
    std::mt19937_64 mt;

    std::uint64_t seed = 0;
    bool          hasFixedSeed;
    dd::fp        epsilon = 0.001;

    virtual void exportDDtoGraphviz(std::ostream& os, bool colored, bool edgeLabels, bool classic, bool memory, bool formatAsPolar);
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
