"""Sampler implementation using QasmSimulatorBackend."""

from __future__ import annotations

import math
from typing import TYPE_CHECKING, Any, Mapping, Sequence, Union

from qiskit.primitives import SamplerResult
from qiskit.primitives.sampler import Sampler as QiskitSampler
from qiskit.result import QuasiDistribution, Result

from mqt.ddsim.qasmsimulator import QasmSimulatorBackend

if TYPE_CHECKING:
    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


class Sampler(QiskitSampler):  # type: ignore[misc]
    """Sampler implementation using QasmSimulatorBackend."""

    _BACKEND = QasmSimulatorBackend()

    def __init__(
        self,
        *,
        options: dict[str, Any] | None = None,
    ) -> None:
        """Initialize a new DDSIM Sampler.

        Args:
            options: Default options.
        """
        super().__init__(options=options)

    @property
    def backend(self) -> QasmSimulatorBackend:
        """The backend used by the sampler."""
        return self._BACKEND

    @property
    def num_circuits(self) -> int:
        """The number of circuits stored in the sampler."""
        return len(self._circuits)

    def _call(
        self,
        circuits: Sequence[int],
        parameter_values: Sequence[Parameters],
        **run_options: Any,
    ) -> SamplerResult:
        """Runs DDSIM backend.

        Args:
            circuits: List of circuit indices to simulate
            parameter_values: List of parameters associated with those circuits
            run_options: Additional run options.

        Returns:
            The result of the sampling process.
        """
        result = self.backend.run([self._circuits[i] for i in circuits], parameter_values, **run_options).result()

        return self._postprocessing(result, circuits)

    @staticmethod
    def _postprocessing(result: Result, circuits: Sequence[int]) -> SamplerResult:
        """Converts counts into quasi-probability distributions.

        Args:
            result: Result from DDSIM backend
            circuits: List of circuit indices

        Returns:
            The result of the sampling process.
        """
        counts = result.get_counts()
        if not isinstance(counts, list):
            counts = [counts]

        shots = sum(counts[0].values())
        metadata: list[dict[str, Any]] = [{"shots": shots} for _ in range(len(circuits))]
        probabilities = [
            QuasiDistribution(
                {k: v / shots for k, v in count.items()},
                shots=shots,
                stddev_upper_bound=1 / math.sqrt(shots),
            )
            for count in counts
        ]

        return SamplerResult(probabilities, metadata)
