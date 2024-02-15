"""Utilities for constructing a Qiskit experiment header for DDSIM backends."""

from __future__ import annotations

from dataclasses import dataclass
from typing import TYPE_CHECKING

from qiskit.result.models import QobjExperimentHeader

if TYPE_CHECKING:
    from qiskit import QuantumCircuit


@dataclass
class DDSIMHeader(QobjExperimentHeader):
    name: str
    n_qubits: int
    memory_slots: int
    global_phase: float
    creg_sizes: list[tuple[str, int]]
    clbit_labels: list[tuple[str, int]]
    qreg_sizes: list[tuple[str, int]]
    qubit_labels: list[tuple[str, int]]

    def __init__(self, qc: QuantumCircuit) -> None:
        super().__init__()
        self.name = qc.name
        self.n_qubits = qc.num_qubits
        self.memory_slots = qc.num_clbits
        self.global_phase = qc.global_phase
        self.creg_sizes = [(creg.name, creg.size) for creg in qc.cregs]
        self.clbit_labels = [(creg.name, j) for creg in qc.cregs for j in range(creg.size)]
        self.qreg_sizes = [(qreg.name, qreg.size) for qreg in qc.qregs]
        self.qubit_labels = [(qreg.name, j) for qreg in qc.qregs for j in range(qreg.size)]
