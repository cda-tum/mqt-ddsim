/*
 * This file is part of MQT DDSIM library which is released under the MIT license.
 * See file README.md or go to https://iic.jku.at/eda/research/quantum/ for more information.
 */
// clang-format off
#include "CircuitSimulator.hpp"
#include "HybridSchrodingerFeynmanSimulator.hpp"
#include "PathSimulator.hpp"
#include "UnitarySimulator.hpp"
#include "qiskit/QasmQobjExperiment.hpp"
#include "qiskit/QuantumCircuit.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>
// clang-format on

namespace py = pybind11;
using namespace pybind11::literals;

static qc::QuantumComputation importCircuit(const py::object& circ) {
    const py::object quantumCircuit =
            py::module::import("qiskit").attr("QuantumCircuit");
    const py::object pyQasmQobjExperiment =
            py::module::import("qiskit.qobj").attr("QasmQobjExperiment");

    auto qc = qc::QuantumComputation();

    if (py::isinstance<py::str>(circ)) {
        const auto file = circ.cast<std::string>();
        qc.import(file);
    } else if (py::isinstance(circ, quantumCircuit)) {
        qc::qiskit::QuantumCircuit::import(qc, circ);
    } else if (py::isinstance(circ, pyQasmQobjExperiment)) {
        qc::qiskit::QasmQobjExperiment::import(qc, circ);
    } else {
        throw std::runtime_error(
                "PyObject is neither py::str, QuantumCircuit, nor QasmQobjExperiment");
    }

    return qc;
}

template<class Simulator, typename... Args>
std::unique_ptr<Simulator> create_simulator(const py::object&   circ,
                                            const double        stepFidelity,
                                            const unsigned int  stepNumber,
                                            const std::string&  approximationMethod,
                                            const long long int seed,
                                            Args&&... args) {
    auto       qc     = std::make_unique<qc::QuantumComputation>(importCircuit(circ));
    const auto approx = ApproximationInfo{stepFidelity, stepNumber, ApproximationInfo::fromString(approximationMethod)};
    if constexpr (std::is_same_v<Simulator, PathSimulator<>>) {
        return std::make_unique<Simulator>(std::move(qc),
                                           std::forward<Args>(args)...);
    } else {
        if (seed < 0) {
            return std::make_unique<Simulator>(std::move(qc),
                                               approx,
                                               std::forward<Args>(args)...);
        } else {
            return std::make_unique<Simulator>(std::move(qc),
                                               approx,
                                               seed,
                                               std::forward<Args>(args)...);
        }
    }
}

template<class Simulator, typename... Args>
std::unique_ptr<Simulator> create_simulator_without_seed(const py::object& circ, Args&&... args) {
    return create_simulator<Simulator>(circ, 1., 1, "fidelity", -1, std::forward<Args>(args)...);
}

void getNumpyMatrixRec(const qc::MatrixDD& e, const std::complex<dd::fp>& amp, std::size_t i, std::size_t j, std::size_t dim, std::complex<dd::fp>* mat) {
    // calculate new accumulated amplitude
    auto w = std::complex<dd::fp>{dd::CTEntry::val(e.w.r), dd::CTEntry::val(e.w.i)};
    auto c = amp * w;

    // base case
    if (e.isTerminal()) {
        mat[i * dim + j] = c;
        return;
    }

    std::size_t x = i | (1 << e.p->v);
    std::size_t y = j | (1 << e.p->v);

    // recursive case
    if (!e.p->e[0].w.approximatelyZero())
        getNumpyMatrixRec(e.p->e[0], c, i, j, dim, mat);
    if (!e.p->e[1].w.approximatelyZero())
        getNumpyMatrixRec(e.p->e[1], c, i, y, dim, mat);
    if (!e.p->e[2].w.approximatelyZero())
        getNumpyMatrixRec(e.p->e[2], c, x, j, dim, mat);
    if (!e.p->e[3].w.approximatelyZero())
        getNumpyMatrixRec(e.p->e[3], c, x, y, dim, mat);
}

template<class Config = dd::DDPackageConfig>
void getNumpyMatrix(UnitarySimulator<Config>& sim, py::array_t<std::complex<dd::fp>>& matrix) {
    const auto&     e            = sim.getConstructedDD();
    py::buffer_info matrixBuffer = matrix.request();
    auto*           dataPtr      = static_cast<std::complex<dd::fp>*>(matrixBuffer.ptr);
    const auto      rows         = matrixBuffer.shape[0];
    const auto      cols         = matrixBuffer.shape[1];
    if (rows != cols) {
        throw std::runtime_error("Provided matrix is not a square matrix.");
    }

    std::size_t dim = 1 << (e.p->v + 1);
    if (static_cast<std::size_t>(rows) != dim) {
        throw std::runtime_error("Provided matrix does not have the right size.");
    }

    getNumpyMatrixRec(e, std::complex<dd::fp>{1.0, 0.0}, 0, 0, dim, dataPtr);
}

void dump_tensor_network(const py::object& circ, const std::string& filename) {
    py::object QuantumCircuit       = py::module::import("qiskit").attr("QuantumCircuit");
    py::object pyQasmQobjExperiment = py::module::import("qiskit.qobj").attr("QasmQobjExperiment");

    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>();

    if (py::isinstance<py::str>(circ)) {
        auto&& file1 = circ.cast<std::string>();
        qc->import(file1);
    } else if (py::isinstance(circ, QuantumCircuit)) {
        qc::qiskit::QuantumCircuit::import(*qc, circ);
    } else if (py::isinstance(circ, pyQasmQobjExperiment)) {
        qc::qiskit::QasmQobjExperiment::import(*qc, circ);
    } else {
        throw std::runtime_error("PyObject is neither py::str, QuantumCircuit, nor QasmQobjExperiment");
    }
    std::ofstream ofs(filename);
    qc->dump(ofs, qc::Format::Tensor);
}

PYBIND11_MODULE(pyddsim, m) {
    m.doc() = "Python interface for the MQT DDSIM quantum circuit simulator";

    py::class_<CircuitSimulator<>>(m, "CircuitSimulator")
            .def(py::init<>(&create_simulator<CircuitSimulator<>>),
                 "circ"_a,
                 "approximation_step_fidelity"_a = 1.,
                 "approximation_steps"_a         = 1,
                 "approximation_method"_a        = "fidelity",
                 "seed"_a                        = -1)
            .def("get_number_of_qubits", &CircuitSimulator<>::getNumberOfQubits)
            .def("get_name", &CircuitSimulator<>::getName)
            .def("simulate", &CircuitSimulator<>::Simulate, "shots"_a)
            .def("statistics", &CircuitSimulator<>::AdditionalStatistics)
            .def("get_vector", &CircuitSimulator<>::getVectorComplex);

    py::enum_<HybridSchrodingerFeynmanSimulator<>::Mode>(m, "HybridMode")
            .value("DD", HybridSchrodingerFeynmanSimulator<>::Mode::DD)
            .value("amplitude", HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude)
            .export_values();

    py::class_<HybridSchrodingerFeynmanSimulator<>>(m, "HybridCircuitSimulator")
            .def(py::init<>(&create_simulator<HybridSchrodingerFeynmanSimulator<>, HybridSchrodingerFeynmanSimulator<>::Mode&, const std::size_t&>),
                 "circ"_a,
                 "approximation_step_fidelity"_a = 1.,
                 "approximation_steps"_a         = 1,
                 "approximation_method"_a        = "fidelity",
                 "seed"_a                        = -1,
                 "mode"_a                        = HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude,
                 "nthreads"_a                    = 2)
            .def("get_number_of_qubits", &CircuitSimulator<>::getNumberOfQubits)
            .def("get_name", &CircuitSimulator<>::getName)
            .def("simulate", &HybridSchrodingerFeynmanSimulator<>::Simulate, "shots"_a)
            .def("statistics", &CircuitSimulator<>::AdditionalStatistics)
            .def("get_vector", &CircuitSimulator<>::getVectorComplex)
            .def("get_mode", &HybridSchrodingerFeynmanSimulator<>::getMode)
            .def("get_final_amplitudes", &HybridSchrodingerFeynmanSimulator<>::getFinalAmplitudes);

    // TODO: Add new strategies here
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

    py::class_<PathSimulator<>>(m, "PathCircuitSimulator")
            .def(py::init<>(&create_simulator_without_seed<PathSimulator<>, PathSimulator<>::Configuration&>),
                 "circ"_a, "config"_a = PathSimulator<>::Configuration())
            .def(py::init<>(&create_simulator_without_seed<PathSimulator<>, PathSimulator<>::Configuration::Mode&, const std::size_t&, const std::size_t&, const std::list<std::size_t>&, const std::size_t&>),
                 "circ"_a, "mode"_a = PathSimulator<>::Configuration::Mode::Sequential, "bracket_size"_a = 2, "starting_point"_a = 0, "gate_cost"_a = std::list<std::size_t>{}, "seed"_a = 0)
            .def("set_simulation_path", py::overload_cast<const PathSimulator<>::SimulationPath::Components&, bool>(&PathSimulator<>::setSimulationPath))
            .def("get_number_of_qubits", &CircuitSimulator<>::getNumberOfQubits)
            .def("get_name", &CircuitSimulator<>::getName)
            .def("simulate", &PathSimulator<>::Simulate, "shots"_a)
            .def("statistics", &CircuitSimulator<>::AdditionalStatistics)
            .def("get_vector", &CircuitSimulator<>::getVectorComplex);

    py::enum_<UnitarySimulator<>::Mode>(m, "ConstructionMode")
            .value("recursive", UnitarySimulator<>::Mode::Recursive)
            .value("sequential", UnitarySimulator<>::Mode::Sequential)
            .export_values();

    py::class_<UnitarySimulator<>>(m, "UnitarySimulator")
            .def(py::init<>(&create_simulator<UnitarySimulator<>, UnitarySimulator<>::Mode&>),
                 "circ"_a,
                 "approximation_step_fidelity"_a = 1.,
                 "approximation_steps"_a         = 1,
                 "approximation_method"_a        = "fidelity",
                 "seed"_a                        = -1,
                 "mode"_a                        = UnitarySimulator<>::Mode::Recursive)
            .def("get_number_of_qubits", &CircuitSimulator<>::getNumberOfQubits)
            .def("get_name", &CircuitSimulator<>::getName)
            .def("construct", &UnitarySimulator<>::Construct)
            .def("get_mode", &UnitarySimulator<>::getMode)
            .def("get_construction_time", &UnitarySimulator<>::getConstructionTime)
            .def("get_final_node_count", &UnitarySimulator<>::getFinalNodeCount)
            .def("get_max_node_count", &UnitarySimulator<>::getMaxNodeCount);

    m.def("get_matrix", &getNumpyMatrix<>, "sim"_a, "mat"_a);

    m.def("dump_tensor_network", &dump_tensor_network, "dump a tensor network representation of the given circuit",
          "circ"_a, "filename"_a);

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
