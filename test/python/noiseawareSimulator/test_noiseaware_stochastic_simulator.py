from __future__ import annotations

import unittest

from qiskit import QuantumCircuit, QuantumRegister

from mqt import ddsim


class MQTStochQasmSimulatorTest(unittest.TestCase):
    """Runs backend checks, the Basic qasm_simulator tests from Qiskit Terra, and some additional tests for the Hybrid DDSIM QasmSimulator."""

    def setUp(self) -> None:
        q = QuantumRegister(4)
        circ = QuantumCircuit(q)
        circ.x(0)
        circ.x(1)
        circ.h(3)
        circ.cx(2, 3)
        circ.t(0)
        circ.t(1)
        circ.t(2)
        circ.tdg(3)
        circ.cx(0, 1)
        circ.cx(2, 3)
        circ.cx(3, 0)
        circ.cx(1, 2)
        circ.cx(0, 1)
        circ.cx(2, 3)
        circ.tdg(0)
        circ.tdg(1)
        circ.tdg(2)
        circ.t(3)
        circ.cx(0, 1)
        circ.cx(2, 3)
        circ.s(3)
        circ.cx(3, 0)
        circ.h(3)

        circ.measure_all(inplace=True)
        self.circuit = circ
        self.non_zeros_in_matrix = 16

    def test_no_noise(self):
        tolerance = 0
        sim = ddsim.StochasticNoiseSimulator(self.circuit, noiseEffects="APD",
                                             noiseProbability=0,
                                             ampDampingProbability=0,
                                             multiQubitGateFactor=2,
                                             seed=1)
        result = sim.simulate(1000)
        assert abs(result['1001'] - 1000) <= tolerance

    def test_def_config(self):
        tolerance = 50
        sim = ddsim.StochasticNoiseSimulator(self.circuit, noiseEffects="APD",
                                             noiseProbability=0.1,
                                             ampDampingProbability=0.1,
                                             multiQubitGateFactor=2,
                                             seed=1)
        result = sim.simulate(1000)
        assert abs(result['0000'] - 211) < tolerance
        assert abs(result['1000'] - 146) < tolerance