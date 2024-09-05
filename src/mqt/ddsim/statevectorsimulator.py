"""Backend for DDSIM."""

from __future__ import annotations

from qiskit.transpiler import Target

from .qasmsimulator import QasmSimulatorBackend


class StatevectorSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM."""

    _SHOW_STATE_VECTOR = True
    _SV_TARGET = Target(
        description="MQT DDSIM Statevector Simulator Target",
        num_qubits=30,  # corresponds to 16GiB memory for storing the full statevector
    )

    def __init__(self) -> None:
        """Constructor for the DDSIM Statevector simulator backend."""
        super().__init__(name="statevector_simulator", description="MQT DDSIM Statevector Simulator")

    @property
    def target(self) -> Target:
        """Return the target of the backend."""
        return self._SV_TARGET
