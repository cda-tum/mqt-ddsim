"""Sampler implementation using QasmSimulatorBackend."""

from __future__ import annotations

import math
from typing import TYPE_CHECKING, Any, Mapping, Sequence, Union

from qiskit.primitives.base import BaseSampler, SamplerResult
from qiskit.primitives.primitive_job import PrimitiveJob
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
        options: dict | None = None,
    ):
        """Initialize a new DDSIM Sampler

        Args:
            options: Default options.
        """

        super().__init__(options=options)
        self._transpiled_circuits: list[QuantumCircuit] = []

    @property
    def transpiled_circuits(self) -> list[QuantumCircuit]:
        self._transpile()
        return self._transpiled_circuits

    @property
    def backend(self):
        return self._BACKEND

    def _transpile(self):
        """Transpiles circuits stored in the instance"""

        from qiskit import transpile

        start = len(self._transpiled_circuits)
        self._transpiled_circuits.extend(
            transpile(
                self._circuits[start:],
                target=self.backend.target,
            ),
        )

    def _run(
        self,
        circuits: Sequence[QuantumCircuit],
        parameter_values: Sequence[Parameters],
        **run_options,
    ) -> PrimitiveJob:
        """Stores circuits and parameters within the instance.
        Executes _call function.

        Args:
            circuits: List of quantum circuits to simulate
            parameter_values: List of parameters associated with those circuits

        Returns:
            PrimitiveJob.
        """

        for circuit in circuits:
            self._circuits.append(circuit)
            self._parameters.append(circuit.parameters)

        job = PrimitiveJob(self._call, parameter_values, **run_options)
        job.submit()
        return job

    def _call(
        self,
        parameter_values: Sequence[Parameters],
        **run_options,
    ) -> SamplerResult:
        """Executes transpilation and runs DDSIM backend

        Returns:
            SamplerResult.
        """

        transpiled_circuits = self.transpiled_circuits
        result = self.backend.run(transpiled_circuits, parameter_values, **run_options).result()

        return self._postprocessing(result)

    def _postprocessing(self, result: Result) -> SamplerResult:
        """Converts counts into quasi-probability distributions

        Returns:
            SamplerResult.
        """

        counts = result.get_counts()
        if not isinstance(counts, list):
            counts = [counts]

        shots = sum(counts[0].values())
        metadata: list[dict[str, Any]] = [{"shots": shots} for _ in range(len(self._circuits))]
        probabilities = []

        for count in counts:
            prob_dist = {k: v / shots for k, v in count.items()}
            probabilities.append(QuasiDistribution(prob_dist, shots=shots, stddev_upper_bound=math.sqrt(1 / shots)))

        return SamplerResult(probabilities, metadata)
