//
// Created by alexander on 21.06.21.
//

#ifndef DDSIM_PARALLELIZATIONSIMULATOR_HPP
#define DDSIM_PARALLELIZATIONSIMULATOR_HPP

#include "CircuitSimulator.hpp"
#include "dd/Export.hpp"

#include <future>
#include <thread>

class ParallelizationSimulator: public CircuitSimulator {
public:
    explicit ParallelizationSimulator(std::unique_ptr<qc::QuantumComputation>&& qc):
        CircuitSimulator(std::move(qc)) {
        // initialize anything that only the new simulator has here
    }

    // This is the simulation routine that is called to perform the actual simulation
    // it returns a dictionary containing the measurement outcomes (of which there are `shots`)
    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

protected:
    std::unique_ptr<dd::Package> ddM = std::make_unique<dd::Package>();

    // declare any methods that are used internally in the simulator here
    static std::vector<std::unique_ptr<qc::Operation>>::iterator getWarmUpDepth(const std::unique_ptr<qc::QuantumComputation>& qc, std::unique_ptr<dd::Package>& ddPackage);
    static qc::VectorDD                                          simulateMatrixVectorBasic(const std::unique_ptr<qc::QuantumComputation>& qc, const qc::VectorDD& initialState, std::unique_ptr<dd::Package>& ddPackage, std::vector<std::unique_ptr<qc::Operation>>::iterator iter);
    static qc::MatrixDD                                          simulateMatrixMatrixBasic(const std::unique_ptr<qc::QuantumComputation>& qc, const qc::MatrixDD& initialMatrix, std::unique_ptr<dd::Package>& ddPackage, std::vector<std::unique_ptr<qc::Operation>>::iterator iter);
    // declare any member needed by the simulator here
    // such as the (potentially synchronized) queue for the computation, or the thread pool for the simulation
};

#endif //DDSIM_PARALLELIZATIONSIMULATOR_HPP
