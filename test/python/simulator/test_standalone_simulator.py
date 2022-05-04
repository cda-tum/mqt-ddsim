import unittest

from qiskit import *

from mqt import ddsim


class MQTStandaloneSimulatorTests(unittest.TestCase):
    def test_standalone(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.CircuitSimulator(circ)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())

    def test_standalone_with_seed(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.CircuitSimulator(circ, 1337)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())
