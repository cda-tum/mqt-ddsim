import time
from jkq.ddsim.taskbasedqasmsimulator import TaskBasedQasmSimulator
from qiskit import execute

from test.python.generate_benchmarks import *


def execute_circuit(qc: QuantumCircuit, backend, shots: int, mode: str = 'sequential'):
    result = execute(qc, backend, shots=shots, mode=mode).result()
    counts = result.get_counts()
    # just for debugging
    print("Counts:", counts)
    result_dict = result.to_dict()

    # print resulting csv string
    print(qc.name, qc.num_qubits, mode, result_dict['time_taken'], sep=';')

    # sleep in order to reduce the effect of successive calculation
    time.sleep(1)


if __name__ == '__main__':
    backend = TaskBasedQasmSimulator()
    shots = 1024

    # uncomment any of the following loops to run the benchmarks

    for n in range(3, 17):
        qc = ghz(n)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'pairwise_recursive')

    # for n in range(3, 33):
    #     qc = qft(n)
    #     execute_circuit(qc, backend, shots)
    #     execute_circuit(qc, backend, shots, 'pairwise_recursive')

    # for n in range(3, 33):
    #     qc = qft_entangled(n)
    #     execute_circuit(qc, backend, shots)
    #     execute_circuit(qc, backend, shots, 'pairwise_recursive')

    # for n in range(4, 65, 2):
    #     qc = graph_state(n)
    #     execute_circuit(qc, backend, shots)
    #     execute_circuit(qc, backend, shots, 'pairwise_recursive')

    # for n in range(3, 129):
    #     qc = w_state(n)
    #     execute_circuit(qc, backend, shots)
    #     execute_circuit(qc, backend, shots, 'pairwise_recursive')

    # for n in range(2, 32):
    #     qc = grover(n)
    #     execute_circuit(qc, backend, shots)
    #     execute_circuit(qc, backend, shots, 'pairwise_recursive')

    # for [i, j] in [[15, 7], [21, 2], [35, 3], [51, 2], [65, 2], [129, 2]]:
    #     qc = shor(i, j)
    #     execute_circuit(qc, backend, shots)
    #     execute_circuit(qc, backend, shots, 'pairwise_recursive')

    # for n in range(1, 33):
    #     qc = qpe_exact(n)
    #     execute_circuit(qc, backend, shots)
    #     execute_circuit(qc, backend, shots, 'pairwise_recursive')

    # for n in range(1, 33):
    #     qc = qpe_inexact(n)
    #     execute_circuit(qc, backend, shots)
    #     execute_circuit(qc, backend, shots, 'pairwise_recursive')
