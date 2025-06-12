# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TypeVar

import pytest
from qiskit import QuantumCircuit
from qiskit.circuit.library import RealAmplitudes
from qiskit.primitives import PrimitiveResult, SamplerPubResult

from mqt.ddsim.primitives import Sampler

T = TypeVar("T", str, int)


@pytest.fixture
def sampler() -> Sampler:
    """The sampler fixture for the tests in this file."""
    return Sampler()


@pytest.fixture
def shots() -> int:
    """Number of shots for the tests in this file."""
    return 65536


@pytest.fixture
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
    prob: list[dict[T, float]] | dict[T, float],
    target: list[dict[T, float]] | dict[T, float],
    tolerance: float = 0.01,
) -> None:
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


def test_run__single_circuit__without_parameters(circuits: list[QuantumCircuit], sampler: Sampler, shots: int) -> None:
    """Test single circuit without parameters."""
    bell = circuits[0]
    target = {0: 0.5, 1: 0, 2: 0, 3: 0.5}
    target_binary = {"00": 0.5, "11": 0.5, "01": 0, "10": 0}

    result = sampler.run([bell], shots=shots).result()
    assert isinstance(result, PrimitiveResult)
    assert isinstance(result[0], SamplerPubResult)

    assert result[0].data["meas"].num_shots == shots

    prob = {key: value / shots for key, value in result[0].data["meas"].get_int_counts().items()}
    compare_probs(prob, target)

    prob = {key: value / shots for key, value in result[0].data["meas"].get_counts().items()}
    compare_probs(prob, target_binary)


def test_run__multiple_circuits__without_parameters(
    circuits: list[QuantumCircuit], sampler: Sampler, shots: int
) -> None:
    """Test multiple circuit without parameters."""
    bell_1 = circuits[0]
    bell_2 = circuits[1]
    target = [
        {0: 0.5, 1: 0, 2: 0, 3: 0.5},
        {0: 0, 1: 0.5, 2: 0.5, 3: 0},
        {0: 0.5, 1: 0, 2: 0, 3: 0.5},
    ]

    result = sampler.run([bell_1, bell_2, bell_1], shots=shots).result()
    assert isinstance(result, PrimitiveResult)

    assert isinstance(result[0], SamplerPubResult)
    prob = {key: value / shots for key, value in result[0].data["meas"].get_int_counts().items()}
    compare_probs(prob, target[0])

    assert isinstance(result[1], SamplerPubResult)
    prob = {key: value / shots for key, value in result[1].data["meas"].get_int_counts().items()}
    compare_probs(prob, target[1])

    assert isinstance(result[2], SamplerPubResult)
    prob = {key: value / shots for key, value in result[2].data["meas"].get_int_counts().items()}
    compare_probs(prob, target[2])


def test_run__single_circuits__with_parameters(circuits: list[QuantumCircuit], sampler: Sampler, shots: int) -> None:
    """Test single circuit with parameters."""
    param_qc = circuits[2]
    parameter_values = [[0, 1, 1, 2, 3, 5], [1, 2, 3, 4, 5, 6]]
    target = [
        {
            "00": 0.1309248462975777,
            "01": 0.3608720796028448,
            "10": 0.09324865232050054,
            "11": 0.41495442177907715,
        },
        {
            "00": 0.06282290651933871,
            "01": 0.02877144385576705,
            "10": 0.606654494132085,
            "11": 0.3017511554928094,
        },
    ]

    # Test multiple PUBs
    result = sampler.run([(param_qc, [parameter_values[0]]), (param_qc, [parameter_values[1]])], shots=shots).result()
    assert isinstance(result, PrimitiveResult)

    assert isinstance(result[0], SamplerPubResult)
    bit_array_0 = result[0].data["meas"]
    prob = {key: value / shots for key, value in bit_array_0.get_counts().items()}
    compare_probs(prob, target[0])

    assert isinstance(result[1], SamplerPubResult)
    bit_array_1 = result[1].data["meas"]
    prob = {key: value / shots for key, value in bit_array_1.get_counts().items()}
    compare_probs(prob, target[1])

    # Test single PUB
    result = sampler.run([(param_qc, parameter_values)], shots=shots).result()
    assert isinstance(result, PrimitiveResult)
    assert isinstance(result[0], SamplerPubResult)

    bit_array_0 = result[0].data["meas"][0]
    prob = {key: value / shots for key, value in bit_array_0.get_counts().items()}
    compare_probs(prob, target[0])

    bit_array_1 = result[0].data["meas"][1]
    prob = {key: value / shots for key, value in bit_array_1.get_counts().items()}
    compare_probs(prob, target[1])


def test_sequential_runs(circuits: list[QuantumCircuit], sampler: Sampler, shots: int) -> None:
    """Test sequential runs."""
    qc_1 = circuits[2]
    qc_2 = circuits[3]
    parameter_values = [
        [0, 1, 1, 2, 3, 5],
        [1, 2, 3, 4, 5, 6],
        [0, 1, 2, 3, 4, 5, 6, 7],
    ]
    target = [
        {
            "00": 0.1309248462975777,
            "01": 0.3608720796028448,
            "10": 0.09324865232050054,
            "11": 0.41495442177907715,
        },
        {
            "00": 0.06282290651933871,
            "01": 0.02877144385576705,
            "10": 0.606654494132085,
            "11": 0.3017511554928094,
        },
        {
            "00": 0.1880263994380416,
            "01": 0.6881971261189544,
            "10": 0.09326232720582443,
            "11": 0.030514147237179892,
        },
    ]

    # First run
    result = sampler.run([(qc_1, parameter_values[0])], shots=shots).result()
    assert isinstance(result, PrimitiveResult)

    assert isinstance(result[0], SamplerPubResult)
    prob = {key: value / shots for key, value in result[0].data["meas"].get_counts().items()}
    compare_probs(prob, target[0])

    # Second run
    result = sampler.run([(qc_2, parameter_values[2]), (qc_1, parameter_values[1])], shots=shots).result()
    assert isinstance(result, PrimitiveResult)

    assert isinstance(result[0], SamplerPubResult)
    prob = {key: value / shots for key, value in result[0].data["meas"].get_counts().items()}
    compare_probs(prob, target[2])

    assert isinstance(result[1], SamplerPubResult)
    prob = {key: value / shots for key, value in result[1].data["meas"].get_counts().items()}
    compare_probs(prob, target[1])
