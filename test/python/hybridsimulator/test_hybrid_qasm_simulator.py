from __future__ import annotations

import unittest

from qiskit import QuantumCircuit, QuantumRegister

from mqt.ddsim.hybridqasmsimulator import HybridQasmSimulatorBackend


class MQTHybridQasmSimulatorTest(unittest.TestCase):
    """Runs backend checks, the Basic qasm_simulator tests from Qiskit Terra, and some additional tests for the Hybrid DDSIM QasmSimulator."""

    def setUp(self) -> None:
        self.backend = HybridQasmSimulatorBackend()
        self.circuit = QuantumCircuit.from_qasm_str("""OPENQASM 2.0;
            include "qelib1.inc";
            qreg q[3];
            qreg r[3];
            h q;
            cx q, r;
            creg c[3];
            creg d[3];
            barrier q;
            measure q->c;
            measure r->d;""")
        self.circuit.name = "test"

    def test_status(self) -> None:
        """Test backend.status()."""
        self.backend.status()

    def test_qasm_simulator_single_shot(self) -> None:
        """Test single shot run."""
        assert self.backend.run(self.circuit, shots=1).result().success

    def test_qasm_simulator(self) -> None:
        """Test data counts output for single circuit run against reference."""
        shots = 1024
        result = self.backend.run(self.circuit, shots=shots).result()
        threshold = 0.04 * shots
        counts = result.get_counts()
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
        for key, value in target.items():
            assert key in counts
            assert abs(value - counts[key]) < threshold

    def test_qasm_simulator_access(self) -> None:
        """Test data counts output for multiple quantum circuits in a single job."""
        shots = 1024
        circuit_1 = QuantumCircuit(2, name="c1")
        circuit_2 = QuantumCircuit(2, name="c2")
        circuit_2.x(0)
        circuit_2.x(1)

        result = self.backend.run([circuit_1, circuit_2], shots=shots).result()
        assert result.success

        counts_1 = result.get_counts(circuit_1.name)
        counts_2 = result.get_counts(circuit_2.name)

        assert counts_1 == {"0": shots}
        assert counts_2 == {"11": shots}

    def test_dd_mode_simulation(self) -> None:
        """Test running a single circuit."""
        q = QuantumRegister(4)
        circ = QuantumCircuit(q)
        circ.h(q)
        circ.cz(3, 1)
        circ.cz(2, 0)
        circ.measure_all(inplace=True)
        print(circ.draw(fold=-1))
        self.circuit = circ
        result = self.backend.run(self.circuit, mode="dd").result()
        assert result.success

    def test_amplitude_mode_simulation(self) -> None:
        """Test running a single circuit."""
        q = QuantumRegister(4)
        circ = QuantumCircuit(q)
        circ.h(q)
        circ.cz(3, 1)
        circ.cz(2, 0)
        circ.measure_all(inplace=True)
        print(circ.draw(fold=-1))
        self.circuit = circ
        result = self.backend.run(self.circuit, mode="amplitude").result()
        assert result.success
