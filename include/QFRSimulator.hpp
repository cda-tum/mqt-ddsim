#ifndef DDSIM_QFRSIMULATOR_HPP
#define DDSIM_QFRSIMULATOR_HPP

#define MAXIMUM_NUMBER_OF_CORES 96
// Define the maximum number of core that are used for concurrent simulation

#include "Simulator.hpp"
#include "QuantumComputation.hpp"

class QFRSimulator : public Simulator {
public:
    QFRSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const unsigned int step_number, const double step_fidelity)
            : qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        if (step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
    }

    QFRSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const unsigned int step_number, const double step_fidelity, unsigned long long seed)
            : Simulator(seed), qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        if (step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
    }

    QFRSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const std::string& noise_effects, double noise_prob, long stoch_runs, const unsigned int step_number, const double step_fidelity, std::string recorded_propeteis)
            : qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        if (step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
        setNoiseEffects((char*) noise_effects.data());
        setRecordedProperties(recorded_propeteis);
        setAmplitudeDampingProbability(noise_prob);
        stochastic_runs = stoch_runs;
    }

    std::map<std::string, unsigned int> Simulate(unsigned int shots) override;
    std::map<std::string, double> StochSimulate() override;

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity",              std::to_string(step_fidelity)},
                {"approximation_runs",         std::to_string(approximation_runs)},
                {"final_fidelity",             std::to_string(final_fidelity)},
        };
    };


    unsigned short getNumberOfQubits() const override { return qc->getNqubits(); };
    unsigned long getNumberOfOps() const override { return qc->getNops(); };
    std::string getName() const override { return qc->getName(); };

private:
    std::unique_ptr<qc::QuantumComputation> &qc;

    const unsigned int step_number;
    const double step_fidelity;
    unsigned long long approximation_runs{0};
    long double final_fidelity{1.0L};


    void single_shot();



    void runStochSimulationForId(int stochRun, int n_qubits, dd::Edge rootEdgePerfectRun,
                                 std::vector<double> &recordedPropertiesStorage,
                                 std::vector<std::tuple<long, std::string>> &recordedPropertiesList);

    dd::Edge generateNoiseOperation(const bool &amplitudeDamping, const unsigned short &target, std::default_random_engine &engine, std::uniform_real_distribution<fp> &distribution, dd::Edge dd_operation, std::unique_ptr<dd::Package> &localDD,
                                    std::array<short, qc::MAX_QUBITS> &line);

    void applyNoiseOperation(unsigned short targets, std::vector<qc::Control> &controls, dd::Edge dd_op, std::unique_ptr<dd::Package> &localDD, dd::Edge &local_root_edge, std::default_random_engine &generator,
                             std::uniform_real_distribution<fp> &dist, std::array<short, qc::MAX_QUBITS> &line, dd::Edge identityDD);
};


#endif //DDSIM_QFRSIMULATOR_HPP
