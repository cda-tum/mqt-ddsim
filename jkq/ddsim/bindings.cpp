/*
 * This file is part of JKQ DDSIM library which is released under the MIT license.
 * See file README.md or go to https://iic.jku.at/eda/research/quantum/ for more information.
 */
// clang-format off
#include "CircuitSimulator.hpp"
#include "HybridSchrodingerFeynmanSimulator.hpp"
#include "qiskit/QasmQobjExperiment.hpp"
#include "qiskit/QuantumCircuit.hpp"

#include "nlohmann/json.hpp"
#include "pybind11/complex.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11_json/pybind11_json.hpp"
// clang-format on

#include <iostream>
#include <memory>

namespace py = pybind11;
namespace nl = nlohmann;
using namespace pybind11::literals;

std::unique_ptr<CircuitSimulator> create_circuit_simulator(const py::object& circ, const long long int seed) {
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
    if (seed < 0) {
        return std::make_unique<CircuitSimulator>(std::move(qc));
    } else {
        return std::make_unique<CircuitSimulator>(std::move(qc), ApproximationInfo{1, 1, ApproximationInfo::ApproximationWhen::FidelityDriven}, seed);
    }
}

std::unique_ptr<CircuitSimulator> create_circuit_simulator_without_seed(const py::object& circ) {
    return create_circuit_simulator(circ, -1);
}

std::unique_ptr<HybridSchrodingerFeynmanSimulator> create_hybrid_circuit_simulator(const py::object& circ, const long long seed, HybridSchrodingerFeynmanSimulator::Mode mode = HybridSchrodingerFeynmanSimulator::Mode::Amplitude, const std::size_t nthreads = 2) {
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

    if (seed < 0) {
        return std::make_unique<HybridSchrodingerFeynmanSimulator>(std::move(qc), mode, nthreads);
    } else {
        return std::make_unique<HybridSchrodingerFeynmanSimulator>(std::move(qc), ApproximationInfo{1, 1, ApproximationInfo::ApproximationWhen::FidelityDriven}, seed, mode, nthreads);
    }
}

std::unique_ptr<HybridSchrodingerFeynmanSimulator> create_hybrid_circuit_simulator_without_seed(const py::object& circ, HybridSchrodingerFeynmanSimulator::Mode mode = HybridSchrodingerFeynmanSimulator::Mode::Amplitude, const std::size_t nthreads = 2) {
    return create_hybrid_circuit_simulator(circ, -1, mode, nthreads);
}

PYBIND11_MODULE(pyddsim, m) {
    m.doc() = "Python interface for the JKQ DDSIM quantum circuit simulator";

    py::class_<CircuitSimulator>(m, "CircuitSimulator")
            .def(py::init<>(&create_circuit_simulator), "circ"_a, "seed"_a)
            .def(py::init<>(&create_circuit_simulator_without_seed), "circ"_a)
            .def("get_number_of_qubits", &CircuitSimulator::getNumberOfQubits)
            .def("get_name", &CircuitSimulator::getName)
            .def("simulate", &CircuitSimulator::Simulate, "shots"_a)
            .def("statistics", &CircuitSimulator::AdditionalStatistics)
            .def("get_vector", &CircuitSimulator::getVectorComplex);

    py::enum_<HybridSchrodingerFeynmanSimulator::Mode>(m, "HybridMode")
            .value("DD", HybridSchrodingerFeynmanSimulator::Mode::DD)
            .value("amplitude", HybridSchrodingerFeynmanSimulator::Mode::Amplitude)
            .export_values();

    py::class_<HybridSchrodingerFeynmanSimulator>(m, "HybridCircuitSimulator")
            .def(py::init<>(&create_hybrid_circuit_simulator), "circ"_a, "seed"_a, "mode"_a = HybridSchrodingerFeynmanSimulator::Mode::Amplitude, "nthreads"_a = 2)
            .def(py::init<>(&create_hybrid_circuit_simulator_without_seed), "circ"_a, "mode"_a = HybridSchrodingerFeynmanSimulator::Mode::Amplitude, "nthreads"_a = 2)
            .def("get_number_of_qubits", &CircuitSimulator::getNumberOfQubits)
            .def("get_name", &CircuitSimulator::getName)
            .def("simulate", &HybridSchrodingerFeynmanSimulator::Simulate, "shots"_a)
            .def("statistics", &CircuitSimulator::AdditionalStatistics)
            .def("get_vector", &CircuitSimulator::getVectorComplex)
            .def("get_mode", &HybridSchrodingerFeynmanSimulator::getMode)
            .def("get_final_amplitudes", &HybridSchrodingerFeynmanSimulator::getFinalAmplitudes);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
