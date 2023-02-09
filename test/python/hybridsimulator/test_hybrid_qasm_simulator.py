import unittest

from qiskit import BasicAer, QuantumCircuit, QuantumRegister, execute

from mqt.ddsim.hybridqasmsimulator import HybridQasmSimulatorBackend


class MQTHybridQasmSimulatorTest(unittest.TestCase):
    """Runs backend checks, the Basic qasm_simulator tests from Qiskit Terra, and some additional tests for the Hybrid DDSIM QasmSimulator."""

    def setUp(self):
        self.backend = HybridQasmSimulatorBackend()
        self.circuit = QuantumCircuit.from_qasm_str(
            """OPENQASM 2.0;
            include "qelib1.inc";
            qreg q[3];
            qreg r[3];
            h q;
            cx q, r;
            creg c[3];
            creg d[3];
            barrier q;
            measure q->c;
            measure r->d;"""
        )
        self.circuit.name = "test"

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

    def test_qasm_simulator_single_shot(self):
        """Test single shot run."""
        result = execute(self.circuit, self.backend, shots=1).result()
        assert result.success

    def test_qasm_simulator(self):
        """Test data counts output for single circuit run against reference."""
        shots = 1024
        result = execute(self.circuit, self.backend, shots=shots).result()
        threshold = 0.04 * shots
        counts = result.get_counts("test")
        target = {
            "100 100": shots / 8,
            "011 011": shots / 8,
            "101 101": shots / 8,
            "111 111": shots / 8,
            "000 000": shots / 8,
            "010 010": shots / 8,
            "110 110": shots / 8,
            "001 001": shots / 8,
        }

        assert len(target) == len(counts)
        for key in target:
            assert key in counts
            assert abs(target[key] - counts[key]) < threshold

    def test_basicaer_simulator(self):
        """Test data counts output for single circuit run against reference."""
        shots = 1024
        result = execute(self.circuit, BasicAer.get_backend("qasm_simulator"), shots=shots).result()
        threshold = 0.04 * shots
        counts = result.get_counts("test")
        target = {
            "100 100": shots / 8,
            "011 011": shots / 8,
            "101 101": shots / 8,
            "111 111": shots / 8,
            "000 000": shots / 8,
            "010 010": shots / 8,
            "110 110": shots / 8,
            "001 001": shots / 8,
        }

        assert len(target) == len(counts)
        for key in target:
            assert key in counts
            assert abs(target[key] - counts[key]) < threshold

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
        result = execute(self.circuit, self.backend, mode="dd").result()
        assert result.success
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
        result = execute(self.circuit, self.backend, mode="amplitude").result()
        assert result.success
        return result
