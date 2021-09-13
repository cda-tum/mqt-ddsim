import datetime
from jkq.ddsim.taskbasedqasmsimulator import TaskBasedQasmSimulator
from qiskit import execute, transpile

from test.python.generate_benchmarks import *


def execute_circuit(qc: QuantumCircuit, backend, shots: int, mode: str = 'sequential', **options):
    print('Starting execution of circuit', qc.name)
    result = execute(qc, backend, shots=shots, mode=mode, optimization_level=0, **options).result()
    counts = result.get_counts()
    # just for debugging
    print("Counts:", counts)
    result_dict = result.to_dict()
    run_results = result_dict['results'][0]

    # print resulting csv string
    with open('results_comparison.csv', 'a+') as file:
        file.write(';{};{};{};{};{}\n'.format(qc.name, qc.num_qubits, mode, run_results['time_setup'],
                                              run_results['time_sim']))

    print(qc.name, qc.num_qubits, mode, run_results['time_taken'], run_results['time_setup'], run_results['time_sim'],
          sep=';')


# todo write function to create the transpile map for two sets of gates
# def create_transpile_map(qc:: QuantumCircuit, basis_gates: list, basis_gates_elementary: list)


def execute_verification(qc: QuantumCircuit, backend, shots: int, mode: str = 'sequential', **options):
    print('Starting execution of circuit', qc.name)
    basis_gates = ['id', 'u0', 'u1', 'u2', 'u3', 'cu3',
                   'x', 'cx', 'ccx', 'mcx_gray', 'mcx_recursive', 'mcx_vchain', 'mcx',
                   'y', 'cy',
                   'z', 'cz',
                   'h', 'ch',
                   's', 'sdg', 't', 'tdg',
                   'rx', 'crx', 'mcrx',
                   'ry', 'cry', 'mcry',
                   'rz', 'crz', 'mcrz',
                   'p', 'cp', 'cu1', 'mcphase',
                   'sx', 'csx',
                   'sxdg',
                   'swap', 'cswap', 'iswap',
                   'snapshot']
    basis_gates_elementary = ['id', 'u0', 'u1', 'u2', 'u3',# 'cu3',
                   'x', 'cx', 'ccx',# 'mcx_gray', 'mcx_recursive', 'mcx_vchain', 'mcx',
                   'y',# 'cy',
                   'z',# 'cz',
                   'h',# 'ch',
                   's',# 'sdg', 't', 'tdg',
                   'rx',# 'crx', 'mcrx',
                   'ry',# 'cry', 'mcry',
                   'rz',# 'crz', 'mcrz',
                   'p',# 'cp', 'cu1',# 'mcphase',
                   'sx',# 'csx',
                   #'sxdg',
                   'swap', 'cswap', 'iswap',
                   'snapshot']

    number_of_newgates = []
    for j in basis_gates:
        if j =='crx':
            testqc = QuantumCircuit(4,4)
            testqc.crx(180,0,3)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
    print(number_of_newgates)
    breakpoint()

    qc=transpile(qc, basis_gates=basis_gates, optimization_level=0)
    if mode == 'avgcase':
        #print('transpiling now')
        qctranspile = transpile(qc, basis_gates=basis_gates_elementary, optimization_level=0)
        qcinv = qctranspile.inverse()
    else:
        qcinv = qc.inverse()
    qccomp = qc.compose(qcinv)
    result = execute(qccomp, backend, shots=shots, mode=mode, optimization_level=0, nGatesQC1=qc.size(),
                     nGatesQC2=qcinv.size(), **options).result()
    #counts = result.get_counts()
    # just for debugging
    #print("Counts:", counts)
    result_dict = result.to_dict()
    run_results = result_dict['results'][0]

    # print resulting csv string
    #with open('results_verification.csv', 'a+') as file:
     #   file.write(';{};{};{};{};{}\n'.format(qc.name, qc.num_qubits, mode, run_results['time_setup'],
                                           #   run_results['time_sim']))

    print(qc.name, qc.num_qubits, mode, run_results['time_taken'], run_results['time_setup'], run_results['time_sim'],
          sep=';')


if __name__ == '__main__':
    #with open('results_verification.csv', 'a+') as file:
     #   file.write(datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S") + ';name;nqubits;mode;time_setup;time_sim\n')

    # settings
    backend = TaskBasedQasmSimulator()
    shots = 1024
    max_time = 1800
    max_repeats = 1024
    plot_ring = False

    # run test benchmarks verification
    for n in [4]:
        qc = ghz(n, include_measurements=False)
        execute_verification(qc, backend, shots, 'bestcase')
        execute_verification(qc, backend, shots, 'avgcase')
        execute_verification(qc, backend, shots)

    # run benchmarks
    # for n in [48, 64, 96, 128]:
    #   qc = ghz(n, include_measurements=False)
    #  execute_verification(qc, backend, shots, 'bestcase')
    # execute_verification(qc, backend, shots)
    # execute_circuit(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    # for n in [48, 64, 96, 128]:
    #   qc = w_state(n, include_measurements=False)
    #  execute_verification(qc, backend, shots, 'bestcase')
    # execute_verification(qc, backend, shots)
    #  execute_circuit(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'bracket3')
    # execute_circuit(qc, backend, shots, 'bracket7')
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    # for n in [42, 44, 46, 48, 50, 52, 54]:
    #   qc = graph_state(n, include_measurements=False)
    #  execute_verification(qc, backend, shots, 'bestcase')
    # execute_verification(qc, backend, shots)
    #  execute_circuit(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)
    # execute_circuit(qc, backend, shots, 'bracket3')
    # execute_circuit(qc, backend, shots, 'bracket7')

    # for n in [16, 32, 48, 64]:
    #   qc = qft(n, include_measurements=False)
    #  execute_verification(qc, backend, shots, 'bestcase')
    # execute_verification(qc, backend, shots)
    #  execute_circuit(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    # for n in [14, 15, 16, 17, 18]:
    #   qc = qft_entangled(n, include_measurements=False)
    #  execute_verification(qc, backend, shots, 'bestcase')
    # execute_verification(qc, backend, shots)
    #  execute_circuit(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'bracket3')
    # execute_circuit(qc, backend, shots, 'bracket7')
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)
    # execute_circuit(qc,backend, shots, 'qftentangled')

    # for n in [26, 27, 28]:
    #   qc = qpe_exact(n, include_measurements=False)
    #  execute_verification(qc, backend, shots, 'bestcase')
    # execute_verification(qc, backend, shots)
    #  execute_circuit(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'bracket3')
    # execute_circuit(qc, backend, shots, 'bracket7')
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    # for n in [16, 17, 18]:
    #   qc = qpe_inexact(n, include_measurements=False)
    #  execute_circuit(qc, backend, shots)
    # execute_verification(qc, backend, shots, 'bestcase')
    # execute_verification(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'bracket3')
    # execute_circuit(qc, backend, shots, 'bracket7')
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)

    # for n in [22, 24]:  # the interesting cases would be [27, 28, 29, 30], but those cannot be dumped to a TN efficiently yet
    #   qc = grover(n, include_measurements=False)
    #  execute_verification(qc, backend, shots, 'bestcase')
    # execute_verification(qc, backend, shots)
    #  execute_circuit(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'grover')
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)
    # execute_circuit(qc, backend, shots, 'bracket3')
    # execute_circuit(qc, backend, shots, 'bracket7')

    #for [i, j] in [[15, 7], [21, 2], [35, 3], [65, 2], [129, 2]]:  # the interesting cases would be [[35, 3], [65, 2], [129, 2], [259, 2]], but those require too much memory during contraction finding
     #   qc = shor(i, j, include_measurements=False)
      #  execute_verification(qc, backend, shots, 'bestcase')
       # execute_verification(qc, backend, shots)
    #  execute_circuit(qc, backend, shots)
    # execute_circuit(qc, backend, shots, 'bracket3')
    # execute_circuit(qc, backend, shots, 'bracket7')
    # execute_circuit(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)
