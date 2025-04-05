#pragma once

#include "dd/ComplexValue.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Node.hpp"
#include "dd/Package.hpp"
#include "dd/RealNumber.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Simulator {
public:
  explicit Simulator(const std::uint64_t randomSeed,
                     const dd::DDPackageConfig& config = dd::DDPackageConfig())
      : seed(randomSeed), hasFixedSeed(true) {
    mt.seed(randomSeed);
    dd = std::make_unique<dd::Package>(dd::Package::DEFAULT_QUBITS, config);
  }

  explicit Simulator(const dd::DDPackageConfig& config = dd::DDPackageConfig())
      : hasFixedSeed(false) {
    // this is probably overkill but better safe than sorry
    std::array<std::mt19937_64::result_type, std::mt19937_64::state_size>
        randomData{};
    std::random_device rd;
    std::generate(std::begin(randomData), std::end(randomData),
                  [&rd]() { return rd(); });
    std::seed_seq seeds(std::begin(randomData), std::end(randomData));
    mt.seed(seeds);
    dd = std::make_unique<dd::Package>(dd::Package::DEFAULT_QUBITS, config);
  }

  virtual ~Simulator() = default;

  /**
   * Run the simulation in the (derived) class.
   * @param shots number of shots to take from the final quantum state
   * @return a map from the strings representing basis states to the number of
   * times they have been measured
   */
  virtual std::map<std::string, std::size_t> simulate(std::size_t shots) = 0;

  virtual std::map<std::string, std::string> additionalStatistics() {
    return {};
  };

  std::string measureAll(bool collapse = false) {
    return dd->measureAll(rootEdge, collapse, mt, epsilon);
  }

  virtual std::map<std::string, std::size_t>
  measureAllNonCollapsing(std::size_t shots) {
    std::map<std::string, std::size_t> results;
    for (std::size_t i = 0; i < shots; i++) {
      const auto m = measureAll(false);
      results[m]++;
    }
    return results;
  }

  std::map<std::string, std::size_t> sampleFromAmplitudeVectorInPlace(
      std::vector<std::complex<dd::fp>>& amplitudes, std::size_t shots);

  [[nodiscard]] virtual std::size_t getActiveNodeCount() const {
    return dd->getUniqueTable<dd::vNode>().getNumActiveEntries();
  }

  [[nodiscard]] virtual std::size_t getMaxNodeCount() const {
    return dd->getUniqueTable<dd::vNode>().getPeakNumActiveEntries();
  }

  [[nodiscard]] virtual std::size_t getMaxMatrixNodeCount() const {
    return dd->getUniqueTable<dd::mNode>().getPeakNumActiveEntries();
  }

  [[nodiscard]] virtual std::size_t getMatrixActiveNodeCount() const {
    return dd->getUniqueTable<dd::mNode>().getNumActiveEntries();
  }

  [[nodiscard]] virtual std::size_t countNodesFromRoot() {
    return rootEdge.size();
  }

  [[nodiscard]] auto getCurrentDD() const -> const dd::vEdge& {
    return rootEdge;
  }

  [[nodiscard]] std::pair<dd::ComplexValue, std::string>
  getPathOfLeastResistance() const;

  [[nodiscard]] std::string getSeed() const {
    return hasFixedSeed ? std::to_string(seed) : "-1";
  }

  [[nodiscard]] virtual std::size_t getNumberOfQubits() const = 0;

  [[nodiscard]] virtual std::size_t getNumberOfOps() const = 0;

  [[nodiscard]] virtual std::string getName() const = 0;

  void setTolerance(const dd::fp tolerance) { dd->cn.setTolerance(tolerance); }
  [[nodiscard]] dd::fp getTolerance() const { return dd::RealNumber::eps; }

  [[nodiscard]] std::vector<
      std::priority_queue<std::pair<double, dd::vNode*>,
                          std::vector<std::pair<double, dd::vNode*>>>>
  getNodeContributions(const dd::vEdge& edge) const;

  double approximateByFidelity(std::unique_ptr<dd::Package>& localDD,
                               dd::vEdge& edge, double targetFidelity,
                               bool allLevels, bool actuallyRemoveNodes,
                               bool verbose = false);
  double approximateByFidelity(double targetFidelity, bool allLevels,
                               bool removeNodes, bool verbose = false) {
    return approximateByFidelity(dd, rootEdge, targetFidelity, allLevels,
                                 removeNodes, verbose);
  }

  double approximateBySampling(std::unique_ptr<dd::Package>& localDD,
                               dd::vEdge& edge, std::size_t nSamples,
                               std::size_t threshold, bool actuallyRemoveNodes,
                               bool verbose = false);
  double approximateBySampling(std::size_t nSamples, std::size_t threshold,
                               bool removeNodes, bool verbose = false) {
    return approximateBySampling(dd, rootEdge, nSamples, threshold, removeNodes,
                                 verbose);
  }

  dd::vEdge static removeNodes(std::unique_ptr<dd::Package>& localDD,
                               dd::vEdge edge,
                               std::map<dd::vNode*, dd::vEdge>& dagEdges);

  std::unique_ptr<dd::Package> dd;
  dd::vEdge rootEdge = dd::vEdge::one();

protected:
  std::mt19937_64 mt;

  std::uint64_t seed = 0;
  bool hasFixedSeed;
  dd::fp epsilon = 0.001;
};
