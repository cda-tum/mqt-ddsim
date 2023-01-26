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

    /* Default to no approximation */
    ApproximationInfo():
        step_fidelity(1), step_number(1), approx_when(ApproximationWhen::FidelityDriven) {}

    ApproximationInfo(double step_fidelity_, unsigned int step_number_, ApproximationWhen approx_when_):
        step_fidelity(step_fidelity_), step_number(step_number_), approx_when(approx_when_) {}

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

template<class Config = dd::DDPackageConfig>
class CircuitSimulator: public Simulator<Config> {
public:
    explicit CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_):
        qc(std::move(qc_)), approx_info(ApproximationInfo(1.0, 1, ApproximationInfo::FidelityDriven)) {
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const unsigned long long seed_):
        Simulator<Config>(seed_),
        qc(std::move(qc_)), approx_info(ApproximationInfo(1.0, 1, ApproximationInfo::FidelityDriven)) {
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const ApproximationInfo approximation_info):
        qc(std::move(qc_)), approx_info(approximation_info) {
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const ApproximationInfo approximation_info, const unsigned long long seed_):
        Simulator<Config>(seed_),
        qc(std::move(qc_)), approx_info(approximation_info) {
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    std::map<std::string, std::size_t> Simulate(std::size_t shots) override;

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity", std::to_string(approx_info.step_fidelity)},
                {"approximation_runs", std::to_string(approximation_runs)},
                {"final_fidelity", std::to_string(final_fidelity)},
                {"single_shots", std::to_string(single_shots)},
        };
    };

    [[nodiscard]] std::size_t getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

protected:
    std::unique_ptr<qc::QuantumComputation> qc;
    std::size_t                             single_shots{0};

    const ApproximationInfo approx_info;
    std::size_t             approximation_runs{0};
    long double             final_fidelity{1.0L};

    std::map<std::size_t, bool> single_shot(bool ignore_nonunitaries);
};

#endif //DDSIM_CIRCUITSIMULATOR_HPP
