import unittest
from qiskit import QuantumCircuit
from jkq.ddsim.qasmsimulator import QasmSimulator
from qiskit import execute


class TestQasmSimulatorJKQBasic(unittest.TestCase):
    """Runs the Basic qasm_simulator tests from Terra on JKU."""

    def setUp(self):
        self.backend = QasmSimulator()
        compiled_circuit = QuantumCircuit.from_qasm_str('''OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
qreg r[3];
h q;
cx q, r;
creg c[3];
creg d[3];
barrier q;
measure q->c;
measure r->d;''')
        compiled_circuit.name = 'test'
        self.circuit = compiled_circuit

    def test_qasm_simulator_single_shot(self):
        """Test single shot run."""
        result = execute(self.circuit, self.backend, shots=1).result()
        self.assertEqual(result.success, True)

    def test_qasm_simulator(self):
        """Test data counts output for single circuit run against reference."""
        shots = 1024
        result = execute(self.circuit, self.backend, shots=shots).result()
        threshold = 0.04 * shots
        counts = result.get_counts('test')
        target = {'100100': shots / 8, '011011': shots / 8,
                  '101101': shots / 8, '111111': shots / 8,
                  '000000': shots / 8, '010010': shots / 8,
                  '110110': shots / 8, '001001': shots / 8}

        self.assertEqual(len(target), len(counts))
        for key in target.keys():
            self.assertTrue(key in counts)
            self.assertLess(abs(target[key] - counts[key]), threshold)
