# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Backend for DDSIM."""

from __future__ import annotations

from qiskit.transpiler import Target

from .pathqasmsimulator import PathQasmSimulatorBackend


class PathStatevectorSimulatorBackend(PathQasmSimulatorBackend):
    """Python interface to MQT DDSIM Simulation Path Framework."""

    _SHOW_STATE_VECTOR = True
    _Path_SV_TARGET = Target(
        description="MQT DDSIM Simulation Path Framework Statevector Target",
        num_qubits=30,  # corresponds to 16GiB memory for storing the full statevector
    )

    def __init__(self) -> None:
        """Constructor for the DDSIM Simulation Path Framework Statevector Simulator backend."""
        super().__init__(
            name="path_sim_statevector_simulator",
            description="MQT DDSIM Simulation Path Framework Statevector Simulator",
        )

    @property
    def target(self) -> Target:
        """Return the target of the backend."""
        return self._Path_SV_TARGET
