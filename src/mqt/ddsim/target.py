"""Utilities for constructing a DDSIM target."""

from __future__ import annotations

import contextlib
from typing import TYPE_CHECKING

import qiskit.circuit.library as qcl
from qiskit.circuit import Parameter

if TYPE_CHECKING:
    from qiskit.transpiler import Target


class DDSIMTargetBuilder:
    @classmethod
    def add_0q_gates(cls, target: Target) -> None:
        with contextlib.suppress(AttributeError):
            target.add_instruction(qcl.GlobalPhaseGate(Parameter("phase")))

    @classmethod
    def add_1q_clifford_gates(cls, target: Target) -> None:
        target.add_instruction(qcl.IGate())
        target.add_instruction(qcl.XGate())
        target.add_instruction(qcl.YGate())
        target.add_instruction(qcl.ZGate())
        target.add_instruction(qcl.HGate())
        target.add_instruction(qcl.SGate())
        target.add_instruction(qcl.SdgGate())
        target.add_instruction(qcl.SXGate())
        target.add_instruction(qcl.SXdgGate())

    @classmethod
    def add_1q_gates(cls, target: Target) -> None:
        cls.add_1q_clifford_gates(target)

        theta = Parameter("theta")
        phi = Parameter("phi")
        lam = Parameter("lam")

        target.add_instruction(qcl.TGate())
        target.add_instruction(qcl.TdgGate())
        target.add_instruction(qcl.RXGate(theta))
        target.add_instruction(qcl.RYGate(theta))
        target.add_instruction(qcl.RZGate(phi))
        target.add_instruction(qcl.PhaseGate(theta))
        target.add_instruction(qcl.U1Gate(theta))
        target.add_instruction(qcl.U2Gate(phi, lam))
        target.add_instruction(qcl.U3Gate(theta, phi, lam))

    @classmethod
    def add_2q_controlled_clifford_gates(cls, target: Target) -> None:
        target.add_instruction(qcl.CXGate())
        target.add_instruction(qcl.CYGate())
        target.add_instruction(qcl.CZGate())

    @classmethod
    def add_2q_controlled_gates(cls, target: Target) -> None:
        cls.add_2q_controlled_clifford_gates(target)

        theta = Parameter("theta")
        phi = Parameter("phi")
        lam = Parameter("lam")

        target.add_instruction(qcl.CHGate())
        target.add_instruction(qcl.CSXGate())
        target.add_instruction(qcl.CRXGate(theta))
        target.add_instruction(qcl.CRYGate(theta))
        target.add_instruction(qcl.CRZGate(theta))
        target.add_instruction(qcl.CPhaseGate(theta))
        target.add_instruction(qcl.CU1Gate(theta))
        target.add_instruction(qcl.CU3Gate(theta, phi, lam))
        with contextlib.suppress(AttributeError):
            target.add_instruction(qcl.CSGate())
            target.add_instruction(qcl.CSdgGate())

    @classmethod
    def add_2q_non_controlled_clifford_gates(cls, target: Target) -> None:
        target.add_instruction(qcl.SwapGate())
        target.add_instruction(qcl.iSwapGate())
        target.add_instruction(qcl.DCXGate())
        target.add_instruction(qcl.ECRGate())

    @classmethod
    def add_2q_non_controlled_gates(cls, target: Target) -> None:
        cls.add_2q_non_controlled_clifford_gates(target)

        beta = Parameter("beta")
        theta = Parameter("theta")

        target.add_instruction(qcl.RXXGate(theta))
        target.add_instruction(qcl.RYYGate(theta))
        target.add_instruction(qcl.RZZGate(theta))
        target.add_instruction(qcl.RZXGate(theta))
        target.add_instruction(qcl.XXMinusYYGate(theta, beta))
        target.add_instruction(qcl.XXPlusYYGate(theta, beta))

    @classmethod
    def add_2q_gates(cls, target: Target) -> None:
        cls.add_2q_controlled_gates(target)
        cls.add_2q_non_controlled_gates(target)

    @classmethod
    def add_3q_gates(cls, target: Target) -> None:
        target.add_instruction(qcl.CCXGate())
        target.add_instruction(qcl.CSwapGate())
        with contextlib.suppress(AttributeError):
            target.add_instruction(qcl.CCZGate())

    @classmethod
    def add_multi_qubit_gates(cls, target: Target) -> None:
        target.add_instruction(qcl.MCXGate, name="mcx")
        target.add_instruction(qcl.MCXGrayCode, name="mcx_gray")
        target.add_instruction(qcl.MCXRecursive, name="mcx_recursive")
        target.add_instruction(qcl.MCXVChain, name="mcx_vchain")
        target.add_instruction(qcl.MCPhaseGate, name="mcphase")
        target.add_instruction(qcl.MCU1Gate, name="mcu1")

    @classmethod
    def add_measure(cls, target: Target) -> None:
        target.add_instruction(qcl.Measure())

    @classmethod
    def add_reset(cls, target: Target) -> None:
        target.add_instruction(qcl.Reset())

    @classmethod
    def add_non_unitary_operations(cls, target: Target) -> None:
        cls.add_measure(target)
        cls.add_reset(target)

    @classmethod
    def add_barrier(cls, target: Target) -> None:
        target.add_instruction(qcl.Barrier, name="barrier")
