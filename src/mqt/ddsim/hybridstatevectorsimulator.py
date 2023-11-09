"""Backend for DDSIM Hybrid Schrodinger-Feynman Simulator."""

from __future__ import annotations

from qiskit.transpiler import Target

from .hybridqasmsimulator import HybridQasmSimulatorBackend


class HybridStatevectorSimulatorBackend(HybridQasmSimulatorBackend):
    """Python interface to MQT DDSIM Hybrid Schrodinger-Feynman Simulator."""

    _SHOW_STATE_VECTOR = True
    _HSF_SV_TARGET = Target(
        description="MQT DDSIM HSF Statevector Simulator Target",
        num_qubits=HybridQasmSimulatorBackend.max_qubits(),
    )

    def __init__(self) -> None:
        super().__init__(
            name="hybrid_statevector_simulator",
            description="MQT DDSIM Hybrid Schrodinger-Feynman Statevector simulator",
        )

    @property
    def target(self) -> Target:
        return self._HSF_SV_TARGET
