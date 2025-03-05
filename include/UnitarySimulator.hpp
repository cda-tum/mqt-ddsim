#pragma once

#include "CircuitSimulator.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Package_fwd.hpp"
#include "ir/QuantumComputation.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

class UnitarySimulator
    : public CircuitSimulator<dd::UnitarySimulatorDDPackageConfig> {
public:
  enum class Mode : std::uint8_t { Sequential, Recursive };

  UnitarySimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                   const ApproximationInfo& approximationInfo_,
                   Mode simMode = Mode::Recursive);

  explicit UnitarySimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                            Mode simMode = Mode::Recursive);

  UnitarySimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                   const ApproximationInfo& approximationInfo_,
                   std::uint64_t seed_, Mode simMode = Mode::Recursive);

  void construct();

  [[nodiscard]] Mode getMode() const { return mode; }
  [[nodiscard]] qc::MatrixDD getConstructedDD() const { return e; }
  [[nodiscard]] double getConstructionTime() const { return constructionTime; }
  [[nodiscard]] std::size_t getFinalNodeCount() const { return e.size(); }
  [[nodiscard]] std::size_t getMaxNodeCount() const override;

private:
  qc::MatrixDD e{};

  Mode mode = Mode::Recursive;

  double constructionTime = 0.;
};
