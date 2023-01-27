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
        stepFidelity(1), stepNumber(1), approxWhen(ApproximationWhen::FidelityDriven) {}

    ApproximationInfo(double stepFidelity_, unsigned int stepNumber_, ApproximationWhen approxWhen_):
        stepFidelity(stepFidelity_), stepNumber(stepNumber_), approxWhen(approxWhen_) {}

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

    const double            stepFidelity;
    const unsigned int      stepNumber;
    const ApproximationWhen approxWhen;
};

template<class Config = dd::DDPackageConfig>
class CircuitSimulator: public Simulator<Config> {
public:
    explicit CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_):
        qc(std::move(qc_)), approximationInfo(ApproximationInfo(1.0, 1, ApproximationInfo::FidelityDriven)) {
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const unsigned long long seed_):
        Simulator<Config>(seed_),
        qc(std::move(qc_)), approximationInfo(ApproximationInfo(1.0, 1, ApproximationInfo::FidelityDriven)) {
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const ApproximationInfo approximation_info):
        qc(std::move(qc_)), approximationInfo(approximation_info) {
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    CircuitSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const ApproximationInfo approximation_info, const unsigned long long seed_):
        Simulator<Config>(seed_),
        qc(std::move(qc_)), approximationInfo(approximation_info) {
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    std::map<std::string, std::size_t> Simulate(std::size_t shots) override;

    std::map<std::string, std::string> AdditionalStatistics() override {
        return {
                {"step_fidelity", std::to_string(approximationInfo.stepFidelity)},
                {"approximation_runs", std::to_string(approximationRuns)},
                {"final_fidelity", std::to_string(finalFidelity)},
                {"single_shots", std::to_string(singleShots)},
        };
    };

    [[nodiscard]] std::size_t getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

protected:
    std::unique_ptr<qc::QuantumComputation> qc;
    std::size_t                             singleShots{0};

    const ApproximationInfo approximationInfo;
    std::size_t             approximationRuns{0};
    long double             finalFidelity{1.0L};

    std::map<std::size_t, bool> singleShot(bool ignore_nonunitaries);
};

#endif //DDSIM_CIRCUITSIMULATOR_HPP
