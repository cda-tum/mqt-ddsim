#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"
#include "dd/Operations.hpp"

using CN = dd::ComplexNumbers;

void DeterministicNoiseSimulator::initializeSimulation(
    const std::size_t nQubits) {
  rootEdge = dd->makeZeroDensityOperator(static_cast<dd::Qubit>(nQubits));
  dd->incRef(DeterministicNoiseSimulator::rootEdge);
}

void DeterministicNoiseSimulator::applyOperationToState(
    std::unique_ptr<qc::Operation>& op) {
  auto operation = dd::getDD(op.get(), *Simulator::dd);
  dd->applyOperationToDensity(DeterministicNoiseSimulator::rootEdge, operation);
  deterministicNoiseFunctionality.applyNoiseEffects(
      DeterministicNoiseSimulator::rootEdge, op);
}

char DeterministicNoiseSimulator::measure(const dd::Qubit i) {
  return Simulator::dd->measureOneCollapsing(
      rootEdge, static_cast<dd::Qubit>(i), Simulator::mt);
}

void DeterministicNoiseSimulator::reset(qc::NonUnitaryOperation* nonUnitaryOp) {
  for (const auto& qubit : nonUnitaryOp->getTargets()) {
    auto const result =
        dd->measureOneCollapsing(rootEdge, static_cast<dd::Qubit>(qubit), mt);
    if (result == '1') {
      const auto x = qc::StandardOperation(qubit, qc::X);
      const auto operation = dd::getDD(&x, *dd);
      rootEdge = dd->applyOperationToDensity(rootEdge, operation);
    }
  }
}

std::map<std::string, std::size_t>
DeterministicNoiseSimulator::sampleFromProbabilityMap(
    const dd::SparsePVecStrKeys& resultProbabilityMap,
    const std::size_t shots) {
  std::vector<dd::fp> weights;
  weights.reserve(resultProbabilityMap.size());
  for (const auto& [state, prob] : resultProbabilityMap) {
    weights.emplace_back(prob);
  }
  std::discrete_distribution<std::size_t> d(
      weights.begin(),
      weights.end()); // NOLINT(misc-const-correctness) false-positive

  // Create the final map containing the measurement results and the
  // corresponding shots
  std::map<std::string, std::size_t> results;
  for (size_t n = 0; n < shots; ++n) {
    const auto sampleIdx = d(mt);
    const auto state = (std::next(resultProbabilityMap.begin(),
                                  static_cast<std::int64_t>(sampleIdx)))
                           ->first;
    results[state] += 1;
  }

  return results;
}
