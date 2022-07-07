import unittest

from qiskit import *

from mqt import ddsim


class MQTStandaloneHybridSimulatorTest(unittest.TestCase):
    def setUp(self) -> None:
        q = QuantumRegister(4)
        circ = QuantumCircuit(q)
        circ.h(q)
        circ.cz(3, 1)
        circ.cz(2, 0)
        circ.measure_all(inplace=True)
        self.circuit = circ

    def test_standalone_amplitude_mode(self):
        sim = ddsim.HybridCircuitSimulator(self.circuit, mode=ddsim.HybridMode.amplitude)
        result = sim.simulate(2048)
        self.assertEqual(len(result.keys()), 16)

    def test_standalone_amplitude_mode_with_seed(self):
        sim = ddsim.HybridCircuitSimulator(self.circuit, seed=1337, mode=ddsim.HybridMode.amplitude)
        result = sim.simulate(2048)
        self.assertEqual(len(result.keys()), 16)

    def test_standalone_dd_mode(self):
        sim = ddsim.HybridCircuitSimulator(self.circuit, mode=ddsim.HybridMode.DD)
        result = sim.simulate(2048)
        self.assertEqual(len(result.keys()), 16)

    def test_standalone_dd_mode_with_seed(self):
        sim = ddsim.HybridCircuitSimulator(self.circuit, seed=1337, mode=ddsim.HybridMode.DD)
        result = sim.simulate(2048)
        self.assertEqual(len(result.keys()), 16)
