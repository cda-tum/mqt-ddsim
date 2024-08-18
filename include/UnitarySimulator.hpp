#pragma once

#include "CircuitSimulator.hpp"
#include "Simulator.hpp"
#include "circuit_optimizer/CircuitOptimizer.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Node.hpp"
#include "dd/Package_fwd.hpp"
#include "ir/QuantumComputation.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <utility>

template <class Config = dd::DDPackageConfig>
class UnitarySimulator : public CircuitSimulator<Config> {
public:
  enum class Mode : std::uint8_t { Sequential, Recursive };

  UnitarySimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                   const ApproximationInfo& approximationInfo_,
                   Mode simMode = Mode::Recursive)
      : CircuitSimulator<Config>(std::move(qc_), approximationInfo_),
        mode(simMode) {
    // remove final measurements
    qc::CircuitOptimizer::removeFinalMeasurements(
        *(CircuitSimulator<Config>::qc));
  }

  explicit UnitarySimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                            Mode simMode = Mode::Recursive)
      : UnitarySimulator(std::move(qc_), {}, simMode) {}

  UnitarySimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                   const ApproximationInfo& approximationInfo_,
                   const std::uint64_t seed_,
                   const Mode simMode = Mode::Recursive)
      : CircuitSimulator<Config>(std::move(qc_), approximationInfo_, seed_),
        mode(simMode) {
    // remove final measurements
    qc::CircuitOptimizer::removeFinalMeasurements(
        *(CircuitSimulator<Config>::qc));
  }

  void construct();

  [[nodiscard]] Mode getMode() const { return mode; }
  [[nodiscard]] qc::MatrixDD getConstructedDD() const { return e; }
  [[nodiscard]] double getConstructionTime() const { return constructionTime; }
  [[nodiscard]] std::size_t getFinalNodeCount() const { return e.size(); }
  [[nodiscard]] std::size_t getMaxNodeCount() const override {
    return Simulator<Config>::dd->template getUniqueTable<dd::mNode>()
        .getPeakNumActiveEntries();
  }

protected:
  /// See Simulator<Config>::exportDDtoGraphviz
  void exportDDtoGraphviz(std::ostream& os, bool colored, bool edgeLabels,
                          bool classic, bool memory,
                          bool formatAsPolar) override;

private:
  qc::MatrixDD e{};

  Mode mode = Mode::Recursive;

  double constructionTime = 0.;
};
