"""Sampler implementation using QasmSimulatorBackend."""

from __future__ import annotations

import math
from typing import TYPE_CHECKING, Any, Mapping, Sequence, Union

from qiskit.primitives.base import BaseSampler, SamplerResult
from qiskit.primitives.primitive_job import PrimitiveJob
from qiskit.primitives.utils import _circuit_key
from qiskit.result import QuasiDistribution, Result

from mqt.ddsim.qasmsimulator import QasmSimulatorBackend

if TYPE_CHECKING:
    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType
    from qiskit.circuit.quantumcircuit import QuantumCircuit

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


class Sampler(BaseSampler):
    _BACKEND = QasmSimulatorBackend()

    def __init__(
        self,
        options: dict[str, Any] | None = None,
    ) -> None:
        """Initialize a new DDSIM Sampler

        Args:
            options: Default options.
        """

        super().__init__(options=options)
        self._circuit_ids: dict[Sequence[int | tuple], int] = {}

    @property
    def backend(self) -> QasmSimulatorBackend:
        return self._BACKEND

    def _run(
        self,
        circuits: Sequence[QuantumCircuit],
        parameter_values: Sequence[Parameters],
        **run_options: Any,
    ) -> PrimitiveJob:
        """Stores circuits and parameters within the instance.
        Executes _call function.

        Args:
            circuits: List of quantum circuits to simulate
            parameter_values: List of parameters associated with those circuits

        Returns:
            PrimitiveJob.
        """
        circuit_indices = []
        for circuit in circuits:
            key = _circuit_key(circuit)
            index = self._circuit_ids.get(key)
            if index is not None:
                circuit_indices.append(index)
            else:
                circuit_indices.append(len(self._circuits))
                self._circuit_ids[key] = len(self._circuits)
                self._circuits.append(circuit)
                self._parameters.append(circuit.parameters)

        job = PrimitiveJob(self._call, circuit_indices, parameter_values, **run_options)
        job.submit()
        return job

    def _call(
        self,
        circuits: Sequence[int],
        parameter_values: Sequence[Parameters],
        **run_options: Any,
    ) -> SamplerResult:
        """Runs DDSIM backend

        Returns:
            SamplerResult.
        """

        result = self.backend.run([self._circuits[i] for i in circuits], parameter_values, **run_options).result()

        return self._postprocessing(result, circuits)

    @staticmethod
    def _postprocessing(result: Result, circuits: Sequence[int]) -> SamplerResult:
        """Converts counts into quasi-probability distributions

        Returns:
            SamplerResult.
        """

        counts = result.get_counts()
        if not isinstance(counts, list):
            counts = [counts]

        shots = sum(counts[0].values())
        metadata: list[dict[str, Any]] = [{"shots": shots} for _ in range(len(circuits))]
        probabilities = [
            QuasiDistribution(
                {k: v / shots for k, v in count.items()}, shots=shots, stddev_upper_bound=1 / math.sqrt(shots)
            )
            for count in counts
        ]

        return SamplerResult(probabilities, metadata)
