#ifndef DDSIM_QFRSIMULATOR_HPP
#define DDSIM_QFRSIMULATOR_HPP

#include "Simulator.hpp"
#include "QuantumComputation.hpp"

class QFRSimulator : public Simulator {
public:
    explicit QFRSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int step_number, const double step_fidelity)
            : qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        if (step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
    }

    explicit QFRSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int step_number, const double step_fidelity, unsigned long long seed)
            : Simulator(seed), qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
        if (step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
    }

    void Simulate() override;

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity", std::to_string(step_fidelity)},
                {"approximation_runs", std::to_string(approximation_runs)},
                {"final_fidelity", std::to_string(final_fidelity)},
        };
    };


    unsigned short getNumberOfQubits() const override {return qc->getNqubits();};
    unsigned long getNumberOfOps() const override {return qc->getNops();};
    std::string getName() const override {return qc->getName();};

private:
    std::unique_ptr<qc::QuantumComputation>& qc;
    const unsigned int step_number;
    const double step_fidelity;
    unsigned long long approximation_runs{0};
    long double final_fidelity{1.0L};
};


#endif //DDSIM_QFRSIMULATOR_HPP
