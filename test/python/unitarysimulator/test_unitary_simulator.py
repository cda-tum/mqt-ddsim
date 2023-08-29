from __future__ import annotations

import unittest

import numpy as np
from qiskit import QuantumCircuit, execute

from mqt.ddsim.unitarysimulator import UnitarySimulatorBackend


class MQTUnitarySimulatorTest(unittest.TestCase):
    def setUp(self):
        self.backend = UnitarySimulatorBackend()
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)
        self.circuit = circ
        self.circuit.name = "test"
        self.non_zeros_in_bell_circuit = 16

    def test_configuration(self):
        """Test backend.configuration()."""
        self.backend.configuration()

    def test_properties(self):
        """Test backend.properties()."""
        properties = self.backend.properties()
        assert properties is None

    def test_status(self):
        """Test backend.status()."""
        self.backend.status()

    def test_unitary_simulator_sequential_mode(self):
        result = execute(self.circuit, self.backend, mode="sequential").result()
        assert result.success
        print(result.get_unitary("test"))
        assert np.count_nonzero(result.get_unitary("test")) == self.non_zeros_in_bell_circuit

    def test_unitary_simulator_recursive_mode(self):
        result = execute(self.circuit, self.backend, mode="recursive").result()
        assert result.success
        print(result.data("test"))
        assert np.count_nonzero(result.get_unitary("test")) == self.non_zeros_in_bell_circuit
