#ifndef DDSIM_PATHSIMULATOR_HPP
#define DDSIM_PATHSIMULATOR_HPP

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

class PathSimulator: public CircuitSimulator {
public:
    // Add new strategies here
    enum class Mode {
        Sequential,
        SequentialFalse,
        PairwiseRecursiveGrouping,
        BracketGrouping3,
        BracketGrouping7,
        BestCase,
        AvgCase,
        Cotengra
    };

    struct SimulationPath {
        struct Step {
            std::size_t                         id;
            std::vector<std::size_t>            operations;
            std::size_t                         parent;
            std::pair<std::size_t, std::size_t> children;

            explicit Step(std::size_t id, std::vector<std::size_t> operations = {}, std::size_t parent = UNKNOWN, std::pair<std::size_t, std::size_t> children = {UNKNOWN, UNKNOWN}):
                id(id), operations(std::move(operations)), parent(parent), children(std::move(children)){};

            static constexpr size_t UNKNOWN = std::numeric_limits<size_t>::max();
        };

        using Path  = std::vector<std::pair<std::size_t, std::size_t>>;
        using Steps = std::vector<Step>;

        SimulationPath() = default;
        SimulationPath(std::size_t nleaves, Path path, const qc::QuantumComputation* qc, bool assumeCorrectOrder = false);

        Path                          path{};
        Steps                         steps{};
        std::size_t                   nleaves{};
        const qc::QuantumComputation* qc{};
    };

    explicit PathSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, Mode mode = Mode::Sequential, std::size_t nthreads = std::thread::hardware_concurrency()):
        CircuitSimulator(std::move(qc)), executor(nthreads) {
        // remove final measurements implement measurement support for task-based simulation
        qc::CircuitOptimizer::removeFinalMeasurements(*(this->qc));
        // Add new strategies here
        switch (mode) {
            case Mode::BracketGrouping3:
                generateBracketSimulationPath(3);
                break;
            case Mode::PairwiseRecursiveGrouping:
                generatePairwiseRecursiveGroupingSimulationPath();
                break;
            case Mode::SequentialFalse:
                generateSequentialSimulationPathFalseOrder();
                break;
            case Mode::Cotengra:
            case Mode::AvgCase:
            case Mode::BestCase:
                // in this case the contraction plan is explicitly set
                break;
            case Mode::Sequential:
            default:
                generateSequentialSimulationPath();
                break;
        }
    }

    PathSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, const ApproximationInfo approx_info, const unsigned long long seed, Mode mode = Mode::Sequential, const std::size_t nthreads = 1):
        CircuitSimulator(std::move(qc), approx_info, seed), executor(nthreads) {
        // remove final measurements implement measurement support for task-based simulation
        qc::CircuitOptimizer::removeFinalMeasurements(*(this->qc));
        // Add new strategies here
        switch (mode) {
            case Mode::BracketGrouping3:
                generateBracketSimulationPath(3);
                break;
            case Mode::PairwiseRecursiveGrouping:
                generatePairwiseRecursiveGroupingSimulationPath();
                break;
            case Mode::SequentialFalse:
                generateSequentialSimulationPathFalseOrder();
                break;
            case Mode::Cotengra:
            case Mode::AvgCase:
            case Mode::BestCase:
                // in this case the contraction plan is explicitly set
                break;
            case Mode::Sequential:
            default:
                generateSequentialSimulationPath();
                break;
        }
    }

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    const SimulationPath& getSimulationPath() const {
        return simulationPath;
    }
    void setSimulationPath(const SimulationPath& plan) {
        simulationPath = plan;
    }
    void setSimulationPath(const SimulationPath::Path& path, bool assumeCorrectOrder = false) {
        simulationPath = SimulationPath(qc->getNops() + 1, path, qc.get(), assumeCorrectOrder);
    }

    // Add new strategies here
    void generateSequentialSimulationPath();
    void generateSequentialSimulationPathFalseOrder();
    void generatePairwiseRecursiveGroupingSimulationPath();
    void generateBracketSimulationPath(std::size_t bracketSize);

private:
    std::unordered_map<std::size_t, tf::Task>                                 tasks{};
    std::unordered_map<std::size_t, std::variant<qc::VectorDD, qc::MatrixDD>> results{};

    tf::Taskflow   taskflow{};
    tf::Executor   executor{};
    SimulationPath simulationPath{};

    void constructTaskGraph();
    void addContractionTask(std::size_t leftID, std::size_t rightID, std::size_t resultID);
};

#endif //DDSIM_PATHSIMULATOR_HPP
