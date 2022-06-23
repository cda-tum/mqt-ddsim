#ifndef DDSIM_STOCHASTICNOISESIMULATOR_HPP
#define DDSIM_STOCHASTICNOISESIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "dd/NoiseFunctionality.hpp"

//#include <cstddef>
//#include <map>
//#include <memory>
//#include <string>
#include <thread>
#include <vector>

class StochasticNoiseSimulator: public Simulator<StochasticNoiseSimulatorDDPackage> {
public:
    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int step_number, const double step_fidelity):
        qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int step_number, const double step_fidelity, std::size_t seed):
        Simulator(seed), qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc,
                             const std::string&                       cNoiseEffects,
                             double                                   cGateNoiseProbability,
                             double                                   cAmplitudeDampingProb,
                             double                                   cMultiQubitGateFactor,
                             std::size_t                              cStochRuns,
                             const std::string&                       recorded_properties,
                             bool                                     unoptimizedSim,
                             unsigned int                             step_number,
                             double                                   step_fidelity,
                             std::size_t                              seed = 0):
        Simulator(seed),
        qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        // setNoiseEffects
        for (auto noise: cNoiseEffects) {
            switch (noise) {
                case 'A':
                    gateNoiseEffects.push_back(dd::amplitudeDamping);
                    break;
                case 'P':
                    gateNoiseEffects.push_back(dd::phaseFlip);
                    break;
                case 'D':
                    gateNoiseEffects.push_back(dd::depolarization);
                    break;
                default:
                    throw std::runtime_error("Unknown noise operation '" + cNoiseEffects + "'\n");
            }
        }

        // initializeNoiseProbabilities
        noiseProbability = cGateNoiseProbability;
        // Default value for amplitude damping prob is double the general error probability
        amplitudeDampingProb = (cAmplitudeDampingProb < 0) ? noiseProbability * 2 : cAmplitudeDampingProb;
        multiQubitGateFactor = cMultiQubitGateFactor;

        if ((amplitudeDampingProb * multiQubitGateFactor) > 1 || noiseProbability < 0) {
            throw std::runtime_error("Error probabilities are faulty!"
                                     "\n single qubit error probability: " +
                                     std::to_string(cGateNoiseProbability) +
                                     " multi qubit error probability: " + std::to_string(cGateNoiseProbability * multiQubitGateFactor) +
                                     "\n single qubit amplitude damping  probability: " + std::to_string(amplitudeDampingProb) +
                                     " multi qubit amplitude damping  probability: " + std::to_string(amplitudeDampingProb * multiQubitGateFactor));
        }

        sequentialApplyNoise = unoptimizedSim;

        // initializeNoiseProbabilities
        if (amplitudeDampingProb < 0) {
            // Default value for amplitude damping prob is double the general error probability
            amplitudeDampingProb = cGateNoiseProbability * 2;
        }

        stochasticRuns = cStochRuns;

        setRecordedProperties(recorded_properties);
    }

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    std::map<std::string, double> StochSimulate();

    [[nodiscard]] std::size_t getMaxMatrixNodeCount() const override { return 0; } // Not available for stochastic simulation

    [[nodiscard]] std::size_t getMatrixActiveNodeCount() const override { return 0; } // Not available for stochastic simulation

    [[nodiscard]] std::size_t countNodesFromRoot() const override { return 0; } // Not available for stochastic simulation

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity", std::to_string(step_fidelity)},
                {"approximation_runs", std::to_string(approximation_runs)},
                {"perfect_run_time", std::to_string(perfect_run_time)},
                {"stoch_wall_time", std::to_string(stoch_run_time)},
                {"mean_stoch_run_time", std::to_string(mean_stoch_time)},
                {"parallel_instances", std::to_string(max_instances)},
        };
    };

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return "stoch_" + qc->getName(); };

    double noiseProbability     = 0.0;
    double amplitudeDampingProb = 0.0;
    double multiQubitGateFactor = 0.0;

    std::size_t stochasticRuns       = 0;
    bool        sequentialApplyNoise = false;

    void setRecordedProperties(const std::string& input);

    std::vector<std::tuple<long, std::string>> recorded_properties;
    std::vector<std::vector<double>>           recorded_properties_per_instance;
    std::vector<std::map<std::string, int>>    classical_measurements_maps;

    std::vector<dd::noiseOperations> gateNoiseEffects;

    const unsigned int max_instances = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 4);
    //    const unsigned int max_instances = 1; // use for debugging only

private:
    std::unique_ptr<qc::QuantumComputation>& qc;

    const unsigned int step_number;
    const double       step_fidelity;
    double             approximation_runs{0};
    long double        final_fidelity{1.0L};

    float  perfect_run_time{0};
    float  stoch_run_time{0};
    double mean_stoch_time{0};

    void perfect_simulation_run();

    void runStochSimulationForId(unsigned int                                stochRun,
                                 dd::Qubit                                   nQubits,
                                 std::vector<double>&                        recordedPropertiesStorage,
                                 std::vector<std::tuple<long, std::string>>& recordedPropertiesList,
                                 std::map<std::string, int>&                 classicalMeasurementsMap,
                                 unsigned long long                          localSeed);

    [[nodiscard]] std::string intToString(long target_number) const;

    void setMeasuredQubitToZero(signed char& at, dd::vEdge& e, std::unique_ptr<StochasticNoiseSimulatorDDPackage>& localDD) const;
};

#endif //DDSIM_STOCHASTICNOISESIMULATOR_HPP
