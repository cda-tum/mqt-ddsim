from __future__ import annotations

from typing import Any

import pytest
from qiskit import QuantumCircuit
from qiskit.circuit.library import RealAmplitudes
from qiskit.primitives import SamplerResult

from mqt.ddsim.sampler import Sampler


@pytest.fixture()
def sampler() -> Sampler:
    """The sampler fixture for the tests in this file."""
    return Sampler()


@pytest.fixture()
def circuits() -> list[QuantumCircuit]:
    """The circuit lists fixture for the tests in this file."""

    bell_1 = QuantumCircuit(2)
    bell_1.h(0)
    bell_1.cx(0, 1)
    bell_1.measure_all()

    bell_2 = QuantumCircuit(2)
    bell_2.h(0)
    bell_2.cx(0, 1)
    bell_2.x(1)
    bell_2.measure_all()

    param_qc_1 = RealAmplitudes(num_qubits=2, reps=2)
    param_qc_1.measure_all()

    param_qc_2 = RealAmplitudes(num_qubits=2, reps=3)
    param_qc_2.measure_all()

    return [bell_1, bell_2, param_qc_1, param_qc_2]


def compare_probs(
    prob: list[dict[Any, float]] | dict[Any, float],
    target: list[dict[Any, float]] | dict[Any, float],
    tolerance: float = 0.045,
):
    if not isinstance(prob, list):
        prob = [prob]
    if not isinstance(target, list):
        target = [target]

    assert len(prob) == len(target)

    for p, targ in zip(prob, target):
        for key, t_val in targ.items():
            if key in p:
                assert abs(p[key] - t_val) < tolerance
            else:
                assert abs(t_val) < tolerance


def test_sampler_run_single_circuit(circuits: list[QuantumCircuit], sampler: Sampler):
    """Test Sampler.run() with single circuits"""
    bell = circuits[0]
    target = {0: 0.5, 1: 0, 2: 0, 3: 0.5}
    target_binary = {"00": 0.5, "11": 0.5, "01": 0, "10": 0}
    result = sampler.run([bell]).result()

    assert isinstance(result, SamplerResult)
    assert result.quasi_dists[0].shots == 1024

    compare_probs(result.quasi_dists[0], target)
    compare_probs(result.quasi_dists[0].binary_probabilities(), target_binary)


def test_sample_run_multiple_circuits(circuits: list[QuantumCircuit], sampler: Sampler):
    """Test Sampler.run() with multiple circuits."""
    bell_1 = circuits[0]
    bell_2 = circuits[1]
    target = [{0: 0.5, 1: 0, 2: 0, 3: 0.5}, {0: 0, 1: 0.5, 2: 0.5, 3: 0}, {0: 0.5, 1: 0, 2: 0, 3: 0.5}]
    result = sampler.run([bell_1, bell_2, bell_1]).result()
    compare_probs(result.quasi_dists, target)


def test_sampler_run_with_parameterized_circuits(circuits: list[QuantumCircuit], sampler: Sampler):
    """Test Sampler.run() with parameterized circuits."""
    param_qc = circuits[2]
    parameter_values = [[0, 1, 1, 2, 3, 5], [1, 2, 3, 4, 5, 6]]
    target = [
        {"00": 0.1309248462975777, "01": 0.3608720796028448, "10": 0.09324865232050054, "11": 0.41495442177907715},
        {"00": 0.06282290651933871, "01": 0.02877144385576705, "10": 0.606654494132085, "11": 0.3017511554928094},
    ]
    result = sampler.run([param_qc, param_qc], parameter_values, shots=5000000).result()
    compare_probs(result.quasi_dists[0].binary_probabilities(), target[0])
    compare_probs(result.quasi_dists[1].binary_probabilities(), target[1])


def test_sequential_run(circuits: list[QuantumCircuit], sampler: Sampler):
    """Sampler stores the information about the circuits in an instance attribute.
    If the same instance is used multiple times, the attribute still keeps the information about the circuits involved in previous runs.
    This test ensures that if a circuit is analyzed in different runs, the information about this circuit is not saved twice.
    """
    qc_1 = circuits[2]
    qc_2 = circuits[3]
    parameter_values = [[0, 1, 1, 2, 3, 5], [1, 2, 3, 4, 5, 6], [0, 1, 2, 3, 4, 5, 6, 7]]
    target = [
        {"00": 0.1309248462975777, "01": 0.3608720796028448, "10": 0.09324865232050054, "11": 0.41495442177907715},
        {"00": 0.06282290651933871, "01": 0.02877144385576705, "10": 0.606654494132085, "11": 0.3017511554928094},
        {"00": 0.1880263994380416, "01": 0.6881971261189544, "10": 0.09326232720582443, "11": 0.030514147237179892},
    ]

    # First run
    result = sampler.run(qc_1, parameter_values[0], shots=5000000).result()
    compare_probs(result.quasi_dists[0].binary_probabilities(), target[0])
    number_stored_circuits_first_run = len(sampler.circuits)

    # Second run
    result = sampler.run([qc_2, qc_1], [parameter_values[2], parameter_values[1]], shots=5000000).result()
    compare_probs(result.quasi_dists[0].binary_probabilities(), target[2])
    compare_probs(result.quasi_dists[1].binary_probabilities(), target[1])
    number_stored_circuits_second_run = len(sampler.circuits)

    assert number_stored_circuits_second_run == number_stored_circuits_first_run + 1
