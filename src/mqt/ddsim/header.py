# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Utilities for constructing a Qiskit experiment header for DDSIM backends."""

from __future__ import annotations

from collections import UserDict
from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    from qiskit import QuantumCircuit


class DDSIMHeader(UserDict[str, Any]):
    """Header for DDSIM backends."""

    @classmethod
    def from_quantum_circuit(cls, qc: QuantumCircuit) -> DDSIMHeader:
        """Create a DDSIM experiment header from a QuantumCircuit."""
        user_dict = {}
        user_dict["name"] = qc.name
        user_dict["n_qubits"] = qc.num_qubits
        user_dict["memory_slots"] = qc.num_clbits
        user_dict["global_phase"] = qc.global_phase
        user_dict["creg_sizes"] = [(creg.name, creg.size) for creg in qc.cregs]
        user_dict["clbit_labels"] = [(creg.name, j) for creg in qc.cregs for j in range(creg.size)]
        user_dict["qreg_sizes"] = [(qreg.name, qreg.size) for qreg in qc.qregs]
        user_dict["qubit_labels"] = [(qreg.name, j) for qreg in qc.qregs for j in range(qreg.size)]
        return cls(user_dict)
