#include "CircuitSimulator.hpp"

#include "dd/DDDefinitions.hpp"
#include "dd/FunctionalityConstruction.hpp"
#include "dd/Node.hpp"
#include "dd/Operations.hpp"
#include "ir/operations/ClassicControlledOperation.hpp"
#include "ir/operations/NonUnitaryOperation.hpp"
#include "ir/operations/OpType.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

std::map<std::string, std::size_t>
CircuitSimulator::simulate(std::size_t shots) {
  const auto analysis = analyseCircuit();

  // easiest case: all gates are unitary --> simulate once and sample away on
  // all qubits
  if (!analysis.isDynamic && !analysis.hasMeasurements) {
    singleShot(false);
    return measureAllNonCollapsing(shots);
  }

  // single shot is enough, but the sampling should only return actually
  // measured qubits
  if (!analysis.isDynamic) {
    singleShot(true);
    std::map<std::string, std::size_t> measurementCounter;
    const auto qubits = qc->getNqubits();
    const auto cbits = qc->getNcbits();

    // MeasureAllNonCollapsing returns a map from measurement over all qubits to
    // the number of occurrences
    for (const auto& [bit_string, count] : measureAllNonCollapsing(shots)) {
      std::string resultString(qc->getNcbits(), '0');

      for (auto const& [qubit_index, bitIndex] : analysis.measurementMap) {
        resultString[cbits - bitIndex - 1] =
            bit_string[qubits - qubit_index - 1];
      }

      measurementCounter[resultString] += count;
    }

    return measurementCounter;
  }

  // the circuit is dynamic and requires single shot simulations :(
  std::map<std::string, std::size_t> measurementCounter;

  for (unsigned int i = 0; i < shots; i++) {
    const auto result = singleShot(false);
    const auto cbits = qc->getNcbits();

    std::string resultString(qc->getNcbits(), '0');

    // result is a map from the cbit index to the Boolean value
    for (const auto& [bitIndex, value] : result) {
      resultString[cbits - bitIndex - 1] = value ? '1' : '0';
    }
    measurementCounter[resultString]++;
  }
  return measurementCounter;
}

auto CircuitSimulator::analyseCircuit() -> CircuitAnalysis {
  auto analysis = CircuitAnalysis{};

  for (auto& op : *qc) {
    if (op->isClassicControlledOperation() || op->getType() == qc::Reset) {
      analysis.isDynamic = true;
    }
    if (const auto* measure = dynamic_cast<qc::NonUnitaryOperation*>(op.get());
        measure != nullptr && measure->getType() == qc::Measure) {
      analysis.hasMeasurements = true;

      const auto& quantum = measure->getTargets();
      const auto& classic = measure->getClassics();

      if (quantum.size() != classic.size()) {
        throw std::runtime_error(
            "Measurement: Sizes of quantum and classic register mismatch.");
      }

      for (unsigned int i = 0; i < quantum.size(); ++i) {
        analysis.measurementMap[quantum.at(i)] = classic.at(i);
      }
    }

    if (analysis.hasMeasurements && op->isUnitary()) {
      analysis.isDynamic = true;
    }
  }
  return analysis;
}

dd::fp
CircuitSimulator::expectationValue(const qc::QuantumComputation& observable) {
  // simulate the circuit to get the state vector
  singleShot(true);

  // construct the DD for the observable
  const auto observableDD = dd::buildFunctionality(observable, *dd);

  // calculate the expectation value
  return dd->expectationValue(observableDD, rootEdge);
}

void CircuitSimulator::initializeSimulation(const std::size_t nQubits) {
  rootEdge = dd->makeZeroState(static_cast<dd::Qubit>(nQubits));
}

char CircuitSimulator::measure(const dd::Qubit i) {
  return dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(i), mt);
}

void CircuitSimulator::reset(qc::NonUnitaryOperation* nonUnitaryOp) {
  rootEdge = dd::applyReset(*nonUnitaryOp, rootEdge, *dd, mt);
}

void CircuitSimulator::applyOperationToState(
    std::unique_ptr<qc::Operation>& op) {
  rootEdge = dd::applyUnitaryOperation(*op, rootEdge, *dd);
}

std::map<std::size_t, bool>
CircuitSimulator::singleShot(const bool ignoreNonUnitaries) {
  singleShots++;
  const auto nQubits = qc->getNqubits();

  initializeSimulation(nQubits);

  std::size_t opNum = 0;
  std::map<std::size_t, bool> classicValues;

  const auto approxMod = static_cast<std::size_t>(
      std::ceil(static_cast<double>(qc->getNops()) /
                (static_cast<double>(approximationInfo.stepNumber + 1))));

  for (auto& op : *qc) {
    if (op->isNonUnitaryOperation()) {
      if (ignoreNonUnitaries) {
        continue;
      }
      if (auto* nonUnitaryOp =
              dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
        if (op->getType() == qc::Measure) {
          const auto& quantum = nonUnitaryOp->getTargets();
          const auto& classic = nonUnitaryOp->getClassics();

          assert(
              quantum.size() ==
              classic.size()); // this should not happen do to check in Simulate

          for (std::size_t i = 0; i < quantum.size(); ++i) {
            auto result = measure(static_cast<dd::Qubit>(quantum.at(i)));
            assert(result == '0' || result == '1');
            classicValues[classic.at(i)] = (result == '1');
          }

        } else if (nonUnitaryOp->getType() == qc::Reset) {
          reset(nonUnitaryOp);
        } else {
          throw std::runtime_error("Unsupported non-unitary functionality.");
        }
      } else {
        throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
      }
      dd->garbageCollect();
    } else {
      if (op->isClassicControlledOperation()) {
        if (auto* classicallyControlledOp =
                dynamic_cast<qc::ClassicControlledOperation*>(op.get())) {
          const auto startIndex = static_cast<std::uint16_t>(
              classicallyControlledOp->getParameter().at(0));
          const auto length = static_cast<std::uint16_t>(
              classicallyControlledOp->getParameter().at(1));
          const auto expectedValue =
              classicallyControlledOp->getExpectedValue();
          unsigned int actualValue = 0;
          for (std::size_t i = 0; i < length; i++) {
            actualValue |= (classicValues[startIndex + i] ? 1U : 0U) << i;
          }

          // std::clog << "expected " << expected_value << " and actual value
          // was " << actual_value << "\n";

          if (actualValue != expectedValue) {
            continue;
          }
        } else {
          throw std::runtime_error(
              "Dynamic cast to ClassicControlledOperation failed.");
        }
      }
      applyOperationToState(op);

      if (approximationInfo.stepNumber > 0 &&
          approximationInfo.stepFidelity < 1.0) {
        if (approximationInfo.strategy == ApproximationInfo::FidelityDriven &&
            (opNum + 1) % approxMod == 0 &&
            approximationRuns < approximationInfo.stepNumber) {
          [[maybe_unused]] const auto sizeBefore = rootEdge.size();
          const auto apFid = approximateByFidelity(
              approximationInfo.stepFidelity, false, true);
          approximationRuns++;
          finalFidelity *= static_cast<long double>(apFid);
        } else if (approximationInfo.strategy ==
                   ApproximationInfo::MemoryDriven) {
          [[maybe_unused]] const auto sizeBefore = rootEdge.size();
          if (dd->template getUniqueTable<dd::vNode>()
                  .possiblyNeedsCollection()) {
            const auto apFid = approximateByFidelity(
                approximationInfo.stepFidelity, false, true);
            approximationRuns++;
            finalFidelity *= static_cast<long double>(apFid);
          }
        }
      }
      dd->garbageCollect();
    }
    opNum++;
  }
  return classicValues;
}
