# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Sampler implementation using QasmSimulatorBackend."""

from __future__ import annotations

from typing import TYPE_CHECKING

import numpy as np
from qiskit.primitives import BitArray, DataBin, StatevectorSampler
from qiskit.primitives.containers import SamplerPubResult

from mqt.ddsim.qasmsimulator import QasmSimulatorBackend

if TYPE_CHECKING:
    from qiskit.primitives.containers import SamplerPub


class Sampler(StatevectorSampler):  # type: ignore[misc]
    """DDSIM implementation of Qiskit's sampler."""

    _BACKEND = QasmSimulatorBackend()

    def __init__(self, *, default_shots: int = 1024, seed: np.random.Generator | int | None = None) -> None:
        """Create a new DDSIM sampler."""
        super().__init__(default_shots=default_shots, seed=seed)

    @property
    def backend(self) -> QasmSimulatorBackend:
        """The backend used by the sampler."""
        return self._BACKEND

    def _run_pub(self, pub: SamplerPub) -> SamplerPubResult:
        circuit = pub.circuit
        cregs = circuit.cregs

        bound_circuits = pub.parameter_values.bind_all(circuit)

        counts: dict[str, list[dict[str, int]]] = {creg.name: [] for creg in cregs}
        for index in np.ndindex(*bound_circuits.shape):
            bound_circuit = bound_circuits[index]
            result = self.backend.run(bound_circuit, shots=pub.shots).result()
            for creg in cregs:
                counts[creg.name].append(result.get_counts())

        meas = {creg.name: BitArray.from_counts(counts[creg.name], creg.size) for creg in cregs}
        return SamplerPubResult(
            DataBin(**meas, shape=pub.shape),
            metadata={"shots": pub.shots, "circuit_metadata": pub.circuit.metadata},
        )
