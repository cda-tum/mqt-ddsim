"""Backend for DDSIM."""
from __future__ import annotations

import time
import uuid

from qiskit import QuantumCircuit
from qiskit.providers import BackendV2, Options
from qiskit.result import Result
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target

from . import CircuitSimulator, __version__
from .header import DDSIMHeaderBuilder
from .job import DDSIMJob
from .target import DDSIMTargetBuilder


class QasmSimulatorBackend(BackendV2):
    """Python interface to MQT DDSIM."""

    SHOW_STATE_VECTOR = False
    TARGET = Target(description="MQT DDSIM Simulator Target", num_qubits=128)

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

    def _initialize_target(self):
        DDSIMTargetBuilder.add_0q_gates(self.TARGET)
        DDSIMTargetBuilder.add_1q_gates(self.TARGET)
        DDSIMTargetBuilder.add_2q_gates(self.TARGET)
        DDSIMTargetBuilder.add_3q_gates(self.TARGET)
        DDSIMTargetBuilder.add_multi_qubit_gates(self.TARGET)
        DDSIMTargetBuilder.add_non_unitary_operations(self.TARGET)
        DDSIMTargetBuilder.add_barrier(self.TARGET)

    def __init__(self) -> None:
        super().__init__(name="qasm_simulator", description="MQT DDSIM QASM Simulator", backend_version=__version__)
        if len(self.TARGET.operations) == 0:
            self._initialize_target()

    @property
    def target(self):
        return self.TARGET

    @property
    def max_circuits(self):
        return None

    def run(self, quantum_circuits: QuantumCircuit | list[QuantumCircuit], **options) -> DDSIMJob:
        if isinstance(quantum_circuits, QuantumCircuit):
            quantum_circuits = [quantum_circuits]

        job_id = str(uuid.uuid4())
        local_job = DDSIMJob(self, job_id, self._run_job, quantum_circuits, **options)
        local_job.submit()
        return local_job

    def _run_job(self, job_id: int, quantum_circuits: list[QuantumCircuit], **options) -> Result:
        start = time.time()
        result_list = [self._run_experiment(q_circ, **options) for q_circ in quantum_circuits]
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

    def _run_experiment(self, qc: QuantumCircuit, **options) -> ExperimentResult:
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
            statevector=None if not self.SHOW_STATE_VECTOR else sim.get_vector(),
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
            header=DDSIMHeaderBuilder.from_circ(qc),
        )
