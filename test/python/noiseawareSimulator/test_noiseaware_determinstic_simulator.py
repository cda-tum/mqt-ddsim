from __future__ import annotations

import unittest

from qiskit import QuantumCircuit

from mqt.ddsim.deterministicnoisesimulator import DeterministicNoiseSimulatorBackend


class MQTDeterministicQasmSimulatorTest(unittest.TestCase):
    """Runs some basic tests for the density matrix simulator"""

    def setUp(self) -> None:
        self.circuit = QuantumCircuit.from_qasm_str("""
            OPENQASM 2.0;
            include "qelib1.inc";
            qreg q[4];
            creg c[4];
            x q[0];
            x q[1];
            h q[3];
            cx q[2], q[3];
            t q[0];
            t q[1];
            t q[2];
            tdg q[3];
            cx q[0], q[1];
            cx q[2], q[3];
            cx q[3], q[0];
            cx q[1], q[2];
            cx q[0], q[1];
            cx q[2], q[3];
            tdg q[0];
            tdg q[1];
            tdg q[2];
            t q[3];
            cx q[0], q[1];
            cx q[2], q[3];
            s q[3];
            cx q[3], q[0];
            h q[3];
            barrier q;
            measure q->c;
            """)
        self.circuit.name = "adder_n4"
        # Quantum circuit is taken from https://github.com/pnnl/QASMBench

    def test_no_noise(self):
        for _i in range(20):
            sim = DeterministicNoiseSimulatorBackend()
            result = sim.run(
                self.circuit,
                shots=1024,
                noise_probability=0,
                noise_effects="",
                amp_damping_probability=0,
                multi_qubit_gate_factor=0,
            ).result()
            counts = result.get_counts()
            assert counts["1001"] - 1024 == 0

    def test_default_config(self):
        tolerance = 100
        sim = sim = DeterministicNoiseSimulatorBackend()
        result = sim.run(self.circuit, shots=1000).result()
        counts = result.get_counts()
        assert abs(counts["0001"] - 173) < tolerance
        assert abs(counts["1001"] - 414) < tolerance

    def test_custom_config(self):
        tolerance = 50
        for i in range(20):
            sim = DeterministicNoiseSimulatorBackend()
            result = sim.run(
                self.circuit,
                shots=1000,
                noise_effects="AP",
                noise_probability=0.001,
                amp_damping_probability=0.002,
                multi_qubit_gate_factor=2,
                seed=i,
            ).result()
            counts = result.get_counts()
            assert abs(counts["1001"] - 936) < tolerance
