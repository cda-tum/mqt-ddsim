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
        """Run a primitive unified block (PUB) on the QasmSimulatorBackend.

        Adapted from Qiskit's `StatevectorSampler._run_pub()`.
        """
        circuit = pub.circuit
        parameter_values = pub.parameter_values
        shots = pub.shots

        bound_circuits = parameter_values.bind_all(circuit)
        bound_circuits_list = np.asarray(bound_circuits, dtype=object).tolist()
        result = self.backend.run(bound_circuits_list, shots=shots).result()

        counts = result.get_counts()
        if isinstance(counts, dict):
            counts = [counts]

        meas = {creg.name: BitArray.from_counts(counts, creg.size) for creg in circuit.cregs}
        return SamplerPubResult(
            DataBin(**meas, shape=pub.shape),
            metadata={"shots": shots, "circuit_metadata": circuit.metadata},
        )
