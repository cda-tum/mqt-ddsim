//
// Created by alexander on 21.06.21.
//

#ifndef DDSIM_PARALLELIZATIONSIMULATOR_HPP
#define DDSIM_PARALLELIZATIONSIMULATOR_HPP

#include "CircuitSimulator.hpp"
#include "boost/tuple/tuple.hpp"
#include "dd/Export.hpp"

#include <future>
#include <thread>
using boost::tuple;
typedef std::vector<tuple<std::vector<std::unique_ptr<qc::Operation>>::iterator, std::vector<std::unique_ptr<qc::Operation>>::iterator>> tuple_list;


template<class T>
class safeJobQueue{
    std::deque<T> q;
    mutable std::mutex m;
public:
    safeJobQueue(){}

    void emplace(T elem){
        std::lock_guard<std::mutex> lock(m);
        q.emplace_back(elem);
    }

    void pop() {
        std::lock_guard<std::mutex> lock(m);
        if(!q.empty()){
            q.pop_front();
        }
    }

    std::size_t size(){
        std::lock_guard<std::mutex> lock(m);
        size_t size = q.size();
        return size;
    }

    T back(){
        std::lock_guard<std::mutex> lock(m);
        assert(!q.empty());
        return q.back();

    }

    T front(){
        std::lock_guard<std::mutex> lock(m);
        assert(!q.empty());
        return q.front();

    }

    bool empty(){
        std::lock_guard<std::mutex> lock(m);
        return q.empty();
    }

    T access(int id){
            std::lock_guard<std::mutex> lock(m);
            return q[id];
    };

};


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
    std::unique_ptr<dd::Package>                        ddM = std::make_unique<dd::Package>();
    safeJobQueue<std::pair<qc::MatrixDD,std::size_t>>     queue;
    safeJobQueue<std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>>                              job_queue;
    safeJobQueue<std::pair<std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>,int>>                job_queuePool;
    safeJobQueue<qc::MatrixDD>                            reference_queue;
    std::size_t                                         maxJobs =1 ;
    int                                                 id =1;
    bool                                                jobFlag;
    std::atomic<bool>                                               mvReady = false;
    std::size_t op_num=1;
    // declare any methods that are used internally in the simulator here
    auto         getStrategieOneThread(double warmupFraction);
    auto         getStrategie1(tuple_list& iter);
    auto         getStrategieStatic(tuple_list& iter, double warmupFraction, long chunkSize);
    auto         getStaticStart(tuple_list& iter, double warmupFraction, long minChunkSize, long maxChunkSize);
    auto         getStartforDynamisch(double warmupFraction);
    auto         getStaticStartPool(tuple_list& iter, double warmupFraction, long minChunkSize, long maxChunkSize);
    qc::VectorDD simulateMatrixVectorBasic(const qc::VectorDD& initialState, std::vector<std::unique_ptr<qc::Operation>>::iterator& iter);
    void         simulateMatrixMatrixBasic(const tuple_list& iter);
    void         simulateMatrixMatrixDynamic();
    void         simulateMatrixMatrixDynamisch(std::vector<std::unique_ptr<qc::Operation>>::iterator& iter);
    void         simulateMatrixMatrixPool(const int id);
    void         dynamicJobQueue(long minChunkSize);
    void         dynamicJobQueuePool(long minChunkSize);
    //void   getJobs(tuple_list& job_list, const std::vector<std::unique_ptr<qc::Operation>>::iterator& iter);
    // declare any member needed by the simulator here
    // such as the (potentially synchronized) queue for the computation, or the thread pool for the simulation
};

#endif //DDSIM_PARALLELIZATIONSIMULATOR_HPP
