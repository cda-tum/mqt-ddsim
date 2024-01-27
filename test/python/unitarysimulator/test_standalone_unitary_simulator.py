from __future__ import annotations

import unittest

import numpy as np
import numpy.typing as npt

from mqt import ddsim
from mqt.core import QuantumComputation


class MQTStandaloneUnitarySimulatorTests(unittest.TestCase):
    def setUp(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)
        self.circuit = circ
        self.unitary: npt.NDArray[np.complex128] = np.zeros(
            (2**circ.num_qubits, 2**circ.num_qubits), dtype=np.complex128
        )
        self.non_zeros_in_bell_circuit = 16

    def test_standalone_sequential_mode(self):
        sim = ddsim.UnitarySimulator(self.circuit, mode=ddsim.ConstructionMode.sequential)
        sim.construct()

        ddsim.get_matrix(sim, self.unitary)
        print(self.unitary)
        assert np.count_nonzero(self.unitary) == self.non_zeros_in_bell_circuit

    def test_standalone_sequential_mode_with_seed(self):
        sim = ddsim.UnitarySimulator(self.circuit, seed=1337, mode=ddsim.ConstructionMode.sequential)
        sim.construct()

        ddsim.get_matrix(sim, self.unitary)
        print(self.unitary)
        assert np.count_nonzero(self.unitary) == self.non_zeros_in_bell_circuit

    def test_standalone_recursive_mode(self):
        sim = ddsim.UnitarySimulator(self.circuit, mode=ddsim.ConstructionMode.recursive)
        sim.construct()

        ddsim.get_matrix(sim, self.unitary)
        print(self.unitary)
        assert np.count_nonzero(self.unitary) == self.non_zeros_in_bell_circuit

    def test_standalone_recursive_mode_with_seed(self):
        sim = ddsim.UnitarySimulator(self.circuit, seed=1337, mode=ddsim.ConstructionMode.recursive)
        sim.construct()

        ddsim.get_matrix(sim, self.unitary)
        print(self.unitary)
        assert np.count_nonzero(self.unitary) == self.non_zeros_in_bell_circuit
