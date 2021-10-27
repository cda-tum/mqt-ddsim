import unittest

from qiskit import *

from jkq import ddsim


class JKQStandaloneSimulatorTests(unittest.TestCase):
    def test_standalone(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())

    def test_standalone_with_seed(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ, 1337)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())

    def test_standalone_pairwise(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ, mode=ddsim.PathMode.pairwise_recursive)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())

    def test_standalone_pairwise_with_seed(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ, 1337, mode=ddsim.PathMode.pairwise_recursive)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())
