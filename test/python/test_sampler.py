from __future__ import annotations

import numpy as np
import pytest
from qiskit import ClassicalRegister, QuantumCircuit, QuantumRegister
from qiskit.circuit import Parameter

from mqt.ddsim.sampler import DDSIMBackendSampler


@pytest.fixture()
def sampler() -> DDSIMBackendSampler:
    """The sampler fixture for the tests in this file."""
    return DDSIMBackendSampler()


@pytest.fixture()
def circuit() -> QuantumCircuit:
    """The circuit fixture for the tests in this file."""

    theta_a = Parameter("theta_a")
    theta_b = Parameter("theta_b")
    qc = QuantumCircuit(name="test")
    q = QuantumRegister(1, "q")
    c = ClassicalRegister(1, "c")
    qc.add_register(q)
    qc.add_register(c)
    qc.rx(theta_a, 0)
    qc.rz(theta_b, 0)
    qc.rx(theta_a, 0)
    qc.barrier()
    qc.measure(q,c)
    
    return qc
    
def test_DDSIM_Sampler(circuit: QuantumCircuit, sampler: DDSIMBackendSampler):
    """Test DDSIM Sampler's functionality"""

    job = sampler.run([circuit],[[np.pi / 2, np.pi]])
    result = job.result()
    counts = result.quasi_dists[0]

    assert counts == {0: 1.0}
 
