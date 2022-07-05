#ifndef DDSIM_PATHSIMULATOR_HPP
#define DDSIM_PATHSIMULATOR_HPP

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"
#include "nlohmann/json.hpp"

#include <future>
#include <limits>
#include <taskflow/taskflow.hpp>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

template<class DDPackage = dd::Package<>>
class PathSimulator: public CircuitSimulator<DDPackage> {
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

        using Components = std::vector<std::pair<std::size_t, std::size_t>>;
        using Steps      = std::vector<Step>;

        SimulationPath() = default;
        SimulationPath(std::size_t nleaves, Components components, const qc::QuantumComputation* qc, bool assumeCorrectOrder = false);

        Components                    components{};
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

        // mode to use
        Mode mode;

        // settings for the bracket size
        std::size_t bracketSize;
        // settings for the alternating mode
        std::size_t alternatingStart;

        // random seed
        std::size_t seed;

        //Add new variables here
        explicit Configuration(Mode mode = Mode::Sequential, std::size_t bracketSize = 2, std::size_t alternatingStart = 0, std::size_t seed = 0):
            mode(mode), bracketSize(bracketSize), alternatingStart(alternatingStart), seed(seed){};

        static Mode modeFromString(const std::string& mode) {
            if (mode == "sequential" || mode == "0") {
                return Mode::Sequential;
            } else if (mode == "pairwise_recursive" || mode == "1") {
                return Mode::PairwiseRecursiveGrouping;
            } else if (mode == "bracket" || mode == "2") {
                return Mode::BracketGrouping;
            } else if (mode == "alternating" || mode == "3") {
                return Mode::Alternating;
            } else if (mode == "cotengra" || mode == "4") {
                return Mode::Cotengra;
            } else {
                throw std::invalid_argument("Invalid simulation path mode: " + mode);
            }
        }

        static std::string modeToString(const Mode& mode) {
            switch (mode) {
                case Mode::Sequential:
                    return "sequential";
                case Mode::PairwiseRecursiveGrouping:
                    return "pairwise_recursive";
                case Mode::BracketGrouping:
                    return "bracket";
                case Mode::Alternating:
                    return "alternating";
                case Mode::Cotengra:
                    return "cotengra";
                default:
                    throw std::invalid_argument("Invalid simulation path mode");
            }
        }

        [[nodiscard]] nlohmann::json json() const {
            nlohmann::json conf{};
            conf["mode"] = modeToString(mode);
            if (mode == Mode::BracketGrouping) {
                conf["bracket_size"] = bracketSize;
            } else if (mode == Mode::Alternating) {
                conf["alternating_start"] = alternatingStart;
            }
            if (seed != 0) {
                conf["seed"] = seed;
            }
            return conf;
        }

        [[nodiscard]] std::string toString() const {
            return json().dump(2);
        }
    };

    explicit PathSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, Configuration configuration = Configuration()):
        CircuitSimulator<DDPackage>(std::move(qc)), executor(1) {
        if (configuration.seed != 0) {
            // override seed in case a non-trivial one is given
            Simulator<DDPackage>::mt.seed(Simulator<DDPackage>::seed);
        }

        // remove final measurements implement measurement support for task-based simulation
        qc::CircuitOptimizer::removeFinalMeasurements(*(this->qc));

        // case distinction for the starting point of the alternating strategy
        if (configuration.alternatingStart == 0) {
            configuration.alternatingStart = (this->qc->getNops()) / 2;
        }

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
                generateAlternatingSimulationPath(configuration.alternatingStart);
                break;
            case Configuration::Mode::Sequential:
            default:
                generateSequentialSimulationPath();
                break;
        }
    }

    PathSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, typename Configuration::Mode mode, std::size_t bracketSize, std::size_t alternatingStart, std::size_t seed):
        PathSimulator(std::move(qc), Configuration{mode, bracketSize, alternatingStart, seed}) {}

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    const SimulationPath& getSimulationPath() const {
        return simulationPath;
    }
    void setSimulationPath(const SimulationPath& path) {
        simulationPath = path;
    }
    void setSimulationPath(const typename SimulationPath::Components& components, bool assumeCorrectOrder = false) {
        simulationPath = SimulationPath(CircuitSimulator<DDPackage>::qc->getNops() + 1, components, CircuitSimulator<DDPackage>::qc.get(), assumeCorrectOrder);
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
