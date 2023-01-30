import unittest

from qiskit import *

from mqt import ddsim


class MQTStandaloneSimulatorTests(unittest.TestCase):
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

    def test_standalone_with_config(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ, ddsim.PathSimulatorConfiguration())
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())

    def test_standalone_with_seed(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        config = ddsim.PathSimulatorConfiguration()
        config.seed = 1337
        sim = ddsim.PathCircuitSimulator(circ, config)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())

    def test_standalone_individual_objects(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        config = ddsim.PathSimulatorConfiguration()
        config.mode = ddsim.PathSimulatorMode.bracket
        config.bracket_size = 2
        sim = ddsim.PathCircuitSimulator(circ, config)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())

    def test_standalone_pairwise_only(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        config = ddsim.PathSimulatorConfiguration()
        config.mode = ddsim.PathSimulatorMode.pairwise_recursive
        config.seed = 1
        sim = ddsim.PathCircuitSimulator(circ, config)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())

    def test_standalone_gatecost_only(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        config = ddsim.PathSimulatorConfiguration()
        config.mode = ddsim.PathSimulatorMode.gate_cost
        config.gate_cost = [1, 1]
        config.starting_point = 2
        sim = ddsim.PathCircuitSimulator(circ, config)
        result = sim.simulate(1000)
        self.assertEqual(len(result.keys()), 2)
        self.assertIn('000', result.keys())
        self.assertIn('111', result.keys())
