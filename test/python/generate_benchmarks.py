from qiskit import QuantumCircuit, QuantumRegister, ClassicalRegister, AncillaRegister
from qiskit.circuit.library import QFT, GraphState, GroverOperator
import numpy as np

# measure qubits in reverse order which is better suited for DD-based simulation
def measure(qc: QuantumCircuit, q: QuantumRegister, c: ClassicalRegister):
    for i in reversed(range(q.size)):
        qc.measure(q[i], c[i])


def ghz(n: int, include_measurements: bool = True):
    q = QuantumRegister(n, 'q')
    c = ClassicalRegister(n, 'c')
    qc = QuantumCircuit(q, c, name='ghz_state')
    qc.h(q[-1])
    for i in range(1, n):
        qc.cx(q[n - i], q[n - i - 1])
    if include_measurements:
        measure(qc, q, c)
    return qc


def qft(n: int, include_measurements: bool = True):
    q = QuantumRegister(n, 'q')
    c = ClassicalRegister(n, 'c')
    qc = QuantumCircuit(q, c, name='qft')
    qc.compose(QFT(num_qubits=n), inplace=True)
    if include_measurements:
        measure(qc, q, c)
    return qc


def qft_entangled(n: int, include_measurements: bool = True):
    qc = ghz(n, include_measurements=False)
    qc.compose(qft(n, include_measurements), inplace=True)
    qc.name = "qft_entangled"
    return qc


def grover(n: int, include_measurements: bool = True):
    from qiskit.algorithms import Grover

    q = QuantumRegister(n, 'q')
    flag = AncillaRegister(1, 'flag')
    c = ClassicalRegister(n)

    state_preparation = QuantumCircuit(q, flag)
    state_preparation.h(q)
    state_preparation.x(flag)

    oracle = QuantumCircuit(q, flag)
    oracle.mcp(np.pi, q, flag)

    operator = GroverOperator(oracle)
    iterations = Grover.optimal_num_iterations(1, n)

    qc = QuantumCircuit(q, flag, c, name='grover')
    qc.compose(state_preparation, inplace=True)

    qc.compose(operator.power(iterations), inplace=True)
    if include_measurements:
        measure(qc, q, c)

    return qc


def graph_state(n, include_measurements: bool = True):
    import networkx as nx

    q = QuantumRegister(n, 'q')
    c = ClassicalRegister(n, 'c')
    qc = QuantumCircuit(q, c, name="graph_state")

    G = nx.random_regular_graph(3, n)
    A = nx.convert_matrix.to_numpy_array(G)
    qc.compose(GraphState(A), inplace=True)
    if include_measurements:
        measure(qc, q, c)
    return qc


def w_state(n: int, include_measurements: bool = True):
    q = QuantumRegister(n, 'q')
    c = ClassicalRegister(n, 'c')
    qc = QuantumCircuit(q, c, name="w_state")

    def f_gate(qc: QuantumCircuit, q: QuantumRegister, i: int, j: int, n: int, k: int):
        theta = np.arccos(np.sqrt(1 / (n - k + 1)))
        qc.ry(-theta, q[j])
        qc.cz(q[i], q[j])
        qc.ry(theta, q[j])

    qc.x(q[-1])
    for l in range(1, n):
        f_gate(qc, q, n - l, n - l - 1, n, l)
    for l in reversed(range(1, n)):
        qc.cx(l - 1, l)

    if include_measurements:
        measure(qc, q, c)

    return qc


def shor(N: int, a: int = 2, include_measurements: bool = True):
    from qiskit.algorithms.factorizers import Shor
    qc = Shor().construct_circuit(N, a, include_measurements)
    qc.name = "shor_" + str(N) + '_' + str(a)
    return qc


def qpe_exact(n: int, include_measurements: bool = True):
    from fractions import Fraction
    import random

    q = QuantumRegister(n, 'q')
    psi = QuantumRegister(1, 'psi')
    c = ClassicalRegister(n + 1, 'c')
    qc = QuantumCircuit(q, psi, c, name="qpe_exact")

    # get random n-bit string as target phase
    theta = 0
    while theta == 0:
        theta = random.getrandbits(n)
    lam = Fraction(0, 1)
    # print("theta : ", theta, "correspond to", theta / (1 << n), "bin: ")
    for i in range(n):
        if theta & (1 << (n - i - 1)):
            lam += Fraction(1, (1 << i))

    qc.x(psi)
    qc.h(q)

    for i in range(n):
        angle = (lam * (1 << i)) % 2
        if angle > 1:
            angle -= 2
        if angle != 0:
            qc.cp(angle * np.pi, psi, q[i])

    qc.compose(QFT(num_qubits=n, inverse=True), inplace=True, qubits=list(range(n)))

    if include_measurements:
        measure(qc, q, c)

    return qc


def qpe_inexact(n: int, include_measurements: bool = True):
    from fractions import Fraction
    import random

    q = QuantumRegister(n, 'q')
    psi = QuantumRegister(1, 'psi')
    c = ClassicalRegister(n + 1, 'c')
    qc = QuantumCircuit(q, psi, c, name="qpe_inexact")

    # get random n+1-bit string as target phase
    theta = 0
    while theta == 0 or (theta & 1) == 0:
        theta = random.getrandbits(n + 1)
    lam = Fraction(0, 1)
    # print("theta : ", theta, "correspond to", theta / (1 << (n+1)), "bin: ")
    for i in range(n + 1):
        if theta & (1 << (n - i)):
            lam += Fraction(1, (1 << i))

    qc.x(psi)
    qc.h(q)

    for i in range(n):
        angle = (lam * (1 << i)) % 2
        if angle > 1:
            angle -= 2
        if angle != 0:
            qc.cp(angle * np.pi, psi, q[i])

    qc.compose(QFT(num_qubits=n, inverse=True), inplace=True, qubits=list(range(n)))

    if include_measurements:
        measure(qc, q, c)

    return qc
