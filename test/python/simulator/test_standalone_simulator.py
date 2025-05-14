# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pathlib
import unittest

from mqt.core import load
from mqt.core.ir import QuantumComputation

from mqt.ddsim import CircuitSimulator


class MQTStandaloneSimulatorTests(unittest.TestCase):
    def setUp(self) -> None:
        self.nonzero_states_ghz = 2

    def test_truly_standalone(self) -> None:
        filename = str(pathlib.Path(__file__).with_name("ghz_03.qasm").absolute())
        circ = load(filename)
        sim = CircuitSimulator(circ)
        result = sim.simulate(1000)
        print(result)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = CircuitSimulator(circ)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_with_seed(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = CircuitSimulator(circ, seed=1337)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_simple_approximation(self) -> None:
        import numpy as np

        # creates a state with <2% probability of measuring |1x>
        circ = QuantumComputation(2)
        circ.h(0)
        circ.cry(np.pi / 8, 0, 1)
        circ.i(0)
        circ.i(0)

        # create a simulator that approximates once and by at most 2%
        sim = CircuitSimulator(circ, approximation_step_fidelity=0.98, approximation_steps=1)
        result = sim.simulate(4096)

        # the result should always be 0
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "00" in result
        assert "01" in result

    @staticmethod
    def test_native_two_qubit_gates() -> None:
        qc = QuantumComputation(2)
        qc.dcx(0, 1)
        qc.ecr(0, 1)
        qc.rxx(0.5, 0, 1)
        qc.rzz(0.5, 0, 1)
        qc.ryy(0.5, 0, 1)
        qc.rzx(0.5, 0, 1)
        qc.xx_minus_yy(0.5, 0.25, 0, 1)
        qc.xx_plus_yy(0.5, 0.25, 0, 1)
        print(qc)
        print(qc.global_phase)
        sim = CircuitSimulator(qc)
        result = sim.simulate(1000)
        print(result)

    @staticmethod
    def test_expectation_value_local_operators() -> None:
        import numpy as np

        max_qubits = 3
        for qubits in range(1, max_qubits + 1):
            qc = QuantumComputation(qubits)
            sim = CircuitSimulator(qc)
            for i in range(qubits):
                x_observable = QuantumComputation(qubits)
                x_observable.x(i)
                assert sim.expectation_value(x_observable) == 0
                z_observable = QuantumComputation(qubits)
                z_observable.z(i)
                assert sim.expectation_value(z_observable) == 1
                h_observable = QuantumComputation(qubits)
                h_observable.h(i)
                assert np.allclose(sim.expectation_value(h_observable), 1 / np.sqrt(2))

    @staticmethod
    def test_expectation_value_global_operators() -> None:
        import numpy as np

        max_qubits = 3
        for qubits in range(1, max_qubits + 1):
            qc = QuantumComputation(qubits)
            sim = CircuitSimulator(qc)
            x_observable = QuantumComputation(qubits)
            z_observable = QuantumComputation(qubits)
            h_observable = QuantumComputation(qubits)
            for i in range(qubits):
                x_observable.x(i)
                z_observable.z(i)
                h_observable.h(i)
            assert sim.expectation_value(x_observable) == 0
            assert sim.expectation_value(z_observable) == 1
            assert np.allclose(sim.expectation_value(h_observable), (1 / np.sqrt(2)) ** qubits)
