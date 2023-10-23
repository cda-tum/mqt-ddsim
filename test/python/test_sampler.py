from __future__ import annotations

import numpy as np
import pytest
from qiskit import QuantumCircuit
from qiskit.circuit import Parameter

from mqt.ddsim.sampler import Sampler


@pytest.fixture()
def sampler() -> Sampler:
    """The sampler fixture for the tests in this file."""
    return Sampler()


@pytest.fixture()
def circuit() -> QuantumCircuit:
    """The circuit fixture for the tests in this file."""

    theta_a = Parameter("theta_a")
    theta_b = Parameter("theta_b")
    qc = QuantumCircuit(1, name="test")
    qc.rx(theta_a, 0)
    qc.rz(theta_b, 0)
    qc.rx(theta_a, 0)
    qc.measure_all()

    return qc


def test_ddsim_sampler(circuit: QuantumCircuit, sampler: Sampler):
    """Test DDSIM Sampler's functionality"""
    job = sampler.run([circuit], [[np.pi / 2, np.pi]])
    result = job.result()
    counts = result.quasi_dists[0]

    assert counts == {0: 1.0}
