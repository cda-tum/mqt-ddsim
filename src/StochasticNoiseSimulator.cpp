#include "StochasticNoiseSimulator.hpp"

#include "dd/DDDefinitions.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Node.hpp"
#include "dd/Operations.hpp"
#include "dd/Package.hpp"
#include "ir/Definitions.hpp"
#include "ir/operations/ClassicControlledOperation.hpp"
#include "ir/operations/NonUnitaryOperation.hpp"
#include "ir/operations/OpType.hpp"

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

std::map<std::string, std::size_t>
StochasticNoiseSimulator::simulate(const size_t nshots) {
  stochasticRuns = nshots;
  classicalMeasurementsMaps.resize(maxInstances);
  std::vector<std::thread> threadArray;
  // The stochastic runs are applied in parallel
  const auto t1Stoch = std::chrono::steady_clock::now();
  for (std::size_t runID = 0U; runID < maxInstances; runID++) {
    threadArray.emplace_back(&StochasticNoiseSimulator::runStochSimulationForId,
                             this, runID, getNumberOfQubits(),
                             std::ref(classicalMeasurementsMaps[runID]), mt());
  }
  // wait for threads to finish
  for (auto& thread : threadArray) {
    thread.join();
  }
  const auto t2Stoch = std::chrono::steady_clock::now();
  stochRunTime = std::chrono::duration<double>(t2Stoch - t1Stoch).count();

  for (const auto& classicalMeasurementsMap : classicalMeasurementsMaps) {
    for (const auto& [state, count] : classicalMeasurementsMap) {
      finalClassicalMeasurementsMap[state] += count;
    }
  }

  return finalClassicalMeasurementsMap;
}

void StochasticNoiseSimulator::runStochSimulationForId(
    std::size_t stochRun, qc::Qubit nQubits,
    std::map<std::string, size_t>& classicalMeasurementsMap,
    std::uint64_t localSeed) {
  std::mt19937_64 generator(localSeed);

  const std::uint64_t numberOfRuns =
      stochasticRuns / maxInstances +
      (stochRun < stochasticRuns % maxInstances ? 1U : 0U);
  const auto approxMod = static_cast<unsigned>(
      std::ceil(static_cast<double>(qc->getNops()) /
                (static_cast<double>(approximationInfo.stepNumber + 1))));

  for (std::size_t currentRun = 0U; currentRun < numberOfRuns; currentRun++) {
    auto localDD = std::make_unique<dd::Package>(
        getNumberOfQubits(), dd::STOCHASTIC_NOISE_SIMULATOR_DD_PACKAGE_CONFIG);
    auto stochasticNoiseFunctionality = dd::StochasticNoiseFunctionality(
        *localDD, static_cast<dd::Qubit>(nQubits), noiseProbability,
        amplitudeDampingProb, multiQubitGateFactor, noiseEffects);

    std::vector<bool> classicValues(qc->getNcbits(), false);

    std::size_t opCount = 0U;

    auto localRootEdge =
        localDD->makeZeroState(static_cast<dd::Qubit>(nQubits));
    for (auto& op : *qc) {
      if (op->getType() == qc::Barrier) {
        continue;
      }
      ++opCount;
      if (const auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get());
          nuOp != nullptr) {
        if (nuOp->getType() == qc::Measure) {
          localRootEdge = applyMeasurement(*nuOp, localRootEdge, *localDD,
                                           generator, classicValues);
        } else if (nuOp->getType() == qc::Reset) {
          localRootEdge = applyReset(*nuOp, localRootEdge, *localDD, generator);
        } else {
          throw std::runtime_error("Unsupported non-unitary functionality.");
        }
        continue;
      }
      dd::mEdge operation;
      if (op->isClassicControlledOperation()) {
        // Check if the operation is controlled by a classical register
        const auto& classicOp =
            dynamic_cast<const qc::ClassicControlledOperation&>(*op);
        localRootEdge = applyClassicControlledOperation(
            classicOp, localRootEdge, *localDD, classicValues);
        continue;
      }
      const auto& targets = op->getTargets();
      const auto& controls = op->getControls();

      if (targets.size() == 1 && controls.empty()) {
        const auto* oper = localDD->stochasticNoiseOperationCache.lookup(
            op->getType(), static_cast<dd::Qubit>(targets.front()));
        if (oper == nullptr) {
          operation = getDD(*op, *localDD);
          localDD->stochasticNoiseOperationCache.insert(
              op->getType(), static_cast<dd::Qubit>(targets.front()),
              operation);
        } else {
          operation = *oper;
        }
      } else {
        operation = getDD(*op, *localDD);
      }

      stochasticNoiseFunctionality.applyNoiseOperation(
          op->getUsedQubits(), operation, localRootEdge, generator);
      if (approximationInfo.stepFidelity < 1. && (opCount % approxMod == 0U)) {
        approximateByFidelity(localDD, localRootEdge,
                              approximationInfo.stepFidelity, false, true);
        ++approximationRuns;
      }
      localDD->garbageCollect();
    }
    localDD->decRef(localRootEdge);

    if (!classicValues.empty()) {
      const auto cbits = qc->getNcbits();
      std::string classicRegisterString(cbits, '0');

      for (std::size_t i = 0; i < cbits; ++i) {
        classicRegisterString[cbits - i - 1] = classicValues[i] ? '1' : '0';
      }
      classicalMeasurementsMap[classicRegisterString] += 1U;
    }
  }
}

std::map<std::string, std::string>
StochasticNoiseSimulator::additionalStatistics() {
  return {
      {"approximation_runs", std::to_string(approximationRuns)},
      {"stoch_wall_time", std::to_string(stochRunTime)},
      {"stoch_runs", std::to_string(stochasticRuns)},
      {"threads", std::to_string(maxInstances)},
  };
}
