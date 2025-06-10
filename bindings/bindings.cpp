/*
 * Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
 * Copyright (c) 2025 Munich Quantum Software Company GmbH
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include "CircuitSimulator.hpp"
#include "DeterministicNoiseSimulator.hpp"
#include "HybridSchrodingerFeynmanSimulator.hpp"
#include "PathSimulator.hpp"
#include "StochasticNoiseSimulator.hpp"
#include "UnitarySimulator.hpp"
#include "ir/QuantumComputation.hpp"

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <pybind11/cast.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // NOLINT(misc-include-cleaner)
#include <string>
#include <utility>

namespace py = pybind11;
using namespace pybind11::literals;

namespace {

template <class Simulator, typename... Args>
std::unique_ptr<Simulator>
constructSimulator(const qc::QuantumComputation& circ,
                   const double stepFidelity, const unsigned int stepNumber,
                   const std::string& approximationStrategy,
                   const std::int64_t seed, Args&&... args) {
  auto qc = std::make_unique<qc::QuantumComputation>(circ);
  const auto approx =
      ApproximationInfo{stepFidelity, stepNumber,
                        ApproximationInfo::fromString(approximationStrategy)};
  if constexpr (std::is_same_v<Simulator, PathSimulator>) {
    return std::make_unique<Simulator>(std::move(qc),
                                       std::forward<Args>(args)...);
  } else {
    if (seed < 0) {
      return std::make_unique<Simulator>(std::move(qc), approx,
                                         std::forward<Args>(args)...);
    }
    return std::make_unique<Simulator>(std::move(qc), approx, seed,
                                       std::forward<Args>(args)...);
  }
}

template <class Simulator, typename... Args>
std::unique_ptr<Simulator>
constructSimulatorWithoutSeed(const qc::QuantumComputation& circ,
                              Args&&... args) {
  return constructSimulator<Simulator>(circ, 1., 1, "fidelity", -1,
                                       std::forward<Args>(args)...);
}

template <class Sim>
py::class_<Sim> createSimulator(py::module_ m, const std::string& name) {
  auto sim = py::class_<Sim>(std::move(m), name.c_str());
  sim.def("get_number_of_qubits", &Sim::getNumberOfQubits,
          "Get the number of qubits")
      .def("get_name", &Sim::getName, "Get the name of the simulator")
      .def("statistics", &Sim::additionalStatistics,
           "Get additional statistics provided by the simulator")
      .def("get_active_vector_node_count", &Sim::getActiveNodeCount,
           "Get the number of active vector nodes, i.e., the number of vector "
           "DD nodes in the unique table with a non-zero reference count.")
      .def("get_active_matrix_node_count", &Sim::getMatrixActiveNodeCount,
           "Get the number of active matrix nodes, i.e., the number of matrix "
           "DD nodes in the unique table with a non-zero reference count.")
      .def("get_max_vector_node_count", &Sim::getMaxNodeCount,
           "Get the maximum number of (active) vector nodes, i.e., the maximum "
           "number of vector DD nodes in the unique table at any point during "
           "the simulation.")
      .def("get_max_matrix_node_count", &Sim::getMaxMatrixNodeCount,
           "Get the maximum number of (active) matrix nodes, i.e., the maximum "
           "number of matrix DD nodes in the unique table at any point during "
           "the simulation.")
      .def("get_tolerance", &Sim::getTolerance,
           "Get the tolerance for the DD package.")
      .def("set_tolerance", &Sim::setTolerance, "tol"_a,
           "Set the tolerance for the DD package.");

  if constexpr (std::is_same_v<Sim, UnitarySimulator>) {
    sim.def("construct", &Sim::construct,
            "Construct the DD representing the unitary matrix of the circuit.");
  } else {
    sim.def("simulate", &Sim::simulate, "shots"_a,
            "Simulate the circuit and return the result as a dictionary of "
            "counts.");
    sim.def("get_constructed_dd", &Sim::getCurrentDD,
            "Get the vector DD resulting from the simulation.");
  }
  return sim;
}

} // namespace

PYBIND11_MODULE(pyddsim, m, py::mod_gil_not_used()) {
  py::module::import("mqt.core.dd");
  m.doc() = "Python interface for the MQT DDSIM quantum circuit simulator";

  // Circuit Simulator
  auto circuitSimulator =
      createSimulator<CircuitSimulator>(m, "CircuitSimulator");
  circuitSimulator
      .def(py::init<>(&constructSimulator<CircuitSimulator>), "circ"_a,
           "approximation_step_fidelity"_a = 1., "approximation_steps"_a = 1,
           "approximation_strategy"_a = "fidelity", "seed"_a = -1)
      .def("expectation_value", &CircuitSimulator::expectationValue,
           "observable"_a);

  // Stoch simulator
  auto stochasticNoiseSimulator =
      createSimulator<StochasticNoiseSimulator>(m, "StochasticNoiseSimulator");
  stochasticNoiseSimulator.def(
      py::init<>(&constructSimulator<StochasticNoiseSimulator, std::string,
                                     double, std::optional<double>, double>),
      "circ"_a, "approximation_step_fidelity"_a = 1.,
      "approximation_steps"_a = 1, "approximation_strategy"_a = "fidelity",
      "seed"_a = -1, "noise_effects"_a = "APD", "noise_probability"_a = 0.01,
      "amp_damping_probability"_a = 0.02, "multi_qubit_gate_factor"_a = 2);

  // Deterministic simulator
  auto deterministicNoiseSimulator =
      createSimulator<DeterministicNoiseSimulator>(
          m, "DeterministicNoiseSimulator");
  deterministicNoiseSimulator.def(
      py::init<>(&constructSimulator<DeterministicNoiseSimulator, std::string,
                                     double, std::optional<double>, double>),
      "circ"_a, "approximation_step_fidelity"_a = 1.,
      "approximation_steps"_a = 1, "approximation_strategy"_a = "fidelity",
      "seed"_a = -1, "noise_effects"_a = "APD", "noise_probability"_a = 0.01,
      "amp_damping_probability"_a = 0.02, "multi_qubit_gate_factor"_a = 2);

  // Hybrid Schrödinger-Feynman Simulator
  py::enum_<HybridSchrodingerFeynmanSimulator::Mode>(m, "HybridMode")
      .value("DD", HybridSchrodingerFeynmanSimulator::Mode::DD)
      .value("amplitude", HybridSchrodingerFeynmanSimulator::Mode::Amplitude)
      .export_values();

  auto hsfSimulator = createSimulator<HybridSchrodingerFeynmanSimulator>(
      m, "HybridCircuitSimulator");
  hsfSimulator
      .def(py::init<>(
               &constructSimulator<HybridSchrodingerFeynmanSimulator,
                                   HybridSchrodingerFeynmanSimulator::Mode&,
                                   const std::size_t&>),
           "circ"_a, "approximation_step_fidelity"_a = 1.,
           "approximation_steps"_a = 1, "approximation_strategy"_a = "fidelity",
           "seed"_a = -1,
           "mode"_a = HybridSchrodingerFeynmanSimulator::Mode::Amplitude,
           "nthreads"_a = 2)
      .def("get_mode", &HybridSchrodingerFeynmanSimulator::getMode)
      .def("get_final_amplitudes",
           &HybridSchrodingerFeynmanSimulator::getVectorFromHybridSimulation);

  // Path Simulator
  py::enum_<PathSimulator::Configuration::Mode>(m, "PathSimulatorMode")
      .value("sequential", PathSimulator::Configuration::Mode::Sequential)
      .value("pairwise_recursive",
             PathSimulator::Configuration::Mode::PairwiseRecursiveGrouping)
      .value("bracket", PathSimulator::Configuration::Mode::BracketGrouping)
      .value("alternating", PathSimulator::Configuration::Mode::Alternating)
      .value("gate_cost", PathSimulator::Configuration::Mode::GateCost)
      .export_values()
      .def(py::init(
          [](const std::string& str) -> PathSimulator::Configuration::Mode {
            return PathSimulator::Configuration::modeFromString(str);
          }));

  py::class_<PathSimulator::Configuration>(
      m, "PathSimulatorConfiguration",
      "Configuration options for the Path Simulator")
      .def(py::init())
      .def_readwrite(
          "mode", &PathSimulator::Configuration::mode,
          R"pbdoc(Setting the mode used for determining a simulation path)pbdoc")
      .def_readwrite("bracket_size", &PathSimulator::Configuration::bracketSize,
                     R"pbdoc(Size of the brackets one wants to combine)pbdoc")
      .def_readwrite(
          "starting_point", &PathSimulator::Configuration::startingPoint,
          R"pbdoc(Start of the alternating or gate_cost strategy)pbdoc")
      .def_readwrite(
          "gate_cost", &PathSimulator::Configuration::gateCost,
          R"pbdoc(A list that contains the number of gates which are considered in each step)pbdoc")
      .def_readwrite("seed", &PathSimulator::Configuration::seed,
                     R"pbdoc(Seed for the simulator)pbdoc")
      .def("json", &PathSimulator::Configuration::json)
      .def("__repr__", &PathSimulator::Configuration::toString);

  auto pathSimulator =
      createSimulator<PathSimulator>(m, "PathCircuitSimulator");
  pathSimulator
      .def(py::init<>(
               &constructSimulatorWithoutSeed<PathSimulator,
                                              PathSimulator::Configuration&>),
           "circ"_a, "config"_a = PathSimulator::Configuration())
      .def(py::init<>(&constructSimulatorWithoutSeed<
                      PathSimulator, PathSimulator::Configuration::Mode&,
                      const std::size_t&, const std::size_t&,
                      const std::list<std::size_t>&, const std::size_t&>),
           "circ"_a, "mode"_a = PathSimulator::Configuration::Mode::Sequential,
           "bracket_size"_a = 2, "starting_point"_a = 0,
           "gate_cost"_a = std::list<std::size_t>{}, "seed"_a = 0)
      .def("set_simulation_path",
           py::overload_cast<const PathSimulator::SimulationPath::Components&,
                             bool>(&PathSimulator::setSimulationPath),
           "path"_a, "assume_correct_order"_a = false);

  // Unitary Simulator
  py::enum_<UnitarySimulator::Mode>(m, "ConstructionMode")
      .value("recursive", UnitarySimulator::Mode::Recursive)
      .value("sequential", UnitarySimulator::Mode::Sequential)
      .export_values();

  auto unitarySimulator =
      createSimulator<UnitarySimulator>(m, "UnitarySimulator");
  unitarySimulator
      .def(py::init<>(
               &constructSimulator<UnitarySimulator, UnitarySimulator::Mode&>),
           "circ"_a, "approximation_step_fidelity"_a = 1.,
           "approximation_steps"_a = 1, "approximation_strategy"_a = "fidelity",
           "seed"_a = -1, "mode"_a = UnitarySimulator::Mode::Recursive)
      .def("get_mode", &UnitarySimulator::getMode)
      .def("get_construction_time", &UnitarySimulator::getConstructionTime)
      .def("get_final_node_count", &UnitarySimulator::getFinalNodeCount)
      .def("get_max_node_count", &UnitarySimulator::getMaxNodeCount)
      .def("get_constructed_dd", &UnitarySimulator::getConstructedDD);
}
