#include "ParallelizationSimulator.hpp"
#include <iterator>
#include <vector>

std::map<std::string, std::size_t> ParallelizationSimulator::Simulate(unsigned int shots) {
    // create initial state
    qc::VectorDD initialState = dd->makeZeroState(qc->getNqubits());
    dd->incRef(initialState);
    qc::MatrixDD initialMatrix = ddM->makeIdent(qc->getNqubits());
    ddM->incRef(initialMatrix);
    auto warmupdepth = ParallelizationSimulator::getWarmUpDepth(qc, dd);
    // asynchronously call the simulation routine with the quantum computation, the initial state, and the already initialized DD package
    auto vector = std::async(std::launch::async, [this, initialState]() {
        return ParallelizationSimulator::simulateMatrixVectorBasic(qc, initialState, dd, warmupdepth);
    });
    auto matrix = std::async(std::launch::async, [this, initialMatrix](){
        return ParallelizationSimulator::simulateMatrixMatrixBasic(qc, initialMatrix,ddM, warmupdepth);
    });
    std::cout << "simulation has started" << std::endl;

    // wait for the result to become available
    qc::VectorDD resultingState = vector.get();
    qc::MatrixDD resultingMatrix = matrix.get();
    std::cout << "simulation has finished" << std::endl;

    // export the resulting state to a svg file (for debugging purposes)
    dd::export2Dot(resultingState, "resultingState.dot", true, true);
    dd::export2Dot(resultingMatrix, "resultingMatrix.dot", true, true);

    // TODO: measure from the resulting state and return the corresponding dictionary
    dd->measureAll(resultingState,false,mt);
    return {};
}

qc::VectorDD ParallelizationSimulator::simulateMatrixVectorBasic(const std::unique_ptr<qc::QuantumComputation>& qc, const qc::VectorDD& initialState, std::unique_ptr<dd::Package>& ddPackage,__gnu_cxx::__normal_iterator<const std::unique_ptr<qc::Operation>*, std::vector<std::unique_ptr<qc::Operation>>> iter) {
    qc::VectorDD state = initialState;
    for (auto ptr = qc->begin(); ptr != iter; ptr++){
            if (!ptr->get()->isUnitary()) {
                throw std::runtime_error("Non-unitary operation encountered. This is not supported for now. Aborting simulation");
            }
            auto tmp = ddPackage->multiply(ptr->get()->getDD(ddPackage), state);
            ddPackage->incRef(tmp);
            ddPackage->decRef(state);
            state = tmp;
            ddPackage->garbageCollect();
    }
    return state;
}

qc::MatrixDD ParallelizationSimulator::simulateMatrixMatrixBasic(const std::unique_ptr<qc::QuantumComputation>& qc, const qc::MatrixDD& initialMatrix, std::unique_ptr<dd::Package>& ddPackage, decltype(qc->begin())& iter) {
    qc::MatrixDD matrix = initialMatrix;
    for (auto ptr =iter; ptr != qc->end();ptr++){
            if (!ptr->get()->isUnitary()) {
                throw std::runtime_error("Non-unitary operation encountered. This is not supported for now. Aborting simulation");
            }
            auto tmp = ddPackage->multiply(ptr->get()->getDD(ddPackage), matrix);
            ddPackage->incRef(tmp);
            ddPackage->decRef(matrix);
            matrix = tmp;
            ddPackage->garbageCollect();
        }
    return matrix;
}

std::vector<std::unique_ptr<qc::Operation>>::iterator getWarmUpDepth(const std::unique_ptr<qc::QuantumComputation>& qc, std::unique_ptr<dd::Package>& ddPackage) {
    // create the iterator
    std::size_t n = qc->size();
    auto ptr = qc->begin();
    std::advance(ptr, floor(n/2));

    return ptr;

}