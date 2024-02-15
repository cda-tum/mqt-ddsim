"""Backend for DDSIM."""

from __future__ import annotations

from qiskit.transpiler import Target

from .pathqasmsimulator import PathQasmSimulatorBackend


class PathStatevectorSimulatorBackend(PathQasmSimulatorBackend):
    """Python interface to MQT DDSIM Simulation Path Framework."""

    _SHOW_STATE_VECTOR = True
    _Path_SV_TARGET = Target(
        description="MQT DDSIM Simulation Path Framework Statevector Target",
        num_qubits=PathQasmSimulatorBackend.max_qubits(),
    )

    def __init__(self) -> None:
        super().__init__(
            name="path_sim_statevector_simulator",
            description="MQT DDSIM Simulation Path Framework Statevector Simulator",
        )

    @property
    def target(self) -> Target:
        return self._Path_SV_TARGET
