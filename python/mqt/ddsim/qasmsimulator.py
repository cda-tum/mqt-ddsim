# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Backend for DDSIM."""

from __future__ import annotations

import time
import uuid
from typing import TYPE_CHECKING, Any, Union, cast

import numpy as np
from mqt.core import load
from qiskit import QuantumCircuit
from qiskit.providers import BackendV2, Options
from qiskit.result import Result
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target

from . import __version__
from .header import DDSIMHeader
from .job import DDSIMJob
from .pyddsim import CircuitSimulator
from .target import DDSIMTargetBuilder

if TYPE_CHECKING:
    from collections.abc import Mapping, Sequence

    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


class QasmSimulatorBackend(BackendV2):  # type: ignore[misc]
    """Python interface to MQT DDSIM."""

    _SHOW_STATE_VECTOR = False
    _TARGET = Target(description="MQT DDSIM Simulator Target", num_qubits=128)

    @staticmethod
    def _add_operations_to_target(target: Target) -> None:
        DDSIMTargetBuilder.add_0q_gates(target)
        DDSIMTargetBuilder.add_1q_gates(target)
        DDSIMTargetBuilder.add_2q_gates(target)
        DDSIMTargetBuilder.add_3q_gates(target)
        DDSIMTargetBuilder.add_multi_qubit_gates(target)
        DDSIMTargetBuilder.add_non_unitary_operations(target)
        DDSIMTargetBuilder.add_barrier(target)

    def _initialize_target(self) -> None:
        if len(self.target.operations) > 0:
            return
        self._add_operations_to_target(self.target)

    def __init__(
        self,
        name: str = "qasm_simulator",
        description: str = "MQT DDSIM QASM Simulator",
    ) -> None:
        """Constructor for the DDSIM QASM simulator backend."""
        super().__init__(name=name, description=description, backend_version=__version__)
        self._initialize_target()

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=1024,
            memory=False,
            parameter_binds=None,
            seed_simulator=None,
            approximation_step_fidelity=1.0,
            approximation_steps=1,
            approximation_strategy="fidelity",
        )

    @property
    def target(self) -> Target:
        """Return the target of the backend."""
        return self._TARGET

    @property
    def max_circuits(self) -> int | None:
        """Return the maximum number of circuits that can be run in a single job."""
        return None

    @staticmethod
    def assign_parameters(
        quantum_circuits: Sequence[QuantumCircuit],
        parameter_values: Sequence[Parameters] | None,
    ) -> list[QuantumCircuit]:
        """Assign parameter values to the circuits.

        Args:
            quantum_circuits: The quantum circuits to assign parameters to.
            parameter_values: The parameter values to bind to the circuits.

        Returns:
            The bound circuits.

        Raises:
            ValueError: If the number of circuits does not match the number of provided parameter sets.
        """
        if not any(qc.parameters for qc in quantum_circuits) and not parameter_values:
            return list(quantum_circuits)

        if parameter_values is None:
            msg = "No parameter values provided although at least one parameterized circuit was supplied."
            raise ValueError(msg)

        if len(quantum_circuits) != len(parameter_values):
            msg = f"The number of circuits ({len(quantum_circuits)}) does not match the number of provided parameter sets ({len(parameter_values)})."
            raise ValueError(msg)

        bound_circuits = [
            qc.assign_parameters(parameters=values) for qc, values in zip(quantum_circuits, parameter_values)
        ]

        # fix the circuit names
        for qcb, qc in zip(bound_circuits, quantum_circuits):
            qcb.name = qc.name

        return bound_circuits

    def run(
        self,
        quantum_circuits: QuantumCircuit | Sequence[QuantumCircuit],
        parameter_values: Sequence[Parameters] | None = None,
        **options: Any,
    ) -> DDSIMJob:
        """Run a quantum circuit or list of quantum circuits on the DDSIM backend.

        Args:
            quantum_circuits: The quantum circuit(s) to run.
            parameter_values: The parameter values to bind to the circuits.
            options: Additional run options.

        Returns:
            The DDSIM job
        """
        if isinstance(quantum_circuits, QuantumCircuit):
            quantum_circuits = [quantum_circuits]

        job_id = str(uuid.uuid4())
        local_job = DDSIMJob(self, job_id, self._run_job, quantum_circuits, parameter_values, **options)
        local_job.submit()
        return local_job

    def _validate(self, quantum_circuits: Sequence[QuantumCircuit]) -> None:
        pass

    def _run_job(
        self,
        job_id: int,
        quantum_circuits: Sequence[QuantumCircuit],
        parameter_values: Sequence[Parameters] | None,
        **options: Any,
    ) -> Result:
        self._validate(quantum_circuits)
        start = time.time()

        bound_circuits = self.assign_parameters(quantum_circuits, parameter_values)
        result_list = [self._run_experiment(q_circ, **options) for q_circ in bound_circuits]

        end = time.time()

        return Result(
            backend_name=self.name,
            backend_version=self.backend_version,
            qobj_id=None,
            job_id=job_id,
            success=all(res.success for res in result_list),
            results=result_list,
            status="COMPLETED",
            time_taken=end - start,
        )

    def _run_experiment(self, qc: QuantumCircuit, **options: dict[str, Any]) -> ExperimentResult:
        start_time = time.time()
        approximation_step_fidelity = cast("float", options.get("approximation_step_fidelity", 1.0))
        approximation_steps = cast("int", options.get("approximation_steps", 1))
        approximation_strategy = str(options.get("approximation_strategy", "fidelity"))
        seed = cast("int", options.get("seed_simulator", -1))
        shots = cast("int", options.get("shots", 1024))

        circuit = load(qc)
        sim = CircuitSimulator(
            circuit,
            approximation_step_fidelity=approximation_step_fidelity,
            approximation_steps=approximation_steps,
            approximation_strategy=approximation_strategy,
            seed=seed,
        )
        counts = sim.simulate(shots=shots)
        end_time = time.time()

        data = ExperimentResultData(
            counts={hex(int(result, 2)): count for result, count in counts.items()},
            statevector=None if not self._SHOW_STATE_VECTOR else np.array(sim.get_constructed_dd().get_vector()),
            time_taken=end_time - start_time,
        )

        return ExperimentResult(
            shots=shots,
            success=True,
            status="DONE",
            seed=seed,
            data=data,
            metadata=qc.metadata,
            header=DDSIMHeader(qc),
        )
