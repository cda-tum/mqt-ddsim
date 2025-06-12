# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import numpy as np
import pytest
from qiskit.circuit import QuantumCircuit
from qiskit.circuit.library import RealAmplitudes
from qiskit.primitives import DataBin, PrimitiveResult, PubResult
from qiskit.quantum_info import Operator, Pauli, SparsePauliOp

from mqt.ddsim.primitives.estimator import Estimator

if TYPE_CHECKING:
    from numpy.typing import NDArray


@pytest.fixture
def estimator() -> Estimator:
    """The estimator fixture for the tests in this file."""
    return Estimator()


@pytest.fixture
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


@pytest.fixture
def observables() -> list[SparsePauliOp]:
    """The observable fixture for the tests in this file."""
    observable = SparsePauliOp.from_list([
        ("II", -1.052373245772859),
        ("IZ", 0.39793742484318045),
        ("ZI", -0.39793742484318045),
        ("ZZ", -0.01128010425623538),
        ("XX", 0.18093119978423156),
    ])

    pauli_x = Pauli("X")
    pauli_y = Pauli("Y")
    pauli_z = Pauli("Z")

    hamiltonian_1 = SparsePauliOp.from_list([("II", 1), ("IZ", 2), ("XI", 3)])
    hamiltonian_2 = SparsePauliOp.from_list([("IZ", 1)])
    hamiltonian_3 = SparsePauliOp.from_list([("ZI", 1), ("ZZ", 1)])

    return [
        observable,
        [hamiltonian_1, hamiltonian_2, hamiltonian_3],
        [pauli_x, pauli_y, pauli_z],
    ]


def get_evs(result: PubResult) -> NDArray[float]:
    """Get the expected values from a PubResult."""
    assert isinstance(result, PubResult)
    assert isinstance(result.data, DataBin)
    return result.data["evs"]


def test_run__single_circuit__single_observable__without_parameters(
    circuits: list[QuantumCircuit],
    observables: list[SparsePauliOp],
    estimator: Estimator,
) -> None:
    """Test single circuit and single observable, without parameters."""
    circuit = circuits[0].assign_parameters([0, 1, 1, 2, 3, 5])
    observable = observables[0]

    result = estimator.run([(circuit, [observable])]).result()
    assert isinstance(result, PrimitiveResult)
    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [-1.284366511861733], rtol=1e-7, atol=1e-7)


def test_run__single_circuit__operator_observable__without_parameters(
    circuits: list[QuantumCircuit], estimator: Estimator
) -> None:
    """Test single circuit and an operator observable, without parameters."""
    circuit = circuits[0].assign_parameters([0, 1, 1, 2, 3, 5])
    matrix = SparsePauliOp.from_operator(
        Operator([
            [-1.06365335, 0.0, 0.0, 0.1809312],
            [0.0, -1.83696799, 0.1809312, 0.0],
            [0.0, 0.1809312, -0.24521829, 0.0],
            [0.1809312, 0.0, 0.0, -1.06365335],
        ])
    )

    result = estimator.run([(circuit, [matrix])]).result()
    assert isinstance(result, PrimitiveResult)
    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [-1.284366511861733], rtol=1e-7, atol=1e-7)


def test_run__single_circuit__single_observable__with_parameters(
    circuits: list[QuantumCircuit],
    observables: list[SparsePauliOp],
    estimator: Estimator,
) -> None:
    """Test single circuit and single observable, with parameters."""
    circuit = circuits[0]
    observable = observables[0]

    result = estimator.run([(circuit, [observable], [[0, 1, 1, 2, 3, 5]])]).result()
    assert isinstance(result, PrimitiveResult)
    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [-1.284366511861733], rtol=1e-7, atol=1e-7)


def test_run__single_circuit__multiple_observables__without_parameters(
    circuits: list[QuantumCircuit],
    observables: list[SparsePauliOp],
    estimator: Estimator,
) -> None:
    """Test single circuit and multiple observables, without parameters."""
    qc_x, _, _ = circuits[2]
    pauli_x, pauli_y, pauli_z = observables[2]

    result = estimator.run([(qc_x, [pauli_x, pauli_y, pauli_z])]).result()
    assert isinstance(result, PrimitiveResult)
    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [1.0, 0.0, 0.0], rtol=1e-7, atol=1e-7)


def test_run__multiple_circuits__multiple_observables__without_parameters(
    circuits: list[QuantumCircuit],
    observables: list[SparsePauliOp],
    estimator: Estimator,
) -> None:
    """Test multiple circuits and multiple observables, without parameters."""
    qc_x, qc_y, qc_z = circuits[2]
    pauli_x, pauli_y, pauli_z = observables[2]

    result = estimator.run([(qc_x, [pauli_x]), (qc_y, [pauli_y]), (qc_z, [pauli_z])]).result()
    assert isinstance(result, PrimitiveResult)

    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [1.0], rtol=1e-7, atol=1e-7)

    evs = get_evs(result[1])
    np.testing.assert_allclose(evs, [1.0], rtol=1e-7, atol=1e-7)

    evs = get_evs(result[2])
    np.testing.assert_allclose(evs, [1.0], rtol=1e-7, atol=1e-7)


def test_run__single_circuit__multiple_observables__with_parameters(
    circuits: list[QuantumCircuit],
    observables: list[SparsePauliOp],
    estimator: Estimator,
) -> None:
    """Test single circuits and multiple observables, with parameters."""
    psi_1, _ = circuits[1]
    hamiltonian_1, _, hamiltonian_3 = observables[1]
    theta_1, theta_2 = (
        [0, 1, 1, 2, 3, 5],
        [1, 2, 3, 4, 5, 6],
    )

    result = estimator.run([(psi_1, [hamiltonian_1, hamiltonian_3], [theta_1, theta_2])]).result()
    assert isinstance(result, PrimitiveResult)
    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [1.55555728, -1.08766318], rtol=1e-7, atol=1e-7)


def test_run__multiple_circuits__multiple_observables__with_parameters(
    circuits: list[QuantumCircuit],
    observables: list[SparsePauliOp],
    estimator: Estimator,
) -> None:
    """Test multiple circuits and multiple observables, with parameters."""
    psi_1, psi_2 = circuits[1]
    hamiltonian_1, hamiltonian_2, hamiltonian_3 = observables[1]
    theta_1, theta_2, theta_3 = (
        [0, 1, 1, 2, 3, 5],
        [0, 1, 1, 2, 3, 5, 8, 13],
        [1, 2, 3, 4, 5, 6],
    )

    result = estimator.run(
        [
            (psi_1, [hamiltonian_1], [theta_1]),
            (psi_2, [hamiltonian_2], [theta_2]),
            (psi_1, [hamiltonian_3], [theta_3]),
        ],
    ).result()
    assert isinstance(result, PrimitiveResult)

    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [1.55555728], rtol=1e-7, atol=1e-7)

    evs = get_evs(result[1])
    np.testing.assert_allclose(evs, [0.17849238], rtol=1e-7, atol=1e-7)

    evs = get_evs(result[2])
    np.testing.assert_allclose(evs, [-1.08766318], rtol=1e-7, atol=1e-7)


def test_sequential_runs(
    circuits: list[QuantumCircuit],
    observables: list[SparsePauliOp],
    estimator: Estimator,
) -> None:
    """Test sequential runs."""
    psi_1, psi_2 = circuits[1]
    hamiltonian_1, hamiltonian_2, hamiltonian_3 = observables[1]
    theta_1, theta_2, theta_3 = (
        [0, 1, 1, 2, 3, 5],
        [0, 1, 1, 2, 3, 5, 8, 13],
        [1, 2, 3, 4, 5, 6],
    )

    # First run
    result = estimator.run([(psi_1, [hamiltonian_1], [theta_1])]).result()
    assert isinstance(result, PrimitiveResult)
    assert isinstance(result[0], PubResult)
    evs = result[0].data["evs"]
    np.testing.assert_allclose(evs, [1.5555573817900956], rtol=1e-7, atol=1e-7)

    # Second run
    result = estimator.run([(psi_2, [hamiltonian_1], [theta_2])]).result()
    assert isinstance(result, PrimitiveResult)
    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [2.97797666], rtol=1e-7, atol=1e-7)

    # Third run
    result = estimator.run([(psi_1, [hamiltonian_2], [theta_1]), (psi_1, [hamiltonian_3], [theta_1])]).result()
    assert isinstance(result, PrimitiveResult)

    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [-0.551653], rtol=1e-7, atol=1e-7)

    evs = get_evs(result[1])
    np.testing.assert_allclose(evs, [0.07535239], rtol=1e-7, atol=1e-7)

    # Last run
    result = estimator.run([
        (psi_1, [hamiltonian_1], [theta_1]),
        (psi_2, [hamiltonian_2], [theta_2]),
        (psi_1, [hamiltonian_3], [theta_3]),
    ]).result()
    assert isinstance(result, PrimitiveResult)

    evs = get_evs(result[0])
    np.testing.assert_allclose(evs, [1.55555728], rtol=1e-7, atol=1e-7)

    evs = get_evs(result[1])
    np.testing.assert_allclose(evs, [0.17849238], rtol=1e-7, atol=1e-7)

    evs = get_evs(result[2])
    np.testing.assert_allclose(evs, [-1.08766318], rtol=1e-7, atol=1e-7)
