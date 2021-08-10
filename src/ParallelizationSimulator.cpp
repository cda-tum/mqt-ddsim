#include "ParallelizationSimulator.hpp"
#include <iterator>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <chrono>
#include <future>
#include "boost/tuple/tuple.hpp"
#include <algorithm>
#include "ThreadPool.hpp"
using boost::tuple;
using namespace std::chrono_literals;


/*
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

auto ParallelizationSimulator::getStrategieOneThread(double warmupFraction) {
    // create the iterator
    std::size_t n   = qc->size();
    auto        ptr = qc->begin();
    // determine the starting position of the warmup part
    std::advance(ptr, floor(std::max(static_cast<double>(n)*warmupFraction,1.)));
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

auto ParallelizationSimulator::getStaticStartPool(tuple_list& job_list, double warmupFraction, long minChunkSize, long preparedJobs) {
    // create the iterator
    std::size_t n   = qc->size();
    auto        ptr = qc->begin();
    long currentChunkSize = minChunkSize;
    auto stop = 0;
    // determine the starting position of the warmup part
    std::advance(ptr, floor(std::max(static_cast<double>(n)*warmupFraction,2.)));
    auto ptrjobs = ptr;
    while (stop < preparedJobs){
        job_list.emplace_back(ptrjobs, ptrjobs + currentChunkSize);
        //std::cout << "else part does something" << std::endl;
        std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, ptrjobs+currentChunkSize};
        ptrjobs += currentChunkSize;
        std::pair<std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>,int> pairId{pair,id};
        job_queuePool.emplace(pairId);
        id++;
        stop++;

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
    std::advance(ptr, floor(std::max(static_cast<double>(n)*warmupFraction,3.)));
    auto ptrjobs = ptr;
    while (stop < preparedJobs){
        job_list.emplace_back(ptrjobs, ptrjobs + currentChunkSize);
        //std::cout << "else part does something" << std::endl;
        std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, ptrjobs+currentChunkSize};
        ptrjobs += currentChunkSize;
        job_queue.emplace(pair);
        stop++;
    }
    return ptr;
}
*/
auto ParallelizationSimulator::getStartforDynamisch(double warmupFraction) {
    // create the iterator
    std::size_t n   = qc->size();
    auto        ptr = qc->begin();
    auto stop = 0;
    // determine the starting position of the warmup part
    std::advance(ptr, floor(std::max(static_cast<double>(n)*warmupFraction,1.)));
    return ptr;
}


std::map<std::string, std::size_t> ParallelizationSimulator::Simulate(unsigned int shots) {
    // create initial state, initial matrix and queue
    jobFlag = false;
    tuple_list job_list{};
    auto warmupFraction = 0.1;
    auto chunkSize = 2;
    auto startMatrixMatrixBlock = 4;
    auto maxChunkSize = 10;
    auto increments = 2;
    auto brackets = 3;
    qc::VectorDD initialState = dd->makeZeroState(qc->getNqubits());
    dd->incRef(initialState);
    //auto warmupDepth = ParallelizationSimulator::getStrategieStatic(job_list, warmupFraction, chunkSize);
    //ParallelizationSimulator::getJobs(job_list, warmupDepth);
    auto warmupDepth = ParallelizationSimulator::getStartforDynamisch(warmupFraction);
    /*// Here starts the static part
    auto thread1 = std::async(std::launch::async,[&](){
      ParallelizationSimulator::dynamicJobQueue(chunkSize);
    });
    auto thread2 = std::async(std::launch::async, [&]() {
        ParallelizationSimulator::simulateMatrixMatrixDynamic();
      //ParallelizationSimulator::simulateMatrixMatrixBasic(job_list);
    });

    // Multi-threading here
    ThreadPool threadPool(6);
    auto t1 = threadPool.enqueue([=]{
        ParallelizationSimulator::dynamicJobQueue(chunkSize);
    });
    auto t2 = threadPool.enqueue([=]{
        ParallelizationSimulator::simulateMatrixMatrixDynamic();
    });

    auto vector = ParallelizationSimulator::simulateMatrixVectorBasic(initialState, warmupDepth);
    qc::VectorDD resultingState = vector;
    // This is for ParallelizationSimulator::simulateMatrixMatrixDynamic();

    while(true){
        while (queue.empty()){
            sleep(0.000);
        }
        auto result = queue.front();
        auto tmp = dd->multiply(result.first, resultingState);
        dd -> incRef(tmp);
        dd ->decRef(resultingState);

        reference_queue.emplace(result.first);
        resultingState = tmp;
        dd ->garbageCollect();
        queue.pop();
        if (thread2.wait_for(0ms)==std::future_status::ready){
            while(!queue.empty()){
                auto result = queue.front();
                auto tmp = dd->multiply(result.first, resultingState);
                dd -> incRef(tmp);
                dd ->decRef(resultingState);
                reference_queue.emplace(result.first);
                resultingState = tmp;
                dd ->garbageCollect();
                queue.pop();
            }
            break;
        }

    }
    */
    // Here starts the dynamic part
    auto t1 = std::async(std::launch::async, [&](){
        ParallelizationSimulator::simulateMatrixMatrixDynamisch(warmupDepth);
    });
    auto vector = ParallelizationSimulator::simulateMatrixVectorBasic(initialState, warmupDepth);
    qc::VectorDD resultingState = vector;
    mvReady = true;
    while(true){
        while (queue.empty()){
            sleep(0);
        }
        //std::cout << "taking queue element " << mvReady << std::endl;
        auto result = queue.front();
        //std::cout << mvReady << std::endl;
        mvReady = false;
        //std::cout << mvReady << std::endl;
        auto tmp = dd->multiply(result.first, resultingState);
        dd -> incRef(tmp);
        dd ->decRef(resultingState);
        reference_queue.emplace(result.first);
        resultingState = tmp;
        dd ->garbageCollect();
        queue.pop();
        /*
        if (op_num%200 == 0){
            std::string var = std::to_string(op_num);
            dd::export2Dot(resultingState, "result_w_parallel"+var+".dot", true, true);
            sleep(2);
        }*/
        if (t1.wait_for(0ms)==std::future_status::ready){
            while(!queue.empty()){
                auto result = queue.front();
                auto tmp = dd->multiply(result.first, resultingState);
                dd -> incRef(tmp);
                dd ->decRef(resultingState);
                reference_queue.emplace(result.first);
                resultingState = tmp;
                dd ->garbageCollect();
                queue.pop();
            }
            break;
        }
        mvReady = true;

    }
    //qc->print(std::cout);std::cout<<std::endl;
    dd::export2Dot(resultingState, "result_w_queue.dot", true, true);
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
        //dd->garbageCollect();
        //ptr->get()->print(std::cout);std::cout<<std::endl;
    }
    return state;
}

void ParallelizationSimulator::simulateMatrixMatrixDynamisch(std::vector<std::unique_ptr<qc::Operation>>::iterator& iter){
    std::size_t jobnumber=1;
    auto ptrNow = iter;
    while(true){
        qc::MatrixDD matrix = ddM->makeIdent(qc->getNqubits());
        ddM->incRef(matrix);
        //std::cout<<"---"<<std::endl;
        for(auto ptr = ptrNow; ptr != qc->end(); ptr++){
            //std::cout << mvReady << std::endl;
            auto tmp = ddM->multiply((ptr->get()->getDD(ddM)),matrix);
            ddM->incRef(tmp);
            ddM->decRef(matrix);
            matrix = tmp;
            ddM->garbageCollect();
            //ptr->get()->print(std::cout);std::cout<<std::endl;
            ptrNow = ptr+1;
            op_num++;
            /*
            if (op_num%200 == 0){
                std::cout << op_num <<std::endl;
                while(!mvReady){
                    sleep(0);
                }
            }*/
            //std::cout << mvReady << std::endl;
            if(mvReady){
                break;
            }
        }
        std::pair<qc::MatrixDD,std::size_t> pair{matrix,jobnumber};
        queue.emplace(pair);
        if (!reference_queue.empty()){
            ddM->decRef(reference_queue.front());
            reference_queue.pop();
        }
        //std::cout<<"---"<<std::endl;
        jobnumber++;
        if(ptrNow==qc->end()){
            break;
        }
    }

}
/*
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
        std::pair<qc::MatrixDD,std::size_t> pair{matrix,jobnumber};
        queue.emplace(pair);
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
        //std::cout<<"---"<<std::endl;
        for(auto ptr = job.first; ptr!=job.second; ptr++){
            auto tmp = ddM->multiply(ptr->get()->getDD(ddM), matrix);
            ddM->incRef(tmp);
            ddM->decRef(matrix);
            matrix = tmp;
            ddM->garbageCollect();
            //ptr->get()->print(std::cout);std::cout<<std::endl;
        }
        std::pair<qc::MatrixDD,std::size_t> pair{matrix,jobnumber};
        queue.emplace(pair);

        if (!reference_queue.empty()){
            ddM->decRef(reference_queue.front());
            reference_queue.pop();
        }
        jobnumber++;
        job_queue.pop();

    }
    //std::cout<<"---"<<std::endl;
}

void ParallelizationSimulator::dynamicJobQueue(long minChunkSize){
    auto queueEnd = job_queue.back();
    auto ptr = queueEnd.second;
    auto currentChunkSize = minChunkSize;
    for (auto ptrjobs = ptr; ptrjobs != qc->end();ptrjobs++){
        if (std::distance(ptrjobs, qc->end()) < currentChunkSize) {
            std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, qc->end()};
            job_queue.emplace(pair);
            break;
        }
        if (job_queue.size() > 200){
            // chunkSize increasing by 1
            currentChunkSize = currentChunkSize+1;
            if (std::distance(ptrjobs, qc->end()) <= currentChunkSize) {
                std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, qc->end()};
                job_queue.emplace(pair);
                break;
            }
            std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, ptrjobs+currentChunkSize};
            job_queue.emplace(pair);
            ptrjobs += currentChunkSize;
            maxJobs ++;
        }
        else{
            std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, ptrjobs+currentChunkSize};
            job_queue.emplace(pair);
            ptrjobs += currentChunkSize-1;
            maxJobs ++;
        }

    }
}

void ParallelizationSimulator::dynamicJobQueuePool(long minChunkSize){
    auto queueEnd = job_queue.back();
    auto ptr = queueEnd.second;
    auto currentChunkSize = minChunkSize;
    for (auto ptrjobs = ptr; ptrjobs != qc->end();ptrjobs++){
        if (std::distance(ptrjobs, qc->end()) < currentChunkSize) {
            std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, qc->end()};
            std::pair<std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>,int> pairId{pair,id};
            job_queuePool.emplace(pairId);
            id++;
            break;
        }
        if (job_queue.size() > 200){
            // chunkSize increasing by 1
            currentChunkSize = currentChunkSize+1;
            if (std::distance(ptrjobs, qc->end()) <= currentChunkSize) {
                std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, qc->end()};
                std::pair<std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>,int> pairId{pair,id};
                job_queuePool.emplace(pairId);
                id++;
                break;
            }
            std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, ptrjobs+currentChunkSize};
            std::pair<std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>,int> pairId{pair,id};
            job_queuePool.emplace(pairId);
            id++;
            ptrjobs += currentChunkSize;
            maxJobs ++;
        }
        else{
            std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator> pair{ptrjobs, ptrjobs+currentChunkSize};
            std::pair<std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>,int> pairId{pair,id};
            job_queuePool.emplace(pairId);
            id++;
            ptrjobs += currentChunkSize-1;
            maxJobs ++;
        }

    }
}


void ParallelizationSimulator::simulateMatrixMatrixPool(int id) {
    while(!job_queuePool.empty()){
        auto job = job_queuePool.access(id);
        qc::MatrixDD matrix = ddM->makeIdent(qc->getNqubits());
        ddM->incRef(matrix);
        //std::cout<<"---"<<std::endl;
        for(auto ptr = job.first.first; ptr!=job.first.second; ptr++){
            auto tmp = ddM->multiply(ptr->get()->getDD(ddM), matrix);
            ddM->incRef(tmp);
            ddM->decRef(matrix);
            matrix = tmp;
            ddM->garbageCollect();
            //ptr->get()->print(std::cout);std::cout<<std::endl;
        }
        std::pair<qc::MatrixDD,std::size_t> pair{matrix,id};
        queue.emplace(pair);

        if (!reference_queue.empty()){
            ddM->decRef(reference_queue.front());
            reference_queue.pop();
        }
        job_queuePool.pop();
    }
}
*/
