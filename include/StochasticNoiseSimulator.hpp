#pragma once

#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "dd/NoiseFunctionality.hpp"

#include <optional>
#include <thread>
#include <vector>

template<class DDPackage = StochasticNoisePackage>
class StochasticNoiseSimulator: public Simulator<DDPackage> {
public:
    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc,
                             const std::string&                       noiseEffects,
                             double                                   noiseProbability,
                             std::optional<double>                    ampDampingProbability,
                             double                                   multiQubitGateFactor,
                             std::size_t                              stochRuns,
                             const std::string&                       recordedProperties,
                             bool                                     unoptimizedSim,
                             unsigned int                             stepNumber,
                             double                                   stepFidelity,
                             std::size_t                              seed = 0U):
        Simulator<DDPackage>(seed),
        qc(qc),
        stepNumber(stepNumber),
        stepFidelity(stepFidelity),
        noiseProbability(noiseProbability),
        amplitudeDampingProb((ampDampingProbability) ? ampDampingProbability.value() : noiseProbability * 2),
        multiQubitGateFactor(multiQubitGateFactor),
        sequentiallyApplyNoise(unoptimizedSim),
        stochasticRuns(stochRuns),
        maxInstances(std::thread::hardware_concurrency() > 4 ? std::thread::hardware_concurrency() - 4 : 1),
        noiseEffects(initializeNoiseEffects(noiseEffects)) {
        sanityCheckOfNoiseProbabilities(this->noiseProbability, this->amplitudeDampingProb, this->multiQubitGateFactor);
        setRecordedProperties(recordedProperties);
        Simulator<DDPackage>::dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int stepNumber, const double stepFidelity):
        StochasticNoiseSimulator(qc, std::string("APD"), 0.001, std::optional<double>{}, 2, 1000, std::string("0-256"), false, stepNumber, stepFidelity) {}

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int stepNumber, const double stepFidelity, std::size_t seed):
        StochasticNoiseSimulator(qc, std::string("APD"), 0.001, std::optional<double>{}, 2, 1000, std::string("0-256"), false, stepNumber, stepFidelity, seed) {}

    std::vector<std::pair<long, std::string>>        recordedProperties;
    std::vector<std::vector<double>>                 recordedPropertiesPerInstance;
    std::vector<double>                              finalProperties;
    std::vector<std::map<std::string, unsigned int>> classicalMeasurementsMaps;
    std::map<std::string, unsigned int>              finalClassicalMeasurementsMap;

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;
    std::map<std::string, double>      StochSimulate();

    [[nodiscard]] std::size_t    getMaxMatrixNodeCount() const override { return 0U; }    // Not available for stochastic simulation
    [[nodiscard]] std::size_t    getMatrixActiveNodeCount() const override { return 0U; } // Not available for stochastic simulation
    [[nodiscard]] std::size_t    countNodesFromRoot() const override { return 0U; }       // Not available for stochastic simulation
    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };
    [[nodiscard]] std::size_t    getNumberOfOps() const override { return qc->getNops(); };
    [[nodiscard]] std::string    getName() const override { return "stoch_" + qc->getName(); };

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
                    noiseOperationVector.push_back(dd::amplitudeDamping);
                    break;
                case 'P':
                    noiseOperationVector.push_back(dd::phaseFlip);
                    break;
                case 'D':
                    noiseOperationVector.push_back(dd::depolarization);
                    break;
                case 'I':
                    noiseOperationVector.push_back(dd::identity);
                    break;
                default:
                    throw std::runtime_error("Unknown noise operation '" + cNoiseEffects + "'\n");
            }
        }
        return noiseOperationVector;
    }

    void setRecordedProperties(const std::string& input);

    std::map<std::string, std::string> AdditionalStatistics() override {
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
    std::unique_ptr<qc::QuantumComputation>& qc;

    const std::size_t stepNumber{};
    const double      stepFidelity{};
    double            approximationRuns{0};

    const double       noiseProbability{};
    const double       amplitudeDampingProb{};
    const double       multiQubitGateFactor{};
    const bool         sequentiallyApplyNoise{};
    const std::size_t  stochasticRuns{};
    const unsigned int maxInstances{};

    std::vector<dd::NoiseOperations> noiseEffects;

    double perfectRunTime{};
    double stochRunTime{};
    double meanStochTime{};

    void perfectSimulationRun();

    void runStochSimulationForId(std::size_t                                stochRun,
                                 dd::Qubit                                  nQubits,
                                 std::vector<double>&                       recordedPropertiesStorage,
                                 std::vector<std::pair<long, std::string>>& recordedPropertiesList,
                                 std::map<std::string, unsigned int>&       classicalMeasurementsMap,
                                 unsigned long long                         localSeed);

    [[nodiscard]] std::string intToString(long targetNumber) const;
};
