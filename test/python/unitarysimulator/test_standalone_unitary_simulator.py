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
from mqt.core.ir import QuantumComputation

from mqt.ddsim import ConstructionMode, UnitarySimulator


class MQTStandaloneUnitarySimulatorTests(unittest.TestCase):
    def setUp(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)
        self.circuit = circ
        self.non_zeros_in_bell_circuit = 16

    def test_standalone_sequential_mode(self) -> None:
        sim = UnitarySimulator(self.circuit, mode=ConstructionMode.sequential)
        sim.construct()

        unitary = np.array(sim.get_constructed_dd().get_matrix(sim.get_number_of_qubits()), copy=False)
        print(unitary)
        assert np.count_nonzero(unitary) == self.non_zeros_in_bell_circuit

    def test_standalone_sequential_mode_with_seed(self) -> None:
        sim = UnitarySimulator(self.circuit, seed=1337, mode=ConstructionMode.sequential)
        sim.construct()

        unitary = np.array(sim.get_constructed_dd().get_matrix(sim.get_number_of_qubits()), copy=False)
        print(unitary)
        assert np.count_nonzero(unitary) == self.non_zeros_in_bell_circuit

    def test_standalone_recursive_mode(self) -> None:
        sim = UnitarySimulator(self.circuit, mode=ConstructionMode.recursive)
        sim.construct()

        unitary = np.array(sim.get_constructed_dd().get_matrix(sim.get_number_of_qubits()), copy=False)
        print(unitary)
        assert np.count_nonzero(unitary) == self.non_zeros_in_bell_circuit

    def test_standalone_recursive_mode_with_seed(self) -> None:
        sim = UnitarySimulator(self.circuit, seed=1337, mode=ConstructionMode.recursive)
        sim.construct()

        unitary = np.array(sim.get_constructed_dd().get_matrix(sim.get_number_of_qubits()), copy=False)
        print(unitary)
        assert np.count_nonzero(unitary) == self.non_zeros_in_bell_circuit
