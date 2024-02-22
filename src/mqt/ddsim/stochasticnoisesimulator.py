"""Backend for DDSIM Unitary Simulator."""

from __future__ import annotations

from qiskit.transpiler import Target

from .qasmsimulator import QasmSimulatorBackend


class StochasticNoiseSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM stochastic noise-aware simulator."""

    _SHOW_STATE_VECTOR = True
    _SNS_SV_TARGET = Target(
        description="MQT DDSIM decision diagram-based stochastic noise-aware simulator target",
        num_qubits=QasmSimulatorBackend.max_qubits(),
    )

    def __init__(self) -> None:
        super().__init__(
            name="stochastic_dd_simulator",
            description="MQT DDSIM noise-aware stochastic simulator based on decision diagrams",
        )

    @property
    def target(self) -> Target:
        return self._SNS_SV_TARGET
