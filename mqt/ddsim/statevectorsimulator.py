"""Backend for DDSIM."""
from __future__ import annotations

from mqt.ddsim.qasmsimulator import QasmSimulatorBackend


class StatevectorSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM"""

    SHOW_STATE_VECTOR = True

    def __init__(self) -> None:
        super().__init__()
        self.name = "statevector_simulator"
        self.description = "MQT DDSIM Statevector Simulator"
