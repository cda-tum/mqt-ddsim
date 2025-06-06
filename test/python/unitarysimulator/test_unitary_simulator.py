# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import unittest

import numpy as np
from mqt.ddsim.unitarysimulator import UnitarySimulatorBackend
from qiskit import QuantumCircuit


class MQTUnitarySimulatorTest(unittest.TestCase):
    def setUp(self) -> None:
        self.backend = UnitarySimulatorBackend()
        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)
        self.circuit = circ
        self.circuit.name = "test"
        self.non_zeros_in_bell_circuit = 16

    def test_unitary_simulator_sequential_mode(self) -> None:
        result = self.backend.run(self.circuit, mode="sequential").result()
        assert result.success
        print(result.get_unitary())
        assert np.count_nonzero(result.get_unitary()) == self.non_zeros_in_bell_circuit

    def test_unitary_simulator_recursive_mode(self) -> None:
        result = self.backend.run(self.circuit, mode="recursive").result()
        assert result.success
        assert np.count_nonzero(result.get_unitary()) == self.non_zeros_in_bell_circuit
