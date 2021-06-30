import unittest

from qiskit import QuantumCircuit, BasicAer, QuantumRegister
from jkq.ddsim.hybridqasmsimulator import HybridQasmSimulator
from qiskit import execute


class JKQHybridQasmSimulatorTest(unittest.TestCase):
    """Runs backend checks, the Basic qasm_simulator tests from Qiskit Terra, and some additional tests for the Hybrid JKQ QasmSimulator."""

    def setUp(self):
        self.backend = HybridQasmSimulator()
        self.circuit = QuantumCircuit.from_qasm_str('''OPENQASM 2.0;
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
        self.circuit.name = 'test'

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
        target = {'100 100': shots / 8, '011 011': shots / 8,
                  '101 101': shots / 8, '111 111': shots / 8,
                  '000 000': shots / 8, '010 010': shots / 8,
                  '110 110': shots / 8, '001 001': shots / 8}

        self.assertEqual(len(target), len(counts))
        for key in target.keys():
            self.assertIn(key, counts)
            self.assertLess(abs(target[key] - counts[key]), threshold)

    def test_basicaer_simulator(self):
        """Test data counts output for single circuit run against reference."""
        shots = 1024
        result = execute(self.circuit, BasicAer.get_backend('qasm_simulator'), shots=shots).result()
        threshold = 0.04 * shots
        counts = result.get_counts('test')
        target = {'100 100': shots / 8, '011 011': shots / 8,
                  '101 101': shots / 8, '111 111': shots / 8,
                  '000 000': shots / 8, '010 010': shots / 8,
                  '110 110': shots / 8, '001 001': shots / 8}

        self.assertEqual(len(target), len(counts))
        for key in target.keys():
            self.assertIn(key, counts)
            self.assertLess(abs(target[key] - counts[key]), threshold)

    def test_dd_mode_simulation(self):
        """Test running a single circuit."""
        q = QuantumRegister(4)
        circ = QuantumCircuit(q)
        circ.h(q)
        circ.cz(3, 1)
        circ.cz(2, 0)
        circ.measure_all(inplace=True)
        print(circ.draw(fold=-1))
        self.circuit = circ
        result = execute(self.circuit, self.backend, mode='dd').result()
        self.assertEqual(result.success, True)
        return result

    def test_amplitude_mode_simulation(self):
        """Test running a single circuit."""
        q = QuantumRegister(4)
        circ = QuantumCircuit(q)
        circ.h(q)
        circ.cz(3, 1)
        circ.cz(2, 0)
        circ.measure_all(inplace=True)
        print(circ.draw(fold=-1))
        self.circuit = circ
        result = execute(self.circuit, self.backend, mode='amplitude').result()
        self.assertEqual(result.success, True)
        return result
