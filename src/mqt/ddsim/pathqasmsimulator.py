"""Backend for DDSIM Task-Based Simulator."""

from __future__ import annotations

import time
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    from qiskit import QuantumCircuit


from qiskit.providers import Options
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target

from mqt.core import load

from .header import DDSIMHeader
from .pyddsim import PathCircuitSimulator, PathSimulatorConfiguration, PathSimulatorMode
from .qasmsimulator import QasmSimulatorBackend
from .target import DDSIMTargetBuilder


class PathQasmSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM Simulation Path Framework."""

    _PATH_TARGET = Target(description="MQT DDSIM Simulation Path Framework Target", num_qubits=128)

    @staticmethod
    def _add_operations_to_target(target: Target) -> None:
        DDSIMTargetBuilder.add_0q_gates(target)
        DDSIMTargetBuilder.add_1q_gates(target)
        DDSIMTargetBuilder.add_2q_gates(target)
        DDSIMTargetBuilder.add_3q_gates(target)
        DDSIMTargetBuilder.add_multi_qubit_gates(target)
        DDSIMTargetBuilder.add_barrier(target)
        DDSIMTargetBuilder.add_measure(target)

    def __init__(
        self,
        name: str = "path_sim_qasm_simulator",
        description: str = "MQT DDSIM Simulation Path Framework",
    ) -> None:
        """Constructor for the DDSIM Simulation Path Framework QASM Simulator backend.

        Args:
            name: The name of the backend.
            description: The description of the backend.
        """
        super().__init__(name=name, description=description)

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            pathsim_configuration=PathSimulatorConfiguration(),
            mode=None,
            bracket_size=None,
            alternating_start=None,
            gate_cost=None,
            seed=None,
        )

    @property
    def target(self) -> Target:
        """Return the target of the backend."""
        return self._PATH_TARGET

    def _run_experiment(self, qc: QuantumCircuit, **options: Any) -> ExperimentResult:
        start_time = time.time()

        pathsim_configuration = options.get("pathsim_configuration", PathSimulatorConfiguration())

        mode = options.get("mode")
        if mode is not None:
            pathsim_configuration.mode = PathSimulatorMode(mode)

        bracket_size = options.get("bracket_size")
        if bracket_size is not None:
            pathsim_configuration.bracket_size = bracket_size

        alternating_start = options.get("alternating_start")
        if alternating_start is not None:
            pathsim_configuration.alternating_start = alternating_start

        gate_cost = options.get("gate_cost")
        if gate_cost is not None:
            pathsim_configuration.gate_cost = gate_cost

        seed: int | None = options.get("seed")
        if seed is not None:
            pathsim_configuration.seed = seed

        circuit = load(qc)
        sim = PathCircuitSimulator(circuit, config=pathsim_configuration)

        shots = options.get("shots", 1024)
        setup_time = time.time()
        counts = sim.simulate(shots)
        end_time = time.time()

        data = ExperimentResultData(
            counts={hex(int(result, 2)): count for result, count in counts.items()},
            statevector=None if not self._SHOW_STATE_VECTOR else sim.get_vector(),
            time_taken=end_time - start_time,
            time_setup=setup_time - start_time,
            time_sim=end_time - setup_time,
        )

        return ExperimentResult(
            shots=shots,
            success=True,
            status="DONE",
            config=pathsim_configuration,
            seed=seed,
            data=data,
            metadata=qc.metadata,
            header=DDSIMHeader(qc),
        )
