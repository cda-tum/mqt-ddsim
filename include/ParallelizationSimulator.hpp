//
// Created by alexander on 21.06.21.
//

#ifndef DDSIM_PARALLELIZATIONSIMULATOR_HPP
#define DDSIM_PARALLELIZATIONSIMULATOR_HPP

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "dd/Export.hpp"

#include <cmath>
#include <complex>
#include <memory>
#include <omp.h>


class ParallelizationSimulator: public Simulator {
public:
    explicit ParallelizationSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_):
            qc(std::move(qc_)), approx_info(ApproximationInfo(1.0, 1, ApproximationInfo::FidelityDriven)) {
        dd->resize(qc->getNqubits());
    }

    ParallelizationSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const ApproximationInfo approx_info):
            qc(std::move(qc_)), approx_info(approx_info) {
        dd->resize(qc->getNqubits());
    }

    ParallelizationSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, const ApproximationInfo approx_info, const unsigned long long seed):
            Simulator(seed),
            qc(std::move(qc_)), approx_info(approx_info) {
    }

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;



    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

protected:
    std::unique_ptr<qc::QuantumComputation> qc;
    std::size_t                             single_shots{0};
    std::size_t                             depth;

    const ApproximationInfo approx_info;
    std::size_t             approximation_runs{0};
    long double             final_fidelity{1.0L};

    dd::Edge<dd::Package::vNode> SimulateMatrixVector();


};

#endif //DDSIM_PARALLELIZATIONSIMULATOR_HPP
