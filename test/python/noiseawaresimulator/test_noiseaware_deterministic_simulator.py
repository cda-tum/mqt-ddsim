# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest
from mqt.ddsim.deterministicnoisesimulator import DeterministicNoiseSimulatorBackend
from qiskit import QuantumCircuit, qasm2


@pytest.fixture
def circuit() -> QuantumCircuit:
    """The circuit fixture for the tests in this file."""
    circ = qasm2.loads(
        """OPENQASM 2.0;
            include "qelib1.inc";
            qreg q[4];
            creg c[4];
            x q[0];
            x q[1];
            h q[3];
            cx q[2], q[3];
            t q[0];
            t q[1];
            t q[2];
            tdg q[3];
            cx q[0], q[1];
            cx q[2], q[3];
            cx q[3], q[0];
            cx q[1], q[2];
            cx q[0], q[1];
            cx q[2], q[3];
            tdg q[0];
            tdg q[1];
            tdg q[2];
            t q[3];
            cx q[0], q[1];
            cx q[2], q[3];
            s q[3];
            cx q[3], q[0];
            h q[3];
            barrier q;
            measure q->c;
            """
    )
    circ.name = "adder_n4"
    return circ


@pytest.fixture
def backend() -> DeterministicNoiseSimulatorBackend:
    """The backend fixture for the tests in this file."""
    return DeterministicNoiseSimulatorBackend()


def test_no_noise(circuit: QuantumCircuit, backend: DeterministicNoiseSimulatorBackend) -> None:
    shots = 1024
    result = backend.run(
        circuit,
        shots=shots,
        noise_probability=0,
        noise_effects="",
        amp_damping_probability=0,
        multi_qubit_gate_factor=0,
    ).result()
    counts = result.get_counts()
    assert counts["1001"] == shots


def test_default_config(circuit: QuantumCircuit, backend: DeterministicNoiseSimulatorBackend) -> None:
    tolerance = 100
    result = backend.run(circuit, shots=1000).result()
    counts = result.get_counts()
    assert abs(counts["0001"] - 173) < tolerance
    assert abs(counts["1001"] - 414) < tolerance


def test_custom_config(circuit: QuantumCircuit) -> None:
    tolerance = 50
    for i in range(20):
        sim = DeterministicNoiseSimulatorBackend()
        result = sim.run(
            circuit,
            shots=1000,
            noise_effects="AP",
            noise_probability=0.001,
            amp_damping_probability=0.002,
            multi_qubit_gate_factor=2,
            simulator_seed=i,
        ).result()
        counts = result.get_counts()
        assert abs(counts["1001"] - 936) < tolerance
