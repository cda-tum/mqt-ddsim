import unittest

from qiskit import *

from mqt import ddsim

import numpy as np


class MQTStandaloneUnitarySimulatorTests(unittest.TestCase):
    def setUp(self) -> None:
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)
        self.circuit = circ
        self.unitary = np.zeros((2 ** circ.num_qubits, 2 ** circ.num_qubits), dtype=complex)

    def test_standalone_sequential_mode(self):
        sim = ddsim.UnitarySimulator(self.circuit, mode=ddsim.ConstructionMode.sequential)
        sim.construct()

        ddsim.get_matrix(sim, self.unitary)
        print(self.unitary)
        self.assertEqual(np.count_nonzero(self.unitary), 16)

    def test_standalone_sequential_mode_with_seed(self):
        sim = ddsim.UnitarySimulator(self.circuit, 1337, mode=ddsim.ConstructionMode.sequential)
        sim.construct()

        ddsim.get_matrix(sim, self.unitary)
        print(self.unitary)
        self.assertEqual(np.count_nonzero(self.unitary), 16)

    def test_standalone_recursive_mode(self):
        sim = ddsim.UnitarySimulator(self.circuit, mode=ddsim.ConstructionMode.recursive)
        sim.construct()

        ddsim.get_matrix(sim, self.unitary)
        print(self.unitary)
        self.assertEqual(np.count_nonzero(self.unitary), 16)

    def test_standalone_recursive_mode_with_seed(self):
        sim = ddsim.UnitarySimulator(self.circuit, 1337, mode=ddsim.ConstructionMode.recursive)
        sim.construct()

        ddsim.get_matrix(sim, self.unitary)
        print(self.unitary)
        self.assertEqual(np.count_nonzero(self.unitary), 16)
