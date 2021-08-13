//
// Created by Lukas Burgholzer on 09.08.21.
//

#ifndef DDSIM_TASKBASEDSIMULATOR_HPP
#define DDSIM_TASKBASEDSIMULATOR_HPP

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"

#include <future>
#include <limits>
#include <taskflow/taskflow.hpp>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

class TaskBasedSimulator: public CircuitSimulator {
public:
    // TODO: Add new strategies here
    enum class Mode {
        Sequential,
        PairwiseRecursiveGrouping
    };

    struct ContractionPlan {
        struct Step {
            std::size_t                         id;
            std::size_t                         parent;
            std::pair<std::size_t, std::size_t> children;

            explicit Step(std::size_t id, std::size_t parent = UNKNOWN, std::pair<std::size_t, std::size_t> children = {UNKNOWN, UNKNOWN}):
                id(id), parent(parent), children(std::move(children)){};

            static constexpr size_t UNKNOWN = std::numeric_limits<size_t>::max();
        };

        using Path  = std::vector<std::pair<std::size_t, std::size_t>>;
        using Steps = std::vector<Step>;

        ContractionPlan() = default;
        ContractionPlan(std::size_t nleaves, const Path& path);

        Path        path{};
        Steps       steps{};
        std::size_t nleaves{};
    };

    explicit TaskBasedSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, Mode mode = Mode::Sequential, std::size_t nthreads = std::thread::hardware_concurrency()):
        CircuitSimulator(std::move(qc)), executor(nthreads) {
        // remove final measurements TODO: implement measurement support for task-based simulation
        qc::CircuitOptimizer::removeFinalMeasurements(*(this->qc));
        // TODO: Add new strategies here
        switch (mode) {
            case Mode::PairwiseRecursiveGrouping:
                generatePairwiseRecursiveGroupingContractionPlan();
                break;
            case Mode::Sequential:
            default:
                generateSequentialContractionPlan();
                break;
        }
    }

    TaskBasedSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, const ApproximationInfo approx_info, const unsigned long long seed, Mode mode = Mode::Sequential, const std::size_t nthreads = 1):
        CircuitSimulator(std::move(qc), approx_info, seed), executor(nthreads) {
        // remove final measurements TODO: implement measurement support for task-based simulation
        qc::CircuitOptimizer::removeFinalMeasurements(*(this->qc));
        // TODO: Add new strategies here
        switch (mode) {
            case Mode::PairwiseRecursiveGrouping:
                generatePairwiseRecursiveGroupingContractionPlan();
                break;
            case Mode::Sequential:
            default:
                generateSequentialContractionPlan();
                break;
        }
    }

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    const ContractionPlan& getContractionPlan() const {
        return contractionPlan;
    }
    void setContractionPlan(const ContractionPlan& plan) {
        contractionPlan = plan;
    }
    void setContractionPlan(const ContractionPlan::Path& path) {
        contractionPlan = ContractionPlan(qc->getNops() + 1, path);
    }

    // TODO: Add new strategies here
    void generateSequentialContractionPlan();
    void generatePairwiseRecursiveGroupingContractionPlan();

private:
    std::unordered_map<std::size_t, tf::Task>                                 tasks{};
    std::unordered_map<std::size_t, std::variant<qc::VectorDD, qc::MatrixDD>> results{};

    tf::Taskflow    taskflow{};
    tf::Executor    executor{};
    ContractionPlan contractionPlan{};

    void constructTaskGraph();
    void addContractionTask(std::size_t leftID, std::size_t rightID, std::size_t resultID);
};

#endif //DDSIM_TASKBASEDSIMULATOR_HPP
