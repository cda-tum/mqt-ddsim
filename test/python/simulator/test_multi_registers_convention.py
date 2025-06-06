# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import math

from mqt.ddsim.statevectorsimulator import StatevectorSimulatorBackend
from qiskit import ClassicalRegister, QuantumCircuit, QuantumRegister
from qiskit.quantum_info import Statevector, state_fidelity


def test_circuit_multi() -> None:
    """Test circuit multi regs declared at start."""
    qreg0 = QuantumRegister(2, "q0")
    creg0 = ClassicalRegister(2, "c0")

    qreg1 = QuantumRegister(2, "q1")
    creg1 = ClassicalRegister(2, "c1")

    circ = QuantumCircuit(qreg0, qreg1)
    circ.x(qreg0[1])
    circ.x(qreg1[0])

    meas = QuantumCircuit(qreg0, qreg1, creg0, creg1)
    meas.measure(qreg0, creg0)
    meas.measure(qreg1, creg1)

    qc = meas.compose(circ, front=True)

    backend_sim = StatevectorSimulatorBackend()

    result = backend_sim.run(qc).result()
    counts = result.get_counts(qc)

    target = {"01 10": 1024}

    result = backend_sim.run(circ).result()
    state = result.get_statevector()

    assert counts == target
    assert math.isclose(state_fidelity(Statevector.from_label("0110"), state), 1.0, abs_tol=0.000001)
