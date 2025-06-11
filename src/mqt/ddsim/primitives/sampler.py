# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Sampler implementation using QasmSimulatorBackend."""

from __future__ import annotations

from typing import TYPE_CHECKING, Union

from qiskit.primitives import StatevectorSampler

from mqt.ddsim.qasmsimulator import QasmSimulatorBackend

if TYPE_CHECKING:
    from collections.abc import Mapping, Sequence

    import numpy as np
    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


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
