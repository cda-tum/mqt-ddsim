#include "ParallelizationSimulator.hpp"

std::map<std::string, std::size_t> ParallelizationSimulator::Simulate(unsigned int shots) {
    // create initial state
    qc::VectorDD initialState = dd->makeZeroState(qc->getNqubits());
    dd->incRef(initialState);

    // asynchronously call the simulation routine with the quantum computation, the initial state, and the already initialized DD package
    auto future = std::async(std::launch::async, [this, initialState]() {
        return ParallelizationSimulator::simulateMatrixVectorBasic(qc, initialState, dd);
    });
    std::cout << "simulation has started" << std::endl;

    // wait for the result to become available
    qc::VectorDD resultingState = future.get();
    std::cout << "simulation has finished" << std::endl;

    // export the resulting state to a svg file (for debugging purposes)
    dd::export2Dot(resultingState, "resultingState.dot", true, true);

    // TODO: measure from the resulting state and return the corresponding dictionary

    return {};
}

qc::VectorDD ParallelizationSimulator::simulateMatrixVectorBasic(const std::unique_ptr<qc::QuantumComputation>& qc, const qc::VectorDD& initialState, std::unique_ptr<dd::Package>& ddPackage) {
    qc::VectorDD state = initialState;
    for (const auto& op: *qc) {
        if (!op->isUnitary()) {
            throw std::runtime_error("Non-unitary operation encountered. This is not supported for now. Aborting simulation");
        }

        auto tmp = ddPackage->multiply(op->getDD(ddPackage), state);
        ddPackage->incRef(tmp);
        ddPackage->decRef(state);
        state = tmp;
        ddPackage->garbageCollect();
    }

    return state;
}
