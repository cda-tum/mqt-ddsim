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
    std::queue<std::pair<qc::MatrixDD,std::size_t>>     queue;
    std::queue<std::pair<std::vector<std::unique_ptr<qc::Operation>>::iterator,std::vector<std::unique_ptr<qc::Operation>>::iterator>>                              job_queue;
    std::queue<qc::MatrixDD>                            reference_queue;
    std::chrono::high_resolution_clock                  mvBegin;
    std::chrono::high_resolution_clock                  mvEnd;
    std::chrono::high_resolution_clock                  mmBegin;
    std::chrono::high_resolution_clock                  mmEnd;
    // declare any methods that are used internally in the simulator here
    auto         getStrategie1(tuple_list& iter);
    auto         getStrategieStatic(tuple_list& iter, double warmupFraction, long chunkSize);
    auto         getStaticStart(tuple_list& iter, double warmupFraction, long minChunkSize, long maxChunkSize);
    qc::VectorDD simulateMatrixVectorBasic(const qc::VectorDD& initialState, std::vector<std::unique_ptr<qc::Operation>>::iterator& iter);
    void         simulateMatrixMatrixBasic(const tuple_list& iter);
    void         simulateMatrixMatrixDynamic();
    void         dynamicJobQueue(long minChunkSize);
    //void   getJobs(tuple_list& job_list, const std::vector<std::unique_ptr<qc::Operation>>::iterator& iter);
    // declare any member needed by the simulator here
    // such as the (potentially synchronized) queue for the computation, or the thread pool for the simulation
};

#endif //DDSIM_PARALLELIZATIONSIMULATOR_HPP
