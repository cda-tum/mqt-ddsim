from __future__ import annotations

import unittest

from qiskit import BasicAer, QuantumCircuit, execute

from mqt.ddsim.qasmsimulator import QasmSimulatorBackend


class MQTQasmSimulatorTest(unittest.TestCase):
    """Runs backend checks and the Basic qasm_simulator tests from Qiskit Terra."""

    def setUp(self):
        self.backend = QasmSimulatorBackend()
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
        self.backend.configuration()

    def test_properties(self):
        """Test backend.properties()."""
        properties = self.backend.properties()
        assert properties is None

    def test_status(self):
        """Test backend.status()."""
        self.backend.status()

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

    def test_qasm_simulator_approximation(self):
        """Test data counts output for single circuit run against reference."""
        shots = 1024
        circuit = QuantumCircuit.from_qasm_str(
            """OPENQASM 2.0;
            include "qelib1.inc";
            qreg q[2];
            h q[0];
            cx q[0], q[1];
            """
        )
        result = execute(
            circuit, self.backend, shots=shots, approximation_step_fidelity=0.4, approximation_steps=3
        ).result()
        counts = result.get_counts()
        assert len(counts) == 1

    def test_qasm_simulator_portfolioqaoa(self):
        """Run simulator with with 2-target gates that take a parameter. Circuit taken from MQT Bench."""
        circuit = QuantumCircuit.from_qasm_str(
            """OPENQASM 2.0;
            include "qelib1.inc";
            qreg q[3];
            creg meas[3];
            u2(0.41951949,-pi) q[0];
            u2(0.41620669,-pi) q[1];
            rzz(-0.420917333908502) q[0],q[1];
            u2(0.41905329,-pi) q[2];
            rzz(-0.421016123405307) q[0],q[2];
            u3(2.2348228,1.2558831,-pi/2) q[0];
            rzz(-0.420940441831552) q[1],q[2];
            u3(2.2348228,1.2087537,-pi/2) q[1];
            rzz(-5.98815838177421) q[0],q[1];
            u3(2.2348228,1.2492507,-pi/2) q[2];
            rzz(-5.98956380537088) q[0],q[2];
            u3(0.56042125,-1.2358007,pi/2) q[0];
            rzz(-5.98848712542991) q[1],q[2];
            u3(0.56042125,-1.2880621,pi/2) q[1];
            rzz(-6.64023274758061) q[0],q[1];
            u3(0.56042125,-1.2431553,pi/2) q[2];
            rzz(-6.6417912133385) q[0],q[2];
            rx(-4.06512402388918) q[0];
            rzz(-6.64059728943955) q[1],q[2];
            rx(-4.06512402388918) q[1];
            rx(-4.06512402388918) q[2];
            barrier q[0],q[1],q[2];
            measure q[0] -> meas[0];
            measure q[1] -> meas[1];
            measure q[2] -> meas[2];
            """
        )
        result = execute(circuit, self.backend, seed_simulator=1337, shots=8192).result()
        counts = result.get_counts()
        print(counts)
        assert len(counts) == 8
