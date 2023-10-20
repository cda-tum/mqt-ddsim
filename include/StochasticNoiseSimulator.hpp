#pragma once

#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "dd/NoiseFunctionality.hpp"
#include "nlohmann/json.hpp"

#include <future>
#include <limits>
#include <optional>
#include <taskflow/taskflow.hpp>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

template<class Config = StochasticNoiseSimulatorDDPackageConfig>
class StochasticNoiseSimulator: public Simulator<Config> {
public:
    //    struct Configuration {
    //        std::string           noiseEffects;
    //        double                noiseProbability;
    //        std::optional<double> ampDampingProbability;
    //        double                multiQubitGateFactor;
    //        std::size_t           stochRuns;
    //        std::string           recordedProperties;
    //        bool                  unoptimizedSim;
    //        std::size_t           seed;
    //
    //        //Add new variables here
    //        explicit Configuration(std::string           noiseEffects_          = "APD",
    //                               double                noiseProbability_      = 0.001,
    //                               std::optional<double> ampDampingProbability_ = 0.002,
    //                               double                multiQubitGateFactor_  = 2,
    //                               std::size_t           stochRuns_             = 1000,
    //                               std::string           recordedProperties_    = "0-256",
    //                               bool                  unoptimizedSim_        = false,
    //                               std::size_t           seed_                  = 0U):
    //            noiseEffects(noiseEffects_),
    //            noiseProbability(noiseProbability_),
    //            ampDampingProbability(ampDampingProbability_),
    //            multiQubitGateFactor(multiQubitGateFactor_),
    //            recordedProperties(recordedProperties_),
    //            stochRuns(stochRuns_),
    //            unoptimizedSim(unoptimizedSim_),
    //            seed(seed_){};
    //    };

    explicit StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                                      std::string                               noiseEffects_          = "APD",
                                      double                                    noiseProbability_      = 0.001,
                                      std::optional<double>                     ampDampingProbability_ = 0.002,
                                      double                                    multiQubitGateFactor_  = 2,
                                      std::size_t                               stochRuns_             = 1000,
                                      std::string                               recordedProperties_    = "0-256",
                                      bool                                      unoptimizedSim_        = false,
                                      std::size_t                               seed_                  = 0U,
                                      const unsigned int                        stepNumber_            = 1,
                                      const double                              stepFidelity_          = 1.0):
        Simulator<Config>(seed_),
        qc(std::move(qc_)),
        stepNumber(stepNumber_),
        stepFidelity(stepFidelity_),
        approximationStrategy("fidelity"),
        noiseProbability(noiseProbability_),
        amplitudeDampingProb((ampDampingProbability_) ? ampDampingProbability_.value() : noiseProbability_ * 2),
        multiQubitGateFactor(multiQubitGateFactor_),
        sequentiallyApplyNoise(unoptimizedSim_),
        stochasticRuns(stochRuns_),
        maxInstances(1),
        //        maxInstances(std::thread::hardware_concurrency() > 4 ? std::thread::hardware_concurrency() - 4 : 1),
        noiseEffects(initializeNoiseEffects(noiseEffects_)) {
        sanityCheckOfNoiseProbabilities(noiseProbability_, amplitudeDampingProb, multiQubitGateFactor_);
        setRecordedProperties(recordedProperties_);
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const unsigned int stepNumber_, const double stepFidelity_):
        StochasticNoiseSimulator(std::move(qc_), std::string("APD"), 0.001, 0.002, 2, 1000, std::string("0-256"), false, 0, stepNumber_, stepFidelity_) {}

    //    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
    //                             const std::string& noiseEffects_,
    //                             double                noiseProbability_,
    //                             std::optional<double> ampDampingProbability_,
    //                             double                multiQubitGateFactor_,
    //                             std::size_t           stochRuns_,
    //                             const std::string&    recordedProperties_,
    //                             bool                  unoptimizedSim_,
    //                             std::size_t           seed_):
    //        StochasticNoiseSimulator(std::move(qc_), {noiseEffects_, noiseProbability_, ampDampingProbability_, multiQubitGateFactor_, stochRuns_, recordedProperties_, unoptimizedSim_, seed_}) {}

    //    explicit StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, stepNumner_, stepFidelity_ = 0.1):
    //        StochasticNoiseSimulator(std::move(qc_), std::string("APD"), 0.001, 0.002, 2, 1000, std::string("0-256"), false, stepNumber_, stepFidelity_) {}

    //    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, std::size_t seed_):
    //        StochasticNoiseSimulator(std::move(qc_), Configuration{std::string("APD"), 0.001, 0.002, 2, 1000, std::string("0-256"), false, seed_}) {}

    std::vector<std::pair<std::int64_t, std::string>> recordedProperties;
    std::vector<std::vector<double>>                  recordedPropertiesPerInstance;
    std::vector<double>                               finalProperties;
    std::vector<std::map<std::string, unsigned int>>  classicalMeasurementsMaps;
    std::map<std::string, size_t>                     finalClassicalMeasurementsMap;

    std::map<std::string, std::size_t> simulate(std::size_t shots) override;
    std::string                        measureAll(bool collapse = false) {
        throw std::runtime_error("Not supported for stochastic simulation!\n");
    }

    char measureOneCollapsing(const qc::Qubit index, const bool assumeProbabilityNormalization = true) {
        throw std::runtime_error("Not supported for stochastic simulation!\n");
    }

    std::map<std::string, std::size_t> stochSimulate();

    [[nodiscard]] std::size_t getMaxMatrixNodeCount() const override { return 0U; }    // Not available for stochastic simulation
    [[nodiscard]] std::size_t getMatrixActiveNodeCount() const override { return 0U; } // Not available for stochastic simulation
    [[nodiscard]] std::size_t countNodesFromRoot() override { return 0U; }             // Not available for stochastic simulation
    [[nodiscard]] std::size_t getNumberOfQubits() const override { return qc->getNqubits(); };
    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };
    [[nodiscard]] std::string getName() const override { return "stoch_" + qc->getName(); };

    [[maybe_unused]] static void sanityCheckOfNoiseProbabilities(double noiseProbability, double amplitudeDampingProb, double multiQubitGateFactor) {
        if (noiseProbability < 0 || amplitudeDampingProb < 0 || noiseProbability * multiQubitGateFactor > 1 || amplitudeDampingProb * multiQubitGateFactor > 1) {
            throw std::runtime_error("Error probabilities are faulty!"
                                     "\n single qubit error probability: " +
                                     std::to_string(noiseProbability) +
                                     " multi qubit error probability: " + std::to_string(noiseProbability * multiQubitGateFactor) +
                                     "\n single qubit amplitude damping  probability: " + std::to_string(amplitudeDampingProb) +
                                     " multi qubit amplitude damping  probability: " + std::to_string(amplitudeDampingProb * multiQubitGateFactor));
        }
    }

    [[maybe_unused]] static std::vector<dd::NoiseOperations> initializeNoiseEffects(const std::string& cNoiseEffects) {
        std::vector<dd::NoiseOperations> noiseOperationVector{};
        for (const auto noise: cNoiseEffects) {
            switch (noise) {
                case 'A':
                    noiseOperationVector.push_back(dd::AmplitudeDamping);
                    break;
                case 'P':
                    noiseOperationVector.push_back(dd::PhaseFlip);
                    break;
                case 'D':
                    noiseOperationVector.push_back(dd::Depolarization);
                    break;
                case 'I':
                    noiseOperationVector.push_back(dd::Identity);
                    break;
                default:
                    throw std::runtime_error("Unknown noise operation '" + cNoiseEffects + "'\n");
            }
        }
        return noiseOperationVector;
    }

    void setRecordedProperties(const std::string& input);

    std::map<std::string, std::string> additionalStatistics() override {
        return {
                {"step_fidelity", std::to_string(stepFidelity)},
                {"approximation_runs", std::to_string(approximationRuns)},
                {"perfect_run_time", std::to_string(perfectRunTime)},
                {"stoch_wall_time", std::to_string(stochRunTime)},
                {"mean_stoch_run_time", std::to_string(meanStochTime)},
                {"parallel_instances", std::to_string(maxInstances)},
                {"stoch_runs", std::to_string(stochasticRuns)},
                {"threads", std::to_string(maxInstances)},
        };
    };

private:
    std::unique_ptr<qc::QuantumComputation> qc;

    std::size_t stepNumber{};
    double      stepFidelity{};
    std::string approximationStrategy{};
    double      approximationRuns{0};

    double       noiseProbability{};
    double       amplitudeDampingProb{};
    double       multiQubitGateFactor{};
    bool         sequentiallyApplyNoise{};
    std::size_t  stochasticRuns{};
    unsigned int maxInstances{};

    std::vector<dd::NoiseOperations> noiseEffects;

    double perfectRunTime{};
    double stochRunTime{};
    double meanStochTime{};

    void perfectSimulationRun();

    void runStochSimulationForId(std::size_t stochRun, qc::Qubit nQubits, std::map<std::string, unsigned int>& classicalMeasurementsMap, std::uint64_t localSeed);

    [[nodiscard]] std::string intToString(std::int64_t targetNumber) const;
};
