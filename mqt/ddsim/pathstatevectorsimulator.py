"""Backend for DDSIM."""
from __future__ import annotations

from mqt.ddsim.pathqasmsimulator import PathQasmSimulatorBackend


class PathStatevectorSimulatorBackend(PathQasmSimulatorBackend):
    """Python interface to MQT DDSIM Simulation Path Framework."""

    SHOW_STATE_VECTOR = True

    def __init__(self) -> None:
        super().__init__()
        self.name = "path_sim_statevector_simulator"
        self.description = "MQT DDSIM C++ simulation path framework"
