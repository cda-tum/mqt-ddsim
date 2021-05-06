#ifndef DDSIM_STOCHASTICNOISESIMULATOR_HPP
#define DDSIM_STOCHASTICNOISESIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

class StochasticNoiseSimulator: public Simulator {
public:
    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int step_number, const double step_fidelity):
        qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int step_number, const double step_fidelity, unsigned long long seed):
        Simulator(seed), qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        dd->resize(qc->getNqubits());
    }

    StochasticNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc,
                             const std::string&                       noise_effects,
                             double                                   noise_prob,
                             long                                     stoch_runs,
                             unsigned int                             step_number,
                             double                                   step_fidelity,
                             const std::string&                       recorded_properties):
        qc(qc),
        step_number(step_number), step_fidelity(step_fidelity) {
        setNoiseEffects(noise_effects);
        setRecordedProperties(recorded_properties);
        setAmplitudeDampingProbability(noise_prob);
        stochastic_runs = stoch_runs;
    }

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    std::map<std::string, double> StochSimulate();

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity", std::to_string(step_fidelity)},
                {"approximation_runs", std::to_string(approximation_runs)},
                {"final_fidelity", std::to_string(final_fidelity)},
                {"perfect_run_time", std::to_string(perfect_run_time)},
                {"stoch_wall_time", std::to_string(stoch_run_time)},
                {"mean_stoch_run_time", std::to_string(mean_stoch_time)},
                {"parallel_instances", std::to_string(max_instances)},
        };
    };

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return "stoch_" + gate_noise_types + "_" + qc->getName(); };

    dd::Package::vEdge MeasureOneCollapsingConcurrent(unsigned short index, const std::unique_ptr<dd::Package>& localDD,
                                                      dd::Package::vEdge local_root_edge,
                                                      std::mt19937_64&   generator,
                                                      char*              result,
                                                      bool               assume_probability_normalization = true);

    void setNoiseEffects(const std::string& cGateNoise) { gate_noise_types = cGateNoise; }

    double           noise_probability = 0.0;
    dd::ComplexValue sqrt_amplitude_damping_probability{};
    dd::ComplexValue one_minus_sqrt_amplitude_damping_probability{};

    void setAmplitudeDampingProbability(double cGateNoiseProbability) {
        //The probability of amplitude damping (t1) often is double the probability , of phase flip, which is why I double it here
        noise_probability                            = cGateNoiseProbability;
        sqrt_amplitude_damping_probability           = {sqrt(noise_probability * 2), 0};
        one_minus_sqrt_amplitude_damping_probability = {sqrt(1 - noise_probability * 2), 0};
    }

    double       stoch_error_margin = 0.01;
    double       stoch_confidence   = 0.05;
    unsigned int stochastic_runs    = 0;

    void setRecordedProperties(const std::string& input);

    std::vector<std::tuple<long, std::string>> recorded_properties;
    std::vector<std::vector<double>>           recorded_properties_per_instance;

    std::string gate_noise_types;

    const unsigned int max_instances = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 4);

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
                                 int                                         n_qubits,
                                 dd::Package::vEdge                          rootEdgePerfectRun,
                                 std::vector<double>&                        recordedPropertiesStorage,
                                 std::vector<std::tuple<long, std::string>>& recordedPropertiesList,
                                 unsigned long long                          local_seed);

    dd::Package::mEdge generateNoiseOperation(bool                                    amplitudeDamping,
                                              dd::Qubit                               target,
                                              std::mt19937_64&                        engine,
                                              std::uniform_real_distribution<dd::fp>& distribution,
                                              dd::Package::mEdge                      dd_operation,
                                              std::unique_ptr<dd::Package>&           localDD);

    void applyNoiseOperation(const qc::Targets&                      targets,
                             const dd::Controls&                     control_qubits,
                             dd::Package::mEdge                      dd_op,
                             std::unique_ptr<dd::Package>&           localDD,
                             dd::Package::vEdge&                     local_root_edge,
                             std::mt19937_64&                        generator,
                             std::uniform_real_distribution<dd::fp>& dist,
                             dd::Package::mEdge                      identityDD);

    [[nodiscard]] dd::NoiseOperationKind ReturnNoiseOperation(char i, double d) const;

    [[nodiscard]] std::string intToString(long target_number) const;

    double ApproximateEdgeByFidelity(std::unique_ptr<dd::Package>& localDD, dd::Package::vEdge& edge, double targetFidelity, bool allLevels, bool removeNodes);

    dd::Package::vEdge RemoveNodesInPackage(std::unique_ptr<dd::Package>& localDD, dd::Package::vEdge e, std::map<dd::Package::vNode*, dd::Package::vEdge>& dag_edges);
};

#endif //DDSIM_STOCHASTICNOISESIMULATOR_HPP
