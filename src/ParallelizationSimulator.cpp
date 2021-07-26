#include "ParallelizationSimulator.hpp"
#include <iterator>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <chrono>
#include "boost/tuple/tuple.hpp"
#include <algorithm>
using boost::tuple;



auto ParallelizationSimulator::getStrategie1(tuple_list& job_list) {
    // create the iterator
    std::size_t n   = qc->size();
    auto        ptr = qc->begin();
    std::advance(ptr, floor(std::max(n/100,1UL)));
    for (auto ptrjobs = ptr; ptrjobs != qc->end();ptrjobs++){
        auto next = ptrjobs+1;
        if (next == qc->end()){
            job_list.emplace_back(ptrjobs,next);
            break;
        }
        else{
            job_list.emplace_back(ptrjobs, next+1);
            ptrjobs++;
        }

    }
    return ptr;
}


auto ParallelizationSimulator::getStrategieStatic(tuple_list& job_list, double warmupFraction, long chunkSize) {
    // create the iterator
    std::size_t n   = qc->size();
    auto        ptr = qc->begin();
    // determine the starting position of the warmup part
    std::advance(ptr, floor(std::max(static_cast<double>(n)*warmupFraction,1.)));
    for (auto ptrjobs = ptr; ptrjobs != qc->end();ptrjobs++) {
        if (std::distance(ptrjobs, qc->end()) < chunkSize) {
            job_list.emplace_back(ptrjobs, ptrjobs + std::distance(ptrjobs, qc->end()));
            break;
        }
        job_list.emplace_back(ptrjobs, ptrjobs + chunkSize);
        //std::cout << "else part does something" << std::endl;
        ptrjobs += chunkSize-1;
    }
    return ptr;
}


auto ParallelizationSimulator::getStaticStart(tuple_list& job_list, double warmupFraction, long minChunkSize, long preparedJobs) {
    // create the iterator
    std::size_t n   = qc->size();
    auto        ptr = qc->begin();
    long currentChunkSize = minChunkSize;
    auto stop = 0;
    // determine the starting position of the warmup part
    std::advance(ptr, floor(std::max(static_cast<double>(n)*warmupFraction,1.)));
    auto ptrjobs = ptr;
    while (stop <= preparedJobs){
        job_list.emplace_back(ptrjobs, ptrjobs + currentChunkSize);
        //std::cout << "else part does something" << std::endl;
        job_queue.emplace(ptrjobs,ptrjobs+currentChunkSize);
        ptrjobs += currentChunkSize;
        stop++;

    }
    return ptr;
}

void ParallelizationSimulator::dynamicJobQueue(long minChunkSize){
    auto queueEnd = job_queue.back();
    auto ptr = queueEnd.second;
    for (auto ptrjobs = ptr; ptrjobs != qc->end();ptrjobs++){
        if (job_queue.size() > 2000){
            auto currentChunkSize = 2*minChunkSize;
            job_queue.emplace(ptr,ptr+currentChunkSize);
            ptr += currentChunkSize;
            break;
        }
        job_queue.emplace(ptr,ptr+minChunkSize);
        ptr += minChunkSize;
    }
}

std::map<std::string, std::size_t> ParallelizationSimulator::Simulate(unsigned int shots) {
    // create initial state, initial matrix and queue
    tuple_list job_list{};
    auto warmupFraction = 0.1;
    auto chunkSize = 3;
    auto startMatrixMatrixBlock = 5;
    auto maxChunkSize = 10;
    auto increments = 2;
    auto brackets = 3;
    qc::VectorDD initialState = dd->makeZeroState(qc->getNqubits());
    dd->incRef(initialState);
    auto warmupdepth = ParallelizationSimulator::getStaticStart(job_list, warmupFraction, chunkSize, startMatrixMatrixBlock);
    //auto warmupdepth = ParallelizationSimulator::getStrategieStatic(job_list, warmupFraction, chunkSize);
    //ParallelizationSimulator::getJobs(job_list, warmupdepth);
    auto thread1 = std::async(std::launch::async,[&](){
      ParallelizationSimulator::dynamicJobQueue(chunkSize);
    });
    auto thread2 = std::async(std::launch::async, [&]() {
        ParallelizationSimulator::simulateMatrixMatrixDynamic();
      //ParallelizationSimulator::simulateMatrixMatrixBasic(job_list);
    });

    //std::cout<<"jobs submitted"<<std::endl;
    auto vector = ParallelizationSimulator::simulateMatrixVectorBasic(initialState, warmupdepth);
    //ParallelizationSimulator::simulateMatrixMatrixBasic(job_list);
    //std::cout<<"warmup done"<<std::endl;
    // wait for the result to become available
    qc::VectorDD resultingState = vector;
    //std::cout <<" there are " << queue.size() << " jobs in the queue" << std::endl;
    while(true){
        while (queue.empty()){
            sleep(0.000);
        }
        //measure time of the next matrix vector mlply
        auto t1 = std::chrono::high_resolution_clock::now();
        auto result = queue.front();
        auto tmp = dd->multiply(result.first, resultingState);
        dd -> incRef(tmp);
        dd ->decRef(resultingState);
        reference_queue.push(result.first);
        resultingState = tmp;
        dd ->garbageCollect();
        std::cout << "collecting garbage " << std::endl;
        auto t2 = std::chrono::high_resolution_clock::now();
        if (result.second==job_list.size()){
            std::cout << "still working herer" << std::endl;
            break;
        }
        queue.pop();
        std::cout << "still poping queues like flies" << std::endl;
        //std::cout<<"after another operation is done the size is " << queue.size() << std::endl;
    }
    dd::export2Dot(resultingState, "result_w_queue.dot", true, true);
    std::cout << "am i even trying? " << std::endl;
    /*// export the resulting state to a svg file (for debugging purposes)
    dd::export2Dot(resultingState, "resultingState.dot", true, true);
    dd::export2Dot(resultingMatrix, "resultingMatrix.dot", true, true);

    // TODO: measure from the resulting state and return the corresponding dictionary
    dd->measureAll(resultingState,false,mt);
    dd->measureAll(result, false, mt);*/
    return {};
}

qc::VectorDD ParallelizationSimulator::simulateMatrixVectorBasic(const qc::VectorDD& initialState, std::vector<std::unique_ptr<qc::Operation>>::iterator& iter) {
    qc::VectorDD state = initialState;
    for (auto ptr = qc->begin(); ptr != iter; ptr++) {
        if (!ptr->get()->isUnitary()) {
            throw std::runtime_error("Non-unitary operation encountered. This is not supported for now. Aborting simulation");
        }
        auto tmp = dd->multiply(ptr->get()->getDD(dd), state);
        dd->incRef(tmp);
        dd->decRef(state);
        state = tmp;
        dd->garbageCollect();
        //ptr->get()->print(std::cout);std::cout<<std::endl;
    }
    return state;
}

void ParallelizationSimulator::simulateMatrixMatrixBasic(const tuple_list& job_list) {
    std::size_t jobnumber=1;
    for(const auto& job:job_list) {
        qc::MatrixDD matrix = ddM->makeIdent(qc->getNqubits());
        ddM->incRef(matrix);
        std::cout<<"---"<<std::endl;
        for(auto ptr = job.get<0>(); ptr!=job.get<1>(); ptr++){
            auto tmp = ddM->multiply(ptr->get()->getDD(ddM), matrix);
            ddM->incRef(tmp);
            ddM->decRef(matrix);
            matrix = tmp;
            ddM->garbageCollect();
            ptr->get()->print(std::cout);std::cout<<std::endl;
        }
        queue.emplace(matrix,jobnumber);
        if (!reference_queue.empty()){
            ddM->decRef(reference_queue.front());
            reference_queue.pop();
        }
        jobnumber++;
    }
    std::cout<<"---"<<std::endl;
}

void ParallelizationSimulator::simulateMatrixMatrixDynamic() {
    std::size_t jobnumber=1;
    while(!job_queue.empty()) {
        auto job = job_queue.front();
        qc::MatrixDD matrix = ddM->makeIdent(qc->getNqubits());
        ddM->incRef(matrix);
        std::cout<<"---"<<std::endl;
        for(auto ptr = job.first; ptr!=job.second; ptr++){
            auto tmp = ddM->multiply(ptr->get()->getDD(ddM), matrix);
            ddM->incRef(tmp);
            ddM->decRef(matrix);
            matrix = tmp;
            ddM->garbageCollect();
            ptr->get()->print(std::cout);std::cout<<std::endl;
        }
        queue.emplace(matrix,jobnumber);
        if (!reference_queue.empty()){
            ddM->decRef(reference_queue.front());
            reference_queue.pop();
        }
        jobnumber++;
        job_queue.pop();
    }
    std::cout<<"---"<<std::endl;
}