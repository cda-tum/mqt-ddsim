from __future__ import annotations

import numpy as np
import pytest
from qiskit.circuit import QuantumCircuit
from qiskit.circuit.library import RealAmplitudes
from qiskit.primitives import EstimatorResult
from qiskit.quantum_info import Operator, Pauli, SparsePauliOp

from mqt.ddsim.primitives.estimator import Estimator


@pytest.fixture()
def estimator() -> Estimator:
    """The estimator fixture for the tests in this file."""
    return Estimator()


@pytest.fixture()
def circuits() -> list[QuantumCircuit]:
    """The circuit fixture for the tests in this file."""
    qc_z = QuantumCircuit(1)
    qc_z.ry(np.pi / 2, 0)
    qc_z.rz(np.pi, 0)
    qc_z.ry(np.pi / 2, 0)

    qc_y = QuantumCircuit(1)
    qc_y.rx(-np.pi / 2, 0)

    qc_x = QuantumCircuit(1)
    qc_x.ry(np.pi / 2, 0)

    ansatz = RealAmplitudes(num_qubits=2, reps=2)
    param_qc_1 = RealAmplitudes(num_qubits=2, reps=2)
    param_qc_2 = RealAmplitudes(num_qubits=2, reps=3)

    return [ansatz, [param_qc_1, param_qc_2], [qc_x, qc_y, qc_z]]


@pytest.fixture()
def observables() -> list[SparsePauliOp]:
    """The observable fixture for the tests in this file."""
    observable = SparsePauliOp.from_list(
        [
            ("II", -1.052373245772859),
            ("IZ", 0.39793742484318045),
            ("ZI", -0.39793742484318045),
            ("ZZ", -0.01128010425623538),
            ("XX", 0.18093119978423156),
        ]
    )

    pauli_x = Pauli("X")
    pauli_y = Pauli("Y")
    pauli_z = Pauli("Z")

    hamiltonian_1 = SparsePauliOp.from_list([("II", 1), ("IZ", 2), ("XI", 3)])
    hamiltonian_2 = SparsePauliOp.from_list([("IZ", 1)])
    hamiltonian_3 = SparsePauliOp.from_list([("ZI", 1), ("ZZ", 1)])

    return [observable, [hamiltonian_1, hamiltonian_2, hamiltonian_3], [pauli_x, pauli_y, pauli_z]]


def test_estimator_run_single_circuit__observable_no_params(
    circuits: list[QuantumCircuit], observables: list[SparsePauliOp], estimator: Estimator
):
    """test for estimator with a single circuit/observable and no parameters"""
    circuit = circuits[0].assign_parameters([0, 1, 1, 2, 3, 5])
    observable = observables[0]

    # Pass circuit and observable as a sequence
    result = estimator.run([circuit], [observable]).result()

    assert isinstance(result, EstimatorResult)
    np.testing.assert_allclose(result.values, [-1.284366511861733], rtol=1e-7, atol=1e-7)

    # Pass circuit and observable as an object
    result = estimator.run(circuit, observable).result()

    assert isinstance(result, EstimatorResult)
    np.testing.assert_allclose(result.values, [-1.284366511861733], rtol=1e-7, atol=1e-7)


def test_run_with_operator(circuits: list[QuantumCircuit], estimator: Estimator):
    """test for run with Operator as an observable"""
    circuit = circuits[0].assign_parameters([0, 1, 1, 2, 3, 5])
    matrix = Operator(
        [
            [-1.06365335, 0.0, 0.0, 0.1809312],
            [0.0, -1.83696799, 0.1809312, 0.0],
            [0.0, 0.1809312, -0.24521829, 0.0],
            [0.1809312, 0.0, 0.0, -1.06365335],
        ]
    )
    result = estimator.run([circuit], [matrix]).result()

    assert isinstance(result, EstimatorResult)
    np.testing.assert_allclose(result.values, [-1.284366511861733], rtol=1e-7, atol=1e-7)


def test_estimator_run_single_circuit__observable_with_params(
    circuits: list[QuantumCircuit], observables: list[SparsePauliOp], estimator: Estimator
):
    """test for estimator with a single circuit/observable and parameters"""
    circuit = circuits[0]
    observable = observables[0]

    # Pass circuit, observable and parameter values as a sequence
    result = estimator.run([circuit], [observable], [[0, 1, 1, 2, 3, 5]]).result()

    assert isinstance(result, EstimatorResult)
    np.testing.assert_allclose(result.values, [-1.284366511861733], rtol=1e-7, atol=1e-7)

    # Pass circuit, observable and parameter values as objects
    result = estimator.run(circuit, observable, [0, 1, 1, 2, 3, 5]).result()

    assert isinstance(result, EstimatorResult)
    np.testing.assert_allclose(result.values, [-1.284366511861733], rtol=1e-7, atol=1e-7)


def test_estimator_run_multiple_circuits_observables_no_params(
    circuits: list[QuantumCircuit], observables: list[SparsePauliOp], estimator: Estimator
):
    """test for estimator with multiple circuits/observables and no parameters"""
    qc_x, qc_y, qc_z = circuits[2]
    pauli_x, pauli_y, pauli_z = observables[2]

    result = estimator.run([qc_x, qc_y, qc_z], [pauli_x, pauli_y, pauli_z]).result()

    assert isinstance(result, EstimatorResult)
    assert np.array_equal(result.values, [1, 1, 1])


def test_estimator_run_multiple_circuits_observables_with_params(
    circuits: list[QuantumCircuit], observables: list[SparsePauliOp], estimator: Estimator
):
    """test for estimator with multiple circuits/observables with parameters"""
    psi1, psi2 = circuits[1]
    hamiltonian_1, hamiltonian_2, hamiltonian_3 = observables[1]
    theta_1, theta_2, theta_3 = ([0, 1, 1, 2, 3, 5], [0, 1, 1, 2, 3, 5, 8, 13], [1, 2, 3, 4, 5, 6])

    result = estimator.run(
        [psi1, psi2, psi1], [hamiltonian_1, hamiltonian_2, hamiltonian_3], [theta_1, theta_2, theta_3]
    ).result()

    assert isinstance(result, EstimatorResult)
    np.testing.assert_allclose(result.values, [1.55555728, 0.17849238, -1.08766318], rtol=1e-7, atol=1e-7)


def test_estimator_sequenctial_run(
    circuits: list[QuantumCircuit], observables: list[SparsePauliOp], estimator: Estimator
):
    """test for estimator's sequenctial run"""
    psi1, psi2 = circuits[1]
    hamiltonian_1, hamiltonian_2, hamiltonian_3 = observables[1]
    theta_1, theta_2, theta_3 = ([0, 1, 1, 2, 3, 5], [0, 1, 1, 2, 3, 5, 8, 13], [1, 2, 3, 4, 5, 6])

    # First run
    result = estimator.run([psi1], [hamiltonian_1], [theta_1]).result()

    assert isinstance(result, EstimatorResult)
    np.testing.assert_allclose(result.values, [1.5555573817900956], rtol=1e-7, atol=1e-7)

    # Second run
    result2 = estimator.run([psi2], [hamiltonian_1], [theta_2]).result()

    assert isinstance(result2, EstimatorResult)
    np.testing.assert_allclose(result2.values, [2.97797666], rtol=1e-7, atol=1e-7)

    # Third run
    result3 = estimator.run([psi1, psi1], [hamiltonian_2, hamiltonian_3], [theta_1] * 2).result()

    assert isinstance(result3, EstimatorResult)
    np.testing.assert_allclose(result3.values, [-0.551653, 0.07535239], rtol=1e-7, atol=1e-7)

    # Last run
    result4 = estimator.run(
        [psi1, psi2, psi1], [hamiltonian_1, hamiltonian_2, hamiltonian_3], [theta_1, theta_2, theta_3]
    ).result()

    assert isinstance(result4, EstimatorResult)
    np.testing.assert_allclose(result4.values, [1.55555728, 0.17849238, -1.08766318], rtol=1e-7, atol=1e-7)
