"""Backend for DDSIM Unitary Simulator."""

from __future__ import annotations

import time
from typing import TYPE_CHECKING, Any

import numpy as np
import numpy.typing as npt
from qiskit import QiskitError
from qiskit.providers import Options
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target

from mqt.core import load

from .header import DDSIMHeader
from .pyddsim import ConstructionMode, UnitarySimulator, get_matrix
from .qasmsimulator import QasmSimulatorBackend
from .target import DDSIMTargetBuilder

if TYPE_CHECKING:
    from collections.abc import Sequence

    from qiskit import QuantumCircuit


class UnitarySimulatorBackend(QasmSimulatorBackend):
    """Decision diagram-based unitary simulator."""

    _US_TARGET = Target(
        description="MQT DDSIM Unitary Simulator Target",
        num_qubits=15,  # corresponds to 16GiB memory for storing the full matrix
    )

    @staticmethod
    def _add_operations_to_target(target: Target) -> None:
        DDSIMTargetBuilder.add_0q_gates(target)
        DDSIMTargetBuilder.add_1q_gates(target)
        DDSIMTargetBuilder.add_2q_gates(target)
        DDSIMTargetBuilder.add_3q_gates(target)
        DDSIMTargetBuilder.add_multi_qubit_gates(target)
        DDSIMTargetBuilder.add_barrier(target)

    def __init__(self) -> None:
        """Constructor for the DDSIM unitary simulator backend."""
        super().__init__(name="unitary_simulator", description="MQT DDSIM Unitary Simulator")

    @classmethod
    def _default_options(cls) -> Options:
        return Options(shots=1, mode="recursive", parameter_binds=None)

    @property
    def target(self) -> Target:
        """Return the target of the backend."""
        return self._US_TARGET

    @classmethod
    def _run_experiment(cls, qc: QuantumCircuit, **options: Any) -> ExperimentResult:
        start_time = time.time()
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
            raise QiskitError(msg)

        circuit = load(qc)
        sim = UnitarySimulator(circuit, seed=seed, mode=construction_mode)
        sim.construct()
        # Extract resulting matrix from final DD and write data
        unitary: npt.NDArray[np.complex128] = np.zeros((2**qc.num_qubits, 2**qc.num_qubits), dtype=np.complex128)
        get_matrix(sim, unitary)
        end_time = time.time()

        data = ExperimentResultData(
            unitary=unitary,
            construction_time=sim.get_construction_time(),
            max_dd_nodes=sim.get_max_node_count(),
            dd_nodes=sim.get_final_node_count(),
            time_taken=end_time - start_time,
        )

        return ExperimentResult(
            shots=1,
            success=True,
            status="DONE",
            seed=seed,
            data=data,
            metadata=qc.metadata,
            header=DDSIMHeader(qc),
        )

    def _validate(self, quantum_circuits: Sequence[QuantumCircuit]) -> None:
        """Semantic validations of the quantum circuits which cannot be done via schemas.

        1. No shots
        2. No measurements in the middle.
        """
        for qc in quantum_circuits:
            name = qc.name
            n_qubits = qc.num_qubits
            max_qubits = self.target.num_qubits

            if n_qubits > max_qubits:
                msg = f"Number of qubits {n_qubits} is greater than maximum ({max_qubits}) for '{self.name}'."
                raise QiskitError(msg)

            if qc.metadata is not None and "shots" in qc.metadata and qc.metadata["shots"] != 1:
                qc.metadata["shots"] = 1

            for instruction in qc.data:
                operation = instruction.operation
                if operation.name in {"measure", "reset"}:
                    msg = f"Unsupported '{self.name}' instruction '{operation.name}' in circuit '{name}'."
                    raise QiskitError(msg)
