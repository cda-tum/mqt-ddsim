#ifndef DDSIM_CIRCUITSIMULATOR_HPP
#define DDSIM_CIRCUITSIMULATOR_HPP

#include "Definitions.hpp"
#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/DDpackageConfig.hpp"
#include "operations/NonUnitaryOperation.hpp"
#include "operations/Operation.hpp"

#include <cstddef>
#include <cstdint>
#include <istream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

struct ApproximationInfo {
  enum ApproximationStrategy : std::uint8_t { FidelityDriven, MemoryDriven };

  /* Default to no approximation */
  ApproximationInfo() = default;
  ApproximationInfo(const double stepFidelity_, const std::size_t stepNumber_,
                    const ApproximationStrategy strategy_)
      : stepFidelity(stepFidelity_), stepNumber(stepNumber_),
        strategy(strategy_) {}

  static ApproximationStrategy fromString(const std::string& str) {
    if (str == "fidelity") {
      return FidelityDriven;
    }
    if (str == "memory") {
      return MemoryDriven;
    }

    throw std::runtime_error("Unknown approximation strategy '" + str + "'.");
  }

  friend std::istream& operator>>(std::istream& in,
                                  ApproximationStrategy& strategy_) {
    std::string token;
    in >> token;
    strategy_ = fromString(token);
    return in;
  }

  double stepFidelity = 1.;
  std::size_t stepNumber = 1;
  ApproximationStrategy strategy = FidelityDriven;
};

template <class Config = dd::DDPackageConfig>
class CircuitSimulator : public Simulator<Config> {
public:
  explicit CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_)
      : qc(std::move(qc_)) {
    Simulator<Config>::dd->resize(qc->getNqubits());
  }

  CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                   const std::uint64_t seed_)
      : Simulator<Config>(seed_), qc(std::move(qc_)) {
    Simulator<Config>::dd->resize(qc->getNqubits());
  }

  CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                   const ApproximationInfo& approximationInfo_)
      : qc(std::move(qc_)), approximationInfo(approximationInfo_) {
    Simulator<Config>::dd->resize(qc->getNqubits());
  }

  CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                   const ApproximationInfo& approximationInfo_,
                   const std::uint64_t seed_)
      : Simulator<Config>(seed_), qc(std::move(qc_)),
        approximationInfo(approximationInfo_) {
    Simulator<Config>::dd->resize(qc->getNqubits());
  }

  std::map<std::string, std::size_t>
  measureAllNonCollapsing(std::size_t shots) override {
    return Simulator<Config>::measureAllNonCollapsing(shots);
  }

  std::map<std::string, std::size_t> simulate(std::size_t shots) override;

  virtual dd::fp expectationValue(const qc::QuantumComputation& observable);

  std::map<std::string, std::string> additionalStatistics() override {
    return {
        {"step_fidelity", std::to_string(approximationInfo.stepFidelity)},
        {"approximation_runs", std::to_string(approximationRuns)},
        {"final_fidelity", std::to_string(finalFidelity)},
        {"single_shots", std::to_string(singleShots)},
    };
  };

  [[nodiscard]] std::size_t getNumberOfQubits() const override {
    return qc->getNqubits();
  };

  [[nodiscard]] std::size_t getNumberOfOps() const override {
    return qc->getNops();
  };

  [[nodiscard]] std::string getName() const override { return qc->getName(); };

protected:
  std::unique_ptr<qc::QuantumComputation> qc;
  std::size_t singleShots{0};

  ApproximationInfo approximationInfo;
  std::size_t approximationRuns{0};
  long double finalFidelity{1.0L};

  struct CircuitAnalysis {
    bool isDynamic = false;
    bool hasMeasurements = false;
    std::map<qc::Qubit, size_t> measurementMap;
  };

  CircuitAnalysis analyseCircuit();

  virtual std::map<std::size_t, bool> singleShot(bool ignoreNonUnitaries);
  virtual void initializeSimulation(std::size_t nQubits);
  virtual char measure(dd::Qubit i);

  virtual void reset(qc::NonUnitaryOperation* nonUnitaryOp);
  virtual void applyOperationToState(std::unique_ptr<qc::Operation>& op);
};

#endif // DDSIM_CIRCUITSIMULATOR_HPP
