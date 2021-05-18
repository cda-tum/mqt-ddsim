/*
 * This file is part of JKQ DDSIM library which is released under the MIT license.
 * See file README.md or go to https://iic.jku.at/eda/research/quantum/ for more information.
 */
#include "CircuitSimulator.hpp"

#include "QiskitImport.hpp"

#include "nlohmann/json.hpp"

#include "pybind11/pybind11.h"
#include "pybind11/complex.h"
#include "pybind11_json/pybind11_json.hpp"
#include "pybind11/stl.h"

#include <iostream>
#include <memory>

namespace py = pybind11;
namespace nl = nlohmann;


std::unique_ptr<CircuitSimulator> create_circuit_simulator(const py::object& circ) {
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>();

    if(py::isinstance<py::str>(circ)) {
        auto&& file1 = circ.cast<std::string>();
        qc->import(file1);
    } else {
        import(*qc, circ);
    }

    return std::make_unique<CircuitSimulator>(std::move(qc));
}

PYBIND11_MODULE(pyddsim, m) {
    m.doc() = "Python interface for the JKQ DDSIM quantum circuit simulator";

    py::class_<CircuitSimulator>(m, "CircuitSimulator")
            .def(py::init(&create_circuit_simulator))
            .def("get_number_of_qubits", &CircuitSimulator::getNumberOfQubits)
            .def("get_name", &CircuitSimulator::getName)
            .def("simulate", &CircuitSimulator::Simulate)
            .def("statistics", &CircuitSimulator::AdditionalStatistics)
            .def("get_vector", &CircuitSimulator::getVectorComplex);



    #ifdef VERSION_INFO
        m.attr("__version__") = VERSION_INFO;
    #else
        m.attr("__version__") = "dev";
    #endif
}