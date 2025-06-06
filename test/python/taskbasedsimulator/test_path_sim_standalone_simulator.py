# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import unittest

from mqt.core.ir import QuantumComputation

from mqt.ddsim import PathCircuitSimulator, PathSimulatorConfiguration, PathSimulatorMode


class MQTStandaloneSimulatorTests(unittest.TestCase):
    def setUp(self) -> None:
        self.nonzero_states_ghz = 2

    def test_standalone(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = PathCircuitSimulator(circ)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_with_config(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = PathCircuitSimulator(circ, PathSimulatorConfiguration())
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_with_seed(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = PathCircuitSimulator(circ, seed=1337)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_individual_objects(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = PathCircuitSimulator(circ, seed=0, mode=PathSimulatorMode.bracket, bracket_size=2)
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_pairwise_only(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = PathCircuitSimulator(
            circ,
            seed=1,
            mode=PathSimulatorMode.pairwise_recursive,
            bracket_size=2,
        )
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result

    def test_standalone_gatecost_only(self) -> None:
        circ = QuantumComputation(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        sim = PathCircuitSimulator(
            circ,
            mode=PathSimulatorMode.gate_cost,
            starting_point=2,
            gate_cost=[1, 1],
        )
        result = sim.simulate(1000)
        assert len(result.keys()) == self.nonzero_states_ghz
        assert "000" in result
        assert "111" in result
