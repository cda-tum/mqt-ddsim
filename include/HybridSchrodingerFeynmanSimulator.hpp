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
#include "circuit_optimizer/CircuitOptimizer.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/Node.hpp"
#include "dd/Package.hpp"
#include "ir/Definitions.hpp"
#include "ir/QuantumComputation.hpp"
#include "ir/operations/Operation.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

class HybridSchrodingerFeynmanSimulator final : public CircuitSimulator {
public:
  enum class Mode : std::uint8_t { DD, Amplitude };

  HybridSchrodingerFeynmanSimulator(
      std::unique_ptr<qc::QuantumComputation>&& qc_,
      const ApproximationInfo& approxInfo_, Mode mode_ = Mode::Amplitude,
      const std::size_t nthreads_ = 2)
      : CircuitSimulator(std::move(qc_), approxInfo_), mode(mode_),
        nthreads(nthreads_) {
    qc::CircuitOptimizer::flattenOperations(*qc);
    // remove final measurements
    qc::CircuitOptimizer::removeFinalMeasurements(*qc);
  }

  explicit HybridSchrodingerFeynmanSimulator(
      std::unique_ptr<qc::QuantumComputation>&& qc_,
      Mode mode_ = Mode::Amplitude, const std::size_t nthreads_ = 2)
      : HybridSchrodingerFeynmanSimulator(std::move(qc_), {}, mode_,
                                          nthreads_) {}

  HybridSchrodingerFeynmanSimulator(
      std::unique_ptr<qc::QuantumComputation>&& qc_,
      const ApproximationInfo& approxInfo_, const std::uint64_t seed_,
      Mode mode_ = Mode::Amplitude, const std::size_t nthreads_ = 2)
      : CircuitSimulator(std::move(qc_), approxInfo_, seed_), mode(mode_),
        nthreads(nthreads_) {
    // remove final measurements
    qc::CircuitOptimizer::flattenOperations(*qc);
    qc::CircuitOptimizer::removeFinalMeasurements(*qc);
  }

  std::map<std::string, std::size_t> simulate(std::size_t shots) override;

  Mode mode = Mode::Amplitude;

  [[nodiscard]] dd::CVec getVectorFromHybridSimulation() const {
    if (CircuitSimulator::getNumberOfQubits() >= 60) {
      // On 64bit system the vector can hold up to (2^60)-1 elements, if memory
      // permits
      throw std::range_error("getVector only supports less than 60 qubits.");
    }
    if (getMode() == Mode::Amplitude) {
      return finalAmplitudes;
    }
    return CircuitSimulator::rootEdge.getVector();
  }

  //  Get # of decisions for given split_qubit, so that lower slice: q0 < i <
  //  qubit; upper slice: qubit <= i < nqubits
  std::size_t getNDecisions(qc::Qubit splitQubit);

  [[nodiscard]] Mode getMode() const { return mode; }

private:
  std::size_t nthreads = 2;
  dd::CVec finalAmplitudes;

  void simulateHybridTaskflow(qc::Qubit splitQubit);
  void simulateHybridAmplitudes(qc::Qubit splitQubit);

  dd::VectorDD simulateSlicing(std::unique_ptr<dd::Package>& sliceDD,
                               qc::Qubit splitQubit, std::size_t controls);

  class Slice {
  protected:
    qc::Qubit nextControlIdx = 0;

    std::size_t getNextControl() {
      std::size_t idx = 1UL << nextControlIdx;
      nextControlIdx++;
      return controls & idx;
    }

  public:
    qc::Qubit start;
    qc::Qubit end;
    std::size_t controls;
    qc::Qubit nqubits;
    std::size_t nDecisionsExecuted = 0;
    dd::VectorDD edge{};

    explicit Slice(std::unique_ptr<dd::Package>& dd, const qc::Qubit start_,
                   const qc::Qubit end_, const std::size_t controls_)
        : start(start_), end(end_), controls(controls_),
          nqubits(end - start + 1),
          edge(dd->makeZeroState(static_cast<dd::Qubit>(nqubits), start_)) {
      dd->incRef(edge);
    }

    explicit Slice(std::unique_ptr<dd::Package>& dd, dd::VectorDD edge_,
                   const qc::Qubit start_, const qc::Qubit end_,
                   const std::size_t controls_)
        : start(start_), end(end_), controls(controls_),
          nqubits(end - start + 1), edge(edge_) {
      dd->incRef(edge);
    }

    // returns true if this operation was a split operation
    bool apply(std::unique_ptr<dd::Package>& sliceDD,
               const std::unique_ptr<qc::Operation>& op);
  };
};
