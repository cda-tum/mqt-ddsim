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

        sim = ddsim.PathCircuitSimulator(circ)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_with_config(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ, ddsim.PathSimulatorConfiguration())
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_with_seed(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ, seed=1337)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_individual_objects(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ, seed=0, mode=ddsim.PathSimulatorMode.bracket, bracket_size=2)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_pairwise_only(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(circ, seed=1, mode=ddsim.PathSimulatorMode.pairwise_recursive, bracket_size=2)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_gatecost_only(self):
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = ddsim.PathCircuitSimulator(
            circ, mode=ddsim.PathSimulatorMode.gate_cost, starting_point=2, gate_cost=[1, 1]
        )
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result
