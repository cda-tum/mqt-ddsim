import time
from jkq.ddsim.taskbasedqasmsimulator import TaskBasedQasmSimulator
from qiskit import execute

from test.python.generate_benchmarks import *


def execute_circuit(qc: QuantumCircuit, backend, shots: int, mode: str = 'sequential', **options):
    result = execute(qc, backend, shots=shots, mode=mode, **options).result()
    counts = result.get_counts()
    # just for debugging
    print("Counts:", counts)
    result_dict = result.to_dict()
    run_results = result_dict['results'][0]

    # print resulting csv string
    print(qc.name, qc.num_qubits, mode, run_results['time_taken'], run_results['time_setup'], run_results['time_sim'], sep=';')


if __name__ == '__main__':
    # settings
    backend = TaskBasedQasmSimulator()
    shots = 1024
    max_time = 60
    max_repeats = 1024
    plot_ring = True

    # create benchmarks
    benchmarks = []
    # for n in range(3, 129):
    #     benchmarks.append(ghz(n, include_measurements=False))
    # for n in range(3, 33):
    #     benchmarks.append(qft(n, include_measurements=False))
    # for n in range(3, 33):
    #     benchmarks.append(qft_entangled(n, include_measurements=False))
    # for n in range(4, 65, 2):
    #     benchmarks.append(graph_state(n, include_measurements=False))
    # for n in range(3, 129):
    #     benchmarks.append(w_state(n, include_measurements=False))
    # for n in range(2, 32):
    #     benchmarks.append(grover(n, include_measurements=False))
    # for [i, j] in [[15, 7], [21, 2], [35, 3], [51, 2], [65, 2], [129, 2]]:
    #     benchmarks.append(shor(i, j, include_measurements=False))
    # for n in range(3, 33):
    #     benchmarks.append(qpe_exact(n, include_measurements=False))
    # for n in range(3, 33):
    #     benchmarks.append(qpe_exact(n, include_measurements=False))

    for n in range(16, 17):
        benchmarks.append(qft_entangled(n, include_measurements=False))

    # run benchmarks
    for qc in benchmarks:
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'pairwise_recursive')
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)
