#include "UnitarySimulator.hpp"

#include "CircuitSimulator.hpp"
#include "circuit_optimizer/CircuitOptimizer.hpp"
#include "dd/Export.hpp"
#include "dd/FunctionalityConstruction.hpp"
#include "dd/Node.hpp"
#include "ir/QuantumComputation.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <utility>

void UnitarySimulator::construct() {
  // carry out actual computation
  auto start = std::chrono::steady_clock::now();
  if (mode == Mode::Sequential) {
    e = dd::buildFunctionality(*qc, *dd);
  } else if (mode == Mode::Recursive) {
    e = dd::buildFunctionalityRecursive(*qc, *dd);
  }
  auto end = std::chrono::steady_clock::now();
  constructionTime = std::chrono::duration<double>(end - start).count();
}

void UnitarySimulator::exportDDtoGraphviz(std::ostream& os, const bool colored,
                                          const bool edgeLabels,
                                          const bool classic, const bool memory,
                                          const bool formatAsPolar) {
  dd::toDot(e, os, colored, edgeLabels, classic, memory, formatAsPolar);
}

UnitarySimulator::UnitarySimulator(
    std::unique_ptr<qc::QuantumComputation>&& qc_,
    const ApproximationInfo& approximationInfo_, UnitarySimulator::Mode simMode)
    : CircuitSimulator(std::move(qc_), approximationInfo_), mode(simMode) {
  // remove final measurements
  qc::CircuitOptimizer::removeFinalMeasurements(*qc);
}

UnitarySimulator::UnitarySimulator(
    std::unique_ptr<qc::QuantumComputation>&& qc_,
    UnitarySimulator::Mode simMode)
    : UnitarySimulator(std::move(qc_), {}, simMode) {}

UnitarySimulator::UnitarySimulator(
    std::unique_ptr<qc::QuantumComputation>&& qc_,
    const ApproximationInfo& approximationInfo_, const std::uint64_t seed_,
    const UnitarySimulator::Mode simMode)
    : CircuitSimulator(std::move(qc_), approximationInfo_, seed_),
      mode(simMode) {
  // remove final measurements
  qc::CircuitOptimizer::removeFinalMeasurements(*qc);
}

std::size_t UnitarySimulator::getMaxNodeCount() const {
  return dd->getUniqueTable<dd::mNode>().getPeakNumActiveEntries();
}
