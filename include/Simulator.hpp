#ifndef DDSIMULATOR_H
#define DDSIMULATOR_H

#include "dd/Package.hpp"

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

class Simulator {
public:
    explicit Simulator(unsigned long long seed):
        seed(seed), has_fixed_seed(true) {
        mt.seed(seed);
    };

    explicit Simulator():
        seed(0), has_fixed_seed(false) {
        // this is probably overkill but better safe than sorry
        std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> random_data{};
        std::random_device                                                    rd;
        std::generate(std::begin(random_data), std::end(random_data), [&rd]() { return rd(); });
        std::seed_seq seeds(std::begin(random_data), std::end(random_data));
        mt.seed(seeds);
    };

    virtual ~Simulator() = default;

    virtual std::map<std::string, std::size_t> Simulate(unsigned int shots) = 0;

    virtual std::map<std::string, std::string> AdditionalStatistics() { return {}; };

    std::string MeasureAll(bool collapse = false);

    std::map<std::string, std::size_t> MeasureAllNonCollapsing(unsigned int shots);

    char MeasureOneCollapsing(dd::Qubit index, bool assume_probability_normalization = true);

    [[nodiscard]] std::vector<dd::ComplexValue> getVector() const;

    [[nodiscard]] std::vector<std::pair<dd::fp, dd::fp>> getVectorPair() const;

    [[nodiscard]] std::vector<std::complex<dd::fp>> getVectorComplex() const;

    [[nodiscard]] std::size_t getActiveNodeCount() const { return dd->vUniqueTable.getActiveNodeCount(); }

    [[nodiscard]] std::size_t getMaxNodeCount() const { return dd->vUniqueTable.getMaxActiveNodes(); }

    [[nodiscard]] std::size_t countNodesFromRoot() const { return dd->size(root_edge); }

    [[nodiscard]] std::pair<dd::ComplexValue, std::string> getPathOfLeastResistance() const;

    [[nodiscard]] std::string getSeed() const { return has_fixed_seed ? std::to_string(seed) : "-1"; }

    [[nodiscard]] virtual dd::QubitCount getNumberOfQubits() const = 0;

    [[nodiscard]] virtual std::size_t getNumberOfOps() const = 0;

    [[nodiscard]] virtual std::string getName() const = 0;

    double ApproximateByFidelity(double targetFidelity, bool allLevels, bool removeNodes, bool verbose = false);

    double ApproximateBySampling(unsigned int nSamples, unsigned int threshold, bool removeNodes, bool verbose = false);

    dd::Package::vEdge
    RemoveNodes(dd::Package::vEdge edge, std::map<dd::Package::vNode*, dd::Package::vEdge>& dag_edges);

    std::unique_ptr<dd::Package> dd = std::make_unique<dd::Package>();
    dd::Package::vEdge           root_edge{};

protected:
    std::mt19937_64 mt;

    const unsigned long long seed;
    const bool               has_fixed_seed;
    const dd::fp             epsilon = 0.001L;

    static void NextPath(std::string& s);

    double assign_probs(dd::Package::vEdge edge, std::unordered_map<dd::Package::vNode*, double>& probs);
};

#endif //DDSIMULATOR_H
