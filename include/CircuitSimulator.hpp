#ifndef DDSIM_CIRCUITSIMULATOR_HPP
#define DDSIM_CIRCUITSIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

#include <cstddef>
#include <istream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

struct ApproximationInfo {
    enum ApproximationWhen {
        FidelityDriven,
        MemoryDriven
    };

    ApproximationInfo(double step_fidelity, unsigned int step_number, ApproximationWhen approx_when):
        step_fidelity(step_fidelity), step_number(step_number), approx_when(approx_when) {
    }

    friend std::istream& operator>>(std::istream& in, ApproximationWhen& when) {
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

    const double            step_fidelity;
    const unsigned int      step_number;
    const ApproximationWhen approx_when;
};

class CircuitSimulator: public Simulator {
public:
    explicit CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_):
        qc(std::move(qc_)), approx_info(ApproximationInfo(1.0, 1, ApproximationInfo::FidelityDriven)) {
        dd->resize(qc->getNqubits());
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const ApproximationInfo approx_info):
        qc(std::move(qc_)), approx_info(approx_info) {
        dd->resize(qc->getNqubits());
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const ApproximationInfo approx_info,
                     const unsigned long long seed):
        Simulator(seed),
        qc(std::move(qc_)), approx_info(approx_info) {
    }

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity", std::to_string(approx_info.step_fidelity)},
                {"approximation_runs", std::to_string(approximation_runs)},
                {"final_fidelity", std::to_string(final_fidelity)},
                {"single_shots", std::to_string(single_shots)},
        };
    };

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

private:
    std::unique_ptr<qc::QuantumComputation> qc;
    std::size_t                             single_shots{0};

    const ApproximationInfo approx_info;
    std::size_t             approximation_runs{0};
    long double             final_fidelity{1.0L};

    std::map<std::size_t, bool> single_shot(bool ignore_nonunitaries);
};

#endif //DDSIM_CIRCUITSIMULATOR_HPP
