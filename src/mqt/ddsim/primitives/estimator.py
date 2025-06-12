# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Estimator implementation using DDSIM CircuitSimulator."""

from __future__ import annotations

from typing import TYPE_CHECKING

import numpy as np
from mqt.core import load
from qiskit.circuit import QuantumCircuit
from qiskit.primitives import DataBin, PubResult, StatevectorEstimator
from qiskit.quantum_info import Pauli, SparsePauliOp

from mqt.ddsim.pyddsim import CircuitSimulator

if TYPE_CHECKING:
    from numpy.typing import NDArray
    from qiskit.primitives.container import BindingsArray, ObservablesArray
    from qiskit.primitives.containers import EstimatorPub
    from qiskit.quantum_info import Pauli


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

    def _get_observable_circuits(
        self,
        circuit: QuantumCircuit,
        observables: ObservablesArray,
    ) -> NDArray[object]:
        """Perform preprocessing."""
        observable_circuits = np.zeros_like(observables, dtype=object)

        for index in np.ndindex(*observables.shape):
            observable = observables[index]

            pauli_strings, coeffs = zip(*observable.items())
            paulis = SparsePauliOp(pauli_strings, coeffs).paulis

            observable_circuits_list = []
            for pauli in paulis:
                qubit_indices = self._get_qubit_indices(pauli)
                observable_circuit = self._get_observable_circuit(circuit.num_qubits, qubit_indices, pauli)
                observable_circuits_list.append(observable_circuit)

            observable_circuits[index] = (coeffs, observable_circuits_list)

        return observable_circuits

    @staticmethod
    def _get_qubit_indices(pauli: Pauli) -> list[int]:
        """Get the indices of the qubits that are part of the Pauli observable."""
        qubit_indices = np.arange(pauli.num_qubits)[pauli.z | pauli.x]
        assert isinstance(qubit_indices, np.ndarray)

        if not np.any(qubit_indices):
            return [0]

        qubit_indices_list = qubit_indices.tolist()
        assert isinstance(qubit_indices_list, list)
        return qubit_indices_list

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

    @staticmethod
    def _get_bound_circuits(
        circuit: QuantumCircuit,
        parameter_values: BindingsArray,
    ) -> NDArray[object]:
        return parameter_values.bind_all(circuit)

    def _run_pub(self, pub: EstimatorPub) -> PubResult:
        circuit = pub.circuit
        observables = pub.observables
        parameter_values = pub.parameter_values

        observable_circuits = self._get_observable_circuits(circuit, observables)
        bound_circuits = self._get_bound_circuits(circuit, parameter_values)
        bc_bound_circuits, bc_observable_circuits = np.broadcast_arrays(bound_circuits, observable_circuits)

        evs = np.zeros_like(bc_bound_circuits, dtype=np.float64)
        stds = np.zeros_like(bc_bound_circuits, dtype=np.float64)

        for index in np.ndindex(*bc_bound_circuits.shape):
            bound_circuit = bc_bound_circuits[index]
            observable_coeffs, observable_circuits = bc_observable_circuits[index]
            expectation_values = self._run_experiment(bound_circuit, observable_circuits, self.seed)
            evs[index] = np.dot(expectation_values, observable_coeffs)

        data = DataBin(evs=evs, stds=stds, shape=evs.shape)
        return PubResult(data)

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
