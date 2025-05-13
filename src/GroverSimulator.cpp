/*
 * Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
 * Copyright (c) 2025 Munich Quantum Software Company GmbH
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include "GroverSimulator.hpp"

#include "dd/DDDefinitions.hpp"
#include "dd/Edge.hpp"
#include "dd/FunctionalityConstruction.hpp"
#include "ir/Definitions.hpp"
#include "ir/QuantumComputation.hpp"
#include "ir/operations/Control.hpp"

#include <cassert>
#include <cstddef>
#include <limits>
#include <map>
#include <string>

std::map<std::string, std::size_t>
GroverSimulator::simulate(std::size_t shots) {
  // Setup X on the last, Hadamard on all qubits
  qc::QuantumComputation qcSetup(nQubits + nAnciallae);
  qcSetup.x(nQubits);
  for (qc::Qubit i = 0; i < nQubits; ++i) {
    qcSetup.h(i);
  }

  const dd::Edge setupOp{dd::buildFunctionality(qcSetup, *dd)};

  // Build the oracle
  qc::QuantumComputation qcOracle(nQubits + nAnciallae);
  qc::Controls controls{};
  for (qc::Qubit i = 0; i < nQubits; i++) {
    controls.emplace(i, oracle.at(i) == '1' ? qc::Control::Type::Pos
                                            : qc::Control::Type::Neg);
  }
  qcOracle.mcz(controls, nQubits);

  const dd::Edge oracleOp{dd::buildFunctionality(qcOracle, *dd)};

  // Build the diffusion stage.
  qc::QuantumComputation qcDiffusion(nQubits + nAnciallae);

  for (qc::Qubit i = 0; i < nQubits; ++i) {
    qcDiffusion.h(i);
  }
  for (qc::Qubit i = 0; i < nQubits; ++i) {
    qcDiffusion.x(i);
  }

  qcDiffusion.h(nQubits - 1);

  qc::Controls diffControls{};
  for (qc::Qubit j = 0; j < nQubits - 1; ++j) {
    diffControls.emplace(j);
  }
  qcDiffusion.mcx(diffControls, nQubits - 1);

  qcDiffusion.h(nQubits - 1);

  for (qc::Qubit i = 0; i < nQubits; ++i) {
    qcDiffusion.x(i);
  }
  for (qc::Qubit i = 0; i < nQubits; ++i) {
    qcDiffusion.h(i);
  }

  const dd::Edge diffusionOp{dd::buildFunctionality(qcDiffusion, *dd)};

  const dd::Edge fullIteration{dd->multiply(oracleOp, diffusionOp)};
  dd->incRef(fullIteration);

  assert(nQubits + nAnciallae <= std::numeric_limits<dd::Qubit>::max());
  rootEdge = dd->makeZeroState(static_cast<dd::Qubit>(nQubits + nAnciallae));
  rootEdge = dd->multiply(setupOp, rootEdge);
  dd->incRef(rootEdge);

  std::size_t jPre = 0;

  while ((iterations - jPre) % 8 != 0) {
    auto tmp = dd->multiply(fullIteration, rootEdge);
    dd->incRef(tmp);
    dd->decRef(rootEdge);
    rootEdge = tmp;
    dd->garbageCollect();
    jPre++;
  }

  for (std::size_t j = jPre; j < iterations; j += 8) {
    auto tmp = dd->multiply(fullIteration, rootEdge);
    for (std::size_t i = 0; i < 7; ++i) {
      tmp = dd->multiply(fullIteration, tmp);
    }
    dd->incRef(tmp);
    dd->decRef(rootEdge);
    rootEdge = tmp;
    dd->garbageCollect();
  }

  return measureAllNonCollapsing(shots);
}
