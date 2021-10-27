import datetime
import time
from jkq.ddsim.pathqasmsimulator import PathQasmSimulator, get_contraction_path
from jkq import ddsim
from qiskit import transpile

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


def create_transpile_map(basis_gates: list, basis_gates_elementary: list):
    number_of_newgates = []
    # create a sample quantum circuit, apply the gate to be deconstructed, apply the transpile operation
    # save the number of qubits and the name of the operation in the list
    for j in basis_gates:
        if j == 'cu3':
            testqc = QuantumCircuit(2, 2)
            testqc.cu(60, 10, 5, 2, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'mcx_gray' or j == 'mcx_recursive' or j == 'mcx_vchain' or j == 'mcx':
            testqc = QuantumCircuit(4, 4)
            testqc.mcx([0, 1, 2], 3)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'cy':
            testqc = QuantumCircuit(2, 2)
            testqc.cy(0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'cz':
            testqc = QuantumCircuit(2, 2)
            testqc.cz(0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'ch':
            testqc = QuantumCircuit(2, 2)
            testqc.ch(0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'sdg':
            testqc = QuantumCircuit(2, 2)
            testqc.sdg(1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'tdg':
            testqc = QuantumCircuit(2, 2)
            testqc.tdg(1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'crx':
            testqc = QuantumCircuit(2, 2)
            testqc.crx(60, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'mcrx':
            testqc = QuantumCircuit(2, 2)
            testqc.crx(60, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'cry':
            testqc = QuantumCircuit(2, 2)
            testqc.cry(60, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'mcry':
            testqc = QuantumCircuit(2, 2)
            testqc.cry(60, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'crz':
            testqc = QuantumCircuit(2, 2)
            testqc.crz(60, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'mcrz':
            testqc = QuantumCircuit(2, 2)
            testqc.crz(60, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'cp':
            testqc = QuantumCircuit(2, 2)
            testqc.cp(60, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'cu1':
            testqc = QuantumCircuit(2, 2)
            testqc.cp(60, 0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'mcphase':
            testqc = QuantumCircuit(4, 4)
            testqc.mcp(20, [0, 1, 2], 3)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'csx':
            testqc = QuantumCircuit(2, 2)
            testqc.csx(0, 1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        elif j == 'sxdg':
            testqc = QuantumCircuit(2, 2)
            testqc.sxdg(1)
            testqc_transpile = transpile(testqc, basis_gates=basis_gates_elementary, optimization_level=0)
            number_of_newgates.append((j, testqc_transpile.size()))
        else:
            number_of_newgates.append((j, 1))
    return number_of_newgates


def execute_verification(qc: QuantumCircuit, backend, shots: int, mode: str = 'sequential', **options):
    print('Starting execution of circuit', qc.name)
    # decide which gates you want to include in the transpiled circuit
    basis_gates_elementary = ['id', 'u0', 'u1', 'u2', 'u3',  # 'cu3',
                              'x', 'cx', 'ccx',  # 'mcx_gray', 'mcx_recursive', 'mcx_vchain', 'mcx',
                              'y',  # 'cy',
                              'z',  # 'cz',
                              'h',  # 'ch',
                              's',  # 'sdg',
                              't',  # 'tdg',
                              'rx',  # 'crx', 'mcrx',
                              'ry',  # 'cry', 'mcry',
                              'rz',  # 'crz', 'mcrz',
                              'p',  # 'cp', 'cu1',# 'mcphase',
                              'sx',  # 'csx',
                              # 'sxdg',
                              'swap', 'cswap', 'iswap',
                              'snapshot']

    configuration_dict = backend.configuration().to_dict()
    basis_gates = configuration_dict['basis_gates']
    print('Transpiling circuit')
    qc = transpile(qc, basis_gates=basis_gates, optimization_level=0)
    # Compose G G'^{-1}
    if mode == 'avgcase':
        qctranspile = transpile(qc, basis_gates=basis_gates_elementary, optimization_level=0)
        qcinv = qctranspile.inverse()
    else:
        qcinv = qc.inverse()
    qccomp = qc.compose(qcinv)

    print('Starting setup')
    start_time = time.time()
    task_based_mode = ddsim.PathMode.sequential
    if mode == 'cotengra':
        task_based_mode = ddsim.PathMode.cotengra
    elif mode == 'bestcase':
        task_based_mode = ddsim.PathMode.bestcase
    elif mode == 'avgcase':
        task_based_mode = ddsim.PathMode.avgcase

    sim = ddsim.PathCircuitSimulator(circ=qccomp, seed=-1, mode=task_based_mode, nthreads=1)

    if mode == 'bestcase':
        n_gates_1 = qc.size()
        n_gates_2 = qcinv.size()
        if n_gates_1 != n_gates_2:
            print('Warning: Circuits do not contain the same number of operations')

        # add first task to path
        path = [(n_gates_1, n_gates_1 + 1)]

        left_id = n_gates_1 - 1
        left_end = 0
        right_id = n_gates_1 + 2
        right_end = n_gates_1 + n_gates_2 + 1
        next_id = n_gates_1 + n_gates_2 + 1

        # alternate between applications from the left and the right
        while left_id != left_end and right_id != right_end:
            path.append((left_id, next_id))
            next_id += 1
            path.append((next_id, right_id))
            next_id += 1
            left_id -= 1
            right_id += 1

        # finish the left circuit
        while left_id != left_end:
            path.append((left_id, next_id))
            next_id += 1
            left_id -= 1

        # finish the right circuit
        while right_id != right_end:
            path.append((next_id, right_id))
            next_id += 1
            right_id += 1

        # add the remaining matrix-vector multiplication
        path.append((0, next_id))

        # set contraction path
        sim.set_contraction_path(path, True)
    elif mode == 'cotengra':
        max_time = options.get('cotengra_max_time', 60)
        max_repeats = options.get('cotengra_max_repeats', 1024)
        dump_path = options.get('cotengra_dump_path', False)
        plot_ring = options.get('cotengra_plot_ring', False)
        path = get_contraction_path(qccomp, max_time=max_time, max_repeats=max_repeats, dump_path=dump_path,
                                    plot_ring=plot_ring)
        sim.set_contraction_path(path, False)
    elif mode == 'avgcase':
        transpileMap = create_transpile_map(basis_gates, basis_gates_elementary)
        n_gates_1 = qc.size()
        n_gates_2 = qcinv.size()
        # add first task to path
        path = [(n_gates_1, n_gates_1 + 1)]
        # print("gates1 ", n_gates_1,"gates2 ",  n_gates_2)
        left_id = n_gates_1 - 1
        left_end = 0
        right_id = n_gates_1 + 2
        right_end = n_gates_1 + n_gates_2 + 1
        next_id = n_gates_1 + n_gates_2 + 1
        # go one step to the left and depending on the gate that much to the right
        while left_id != left_end and right_id != right_end:
            path.append((left_id, next_id))
            # print(left_id, next_id)
            next_id += 1
            index = [x[0] for x in transpileMap].index(
                qc[left_id][0].name)  # qobj_experiment.instructions[leftId].name)
            steps = transpileMap[index][1]
            for j in range(1, steps, 1):
                path.append((next_id, right_id))
                # print(next_id, right_id)
                right_id += 1
                next_id += 1
            left_id -= 1
        # finish the left circuit
        while left_id != left_end:
            path.append((left_id, next_id))
            # print(left_id, next_id)
            next_id += 1
            left_id -= 1

        # finish the right circuit
        while right_id != right_end:
            path.append((next_id, right_id))
            # print(next_id, right_id)
            next_id += 1
            right_id += 1
        path.append((0, next_id))
        # print(path)
        sim.set_contraction_path(path, True)

    setup_time = time.time()

    print('Simulating circuit')
    sim.simulate(shots)
    end_time = time.time()
    run_results = {'time_setup': setup_time - start_time, 'time_sim': end_time - setup_time}

    # print resulting csv string
    with open('results_verification.csv', 'a+') as file:
        file.write(';{};{};{};{};{};{}\n'.format(qc.name, qc.num_qubits, qc.size(), mode, run_results['time_setup'],
                                                 run_results['time_sim']))

    print(qc.name, qc.num_qubits, qc.size(), mode, run_results['time_setup'], run_results['time_sim'],
          sep=';')


def execute_verification_all(qc, backend, basis_gates, shots, include_cotengra, max_time, max_repeats, plot_ring):
    print('Transpiling circuit')
    qc = transpile(qc, basis_gates=basis_gates, optimization_level=0)
    execute_verification(qc, backend, shots)
    execute_verification(qc, backend, shots, 'bestcase')
    if include_cotengra:
        execute_verification(qc, backend, shots, 'cotengra', cotengra_max_time=max_time, cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)


if __name__ == '__main__':
    with open('results_verification.csv', 'a+') as file:
        file.write(datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S") + ';name;nqubits;ngates;mode;time_setup;time_sim\n')

    # settings
    backend = PathQasmSimulator()
    configuration_dict = backend.configuration().to_dict()
    basis_gates = configuration_dict['basis_gates']

    shots = 0
    max_time = 3600
    max_repeats = 256
    plot_ring = False

    # run benchmarks
    for n in [48, 64, 96, 128]:
        qc = ghz(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [48, 64, 96, 128]:
        qc = w_state(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [14, 15, 16, 17, 18, 19]:
        qc = qft_entangled(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [25, 26, 27, 28, 29]:
        qc = qpe_exact(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [15, 16, 17, 18]:
        qc = qpe_inexact(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [42, 44, 46, 48, 50, 52]:
        qc = graph_state(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [8, 9, 10]:
        qc = grover(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [24, 25, 26, 27, 28, 29, 30]:
        qc = grover(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=False, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for [i, j] in [[15, 7], [21, 2]]:
        qc = shor(i, j, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for [i, j] in [[35, 3], [65, 2], [129, 2]]:
        qc = shor(i, j, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=False, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    # # this most certainly fails due to the low PARAMETER_TOLERANCE configured in the QFR library
    for n in [16, 32, 48, 64]:
        qc = qft(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True, max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)
