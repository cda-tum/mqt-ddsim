from __future__ import annotations

import unittest

import numpy as np
from qiskit import QuantumCircuit

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

    def test_unitary_simulator_sequential_mode(self):
        result = self.backend.run(self.circuit, mode="sequential").result()
        assert result.success
        print(result.get_unitary())
        assert np.count_nonzero(result.get_unitary()) == self.non_zeros_in_bell_circuit

    def test_unitary_simulator_recursive_mode(self):
        result = self.backend.run(self.circuit, mode="recursive").result()
        assert result.success
        assert np.count_nonzero(result.get_unitary()) == self.non_zeros_in_bell_circuit
