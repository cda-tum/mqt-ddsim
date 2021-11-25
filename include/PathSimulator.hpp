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

        using ComponentsList = std::vector<std::pair<std::size_t, std::size_t>>;
        using Steps          = std::vector<Step>;

        SimulationPath() = default;
        SimulationPath(std::size_t nleaves, ComponentsList components, const qc::QuantumComputation* qc, bool assumeCorrectOrder = false);

        ComponentsList                components{};
        Steps                         steps{};
        std::size_t                   nleaves{};
        const qc::QuantumComputation* qc{};
    };

    struct Configuration {
        // Add new strategies here
        enum class Mode {
            Sequential,
            PairwiseRecursiveGrouping,
            BracketGrouping,
            Alternating,
            Cotengra
        };
        //Number of seeds
        std::size_t seed = 0;
        //Number of threads
        std::size_t nthreads = 1;
        //settings for the alternating mode
        std::size_t alternateStarting = 0;
        //settings for the bracket size
        std::size_t bracketSize = 2;
        Mode        mode        = Mode::Sequential;
        //Add new variables here

        Configuration(Mode mode1, std::size_t var1):
            mode(mode1), alternateStarting(var1), bracketSize(var1) {}
        Configuration() = default;
    };

    explicit PathSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, Configuration configuration):
        CircuitSimulator(std::move(qc)), executor(configuration.nthreads) {
        // remove final measurements implement measurement support for task-based simulation
        qc::CircuitOptimizer::removeFinalMeasurements(*(this->qc));

        // case distinction for the starting point of the alternating strategie
        std::size_t alternateStart = 0;
        if (configuration.alternateStarting == 0)
            alternateStart = (this->qc->getNops()) / 2;
        else
            alternateStart = configuration.alternateStarting;

        // Add new strategies here
        switch (configuration.mode) {
            case Configuration::Mode::BracketGrouping:
                generateBracketSimulationPath(configuration.bracketSize);
                break;
            case Configuration::Mode::PairwiseRecursiveGrouping:
                generatePairwiseRecursiveGroupingSimulationPath();
                break;
            case Configuration::Mode::Cotengra:
                break;
            case Configuration::Mode::Alternating:
                generateAlternatingSimulationPath(alternateStart);
                break;
            case Configuration::Mode::Sequential:
            default:
                generateSequentialSimulationPath();
                break;
        }
    }

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    const SimulationPath& getSimulationPath() const {
        return simulationPath;
    }
    void setSimulationPath(const SimulationPath& path) {
        simulationPath = path;
    }
    void setSimulationPath(const SimulationPath::ComponentsList& components, bool assumeCorrectOrder = false) {
        simulationPath = SimulationPath(qc->getNops() + 1, components, qc.get(), assumeCorrectOrder);
    }

    // Add new strategies here
    void generateSequentialSimulationPath();
    void generatePairwiseRecursiveGroupingSimulationPath();
    void generateBracketSimulationPath(std::size_t bracketSize);
    void generateAlternatingSimulationPath(std::size_t startingPoint);

private:
    std::unordered_map<std::size_t, tf::Task>                                 tasks{};
    std::unordered_map<std::size_t, std::variant<qc::VectorDD, qc::MatrixDD>> results{};

    tf::Taskflow   taskflow{};
    tf::Executor   executor{};
    SimulationPath simulationPath{};

    void constructTaskGraph();
    void addSimulationTask(std::size_t leftID, std::size_t rightID, std::size_t resultID);
};

#endif //DDSIM_PATHSIMULATOR_HPP
