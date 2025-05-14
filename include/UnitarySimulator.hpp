/*
 * Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
 * Copyright (c) 2025 Munich Quantum Software Company GmbH
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#pragma once

#include "CircuitSimulator.hpp"
#include "dd/Node.hpp"
#include "ir/QuantumComputation.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

class UnitarySimulator final : public CircuitSimulator {
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
  [[nodiscard]] dd::MatrixDD getConstructedDD() const { return e; }
  [[nodiscard]] double getConstructionTime() const { return constructionTime; }
  [[nodiscard]] std::size_t getFinalNodeCount() const { return e.size(); }
  [[nodiscard]] std::size_t getMaxNodeCount() const override;

private:
  dd::MatrixDD e{};

  Mode mode = Mode::Recursive;

  double constructionTime = 0.;
};
