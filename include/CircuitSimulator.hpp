#ifndef DDSIM_CIRCUITSIMULATOR_HPP
#define DDSIM_CIRCUITSIMULATOR_HPP

#include "Simulator.hpp"
#include "QuantumComputation.hpp"
#include <istream>


struct ApproximationInfo {
    enum ApproximationWhen {
        FidelityDriven,
        MemoryDriven
    };

    ApproximationInfo(double step_fidelity, unsigned int step_number, ApproximationWhen approx_when) :
        step_fidelity(step_fidelity), step_number(step_number), approx_when(approx_when) {
    }

    friend std::istream& operator>> (std::istream &in, ApproximationWhen &when) {
        std::string token;
        in >> token;

        if (token == "fidelity") {
            when = FidelityDriven;
        } else if (token == "memory") {
            when = MemoryDriven;
        } else {
            throw std::runtime_error("Unknown approximation method '" + token + "'.");
        }

        return in;
    }

    const double step_fidelity;
    const unsigned int step_number;
    const ApproximationWhen approx_when;
};

class CircuitSimulator : public Simulator {
public:
    explicit CircuitSimulator(std::unique_ptr<qc::QuantumComputation> &qc)
            : qc(qc), approx_info(ApproximationInfo(1.0, 1, ApproximationInfo::FidelityDriven)) {
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const ApproximationInfo approx_info)
            : qc(qc), approx_info(approx_info) {
        if (approx_info.step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const ApproximationInfo approx_info, const unsigned long long seed)
            : Simulator(seed), qc(qc), approx_info(approx_info) {
        if (approx_info.step_number == 0) {
            throw std::invalid_argument("step_number has to be greater than zero");
        }
    }

    std::map<std::string, unsigned int> Simulate(unsigned int shots) override;
    std::map<std::string, double> StochSimulate() override {return {};};

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity",     std::to_string(approx_info.step_fidelity)},
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
    unsigned int single_shots{0};


    const ApproximationInfo approx_info;
    unsigned long long approximation_runs{0};
    long double final_fidelity{1.0L};


    std::map<int, bool> single_shot(bool ignore_nonunitaries);
};


#endif //DDSIM_CIRCUITSIMULATOR_HPP
