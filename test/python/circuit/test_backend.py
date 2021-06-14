from qiskit.circuit import QuantumCircuit, QuantumRegister, ClassicalRegister
from qiskit import execute
from jkq.ddsim.qasmsimulator import QasmSimulator

import unittest


class JKQBackendTestCase(unittest.TestCase):
    """Tests for the JKU backend."""

    def setUp(self):
        self.backend = QasmSimulator()

    def test_configuration(self):
        """Test backend.configuration()."""
        configuration = self.backend.configuration()
        return configuration

    def test_properties(self):
        """Test backend.properties()."""
        properties = self.backend.properties()
        self.assertEqual(properties, None)

    def test_status(self):
        """Test backend.status()."""
        status = self.backend.status()
        return status

    def test_run_circuit(self):
        """Test running a single circuit."""
        qr = QuantumRegister(2, name='qr')
        cr = ClassicalRegister(2, name='qc')
        qc = QuantumCircuit(qr, cr, name='bell')
        qc.h(qr[0])
        qc.cx(qr[0], qr[1])
        qc.measure(qr, cr)

        result = execute(qc, self.backend).result()
        self.assertEqual(result.success, True)
        return result

