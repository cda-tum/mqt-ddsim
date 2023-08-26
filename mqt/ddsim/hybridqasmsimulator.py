"""Backend for DDSIM Hybrid Schrodinger-Feynman Simulator."""
from __future__ import annotations

import logging
import time
import uuid
from math import log2

from qiskit import QuantumCircuit
from qiskit.providers import BackendV2, Options
from qiskit.providers.models import BackendStatus
from qiskit.result import Result
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target
from qiskit.utils.multiprocessing import local_hardware_info

from mqt.ddsim import HybridCircuitSimulator, HybridMode, __version__
from mqt.ddsim.error import DDSIMError
from mqt.ddsim.header import DDSIMHeaderBuilder
from mqt.ddsim.job import DDSIMJob
from mqt.ddsim.target import DDSIMTargetBuilder

logger = logging.getLogger(__name__)


class HybridQasmSimulatorBackend(BackendV2):
    """Python interface to MQT DDSIM Hybrid Schrodinger-Feynman Simulator."""

    SHOW_STATE_VECTOR = False
    TARGET = Target(description="MQT DDSIM Simulator Target", num_qubits=128)

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            mode="amplitude",
            nthreads=local_hardware_info()["cpus"],
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
        super().__init__(
            name="hybrid_qasm_simulator",
            description="MQT DDSIM Hybrid Schrodinger-Feynman C++ simulator",
            backend_version=__version__,
        )
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

    def _run_experiment(self, qc: QuantumCircuit, **options) -> ExperimentResult:
        start_time = time.time()
        seed = options.get("seed", -1)
        mode = options.get("mode", "amplitude")
        nthreads = int(options.get("nthreads", local_hardware_info()["cpus"]))
        if mode == "amplitude":
            hybrid_mode = HybridMode.amplitude
            max_qubits = int(log2(local_hardware_info()["memory"] * (1024**3) / 16))
            algorithm_qubits = DDSIMHeaderBuilder.from_circ(qc).n_qubits
            if algorithm_qubits > max_qubits:
                msg = "Not enough memory available to simulate the circuit even on a single thread"
                raise DDSIMError(msg)
            qubit_diff = max_qubits - algorithm_qubits
            nthreads = int(min(2**qubit_diff, nthreads))
        elif mode == "dd":
            hybrid_mode = HybridMode.DD
        else:
            msg = f"Simulation mode{mode} not supported by hybrid simulator. Available modes are 'amplitude' and 'dd'."
            raise DDSIMError(msg)

        sim = HybridCircuitSimulator(qc, seed=seed, mode=hybrid_mode, nthreads=nthreads)

        shots = options.get("shots", 1024)
        if self.SHOW_STATE_VECTOR and shots > 0:
            logger.info(
                "Statevector can only be shown if shots == 0 when using the amplitude hybrid simulation mode. Setting shots=0."
            )
            shots = 0

        counts = sim.simulate(shots)
        end_time = time.time()
        {hex(int(result, 2)): count for result, count in counts.items()}

        data = ExperimentResultData(
            counts={hex(int(result, 2)): count for result, count in counts.items()},
            statevector=None
            if not self.SHOW_STATE_VECTOR
            else sim.get_vector()
            if sim.get_mode() == HybridMode.DD
            else sim.get_final_amplitudes(),
            time_taken=end_time - start_time,
            mode=mode,
            nthreads=nthreads,
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

    def status(self):
        """Return backend status.

        Returns:
            BackendStatus: the status of the backend.
        """
        return BackendStatus(
            backend_name=self.name(),
            backend_version=self.backend_version,
            operational=True,
            pending_jobs=0,
            status_msg="",
        )
