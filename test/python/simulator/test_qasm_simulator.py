from __future__ import annotations

import numpy as np
import pytest
from qiskit import AncillaRegister, ClassicalRegister, QuantumCircuit, QuantumRegister, execute
from qiskit.circuit import Parameter

from mqt.ddsim.qasmsimulator import QasmSimulatorBackend


@pytest.fixture()
def backend() -> QasmSimulatorBackend:
    """The backend fixture for the tests in this file."""
    return QasmSimulatorBackend()


@pytest.fixture()
def circuit() -> QuantumCircuit:
    """The circuit fixture for the tests in this file."""
    qc = QuantumCircuit(name="test")
    q = QuantumRegister(3, "q")
    r = QuantumRegister(3, "r")
    qc.add_register(q)
    qc.add_register(r)
    qc.h(q)
    qc.cx(q, r)
    c = ClassicalRegister(3, "c")
    d = ClassicalRegister(3, "d")
    qc.add_register(c)
    qc.add_register(d)
    qc.barrier(q)
    qc.measure(q, c)
    qc.measure(r, d)
    return qc


@pytest.fixture()
def shots() -> int:
    """Number of shots for the tests in this file."""
    return 8192


def test_qasm_simulator_single_shot(circuit: QuantumCircuit, backend: QasmSimulatorBackend):
    """Test single shot run."""
    result = execute(circuit, backend, shots=1).result()
    assert result.success


def test_qasm_simulator(circuit: QuantumCircuit, backend: QasmSimulatorBackend, shots: int):
    """Test data counts output for single circuit run against reference."""
    result = execute(circuit, backend, shots=shots).result()
    assert result.success

    threshold = 0.04 * shots
    average = shots / 8
    counts = result.get_counts()
    target = {
        "100 100": average,
        "011 011": average,
        "101 101": average,
        "111 111": average,
        "000 000": average,
        "010 010": average,
        "110 110": average,
        "001 001": average,
    }
    assert len(target) == len(counts)
    for key in target:
        assert key in counts
        assert abs(target[key] - counts[key]) < threshold


def test_qasm_simulator_support_parametrized_gates(backend: QasmSimulatorBackend, shots: int):
    """Test backend's adequate support of parametrized gates"""

    theta_a = Parameter("theta_a")
    theta_b = Parameter("theta_b")
    theta_c = Parameter("theta_c")
    circuit_1 = QuantumCircuit(1)
    circuit_2 = QuantumCircuit(1)
    circuit_1.rx(theta_a, 0)
    circuit_2.rx(theta_b, 0)
    circuit_2.rz(theta_c, 0)
    circuit_2.rx(theta_b, 0)
    bare_circuit = QuantumCircuit(1)
    bare_circuit.h(0)

    with pytest.raises(
        ValueError,
        match=r"Mismatching number of values and parameters.*",
    ):
        backend.run([bare_circuit], [[np.pi]], shots=shots).result()

    with pytest.raises(
        ValueError, match=r"No parameter values provided although at least one parameterized circuit was supplied."
    ):
        backend.run([circuit_1, circuit_2], shots=shots).result()

    with pytest.raises(
        ValueError,
        match=r"The number of circuits \(2\) does not match the number of provided parameter sets \(1\)\.",
    ):
        backend.run([circuit_1, circuit_2], [[np.pi / 2]], shots=shots).result()

    # Test backend's correct functionality with multiple circuit
    result = backend.run([circuit_1, circuit_2], [[np.pi], [np.pi / 2, np.pi]], shots=shots).result()
    assert result.success

    counts_1 = result.get_counts(circuit_1)
    counts_2 = result.get_counts(circuit_2)

    assert counts_1 == {"1": shots}
    assert counts_2 == {"0": shots}


def test_qasm_simulator_approximation(backend: QasmSimulatorBackend, shots: int):
    """Test data counts output for single circuit run against reference."""
    circuit = QuantumCircuit(2)
    circuit.h(0)
    circuit.cx(0, 1)
    result = execute(circuit, backend, shots=shots, approximation_step_fidelity=0.4, approximation_steps=3).result()
    assert result.success
    counts = result.get_counts()
    assert len(counts) == 1


def test_qasm_simulator_access(backend: QasmSimulatorBackend, shots: int):
    """Test data counts output for multiple quantum circuits in a single job"""
    circuit_1 = QuantumCircuit(2, name="c1")
    circuit_2 = QuantumCircuit(2, name="c2")
    circuit_2.x(0)
    circuit_2.x(1)

    result = execute([circuit_1, circuit_2], backend, shots=shots).result()
    assert result.success

    counts_1 = result.get_counts(circuit_1.name)
    counts_2 = result.get_counts(circuit_2.name)

    assert counts_1 == {"0": shots}
    assert counts_2 == {"11": shots}


def test_qasm_simulator_portfolioqaoa(backend: QasmSimulatorBackend, shots: int):
    """Run simulator with with 2-target gates that take a parameter. Circuit taken from MQT Bench."""
    circuit = QuantumCircuit.from_qasm_str(
        """OPENQASM 2.0;
        include "qelib1.inc";
        qreg q[3];
        creg meas[3];
        u2(0.41951949,-pi) q[0];
        u2(0.41620669,-pi) q[1];
        rzz(-0.420917333908502) q[0],q[1];
        u2(0.41905329,-pi) q[2];
        rzz(-0.421016123405307) q[0],q[2];
        u3(2.2348228,1.2558831,-pi/2) q[0];
        rzz(-0.420940441831552) q[1],q[2];
        u3(2.2348228,1.2087537,-pi/2) q[1];
        rzz(-5.98815838177421) q[0],q[1];
        u3(2.2348228,1.2492507,-pi/2) q[2];
        rzz(-5.98956380537088) q[0],q[2];
        u3(0.56042125,-1.2358007,pi/2) q[0];
        rzz(-5.98848712542991) q[1],q[2];
        u3(0.56042125,-1.2880621,pi/2) q[1];
        rzz(-6.64023274758061) q[0],q[1];
        u3(0.56042125,-1.2431553,pi/2) q[2];
        rzz(-6.6417912133385) q[0],q[2];
        rx(-4.06512402388918) q[0];
        rzz(-6.64059728943955) q[1],q[2];
        rx(-4.06512402388918) q[1];
        rx(-4.06512402388918) q[2];
        barrier q[0],q[1],q[2];
        measure q[0] -> meas[0];
        measure q[1] -> meas[1];
        measure q[2] -> meas[2];
        """
    )
    result = execute(circuit, backend, shots=shots, seed_simulator=1337).result()
    assert result.success

    counts = result.get_counts()
    assert len(counts) == 8


@pytest.mark.parametrize("num_controls", list(range(1, 8)))
def test_qasm_simulator_mcx_no_ancilla(backend: QasmSimulatorBackend, num_controls: int, shots: int):
    """Test MCX gate with no ancilla qubits."""
    nqubits = num_controls + 1
    q = QuantumRegister(nqubits)
    c = ClassicalRegister(nqubits)
    circuit = QuantumCircuit(q, c)
    controls = q[1:nqubits]
    circuit.x(controls)
    circuit.mcx(controls, q[0], mode="noancilla")
    circuit.measure(q, c)

    print(backend.target.operation_names)

    result = execute(circuit, backend, shots=shots).result()
    assert result.success

    counts = result.get_counts()
    assert len(counts) == 1
    assert counts["1" * nqubits] == shots


@pytest.mark.parametrize("num_controls", list(range(1, 8)))
def test_qasm_simulator_mcx_recursion(backend: QasmSimulatorBackend, num_controls: int, shots: int):
    """Test MCX gate in recursion mode."""
    nqubits = num_controls + 1
    q = QuantumRegister(nqubits)
    c = ClassicalRegister(nqubits)
    anc = AncillaRegister(1)
    circuit = QuantumCircuit(q, c, anc)
    controls = q[1:nqubits]
    circuit.x(controls)
    circuit.mcx(controls, q[0], ancilla_qubits=anc, mode="recursion")
    circuit.measure(q, c)

    result = execute(circuit, backend, shots=shots).result()
    assert result.success

    counts = result.get_counts()
    assert len(counts) == 1
    assert counts["1" * nqubits] == shots


@pytest.mark.parametrize("num_controls", list(range(1, 8)))
def test_qasm_simulator_mcx_vchain(backend: QasmSimulatorBackend, num_controls: int, shots: int):
    """Test MCX gate in v-chain mode."""
    nqubits = num_controls + 1
    q = QuantumRegister(nqubits)
    c = ClassicalRegister(nqubits)
    anc = AncillaRegister(max(0, num_controls - 2))
    circuit = QuantumCircuit(q, c, anc)
    controls = q[1:nqubits]
    circuit.x(controls)
    circuit.mcx(controls, q[0], ancilla_qubits=anc, mode="v-chain")
    circuit.measure(q, c)

    result = execute(circuit, backend, shots=shots).result()
    assert result.success

    counts = result.get_counts()
    assert len(counts) == 1
    assert counts["1" * nqubits] == shots


@pytest.mark.parametrize("num_controls", list(range(1, 8)))
def test_qasm_simulator_mcp(backend: QasmSimulatorBackend, num_controls: int, shots: int):
    """Test MCPhase gate."""
    nqubits = num_controls + 1
    q = QuantumRegister(nqubits)
    c = ClassicalRegister(nqubits)
    circuit = QuantumCircuit(q, c)
    controls = q[1:nqubits]
    circuit.x(controls)
    circuit.h(q[0])
    circuit.mcp(np.pi, controls, q[0])
    circuit.h(q[0])
    circuit.measure(q, c)

    result = execute(circuit, backend, shots=shots).result()
    assert result.success

    counts = result.get_counts()
    assert len(counts) == 1
    assert counts["1" * nqubits] == shots
