import unittest
from qiskit import QuantumCircuit, QuantumRegister
from qiskit import execute

from jkq.ddsim.statevectorsimulator import StatevectorSimulator


class JKUSnapshotTest(unittest.TestCase):
    """Test JKU's statevector return capatbilities."""

    def setUp(self):
        super().setUp()
        self.backend = StatevectorSimulator()
        qr = QuantumRegister(2)
        self.q_circuit = QuantumCircuit(qr)
        self.q_circuit.h(qr[0])
        self.q_circuit.cx(qr[0], qr[1])

    def test_statevector_output(self):
        """Test final state vector for single circuit run."""
        result = execute(self.q_circuit, backend=self.backend).result()
        self.assertEqual(result.success, True)
        actual = result.get_statevector(self.q_circuit)

        # state is 1/sqrt(2)|00> + 1/sqrt(2)|11>, up to a global phase
        self.assertAlmostEqual((abs(actual[0]))**2, 1/2, places=5)
        self.assertEqual(actual[1], 0)
        self.assertEqual(actual[2], 0)
        self.assertAlmostEqual((abs(actual[3]))**2, 1/2, places=5)
