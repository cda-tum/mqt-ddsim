import datetime
from jkq.ddsim.taskbasedqasmsimulator import TaskBasedQasmSimulator
from qiskit import execute

from test.python.generate_benchmarks import *


def execute_circuit(qc: QuantumCircuit, backend, shots: int, mode: str = 'sequential', **options):
    print('Starting execution of circuit', qc.name)
    result = execute(qc, backend, shots=shots, mode=mode, optimization_level=0, **options).result()
    # counts = result.get_counts()
    # just for debugging
    # print("Counts:", counts)
    result_dict = result.to_dict()
    run_results = result_dict['results'][0]

    # print resulting csv string
    with open('results.csv', 'a+') as file:
        file.write(';{};{};{};{};{}\n'.format(qc.name, qc.num_qubits, mode, run_results['time_setup'], run_results['time_sim']))

    print(qc.name, qc.num_qubits, mode, run_results['time_taken'], run_results['time_setup'], run_results['time_sim'], sep=';')


if __name__ == '__main__':
    with open('results.csv', 'a+') as file:
        file.write(datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S") + ';name;nqubits;mode;time_setup;time_sim\n')

    # settings
    backend = TaskBasedQasmSimulator()
    shots = 1024
    max_time = 1800
    max_repeats = 1024
    plot_ring = False

    # run benchmarks
    for n in [48, 64, 96, 128]:
        qc = ghz(n, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    for n in [48, 64, 96, 128]:
        qc = w_state(n, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    for n in [46, 48, 50]:
        qc = graph_state(n, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    for n in [16, 32, 48, 64]:
        qc = qft(n, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    for n in [16, 17, 18]:
        qc = qft_entangled(n, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    for n in [26, 27, 28, 29]:
        qc = qpe_exact(n, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    for n in [16, 17, 18]:
        qc = qpe_inexact(n, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    for n in [8, 9, 10]:  # the interesting cases would be [27, 28, 29, 30], but those cannot be dumped to a TN efficiently yet
        qc = grover(n, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    for [i, j] in [[15, 7], [21, 2]]:  # the interesting cases would be [[35, 3], [65, 2], [129, 2], [259, 2]], but those require too much memory during contraction finding
        qc = shor(i, j, include_measurements=False)
        execute_circuit(qc, backend, shots)
        execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)
