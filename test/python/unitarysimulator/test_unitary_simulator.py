import unittest

from qiskit import QuantumCircuit
from mqt.ddsim.unitarysimulator import UnitarySimulator
from qiskit import execute

import numpy as np


class MQTUnitarySimulatorTest(unittest.TestCase):

    def setUp(self):
        self.backend = UnitarySimulator()
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)
        self.circuit = circ
        self.circuit.name = 'test'

    def test_configuration(self):
        """Test backend.configuration()."""
        configuration = self.backend.configuration()
        return configuration

    def test_properties(self):
        """Test backend.properties()."""
        properties = self.backend.properties()
        self.assertEqual(properties, None)

    def test_status(self):
        """Test backend.status()."""
        status = self.backend.status()
        return status

    def test_unitary_simulator_sequential_mode(self):
        result = execute(self.circuit, self.backend, mode='sequential').result()
        self.assertEqual(result.success, True)
        print(result.get_unitary('test'))
        self.assertEqual(np.count_nonzero(result.get_unitary('test')), 16)
        return result

    def test_unitary_simulator_recursive_mode(self):
        result = execute(self.circuit, self.backend, mode='recursive').result()
        self.assertEqual(result.success, True)
        print(result.data('test'))
        self.assertEqual(np.count_nonzero(result.get_unitary('test')), 16)
        return result
