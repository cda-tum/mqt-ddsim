#include "StochasticNoiseSimulator.hpp"

#include "Definitions.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Operations.hpp"
#include "dd/Package.hpp"
#include "operations/ClassicControlledOperation.hpp"
#include "operations/NonUnitaryOperation.hpp"
#include "operations/OpType.hpp"

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
    auto localDD = std::make_unique<
        dd::Package<dd::StochasticNoiseSimulatorDDPackageConfig>>(
        getNumberOfQubits());
    auto stochasticNoiseFunctionality = dd::StochasticNoiseFunctionality(
        localDD, static_cast<dd::Qubit>(nQubits), noiseProbability,
        amplitudeDampingProb, multiQubitGateFactor, noiseEffects);

    std::map<std::size_t, bool> classicValues;

    std::size_t opCount = 0U;

    dd::vEdge localRootEdge =
        localDD->makeZeroState(static_cast<dd::Qubit>(nQubits));
    localDD->incRef(localRootEdge);

    for (auto& op : *qc) {
      if (op->getType() == qc::Barrier) {
        continue;
      }
      ++opCount;
      if (auto* nuOp = dynamic_cast<qc::NonUnitaryOperation*>(op.get());
          nuOp != nullptr) {
        if (nuOp->getType() == qc::Measure) {
          const auto& quantum = nuOp->getTargets();
          const auto& classic = nuOp->getClassics();

          assert(
              quantum.size() ==
              classic.size()); // this should not happen do to check in Simulate

          for (std::size_t i = 0U; i < quantum.size(); ++i) {
            const auto result = localDD->measureOneCollapsing(
                localRootEdge, static_cast<dd::Qubit>(quantum.at(i)), true,
                generator);
            assert(result == '0' || result == '1');
            classicValues[classic.at(i)] = (result == '1');
          }
        } else if (nuOp->getType() == qc::Reset) {
          // Reset qubit
          const auto& qubits = nuOp->getTargets();
          for (const auto& qubit : qubits) {
            const auto result = localDD->measureOneCollapsing(
                localRootEdge, static_cast<dd::Qubit>(qubits.at(qubit)), true,
                generator);
            if (result == '1') {
              const auto x = qc::StandardOperation(qubit, qc::X);
              auto tmp =
                  localDD->multiply(dd::getDD(&x, *localDD), localRootEdge);
              localDD->incRef(tmp);
              localDD->decRef(localRootEdge);
              localRootEdge = tmp;
              localDD->garbageCollect();
            }
          }
        } else {
          throw std::runtime_error("Unsupported non-unitary functionality.");
        }
        continue;
      }
      dd::mEdge operation;
      if (op->isClassicControlledOperation()) {
        // Check if the operation is controlled by a classical register
        auto* classicOp =
            dynamic_cast<qc::ClassicControlledOperation*>(op.get());
        if (classicOp == nullptr) {
          throw std::runtime_error(
              "Dynamic cast to ClassicControlledOperation* failed.");
        }
        bool executeOp = true;
        auto expValue = classicOp->getExpectedValue();

        for (auto i = classicOp->getControlRegister().first;
             i < classicOp->getControlRegister().second; i++) {
          if (static_cast<std::uint64_t>(classicValues[i]) != (expValue % 2U)) {
            executeOp = false;
            break;
          }
          expValue = expValue >> 1U;
        }
        operation = dd::getDD(classicOp->getOperation(), *localDD);
        if (!executeOp) {
          continue;
        }
      } else {
        const auto& targets = op->getTargets();
        const auto& controls = op->getControls();

        if (targets.size() == 1 && controls.empty()) {
          auto* oper = localDD->stochasticNoiseOperationCache.lookup(
              op->getType(), static_cast<dd::Qubit>(targets.front()));
          if (oper == nullptr) {
            operation = dd::getDD(op.get(), *localDD);
            localDD->stochasticNoiseOperationCache.insert(
                op->getType(), static_cast<dd::Qubit>(targets.front()),
                operation);
          } else {
            operation = *oper;
          }
        } else {
          operation = dd::getDD(op.get(), *localDD);
        }
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

      for (const auto& [bitIndex, value] : classicValues) {
        classicRegisterString[cbits - bitIndex - 1] = value ? '1' : '0';
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
