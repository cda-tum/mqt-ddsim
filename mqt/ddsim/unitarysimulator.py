"""Backend for DDSIM Unitary Simulator."""

import logging
import time
import uuid
from math import log2, sqrt
from typing import Union

import numpy as np
from qiskit import QuantumCircuit
from qiskit.providers import BackendV2, Options
from qiskit.result import Result
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target
from qiskit.utils.multiprocessing import local_hardware_info

from mqt.ddsim import ConstructionMode, UnitarySimulator, __version__, get_matrix
from mqt.ddsim.error import DDSIMError
from mqt.ddsim.header import DDSIMHeaderBuilder
from mqt.ddsim.job import DDSIMJob
from mqt.ddsim.target import DDSIMTargetBuilder

logger = logging.getLogger(__name__)


class UnitarySimulatorBackend(BackendV2):
    """Decision diagram-based unitary simulator."""

    TARGET = Target(
        description="MQT DDSIM Simulator Target",
        num_qubits=min(24, int(log2(sqrt(local_hardware_info()["memory"] * (1024**3) / 16)))),
    )

    def _initialize_target(self):
        DDSIMTargetBuilder.add_0q_gates(self.TARGET)
        DDSIMTargetBuilder.add_1q_gates(self.TARGET)
        DDSIMTargetBuilder.add_2q_gates(self.TARGET)
        DDSIMTargetBuilder.add_3q_gates(self.TARGET)
        DDSIMTargetBuilder.add_multi_qubit_gates(self.TARGET)
        DDSIMTargetBuilder.add_non_unitary_operations(self.TARGET)
        DDSIMTargetBuilder.add_barrier(self.TARGET)

    def __init__(self):
        super().__init__(
            name="unitary_simulator", description="MQT DDSIM C++ Unitary Simulator", backend_version=__version__
        )
        if len(self.TARGET.operations) == 0:
            self._initialize_target()

    @property
    def target(self):
        return self.TARGET

    @property
    def max_circuits(self):
        return None

    @classmethod
    def _default_options(cls):
        return Options(shots=1, mode="recursive", parameter_binds=None)

    def run(self, quantum_circuits: Union[QuantumCircuit, list[QuantumCircuit]], **options) -> DDSIMJob:
        if isinstance(quantum_circuits, QuantumCircuit):
            quantum_circuits = [quantum_circuits]

        job_id = str(uuid.uuid4())
        local_job = DDSIMJob(self, job_id, self._run_job, quantum_circuits, **options)
        local_job.submit()
        return local_job

    def _run_job(self, job_id: int, quantum_circuits: list[QuantumCircuit], **options) -> Result:
        self._validate(quantum_circuits)

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
        start = time.time()
        seed = options.get("seed", -1)
        mode = options.get("mode", "recursive")

        if mode == "sequential":
            construction_mode = ConstructionMode.sequential
        elif mode == "recursive":
            construction_mode = ConstructionMode.recursive
        else:
            msg = (
                f"Construction mode {mode} not supported by DDSIM unitary simulator. Available modes are "
                "'recursive' and 'sequential'"
            )
            raise DDSIMError(msg)

        sim = UnitarySimulator(qc, seed=seed, mode=construction_mode)
        sim.construct()
        # Add extract resulting matrix from final DD and write data
        unitary = np.zeros((2**qc.num_qubits, 2**qc.num_qubits), dtype=complex)
        get_matrix(sim, unitary)
        data = ExperimentResultData(
            unitary = unitary,
            construction_time = sim.get_construction_time(),
            max_dd_nodes = sim.get_max_node_count(),
            dd_nodes = sim.get_final_node_count(),
        )

        end = time.time()

        metadata = qc.metadata
        if metadata is None:
            metadata = {}

        return ExperimentResult(
            shots=1,
            success=True,
            status="DONE",
            seed=seed,
            data=data,
            metadata=metadata,
            header=DDSIMHeaderBuilder.from_circ(qc),
            time_taken=end - start,
        )

    def _validate(self, quantum_circuits: list[QuantumCircuit]):
        """Semantic validations of the quantum circuits which cannot be done via schemas.
        Some of these may later move to backend schemas.
        1. No shots
        2. No measurements in the middle
        """
        for qc in quantum_circuits:
            name = qc.name
            n_qubits = qc.num_qubits
            max_qubits = self.TARGET.num_qubits

            if n_qubits > max_qubits:
                msg = f"Number of qubits {n_qubits} is greater than maximum ({max_qubits}) for '{self.name()}'."
                raise DDSIMError(msg)

            if "shots" in qc.metadata and qc.metadata["shots"] != 1:
                logger.info('"%s" only supports 1 shot. Setting shots=1 for circuit "%s".', self.name(), name)
                qc.metadata["shots"] = 1

            for ii in range(0, len(qc.data)):
                if qc.data[ii].operation.name in ["measure", "reset"]:
                    operation_name = qc.data[ii].operation.name
                    msg = f"Unsupported '{self.name}' instruction '{operation_name}' in circuit '{name}'."
                    raise DDSIMError(msg)
