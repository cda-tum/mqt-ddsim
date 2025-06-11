# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Estimator implementation using DDSIM CircuitSimulator."""

from __future__ import annotations

from typing import TYPE_CHECKING, Union

from qiskit.primitives import StatevectorEstimator

if TYPE_CHECKING:
    from collections.abc import Mapping, Sequence

    import numpy as np
    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


class Estimator(StatevectorEstimator):  # type: ignore[misc]
    """DDSIM implementation of Qiskit's estimator."""

    def __init__(
        self,
        *,
        default_precision: float = 0.0,
        seed: np.random.Generator | int | None = None,
    ) -> None:
        """Create a new DDSIM estimator."""
        super().__init__(default_precision=default_precision, seed=seed)
