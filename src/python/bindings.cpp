/*
 * This file is part of MQT DDSIM library which is released under the MIT license.
 * See file README.md or go to https://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "CircuitSimulator.hpp"
#include "HybridSchrodingerFeynmanSimulator.hpp"
#include "PathSimulator.hpp"
#include "UnitarySimulator.hpp"
#include "python/qiskit/QuantumCircuit.hpp"

#include <memory>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;

static qc::QuantumComputation importCircuit(const py::object& circ) {
    const py::object quantumCircuit =
            py::module::import("qiskit").attr("QuantumCircuit");

    auto qc = qc::QuantumComputation();

    if (py::isinstance<py::str>(circ)) {
        const auto file = circ.cast<std::string>();
        qc.import(file);
    } else if (py::isinstance(circ, quantumCircuit)) {
        qc::qiskit::QuantumCircuit::import(qc, circ);
    } else {
        throw std::runtime_error(
                "PyObject is neither py::str nor QuantumCircuit");
    }

    return qc;
}

template<class Simulator, typename... Args>
std::unique_ptr<Simulator> constructSimulator(const py::object&  circ,
                                              const double       stepFidelity,
                                              const unsigned int stepNumber,
                                              const std::string& approximationStrategy,
                                              const std::int64_t seed,
                                              Args&&... args) {
    auto       qc     = std::make_unique<qc::QuantumComputation>(importCircuit(circ));
    const auto approx = ApproximationInfo{stepFidelity, stepNumber, ApproximationInfo::fromString(approximationStrategy)};
    if constexpr (std::is_same_v<Simulator, PathSimulator<>>) {
        return std::make_unique<Simulator>(std::move(qc),
                                           std::forward<Args>(args)...);
    } else {
        if (seed < 0) {
            return std::make_unique<Simulator>(std::move(qc),
                                               approx,
                                               std::forward<Args>(args)...);
        }
        return std::make_unique<Simulator>(std::move(qc),
                                           approx,
                                           seed,
                                           std::forward<Args>(args)...);
    }
}

template<class Simulator, typename... Args>
std::unique_ptr<Simulator> constructSimulatorWithoutSeed(const py::object& circ, Args&&... args) {
    return constructSimulator<Simulator>(circ, 1., 1, "fidelity", -1, std::forward<Args>(args)...);
}

void getNumPyMatrixRec(const qc::MatrixDD& e, const std::complex<dd::fp>& amp, std::size_t i, std::size_t j, std::size_t dim, std::complex<dd::fp>* mat) {
    // calculate new accumulated amplitude
    auto w = std::complex<dd::fp>{dd::RealNumber::val(e.w.r), dd::RealNumber::val(e.w.i)};
    auto c = amp * w;

    // base case
    if (e.isTerminal()) {
        mat[i * dim + j] = c; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return;
    }

    const std::size_t x = i | (1 << e.p->v);
    const std::size_t y = j | (1 << e.p->v);

    // recursive case
    if (!e.p->e[0].w.approximatelyZero()) {
        getNumPyMatrixRec(e.p->e[0], c, i, j, dim, mat);
    }
    if (!e.p->e[1].w.approximatelyZero()) {
        getNumPyMatrixRec(e.p->e[1], c, i, y, dim, mat);
    }
    if (!e.p->e[2].w.approximatelyZero()) {
        getNumPyMatrixRec(e.p->e[2], c, x, j, dim, mat);
    }
    if (!e.p->e[3].w.approximatelyZero()) {
        getNumPyMatrixRec(e.p->e[3], c, x, y, dim, mat);
    }
}

template<class Config = dd::DDPackageConfig>
void getNumPyMatrix(UnitarySimulator<Config>& sim, py::array_t<std::complex<dd::fp>>& matrix) {
    const auto&     e            = sim.getConstructedDD();
    py::buffer_info matrixBuffer = matrix.request();
    auto*           dataPtr      = static_cast<std::complex<dd::fp>*>(matrixBuffer.ptr);
    const auto      rows         = matrixBuffer.shape[0];
    const auto      cols         = matrixBuffer.shape[1];
    if (rows != cols) {
        throw std::runtime_error("Provided matrix is not a square matrix.");
    }

    const std::size_t dim = 1 << (e.p->v + 1);
    if (static_cast<std::size_t>(rows) != dim) {
        throw std::runtime_error("Provided matrix does not have the right size.");
    }

    getNumPyMatrixRec(e, std::complex<dd::fp>{1.0, 0.0}, 0, 0, dim, dataPtr);
}

void dumpTensorNetwork(const py::object& circ, const std::string& filename) {
    const py::object quantumCircuit = py::module::import("qiskit").attr("QuantumCircuit");

    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>();

    if (py::isinstance<py::str>(circ)) {
        auto&& file1 = circ.cast<std::string>();
        qc->import(file1);
    } else if (py::isinstance(circ, quantumCircuit)) {
        qc::qiskit::QuantumCircuit::import(*qc, circ);
    } else {
        throw std::runtime_error("PyObject is neither py::str nor QuantumCircuit");
    }
    std::ofstream ofs(filename);
    qc->dump(ofs, qc::Format::Tensor);
}

dd::fp expectationValue(CircuitSimulator<>& sim, const py::object& observable) {
    const auto observableCircuit = importCircuit(observable);
    return sim.expectationValue(observableCircuit);
}

template<class Sim>
py::class_<Sim> createSimulator(py::module_ m, const std::string& name) {
    auto sim = py::class_<Sim>(m, name.c_str());
    sim.def("get_number_of_qubits", &Sim::getNumberOfQubits, "Get the number of qubits")
            .def("get_name", &Sim::getName, "Get the name of the simulator")
            .def("statistics", &Sim::additionalStatistics, "Get additional statistics provided by the simulator")
            .def("get_active_vector_node_count", &Sim::getActiveNodeCount, "Get the number of active vector nodes, i.e., the number of vector DD nodes in the unique table with a non-zero reference count.")
            .def("get_active_matrix_node_count", &Sim::getMatrixActiveNodeCount, "Get the number of active matrix nodes, i.e., the number of matrix DD nodes in the unique table with a non-zero reference count.")
            .def("get_max_vector_node_count", &Sim::getMaxNodeCount, "Get the maximum number of (active) vector nodes, i.e., the maximum number of vector DD nodes in the unique table at any point during the simulation.")
            .def("get_max_matrix_node_count", &Sim::getMaxMatrixNodeCount, "Get the maximum number of (active) matrix nodes, i.e., the maximum number of matrix DD nodes in the unique table at any point during the simulation.")
            .def("get_tolerance", &Sim::getTolerance, "Get the tolerance for the DD package.")
            .def("set_tolerance", &Sim::setTolerance, "tol"_a, "Set the tolerance for the DD package.")
            .def("export_dd_to_graphviz_str", &Sim::exportDDtoGraphvizString, "colored"_a = true, "edge_labels"_a = false, "classic"_a = false, "memory"_a = false, "format_as_polar"_a = true, "Get a Graphviz representation of the currently stored DD.")
            .def("export_dd_to_graphviz_file", &Sim::exportDDtoGraphvizFile, "filename"_a, "colored"_a = true, "edge_labels"_a = false, "classic"_a = false, "memory"_a = false, "format_as_polar"_a = true, "Write a Graphviz representation of the currently stored DD to a file.");

    if constexpr (std::is_same_v<Sim, UnitarySimulator<>>) {
        sim.def("construct", &Sim::construct, "Construct the DD representing the unitary matrix of the circuit.");
    } else {
        sim.def("simulate", &Sim::simulate, "shots"_a, "Simulate the circuit and return the result as a dictionary of counts.");
        sim.def("get_vector", &Sim::getVector, "Get the state vector resulting from the simulation.");
    }
    return sim;
}

PYBIND11_MODULE(pyddsim, m) {
    m.doc() = "Python interface for the MQT DDSIM quantum circuit simulator";

    // Circuit Simulator
    auto circuitSimulator = createSimulator<CircuitSimulator<>>(m, "CircuitSimulator");
    circuitSimulator.def(py::init<>(&constructSimulator<CircuitSimulator<>>),
                         "circ"_a,
                         "approximation_step_fidelity"_a = 1.,
                         "approximation_steps"_a         = 1,
                         "approximation_strategy"_a      = "fidelity",
                         "seed"_a                        = -1)
            .def("expectation_value", &expectationValue, "observable"_a);

    // Hybrid Schrödinger-Feynman Simulator
    py::enum_<HybridSchrodingerFeynmanSimulator<>::Mode>(m, "HybridMode")
            .value("DD", HybridSchrodingerFeynmanSimulator<>::Mode::DD)
            .value("amplitude", HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude)
            .export_values();

    auto hsfSimulator = createSimulator<HybridSchrodingerFeynmanSimulator<>>(m, "HybridCircuitSimulator");
    hsfSimulator.def(py::init<>(&constructSimulator<HybridSchrodingerFeynmanSimulator<>, HybridSchrodingerFeynmanSimulator<>::Mode&, const std::size_t&>),
                     "circ"_a,
                     "approximation_step_fidelity"_a = 1.,
                     "approximation_steps"_a         = 1,
                     "approximation_strategy"_a      = "fidelity",
                     "seed"_a                        = -1,
                     "mode"_a                        = HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude,
                     "nthreads"_a                    = 2)
            .def("get_mode", &HybridSchrodingerFeynmanSimulator<>::getMode)
            .def("get_final_amplitudes", &HybridSchrodingerFeynmanSimulator<>::getVectorFromHybridSimulation);

    // Path Simulator
    py::enum_<PathSimulator<>::Configuration::Mode>(m, "PathSimulatorMode")
            .value("sequential", PathSimulator<>::Configuration::Mode::Sequential)
            .value("pairwise_recursive", PathSimulator<>::Configuration::Mode::PairwiseRecursiveGrouping)
            .value("cotengra", PathSimulator<>::Configuration::Mode::Cotengra)
            .value("bracket", PathSimulator<>::Configuration::Mode::BracketGrouping)
            .value("alternating", PathSimulator<>::Configuration::Mode::Alternating)
            .value("gate_cost", PathSimulator<>::Configuration::Mode::GateCost)
            .export_values()
            .def(py::init([](const std::string& str) -> PathSimulator<>::Configuration::Mode { return PathSimulator<>::Configuration::modeFromString(str); }));

    py::class_<PathSimulator<>::Configuration>(m, "PathSimulatorConfiguration", "Configuration options for the Path Simulator")
            .def(py::init())
            .def_readwrite("mode", &PathSimulator<>::Configuration::mode,
                           R"pbdoc(Setting the mode used for determining a simulation path)pbdoc")
            .def_readwrite("bracket_size", &PathSimulator<>::Configuration::bracketSize,
                           R"pbdoc(Size of the brackets one wants to combine)pbdoc")
            .def_readwrite("starting_point", &PathSimulator<>::Configuration::startingPoint,
                           R"pbdoc(Start of the alternating or gate_cost strategy)pbdoc")
            .def_readwrite("gate_cost", &PathSimulator<>::Configuration::gateCost,
                           R"pbdoc(A list that contains the number of gates which are considered in each step)pbdoc")
            .def_readwrite("seed", &PathSimulator<>::Configuration::seed,
                           R"pbdoc(Seed for the simulator)pbdoc")
            .def("json", &PathSimulator<>::Configuration::json)
            .def("__repr__", &PathSimulator<>::Configuration::toString);

    auto pathSimulator = createSimulator<PathSimulator<>>(m, "PathCircuitSimulator");
    pathSimulator.def(py::init<>(&constructSimulatorWithoutSeed<PathSimulator<>, PathSimulator<>::Configuration&>),
                      "circ"_a, "config"_a = PathSimulator<>::Configuration())
            .def(py::init<>(&constructSimulatorWithoutSeed<PathSimulator<>, PathSimulator<>::Configuration::Mode&, const std::size_t&, const std::size_t&, const std::list<std::size_t>&, const std::size_t&>),
                 "circ"_a, "mode"_a = PathSimulator<>::Configuration::Mode::Sequential, "bracket_size"_a = 2, "starting_point"_a = 0, "gate_cost"_a = std::list<std::size_t>{}, "seed"_a = 0)
            .def("set_simulation_path", py::overload_cast<const PathSimulator<>::SimulationPath::Components&, bool>(&PathSimulator<>::setSimulationPath));

    // Unitary Simulator
    py::enum_<UnitarySimulator<>::Mode>(m, "ConstructionMode")
            .value("recursive", UnitarySimulator<>::Mode::Recursive)
            .value("sequential", UnitarySimulator<>::Mode::Sequential)
            .export_values();

    auto unitarySimulator = createSimulator<UnitarySimulator<>>(m, "UnitarySimulator");
    unitarySimulator.def(py::init<>(&constructSimulator<UnitarySimulator<>, UnitarySimulator<>::Mode&>),
                         "circ"_a,
                         "approximation_step_fidelity"_a = 1.,
                         "approximation_steps"_a         = 1,
                         "approximation_strategy"_a      = "fidelity",
                         "seed"_a                        = -1,
                         "mode"_a                        = UnitarySimulator<>::Mode::Recursive)
            .def("get_mode", &UnitarySimulator<>::getMode)
            .def("get_construction_time", &UnitarySimulator<>::getConstructionTime)
            .def("get_final_node_count", &UnitarySimulator<>::getFinalNodeCount)
            .def("get_max_node_count", &UnitarySimulator<>::getMaxNodeCount);

    // Miscellaneous functions
    m.def("get_matrix", &getNumPyMatrix<>, "sim"_a, "mat"_a);

    m.def("dump_tensor_network", &dumpTensorNetwork, "dump a tensor network representation of the given circuit",
          "circ"_a, "filename"_a);
}
