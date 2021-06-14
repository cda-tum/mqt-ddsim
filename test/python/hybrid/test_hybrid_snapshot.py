import unittest
from qiskit import QuantumCircuit, QuantumRegister
from qiskit import execute

from jkq.ddsim.hybridstatevectorsimulator import HybridStatevectorSimulator


class JKQSnapshotTest(unittest.TestCase):
    def setUp(self):
        self.backend = HybridStatevectorSimulator()
        qr = QuantumRegister(2)
        self.q_circuit = QuantumCircuit(qr)
        self.q_circuit.h(qr[0])
        self.q_circuit.cx(qr[0], qr[1])

    def test_statevector_output(self):
        """Test final state vector for single circuit run."""
        result = execute(self.q_circuit, backend=self.backend, shots=0).result()
        self.assertEqual(result.success, True)
        actual = result.get_statevector(self.q_circuit)

        # state is 1/sqrt(2)|00> + 1/sqrt(2)|11>, up to a global phase
        self.assertAlmostEqual((abs(actual[0])) ** 2, 1 / 2, places=5)
        self.assertEqual(actual[1], 0)
        self.assertEqual(actual[2], 0)
        self.assertAlmostEqual((abs(actual[3])) ** 2, 1 / 2, places=5)
