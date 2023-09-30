"""Sampler implementation for an artibtrary Backend object."""

from __future__ import annotations

import math
from typing import TYPE_CHECKING, Any, Mapping, Sequence, Union

from qiskit.primitives.backend_estimator import _prepare_counts
from qiskit.primitives.base import BaseSampler, SamplerResult
from qiskit.primitives.primitive_job import PrimitiveJob
from qiskit.primitives.utils import _circuit_key
from qiskit.providers.options import Options
from qiskit.result import QuasiDistribution, Result

from mqt.ddsim.qasmsimulator import QasmSimulatorBackend

if TYPE_CHECKING:
    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType
    from qiskit.circuit.quantumcircuit import QuantumCircuit
    from qiskit.transpiler.passmanager import PassManager

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


class DDSIMBackendSampler(BaseSampler[PrimitiveJob[SamplerResult]]):
    _BACKEND = QasmSimulatorBackend()

    def __init__(
        self,
        options: dict | None = None,
        bound_pass_manager: PassManager | None = None,
        skip_transpilation: bool = False,
    ):
        """Initialize a new DDSIM Sampler

        Args:
            options: Default options.
            bound_pass_manager: An optional pass manager to run after
                parameter binding.
            skip_transpilation: If this is set to True the internal compilation
                of the input circuits is skipped and the circuit objects
                will be directly executed when this objected is called.
        Raises:
            ValueError: If backend is not provided
        """

        super().__init__(options=options)
        self._transpile_options = Options()
        self._bound_pass_manager = bound_pass_manager
        self._preprocessed_circuits: list[QuantumCircuit] | None = None
        self._transpiled_circuits: list[QuantumCircuit] = []
        self._skip_transpilation = skip_transpilation
        self._circuit_ids: dict = {}

    @property
    def transpiled_circuits(self) -> list[QuantumCircuit]:
        if self._skip_transpilation:
            self._transpiled_circuits = list(self._circuits)
        elif len(self._transpiled_circuits) < len(self._circuits):
            # transpile only circuits that are not transpiled yet
            self._transpile()
        return self._transpiled_circuits

    @property
    def backend(self):
        return self._BACKEND

    @property
    def transpile_options(self) -> Options:
        return self._transpile_options

    def set_transpile_options(self, **fields):
        self._transpile_options.update_options(**fields)

    def _transpile(self):
        from qiskit import transpile

        start = len(self._transpiled_circuits)
        self._transpiled_circuits.extend(
            transpile(
                self._circuits[start:],
                target=self.backend.target,
                **self.transpile_options.__dict__,
            ),
        )

    def _run(
        self,
        circuits: Sequence[QuantumCircuit],
        parameter_values: Sequence[Parameters],
        **run_options,
    ) -> PrimitiveJob:
        circuit_indices = []
        for circuit in circuits:
            index = self._circuit_ids.get(_circuit_key(circuit))
            if index is not None:
                circuit_indices.append(index)
            else:
                circuit_indices.append(len(self._circuits))
                self._circuit_ids[_circuit_key(circuit)] = len(self._circuits)
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
        transpiled_circuits = self.transpiled_circuits
        bound_circuits = self.backend.assign_parameters_from_backend(transpiled_circuits, parameter_values)
        bound_circuits = self._bound_pass_manager_run(bound_circuits)

        result = [self.backend.run(bound_circuits, **run_options).result()]

        return self._postprocessing(result, bound_circuits)

    def _postprocessing(self, result: list[Result], circuits: list[QuantumCircuit]) -> SamplerResult:
        counts = _prepare_counts(result)
        shots = sum(counts[0].values())

        probabilities = []
        metadata: list[dict[str, Any]] = [{} for _ in range(len(circuits))]
        for count in counts:
            prob_dist = {k: v / shots for k, v in count.items()}
            probabilities.append(QuasiDistribution(prob_dist, shots=shots, stddev_upper_bound=math.sqrt(1 / shots)))
            for metadatum in metadata:
                metadatum["shots"] = shots

        return SamplerResult(probabilities, metadata)

    def _bound_pass_manager_run(self, circuits):
        if self._bound_pass_manager is None:
            return circuits

        output = self._bound_pass_manager.run(circuits)
        if not isinstance(output, list):
            output = [output]
        return output
