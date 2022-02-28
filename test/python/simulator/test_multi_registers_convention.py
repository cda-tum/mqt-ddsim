from qiskit import QuantumRegister, ClassicalRegister, QuantumCircuit
from qiskit import execute
from qiskit.quantum_info import state_fidelity, Statevector
from mqt.ddsim.statevectorsimulator import StatevectorSimulator

import unittest


class CircuitMultiRegsTest(unittest.TestCase):
    """QuantumCircuit Qasm tests."""

    def test_circuit_multi(self):
        """Test circuit multi regs declared at start.
        """
        qreg0 = QuantumRegister(2, 'q0')
        creg0 = ClassicalRegister(2, 'c0')

        qreg1 = QuantumRegister(2, 'q1')
        creg1 = ClassicalRegister(2, 'c1')

        circ = QuantumCircuit(qreg0, qreg1)
        circ.x(qreg0[1])
        circ.x(qreg1[0])

        meas = QuantumCircuit(qreg0, qreg1, creg0, creg1)
        meas.measure(qreg0, creg0)
        meas.measure(qreg1, creg1)

        qc = meas.compose(circ, front=True)

        backend_sim = StatevectorSimulator()

        result = execute(qc, backend_sim).result()
        counts = result.get_counts(qc)

        target = {'01 10': 1024}

        result = execute(circ, backend_sim).result()
        state = result.get_statevector(circ)

        self.assertEqual(counts, target)
        self.assertAlmostEqual(state_fidelity(Statevector.from_label('0110'), state), 1.0, places=7)
