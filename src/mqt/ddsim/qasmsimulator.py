"""Backend for DDSIM."""
from __future__ import annotations

import time
import uuid
from math import log2
from typing import TYPE_CHECKING, Any, Mapping, Sequence

from qiskit import QuantumCircuit
from qiskit.providers import BackendV2, Options
from qiskit.providers.models import BackendStatus
from qiskit.result import Result
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target
from qiskit.utils.multiprocessing import local_hardware_info

from . import __version__
from .header import DDSIMHeader
from .job import DDSIMJob
from .pyddsim import CircuitSimulator
from .target import DDSIMTargetBuilder

if TYPE_CHECKING:
    from qiskit.circuit import Parameter


class QasmSimulatorBackend(BackendV2):
    """Python interface to MQT DDSIM."""

    _SHOW_STATE_VECTOR = False
    _TARGET = Target(description="MQT DDSIM Simulator Target", num_qubits=128)

    @staticmethod
    def max_qubits(for_matrix: bool = False) -> int:
        max_complex = local_hardware_info()["memory"] * (1024**3) / 16
        max_qubits = int(log2(max_complex))
        if for_matrix:
            max_qubits = max_qubits // 2
        return max_qubits

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

    def __init__(self, name="qasm_simulator", description="MQT DDSIM QASM Simulator") -> None:
        super().__init__(name=name, description=description, backend_version=__version__)
        self._initialize_target()

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            seed_simulator=None,
            approximation_step_fidelity=1.0,
            approximation_steps=0,
            approximation_strategy="fidelity",
        )

    @property
    def target(self):
        return self._TARGET

    @property
    def max_circuits(self):
        return None

    @staticmethod
    def _bind_circuit_parameters(
        qc: QuantumCircuit,
        values: Sequence[float] | Mapping[Parameter, float],
    ) -> QuantumCircuit:
        if len(qc.parameters) != len(values):
            msg = f"The number of parameters in the circuit '{qc.name}' does not match the number of parameters provided ({len(values)}). Expected number of parameters is '{len(qc.parameters)}'."
            raise ValueError(msg)
        qc_bound = qc.bind_parameters(values)
        qc_bound.name = qc.name  # Preserves circuits' name

        return qc_bound

    @staticmethod
    def _bind_parameters(
        quantum_circuits: Sequence[QuantumCircuit],
        parameter_values: Sequence[Sequence[float]] | Sequence[Mapping[Parameter, float]] | None,
    ) -> list[QuantumCircuit]:
        if parameter_values is None:
            parameter_values = []

        number_parametrized_circuits = sum(1 for qc in quantum_circuits if qc.parameters)

        if number_parametrized_circuits == 0 and len(parameter_values) == 0:
            return list(quantum_circuits)

        if number_parametrized_circuits == 0:
            msg = f"No parametrized circuits found, but {len(parameter_values)} parameters provided. The parameter list should either be empty or None."
            raise ValueError(msg)
        if len(parameter_values) != len(quantum_circuits):
            msg = f"The number of circuits to simulate ({len(quantum_circuits)}) does not match the size of the parameter list ({len(parameter_values)})."
            raise ValueError(msg)
        bound_circuits = []
        for qc, values in zip(quantum_circuits, parameter_values):
            bound_circuits.append(QasmSimulatorBackend._bind_circuit_parameters(qc, values))

        return bound_circuits

    def run(
        self,
        quantum_circuits: QuantumCircuit | Sequence[QuantumCircuit],
        parameter_values: Sequence[Sequence[float]] | Sequence[Mapping[Parameter, float]] | None = None,
        **options,
    ) -> DDSIMJob:
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
        parameter_values: Sequence[Sequence[float]] | Sequence[Mapping[Parameter, float]] | None,
        **options: dict[str, Any],
    ) -> Result:
        self._validate(quantum_circuits)
        start = time.time()

        bound_circuits = self._bind_parameters(quantum_circuits, parameter_values)
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
        approximation_step_fidelity = options.get("approximation_step_fidelity", 1.0)
        approximation_steps = options.get("approximation_steps", 1)
        approximation_strategy = options.get("approximation_strategy", "fidelity")
        seed = options.get("seed_simulator", -1)
        shots = options.get("shots", 1024)

        sim = CircuitSimulator(
            qc,
            approximation_step_fidelity=approximation_step_fidelity,
            approximation_steps=approximation_steps,
            approximation_strategy=approximation_strategy,
            seed=seed,
        )
        counts = sim.simulate(shots=shots)
        end_time = time.time()

        data = ExperimentResultData(
            counts={hex(int(result, 2)): count for result, count in counts.items()},
            statevector=None if not self._SHOW_STATE_VECTOR else sim.get_vector(),
            time_taken=end_time - start_time,
        )

        metadata = qc.metadata
        if metadata is None:
            metadata = {}

        return ExperimentResult(
            shots=shots,
            success=True,
            status="DONE",
            seed=seed,
            data=data,
            metadata=metadata,
            header=DDSIMHeader(qc),
        )

    def status(self) -> BackendStatus:
        """Return backend status.

        Returns:
            BackendStatus: the status of the backend.
        """
        return BackendStatus(
            backend_name=self.name,
            backend_version=self.backend_version,
            operational=True,
            pending_jobs=0,
            status_msg="",
        )
