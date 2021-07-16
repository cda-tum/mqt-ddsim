#include "ParallelizationSimulator.hpp"
#include <iterator>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include "boost/tuple/tuple.hpp"

using boost::tuple;



auto ParallelizationSimulator::getWarmUpDepth(const std::unique_ptr<qc::QuantumComputation>& qc, std::unique_ptr<dd::Package>& ddPackage) {
    // create the iterator
    std::size_t n   = qc->size();
    auto        ptr = qc->begin();
    std::advance(ptr, floor(n / 2));

    return ptr;
}

std::map<std::string, std::size_t> ParallelizationSimulator::Simulate(unsigned int shots) {
    // create initial state, initial matrix and queue
    tuple_list job_list;
    std::queue<qc::MatrixDD> queue;
    qc::VectorDD initialState = dd->makeZeroState(qc->getNqubits());
    dd->incRef(initialState);
    qc::MatrixDD initialMatrix = ddM->makeIdent(qc->getNqubits());
    ddM->incRef(initialMatrix);
    auto warmupdepth = ParallelizationSimulator::getWarmUpDepth(qc, dd);
    auto jobs = ParallelizationSimulator::getJobs(qc, job_list, warmupdepth);
    // asynchronously call the simulation routine with the quantum computation, the initial state, and the already initialized DD package
    auto vector = std::async(std::launch::async, [&]() {
        return ParallelizationSimulator::simulateMatrixVectorBasic(qc, initialState, dd, warmupdepth);
    });
    auto matrix = std::async(std::launch::async, [&]() {
        return ParallelizationSimulator::simulateMatrixMatrixBasic(qc, initialMatrix, ddM, jobs);
    });
    // wait for the result to become available
    qc::VectorDD resultingState = vector.get();
    auto result = ParallelizationSimulator::operatingQueue(resultingState, queue, dd);
    /*// export the resulting state to a svg file (for debugging purposes)
    dd::export2Dot(resultingState, "resultingState.dot", true, true);
    dd::export2Dot(resultingMatrix, "resultingMatrix.dot", true, true);
    dd::export2Dot(result, "result_w_queue.dot", true, true);
    // TODO: measure from the resulting state and return the corresponding dictionary
    dd->measureAll(resultingState,false,mt);
    dd->measureAll(result, false, mt);*/
    return {};
}

qc::VectorDD ParallelizationSimulator::simulateMatrixVectorBasic(const std::unique_ptr<qc::QuantumComputation>& qc, const qc::VectorDD& initialState, std::unique_ptr<dd::Package>& ddPackage, std::vector<std::unique_ptr<qc::Operation>>::iterator iter) {
    qc::VectorDD state = initialState;
    for (auto ptr = qc->begin(); ptr != iter; ptr++) {
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

std::queue<qc::MatrixDD> ParallelizationSimulator::simulateMatrixMatrixBasic(const std::unique_ptr<qc::QuantumComputation>& qc, const qc::MatrixDD& initialMatrix, std::unique_ptr<dd::Package>& ddPackage, std::vector<tuple<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>> job_list) {
    qc::MatrixDD matrix = initialMatrix;
    std::queue<qc::MatrixDD> queue;
    while(!job_list.empty()) {
        for(auto ptr = job_list[0].get<0>(); ptr!=job_list[0].get<1>(); ptr++){
            auto tmp = ddPackage->multiply(matrix, ptr->get()->getDD(ddPackage));
            ddPackage->incRef(tmp);
            ddPackage->decRef(matrix);
            matrix = tmp;
            ddPackage->garbageCollect();
        }
        queue.push(matrix);
    }

    return queue;
}

qc::VectorDD ParallelizationSimulator::operatingQueue(const qc::VectorDD& initalState, std::queue<qc::MatrixDD> queue, std::unique_ptr<dd::Package>& ddPackage) {
    qc::VectorDD state=initalState;
    while (!queue.empty()){
        auto tmp = ddPackage->multiply(queue.front(), initalState);
        ddPackage -> incRef(tmp);
        ddPackage ->decRef(state);
        state = tmp;
        queue.pop();
        ddPackage ->garbageCollect();
    }
    return state;
}

std::vector<tuple<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>> ParallelizationSimulator::getJobs(const std::unique_ptr<qc::QuantumComputation>& qc, std::vector<tuple<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>> job_list, std::vector<std::unique_ptr<qc::Operation>>::iterator iter){
    for (auto ptr = iter; ptr != qc->end();ptr++){
        for (tuple_list::const_iterator i = job_list.begin(); i!=job_list.end();i++){
            auto ptrend = ptr;
            advance(ptrend,1);
            job_list.emplace_back(ptr,ptrend);
            ptr++;
        }
    }
    return job_list;
}