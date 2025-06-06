# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import math
import unittest

from mqt.ddsim.pathstatevectorsimulator import PathStatevectorSimulatorBackend
from qiskit import QuantumCircuit, QuantumRegister


class MQTStatevectorSimulatorTest(unittest.TestCase):
    def setUp(self) -> None:
        self.backend = PathStatevectorSimulatorBackend()
        qr = QuantumRegister(2)
        self.q_circuit = QuantumCircuit(qr)
        self.q_circuit.h(qr[0])
        self.q_circuit.cx(qr[0], qr[1])

    def test_statevector_output(self) -> None:
        """Test final state vector for single circuit run."""
        result = self.backend.run(self.q_circuit).result()
        assert result.success
        actual = result.get_statevector()

        # state is 1/sqrt(2)|00> + 1/sqrt(2)|11>, up to a global phase
        assert math.isclose((abs(actual[0])) ** 2, 0.5, abs_tol=0.0001)
        assert actual[1] == 0
        assert actual[2] == 0
        assert math.isclose((abs(actual[3])) ** 2, 0.5, abs_tol=0.0001)

    def test_statevector_output_pairwise(self) -> None:
        """Test final state vector for single circuit run."""
        mode = "pairwise_recursive"
        result = self.backend.run(self.q_circuit, mode=mode).result()
        assert result.success
        actual = result.get_statevector()

        # state is 1/sqrt(2)|00> + 1/sqrt(2)|11>, up to a global phase
        assert math.isclose((abs(actual[0])) ** 2, 0.5, abs_tol=0.0001)
        assert actual[1] == 0
        assert actual[2] == 0
        assert math.isclose((abs(actual[0])) ** 2, 0.5, abs_tol=0.0001)
