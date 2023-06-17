#ifndef DDSIM_PATHSIMULATOR_HPP
#define DDSIM_PATHSIMULATOR_HPP

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"
#include "dd/Operations.hpp"
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

template<class Config = dd::DDPackageConfig>
class PathSimulator: public CircuitSimulator<Config> {
public:
    struct SimulationPath {
        struct Step {
            std::size_t                         id;
            std::vector<std::size_t>            operations;
            std::size_t                         parent;
            std::pair<std::size_t, std::size_t> children;

            explicit Step(const std::size_t id_, std::vector<std::size_t> operations_ = {}, const std::size_t parent_ = UNKNOWN, std::pair<std::size_t, std::size_t> children_ = {UNKNOWN, UNKNOWN}):
                id(id_), operations(std::move(operations_)), parent(parent_), children(std::move(children_)){};

            static constexpr size_t UNKNOWN = std::numeric_limits<size_t>::max();
        };

        using Components = std::vector<std::pair<std::size_t, std::size_t>>;
        using Steps      = std::vector<Step>;

        SimulationPath() = default;
        SimulationPath(std::size_t nleaves_, Components components_, const qc::QuantumComputation* qc_, bool assumeCorrectOrder = false);

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
            Cotengra,
            GateCost
        };

        // mode to use
        Mode mode;

        // settings for the bracket size
        std::size_t bracketSize;
        // settings for the alternating and gatecost mode, which need a starting point
        std::size_t startingPoint;
        // settings for the gate costs mode
        std::list<std::size_t> gateCost;
        // random seed
        std::size_t seed;

        //Add new variables here
        explicit Configuration(const Mode mode_ = Mode::Sequential, const std::size_t bracketSize_ = 2, const std::size_t startingPoint_ = 0, std::list<std::size_t> gateCost_ = {}, const std::size_t seed_ = 0):
            mode(mode_), bracketSize(bracketSize_), startingPoint(startingPoint_), gateCost(std::move(gateCost_)), seed(seed_){};

        static Mode modeFromString(const std::string& mode) {
            if (mode == "sequential" || mode == "0") {
                return Mode::Sequential;
            }
            if (mode == "pairwise_recursive" || mode == "1") {
                return Mode::PairwiseRecursiveGrouping;
            }
            if (mode == "bracket" || mode == "2") {
                return Mode::BracketGrouping;
            }
            if (mode == "alternating" || mode == "3") {
                return Mode::Alternating;
            }
            if (mode == "cotengra" || mode == "4") {
                return Mode::Cotengra;
            }
            if (mode == "gate_cost" || mode == "5") {
                return Mode::GateCost;
            }

            throw std::invalid_argument("Invalid simulation path mode: " + mode);
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
                case Mode::GateCost:
                    return "gate_cost";
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
                conf["starting_point"] = startingPoint;
            } else if (mode == Mode::GateCost) {
                conf["starting_point"] = startingPoint;
                conf["gate_cost"]      = gateCost;
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

    explicit PathSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, Configuration configuration = Configuration()):
        CircuitSimulator<Config>(std::move(qc_)), executor(1) {
        if (configuration.seed != 0) {
            // override seed in case a non-trivial one is given
            Simulator<Config>::mt.seed(Simulator<Config>::seed);
        }

        // remove final measurements implement measurement support for task-based simulation
        qc::CircuitOptimizer::removeFinalMeasurements(*(CircuitSimulator<Config>::qc));

        // case distinction for the starting point of the alternating strategy
        if (configuration.startingPoint == 0) {
            configuration.startingPoint = (CircuitSimulator<Config>::qc->getNops()) / 2;
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
                generateAlternatingSimulationPath(configuration.startingPoint);
                break;
            case Configuration::Mode::GateCost:
                generateGatecostSimulationPath(configuration.startingPoint, configuration.gateCost);
                break;
            default:
                generateSequentialSimulationPath();
                break;
        }
    }

    PathSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, typename Configuration::Mode mode_, std::size_t bracketSize_, std::size_t startingPoint_, std::list<std::size_t> gateCost_, std::size_t seed_):
        PathSimulator(std::move(qc_), Configuration{mode_, bracketSize_, startingPoint_, std::move(gateCost_), seed_}) {}

    std::map<std::string, std::size_t> simulate(std::size_t shots) override;

    const SimulationPath& getSimulationPath() const {
        return simulationPath;
    }
    void setSimulationPath(const SimulationPath& path) {
        simulationPath = path;
    }
    void setSimulationPath(const typename SimulationPath::Components& components, bool assumeCorrectOrder = false) {
        simulationPath = SimulationPath(CircuitSimulator<Config>::qc->getNops() + 1, components, CircuitSimulator<Config>::qc.get(), assumeCorrectOrder);
    }

    // Add new strategies here
    void generateSequentialSimulationPath();
    void generatePairwiseRecursiveGroupingSimulationPath();
    void generateBracketSimulationPath(std::size_t bracketSize);
    void generateAlternatingSimulationPath(std::size_t startingPoint);
    void generateGatecostSimulationPath(std::size_t startingPoint, std::list<std::size_t>& gateCosts);

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
