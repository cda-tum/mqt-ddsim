"""Backend for DDSIM Unitary Simulator."""

from __future__ import annotations

from qiskit.transpiler import Target

from .qasmsimulator import QasmSimulatorBackend


class DeterministicNoiseSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM deterministic noise-aware simulator."""

    _SHOW_STATE_VECTOR = True
    _DNS_SV_TARGET = Target(
        description="MQT DDSIM decision diagram-based deterministic noise-aware simulator target",
        num_qubits=QasmSimulatorBackend.max_qubits(),
    )

    def __init__(self) -> None:
        super().__init__(
            name="density_matrix_dd_simulator",
            description="MQT DDSIM noise-aware density matrix simulator based on decision diagrams",
        )

    @property
    def target(self) -> Target:
        return self._DNS_SV_TARGET
