import math
import unittest

from qiskit import QuantumCircuit, QuantumRegister, execute

from mqt.ddsim.hybridstatevectorsimulator import HybridStatevectorSimulator


class MQTHybridStatevectorSimulatorTest(unittest.TestCase):
    """Runs backend checks and some very basic functionality tests"""

    def setUp(self):
        self.backend = HybridStatevectorSimulator()
        qr = QuantumRegister(2)
        self.q_circuit = QuantumCircuit(qr)
        self.q_circuit.h(qr[0])
        self.q_circuit.cx(qr[0], qr[1])

    def test_configuration(self):
        """Test backend.configuration()."""
        return self.backend.configuration()

    def test_properties(self):
        """Test backend.properties()."""
        properties = self.backend.properties()
        assert properties is None

    def test_status(self):
        """Test backend.status()."""
        return self.backend.status()

    def test_statevector_output(self):
        """Test final state vector for single circuit run."""
        result = execute(self.q_circuit, backend=self.backend, shots=0).result()
        assert result.success is True
        actual = result.get_statevector(self.q_circuit)

        assert len(actual) == 2**2  # state vector has 2**(#qubits) length

        # state is 1/sqrt(2)|00> + 1/sqrt(2)|11>, up to a global phase
        assert math.isclose((abs(actual[0])) ** 2, 0.5, abs_tol=0.0001)
        assert actual[1] == 0
        assert actual[2] == 0
        assert math.isclose((abs(actual[3])) ** 2, 0.5, abs_tol=0.0001)
