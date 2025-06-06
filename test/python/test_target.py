# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import contextlib

import numpy as np
import pytest
from mqt.ddsim.target import DDSIMTargetBuilder
from qiskit import QuantumCircuit, transpile
from qiskit.transpiler import Target


@pytest.fixture
def target() -> Target:
    """The target fixture for the tests in this file."""
    target = Target(num_qubits=8)
    DDSIMTargetBuilder.add_0q_gates(target)
    DDSIMTargetBuilder.add_1q_gates(target)
    DDSIMTargetBuilder.add_2q_gates(target)
    DDSIMTargetBuilder.add_3q_gates(target)
    DDSIMTargetBuilder.add_multi_qubit_gates(target)
    DDSIMTargetBuilder.add_non_unitary_operations(target)
    DDSIMTargetBuilder.add_barrier(target)
    return target


@pytest.mark.parametrize("gate", ["x", "y", "z", "h", "s", "sdg", "t", "tdg"])
def test_transpilation_preserves_1q_0p_target_gates(target: Target, gate: str) -> None:
    """Test that transpilation does not change single-qubit gates without parameters that are already in the target."""
    qc = QuantumCircuit(1)
    getattr(qc, gate)(0)
    qc_transpiled = transpile(qc, target=target)
    assert len(qc_transpiled.data) == 1
    assert qc_transpiled.data[0].operation.name == gate


@pytest.mark.parametrize("gate", ["rx", "ry", "rz", "p"])
def test_transpile_preserves_1q_1p_target_gates(target: Target, gate: str) -> None:
    """Test that transpilation does not change single-qubit gates with one parameter that are already in the target."""
    qc = QuantumCircuit(1)
    getattr(qc, gate)(np.pi, 0)
    qc_transpiled = transpile(qc, target=target)
    assert len(qc_transpiled.data) == 1
    assert qc_transpiled.data[0].operation.name == gate


@pytest.mark.parametrize("gate", ["cx", "cy", "cz", "ch", "cs", "csdg", "csx", "swap", "iswap", "dcx", "ecr"])
def test_transpilation_preserves_2q_0p_target_gates(target: Target, gate: str) -> None:
    """Test that transpilation does not change two-qubit gates without parameters that are already in the target."""
    qc = QuantumCircuit(2)
    with contextlib.suppress(AttributeError):
        getattr(qc, gate)(0, 1)
        qc_transpiled = transpile(qc, target=target)
        print(qc_transpiled)
        num_gates = len(qc_transpiled.data)
        assert num_gates <= 1
        assert num_gates == 0 or qc_transpiled.data[0].operation.name == gate


@pytest.mark.parametrize("gate", ["rxx", "ryy", "rzz", "rzx", "cp", "crx", "cry", "crz"])
def test_transpilation_preserves_2q_1p_target_gates(target: Target, gate: str) -> None:
    """Test that transpilation does not change two-qubit gates with one parameter that are already in the target."""
    qc = QuantumCircuit(2)
    getattr(qc, gate)(np.pi / 2, 0, 1)
    qc_transpiled = transpile(qc, target=target)
    assert len(qc_transpiled.data) == 1
    assert qc_transpiled.data[0].operation.name == gate


@pytest.mark.parametrize("gate", ["ccx", "ccz", "cswap"])
def test_transpilation_preserves_3q_target_gates(target: Target, gate: str) -> None:
    """Test that transpilation does not change three-qubit gates that are already in the target."""
    qc = QuantumCircuit(3)
    with contextlib.suppress(AttributeError):
        getattr(qc, gate)(0, 1, 2)
        qc_transpiled = transpile(qc, target=target)
        assert len(qc_transpiled.data) == 1
        assert qc_transpiled.data[0].operation.name == gate


@pytest.mark.parametrize("num_controls", list(range(3, 6)))
@pytest.mark.parametrize("mode", ["noancilla", "recursion", "v-chain"])
def test_transpilation_preserves_mcx_target_gates(target: Target, num_controls: int, mode: str) -> None:
    """Test that transpilation does not change MCX gates that are already in the target."""
    nqubits = num_controls + 1
    nancillas = 0
    if mode == "recursion":
        nancillas = 1
    elif mode == "v-chain":
        nancillas = max(0, nqubits - 2)
    qc = QuantumCircuit(nqubits + nancillas)
    controls = list(range(1, nqubits))
    qc.mcx(controls, 0, ancilla_qubits=list(range(nqubits, nqubits + nancillas)), mode=mode)
    qc_transpiled = transpile(qc, target=target)
    assert len(qc_transpiled.data) == 1
    if mode == "noancilla":
        assert qc_transpiled.data[0].operation.name in {"mcx_gray", "mcx"}
    elif mode == "recursion":
        assert qc_transpiled.data[0].operation.name == "mcx_recursive"
    elif mode == "v-chain":
        assert qc_transpiled.data[0].operation.name == "mcx_vchain"


@pytest.mark.parametrize("num_controls", list(range(3, 6)))
def test_transpilation_preserves_mcp_target_gates(target: Target, num_controls: int) -> None:
    """Test that transpilation does not change MCP gates that are already in the target."""
    nqubits = num_controls + 1
    qc = QuantumCircuit(nqubits)
    controls = list(range(1, nqubits))
    qc.mcp(np.pi, controls, 0)
    qc_transpiled = transpile(qc, target=target)
    assert len(qc_transpiled.data) == 1
    assert qc_transpiled.data[0].operation.name == "mcphase"
