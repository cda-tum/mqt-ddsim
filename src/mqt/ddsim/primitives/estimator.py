# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Estimator implementation using DDSIM CircuitSimulator."""

from __future__ import annotations

from typing import TYPE_CHECKING, Any, Union

import numpy as np
from mqt.core import load
from qiskit.circuit import QuantumCircuit
from qiskit.primitives import DataBin, PubResult, StatevectorEstimator
from qiskit.quantum_info import Pauli, SparsePauliOp

from mqt.ddsim.pyddsim import CircuitSimulator

if TYPE_CHECKING:
    from collections.abc import Mapping, Sequence

    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType
    from qiskit.primitives.container.observables_array import ObservablesArray
    from qiskit.primitives.containers.estimator_pub import EstimatorPub
    from qiskit.quantum_info import Pauli

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


class Estimator(StatevectorEstimator):  # type: ignore[misc]
    """DDSIM implementation of Qiskit's estimator."""

    def __init__(
        self,
        *,
        default_precision: float = 0.0,
        seed: np.random.Generator | int | None = -1,
    ) -> None:
        """Create a new DDSIM estimator."""
        if not isinstance(seed, int):
            msg = "seed must be an int."
            raise TypeError(msg)

        super().__init__(default_precision=default_precision, seed=seed)

    def _preprocess(
        self,
        circuit: QuantumCircuit,
        observables: ObservablesArray,
    ) -> list[QuantumCircuit]:
        """Perform preprocessing."""
        observable_circuits: list[QuantumCircuit] = []

        for observable in observables:
            pauli_strings, coeffs = zip(*observable.items())
            paulis = SparsePauliOp(pauli_strings, coeffs).paulis

            for pauli in paulis:
                qubit_indices = self._get_qubit_indices(pauli)
                observable_circuit = self._get_observable_circuit(circuit.num_qubits, qubit_indices, pauli)
                observable_circuit.metadata = {
                    "paulis": paulis,
                    "coeffs": np.real_if_close(coeffs),
                }
                observable_circuits.append(observable_circuit)

        return observable_circuits

    @staticmethod
    def _get_qubit_indices(pauli: Pauli) -> list[int]:
        """Get the indices of the qubits that are part of the Pauli observable."""
        qubit_indices = np.arange(pauli.num_qubits)[pauli.z | pauli.x]

        if not np.any(qubit_indices):
            return [0]

        return qubit_indices  # type: ignore [no-any-return]

    @staticmethod
    def _get_observable_circuit(num_qubits: int, qubit_indices: list[int], pauli: Pauli) -> QuantumCircuit:
        """Creates a quantum circuit representation of the Pauli observable."""
        observable_circuit = QuantumCircuit(num_qubits, len(qubit_indices))
        for i in qubit_indices:
            if pauli.x[i]:
                if pauli.z[i]:
                    observable_circuit.y(i)
                else:
                    observable_circuit.x(i)
            elif pauli.z[i]:
                observable_circuit.z(i)

        return observable_circuit

    def _run_pub(self, pub: EstimatorPub) -> PubResult:
        circuit = pub.circuit
        observables = pub.observables
        parameter_values = pub.parameter_values

        # Get observable circuits
        observable_circuits = self._preprocess(circuit, observables)

        # Extract metadata from observable circuits
        observable_metadatas = [observable_circuit.metadata for observable_circuit in observable_circuits]
        for observable_circuit in observable_circuits:
            observable_circuit.metadata = {}

        # Bind parameters
        if parameter_values.num_parameters == 0:
            bound_circuit = circuit
        else:
            raise NotImplementedError

        expectation_values = self._run_experiment(bound_circuit, observable_circuits, self.seed)

        return self._postprocess(expectation_values, observable_metadatas)

    @staticmethod
    def _run_experiment(
        bound_circuit: QuantumCircuit,
        observable_circuits: list[QuantumCircuit],
        seed: int = -1,
    ) -> list[float]:
        qc = load(bound_circuit)
        sim = CircuitSimulator(qc, seed=seed)

        return [
            sim.expectation_value(observable=load(observable_circuit)) for observable_circuit in observable_circuits
        ]

    @staticmethod
    def _postprocess(expectation_values: list[float], observable_metadatas: list[dict[str, Any]]) -> PubResult:
        """Perform postprocessing."""
        coeffs = observable_metadatas[0]["coeffs"]
        evs = np.dot(expectation_values, coeffs)
        data = DataBin(evs=evs, stds=0)
        return PubResult(data)
