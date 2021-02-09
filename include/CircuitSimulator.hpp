#ifndef DDSIM_CIRCUITSIMULATOR_HPP
#define DDSIM_CIRCUITSIMULATOR_HPP

#include "Simulator.hpp"
#include "QuantumComputation.hpp"

class CircuitSimulator : public Simulator {
public:
    CircuitSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const unsigned int step_number, const double step_fidelity)
            : qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        if (step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const unsigned int step_number, const double step_fidelity, const unsigned long long seed)
            : Simulator(seed), qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        if (step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
    }

    std::map<std::string, unsigned int> Simulate(unsigned int shots) override;
    std::map<std::string, double> StochSimulate() override {return {};};

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity",     std::to_string(step_fidelity)},
                {"approximation_runs",std::to_string(approximation_runs)},
                {"final_fidelity",    std::to_string(final_fidelity)},
                {"single_shots",      std::to_string(single_shots)},
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
    unsigned int single_shots{0};

    std::map<int, bool> single_shot(bool ignore_nonunitaries);
};


#endif //DDSIM_CIRCUITSIMULATOR_HPP
