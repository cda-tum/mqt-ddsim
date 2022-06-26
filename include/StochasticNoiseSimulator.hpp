#ifndef DDSIM_STOCHASTICNOISESIMULATOR_HPP
#define DDSIM_STOCHASTICNOISESIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "dd/NoiseFunctionality.hpp"

//#include <cstddef>
//#include <map>
//#include <memory>
//#include <string>
#include <optional>
#include <thread>
#include <vector>

class StochasticNoiseSimulator: public Simulator<StochasticNoisePackage> {
public:
    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int stepNumber, const double stepFidelity):
        qc(qc), stepNumber(stepNumber), stepFidelity(stepFidelity) {
        dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int stepNumber, const double stepFidelity, std::size_t seed):
        Simulator(seed), qc(qc), stepNumber(stepNumber), stepFidelity(stepFidelity) {
        dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc,
                             const std::string&                       cNoiseEffects,
                             double                                   cNoiseProbability,
                             std::optional<double>                    cAmpDampingProbability,
                             double                                   cMultiQubitGateFactor,
                             std::size_t                              cStochRuns,
                             const std::string&                       recordedProperties,
                             bool                                     unoptimizedSim,
                             unsigned int                             stepNumber,
                             double                                   stepFidelity,
                             std::size_t                              seed = 0U):
        Simulator(seed),
        qc(qc), stepNumber(stepNumber), stepFidelity(stepFidelity) {
        // setNoiseEffects
        for (const auto noise: cNoiseEffects) {
            switch (noise) {
                case 'A':
                    noiseEffects.push_back(dd::amplitudeDamping);
                    break;
                case 'P':
                    noiseEffects.push_back(dd::phaseFlip);
                    break;
                case 'D':
                    noiseEffects.push_back(dd::depolarization);
                    break;
                default:
                    throw std::runtime_error("Unknown noise operation '" + cNoiseEffects + "'\n");
            }
        }

        // initializeNoiseProbabilities
        noiseProbability = cNoiseProbability;
        // Default value for amplitude damping prob is double the general error probability
        amplitudeDampingProb = (cAmpDampingProbability) ? cAmpDampingProbability.value() : noiseProbability * 2;
        multiQubitGateFactor = cMultiQubitGateFactor;

        if (noiseProbability < 0 || amplitudeDampingProb < 0 || noiseProbability * multiQubitGateFactor > 1 || amplitudeDampingProb * multiQubitGateFactor > 1) {
            throw std::runtime_error("Error probabilities are faulty!"
                                     "\n single qubit error probability: " +
                                     std::to_string(cNoiseProbability) +
                                     " multi qubit error probability: " + std::to_string(cNoiseProbability * multiQubitGateFactor) +
                                     "\n single qubit amplitude damping  probability: " + std::to_string(amplitudeDampingProb) +
                                     " multi qubit amplitude damping  probability: " + std::to_string(amplitudeDampingProb * multiQubitGateFactor));
        }

        sequentiallyApplyNoise = unoptimizedSim;

        stochasticRuns = cStochRuns;

        setRecordedProperties(recordedProperties);

        dd->resize(qc->getNqubits());
    }

    std::vector<std::pair<long, std::string>>        recordedProperties;
    std::vector<std::vector<double>>                 recordedPropertiesPerInstance;
    std::vector<double>                              finalProperties;
    std::vector<std::map<std::string, unsigned int>> classicalMeasurementsMaps;
    std::map<std::string, unsigned int>              finalClassicalMeasurementsMap;

    void setRecordedProperties(const std::string& input);

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;
    std::map<std::string, double>      StochSimulate();

    [[nodiscard]] std::size_t    getMaxMatrixNodeCount() const override { return 0U; }    // Not available for stochastic simulation
    [[nodiscard]] std::size_t    getMatrixActiveNodeCount() const override { return 0U; } // Not available for stochastic simulation
    [[nodiscard]] std::size_t    countNodesFromRoot() const override { return 0U; }       // Not available for stochastic simulation
    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };
    [[nodiscard]] std::size_t    getNumberOfOps() const override { return qc->getNops(); };
    [[nodiscard]] std::string    getName() const override { return "stoch_" + qc->getName(); };

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
    double      noiseProbability       = 0.0;
    double      amplitudeDampingProb   = 0.0;
    double      multiQubitGateFactor   = 0.0;
    std::size_t stochasticRuns         = 0U;
    bool        sequentiallyApplyNoise = false;

    std::vector<dd::NoiseOperations> noiseEffects;

    const unsigned int maxInstances = std::max(1U, std::thread::hardware_concurrency() - 4U);
    //    const unsigned int maxInstances = 1; // use for debugging only

    std::unique_ptr<qc::QuantumComputation>& qc;

    const std::size_t stepNumber;
    const double      stepFidelity;
    double            approximationRuns{0};

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

#endif //DDSIM_STOCHASTICNOISESIMULATOR_HPP
