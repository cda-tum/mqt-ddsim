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
    struct Configuration {
        // Add new strategies here
        enum class Mode {
            Sequential,
            PairwiseRecursiveGrouping,
            BracketGrouping,
            Alternating,
            Cotengra,
            GateCost
        };

        // mode to use
        Mode mode;

        // settings for the bracket size
        std::size_t bracketSize;
        // settings for the alternating and gatecost mode, which need a starting point
        std::size_t startingPoint;
        // settings for the gate costs mode
        std::list<std::size_t> gateCost;
        // random seed
        std::size_t seed;

        //Add new variables here
        explicit Configuration(const Mode mode_ = Mode::Sequential, const std::size_t bracketSize_ = 2, const std::size_t startingPoint_ = 0, std::list<std::size_t> gateCost_ = {}, const std::size_t seed_ = 0):
            mode(mode_), bracketSize(bracketSize_), startingPoint(startingPoint_), gateCost(std::move(gateCost_)), seed(seed_){};

        static Mode modeFromString(const std::string& mode) {
            if (mode == "sequential" || mode == "0") {
                return Mode::Sequential;
            }
            if (mode == "pairwise_recursive" || mode == "1") {
                return Mode::PairwiseRecursiveGrouping;
            }
            if (mode == "bracket" || mode == "2") {
                return Mode::BracketGrouping;
            }
            if (mode == "alternating" || mode == "3") {
                return Mode::Alternating;
            }
            if (mode == "cotengra" || mode == "4") {
                return Mode::Cotengra;
            }
            if (mode == "gate_cost" || mode == "5") {
                return Mode::GateCost;
            }

            throw std::invalid_argument("Invalid simulation path mode: " + mode);
        }

        static std::string modeToString(const Mode& mode) {
            switch (mode) {
                case Mode::Sequential:
                    return "sequential";
                case Mode::PairwiseRecursiveGrouping:
                    return "pairwise_recursive";
                case Mode::BracketGrouping:
                    return "bracket";
                case Mode::Alternating:
                    return "alternating";
                case Mode::Cotengra:
                    return "cotengra";
                case Mode::GateCost:
                    return "gate_cost";
                default:
                    throw std::invalid_argument("Invalid simulation path mode");
            }
        }

        [[nodiscard]] nlohmann::json json() const {
            nlohmann::json conf{};
            conf["mode"] = modeToString(mode);
            if (mode == Mode::BracketGrouping) {
                conf["bracket_size"] = bracketSize;
            } else if (mode == Mode::Alternating) {
                conf["starting_point"] = startingPoint;
            } else if (mode == Mode::GateCost) {
                conf["starting_point"] = startingPoint;
                conf["gate_cost"]      = gateCost;
            }
            if (seed != 0) {
                conf["seed"] = seed;
            }
            return conf;
        }

        [[nodiscard]] std::string toString() const {
            return json().dump(2);
        }
    };

    explicit StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const double stepFidelity, const unsigned int stepNumber, const std::string& approximationStrategy, const std::int64_t seed, Configuration configuration = Configuration()):
        StochasticNoiseSimulator<Config>(std::move(qc_), 1, 1) {}

    explicit StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, Configuration configuration = Configuration()):
        StochasticNoiseSimulator<Config>(std::move(qc_), 1, 1) {}

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                             const std::string&                        noiseEffects_,
                             double                                    noiseProbability_,
                             std::optional<double>                     ampDampingProbability,
                             double                                    multiQubitGateFactor_,
                             std::size_t                               stochRuns,
                             const std::string&                        recordedProperties_,
                             bool                                      unoptimizedSim,
                             std::uint32_t                             stepNumber_,
                             double                                    stepFidelity_,
                             std::size_t                               seed_ = 0U):
        Simulator<Config>(seed_),
        qc(std::move(qc_)),
        stepNumber(stepNumber_),
        stepFidelity(stepFidelity_),
        noiseProbability(noiseProbability_),
        amplitudeDampingProb((ampDampingProbability) ? ampDampingProbability.value() : noiseProbability_ * 2),
        multiQubitGateFactor(multiQubitGateFactor_),
        sequentiallyApplyNoise(unoptimizedSim),
        stochasticRuns(stochRuns),
        maxInstances(std::thread::hardware_concurrency() > 4 ? std::thread::hardware_concurrency() - 4 : 1),
        noiseEffects(initializeNoiseEffects(noiseEffects_)) {
        sanityCheckOfNoiseProbabilities(noiseProbability, amplitudeDampingProb, multiQubitGateFactor);
        setRecordedProperties(recordedProperties_);
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const unsigned int stepNumber_, const double stepFidelity_):
        StochasticNoiseSimulator(std::move(qc_), std::string("APD"), 0.001, std::optional<double>{}, 2, 1000, std::string("0-256"), false, stepNumber_, stepFidelity_) {}

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const unsigned int stepNumber_, const double stepFidelity_, std::size_t seed_):
        StochasticNoiseSimulator(std::move(qc_), std::string("APD"), 0.001, std::optional<double>{}, 2, 1000, std::string("0-256"), false, stepNumber_, stepFidelity_, seed_) {}

    std::vector<std::pair<std::int64_t, std::string>> recordedProperties;
    std::vector<std::vector<double>>                  recordedPropertiesPerInstance;
    std::vector<double>                               finalProperties;
    std::vector<std::map<std::string, unsigned int>>  classicalMeasurementsMaps;
    std::map<std::string, unsigned int>               finalClassicalMeasurementsMap;

    std::map<std::string, std::size_t> simulate(std::size_t shots) override;
    std::map<std::string, double>      stochSimulate();

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

    void runStochSimulationForId(std::size_t                                        stochRun,
                                 qc::Qubit                                          nQubits,
                                 std::vector<double>&                               recordedPropertiesStorage,
                                 std::vector<std::pair<std::int64_t, std::string>>& recordedPropertiesList,
                                 std::map<std::string, unsigned int>&               classicalMeasurementsMap,
                                 std::uint64_t                                      localSeed);

    [[nodiscard]] std::string intToString(std::int64_t targetNumber) const;
};
