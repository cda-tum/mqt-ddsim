"""Estimator implementation using DDSIM CircuitSimulator"""

from __future__ import annotations

from itertools import accumulate
from typing import TYPE_CHECKING, Any, Mapping, Sequence, Union

import numpy as np
from qiskit.circuit import QuantumCircuit
from qiskit.primitives import Estimator as QiskitEstimator
from qiskit.primitives import EstimatorResult
from qiskit.quantum_info import Pauli, PauliList

from mqt.ddsim.pyddsim import CircuitSimulator
from mqt.ddsim.qasmsimulator import QasmSimulatorBackend

if TYPE_CHECKING:
    from qiskit.circuit import Parameter
    from qiskit.circuit.parameterexpression import ParameterValueType

    Parameters = Union[Mapping[Parameter, ParameterValueType], Sequence[ParameterValueType]]


class Estimator(QiskitEstimator):
    """DDSIM implementation of qiskit's sampler.
    Code adapted from Qiskit's BackendEstimator class.
    """

    def __init__(
        self,
        options: dict | None = None,
        abelian_grouping: bool = False,
    ) -> None:
        """Initialize a new Estimator instance

        Args:
            options: Default options.
            abelian_grouping: Whether the observable should be grouped into
                commuting
        """
        super().__init__(options=options)

        self._abelian_grouping = abelian_grouping

        self._preprocessed_circuits: tuple[list[QuantumCircuit], list[list[QuantumCircuit]]] | None = None

        self._grouping = list(zip(range(len(self._circuits)), range(len(self._observables))))

        self._circuit_ids: dict[tuple, int] = {}
        self._observable_ids: dict[tuple, int] = {}

    @property
    def preprocessed_circuits(
        self,
    ) -> tuple[list[QuantumCircuit], list[list[QuantumCircuit]]]:
        """
        Generate quantum circuits for states and observables produced by preprocessing.

        Returns:
        Tuple: A tuple containing two entries:
            - List: Quantum circuits list entered in run() method.
            - List: Quantum circuit representations of the observables.
        """
        self._preprocessed_circuits = self._preprocessing()
        return self._preprocessed_circuits

    def _preprocessing(self) -> tuple[list[QuantumCircuit], list[list[QuantumCircuit]]]:
        """
        Perform preprocessing for circuit arranging and packaging.

        Returns:
        Tuple:
            - List: Quantum circuits list entered in run() method.
            - List: Quantum circuit representations of the observables.

        This method performs preprocessing for circuit arranging and packaging. It processes quantum circuits and observables
        based on the specified grouping and abelian grouping conditions.
        """
        state_circuits = []
        observable_circuits = []
        for group in self._grouping:
            circuit = self._circuits[group[0]]
            observable = self._observables[group[1]]
            diff_circuits: list[QuantumCircuit] = []
            if self._abelian_grouping:
                for obs in observable.group_commuting(qubit_wise=True):
                    basis = Pauli((
                        np.logical_or.reduce(obs.paulis.z),
                        np.logical_or.reduce(obs.paulis.x),
                    ))
                    obs_circuit, indices = self._observable_circuit(circuit.num_qubits, basis)
                    paulis = PauliList.from_symplectic(
                        obs.paulis.z[:, indices],
                        obs.paulis.x[:, indices],
                        obs.paulis.phase,
                    )
                    obs_circuit.metadata = {
                        "paulis": paulis,
                        "coeffs": np.real_if_close(obs.coeffs),
                    }
                    diff_circuits.append(obs_circuit)
            else:
                for basis, obs in zip(observable.paulis, observable):
                    obs_circuit, indices = self._observable_circuit(circuit.num_qubits, basis)
                    paulis = PauliList.from_symplectic(
                        obs.paulis.z[:, indices],
                        obs.paulis.x[:, indices],
                        obs.paulis.phase,
                    )
                    obs_circuit.metadata = {
                        "paulis": paulis,
                        "coeffs": np.real_if_close(obs.coeffs),
                    }
                    diff_circuits.append(obs_circuit)

            state_circuits.append(circuit.copy())
            observable_circuits.append(diff_circuits)
        return state_circuits, observable_circuits

    @staticmethod
    def _observable_circuit(num_qubits: int, pauli: Pauli) -> tuple[QuantumCircuit, list[int]]:
        """
        Creates the quantum circuit representation of an observable given as a Pauli string.

        Parameters:
            - num_qubits: Number of qubits of the observable.
            - pauli_string (str): The Pauli string representing the observable.

        Returns:
        Tuple: A tuple containing two entries:
            - QuantumCircuit: The quantum circuit representation of the observable.
            - List: A list of the qubits involved in the observables operators
        """
        qubit_indices = np.arange(pauli.num_qubits)[pauli.z | pauli.x]
        if not np.any(qubit_indices):
            qubit_indices = [0]
        obs_circuit = QuantumCircuit(num_qubits, len(qubit_indices))
        for i in qubit_indices:
            if pauli.x[i]:
                if pauli.z[i]:
                    obs_circuit.y(i)
                else:
                    obs_circuit.x(i)
            elif pauli.z[i]:
                obs_circuit.z(i)

        return obs_circuit, qubit_indices

    def _call(
        self,
        circuits: Sequence[int],
        observables: Sequence[int],
        parameter_values: Sequence[Parameters],
        **run_options: dict[str, Any],
    ) -> EstimatorResult:
        # Organize circuits
        self._grouping = list(zip(circuits, observables))
        state_circuits, observable_circuits = self.preprocessed_circuits
        num_observables = [len(obs_circ_list) for obs_circ_list in observable_circuits]
        accum = [0, *list(accumulate(num_observables))]

        # Extract metadata from circuits
        metadata = [circ.metadata for obs_circ_list in observable_circuits for circ in obs_circ_list]
        for obs_circ_list in observable_circuits:
            for circ in obs_circ_list:
                circ.metadata = {}

        # Bind parameters
        bound_circuits = QasmSimulatorBackend.assign_parameters(state_circuits, parameter_values)

        # Run and bind parameters
        result_list = [
            exp
            for circ, obs_circ_list in zip(bound_circuits, observable_circuits)
            for exp in self._run_experiment(circ, obs_circ_list, **run_options)
        ]

        return self._postprocessing(result_list, accum, metadata)

    @staticmethod
    def _run_experiment(
        circ: QuantumCircuit,
        obs_circ_list: list[QuantumCircuit],
        **options: dict[str, Any],
    ) -> list[float]:
        approximation_step_fidelity = options.get("approximation_step_fidelity", 1.0)
        approximation_steps = options.get("approximation_steps", 1)
        approximation_strategy = options.get("approximation_strategy", "fidelity")
        seed = options.get("seed_simulator", -1)

        sim = CircuitSimulator(
            circ,
            approximation_step_fidelity=approximation_step_fidelity,
            approximation_steps=approximation_steps,
            approximation_strategy=approximation_strategy,
            seed=seed,
        )

        return [sim.expectation_value(observable=obs) for obs in obs_circ_list]

    @staticmethod
    def _postprocessing(result_list: list[float], accum: list[int], metadata: list[dict]) -> EstimatorResult:
        """
        Perform postprocessing for the evaluation of expectation values.

        Parameters:
        - result_list (list[float]): A list of measurement results.
        - accum (list[int]): A list representing accumulated indices for grouping measurement results.
        - metadata (list[dict]): A list of dictionaries containing metadata associated with each measurement.

        Returns:
        EstimatorResult: An instance of the EstimatorResult class containing the processed expectation values and metadata.

        This method calculates the expectation values by combining measurement results according to the provided accumulation indices.
        The resulting expectation values are then packaged into an EstimatorResult object along with metadata.

        The input metadata is overwritten, setting "variance" and "shots" to 0 for each expectation value.
        """
        expval_list = []

        for i, j in zip(accum, accum[1:]):
            combined_expval = 0.0

            for k in range(i, j):
                expval = [result_list[k]]
                meta = metadata[k]
                coeffs = meta["coeffs"]

                # Accumulate
                combined_expval += np.dot(expval, coeffs)

            expval_list.append(combined_expval)

        metadata = [{"variance": 0, "shots": 0} for _ in expval_list]

        return EstimatorResult(np.real_if_close(expval_list), metadata)
