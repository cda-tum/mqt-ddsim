import math
import unittest

from qiskit import ClassicalRegister, QuantumCircuit, QuantumRegister, execute
from qiskit.quantum_info import Statevector, state_fidelity

from mqt.ddsim.statevectorsimulator import StatevectorSimulatorBackend


class CircuitMultiRegsTest(unittest.TestCase):
    """QuantumCircuit Qasm tests."""

    def test_circuit_multi(self):
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

        result = execute(qc, backend_sim).result()
        counts = result.get_counts()

        target = {"01 10": 1024}

        result = execute(circ, backend_sim).result()
        state = result.get_statevector()

        assert counts == target
        assert math.isclose(state_fidelity(Statevector.from_label("0110"), state), 1.0, abs_tol=0.000001)
