import unittest

from qiskit import QuantumCircuit

from mqt import ddsim


class MQTStandaloneSimulatorTests(unittest.TestCase):
    def setUp(self) -> None:
        self.nonzero_states_ghz = 2

    def test_standalone(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.CircuitSimulator(circ)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_with_seed(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.CircuitSimulator(circ, seed=1337)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_simple_approximation(self):
        import numpy as np

        # creates a state with <2% probability of measuring |1x>
        circ = QuantumCircuit(2)
        circ.h(0)
        circ.cry(np.pi / 8, 0, 1)

        # create a simulator that approximates once and by at most 2%
        sim = ddsim.CircuitSimulator(circ, approximation_step_fidelity=0.98, approximation_steps=2)
        result = sim.simulate(4096)

        # the result should always be 0
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "00" in result
        assert "01" in result
