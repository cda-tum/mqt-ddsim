import math
import unittest

from qiskit import QuantumCircuit, QuantumRegister, execute

from mqt.ddsim.statevectorsimulator import StatevectorSimulatorBackend


class MQTStatevectorSimulatorTest(unittest.TestCase):
    def setUp(self):
        self.backend = StatevectorSimulatorBackend()
        qr = QuantumRegister(2)
        self.q_circuit = QuantumCircuit(qr)
        self.q_circuit.h(qr[0])
        self.q_circuit.cx(qr[0], qr[1])

    def test_target(self):
        """Test backend.target"""
        return self.backend.target

    def test_configuration(self):
        """Test backend.configuration()."""
        return self.backend.configuration()

    def test_status(self):
        """Test backend.status()."""
        return self.backend.status()

    def test_statevector_output(self):
        """Test final state vector for single circuit run."""
        result = execute(self.q_circuit, backend=self.backend).result()
        assert result.success
        actual = result.get_statevector()

        # state is 1/sqrt(2)|00> + 1/sqrt(2)|11>, up to a global phase
        assert math.isclose((abs(actual[0])) ** 2, 0.5, abs_tol=0.0001)
        assert actual[1] == 0
        assert actual[2] == 0
        assert math.isclose((abs(actual[3])) ** 2, 0.5, abs_tol=0.0001)
