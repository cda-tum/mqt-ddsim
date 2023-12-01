"""Backend for DDSIM Unitary Simulator."""

from __future__ import annotations

from qiskit.transpiler import Target

from .qasmsimulator import QasmSimulatorBackend


class StochasticNoiseSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM stochastic noise-aware simulator."""

    _SHOW_STATE_VECTOR = True
    _HSF_SV_TARGET = Target(
        description="MQT DDSIM Decision Diagram-based Noise-Aware Simulator Target",
        num_qubits=QasmSimulatorBackend.max_qubits(),
    )

    def __init__(self) -> None:
        super().__init__(
            name="noise-aware_stochastic_decision-diagram_simulator",
            description="MQT DDSIM Noise-Aware Stochastic Decision Diagram simulator",
        )

    @property
    def target(self):
        return self._HSF_SV_TARGET
