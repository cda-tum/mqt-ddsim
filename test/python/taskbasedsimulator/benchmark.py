import datetime
import time
from mqt.ddsim.pathqasmsimulator import PathQasmSimulator, get_simulation_path
from mqt import ddsim
from qiskit import transpile

from test.python.generate_benchmarks import *
from typing import Union


def execute_circuit(qc: QuantumCircuit, backend, shots: int, mode: Union[str, ddsim.PathSimulatorMode] = 'sequential', **options):
    print('Starting execution of circuit', qc.name)
    result = execute_verification(qc, backend, shots=shots, mode=mode, optimization_level=0, **options).result()
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


def execute_verification(qc: QuantumCircuit, backend, shots: int, mode: Union[str, ddsim.PathSimulatorMode] = 'sequential', **options):
    print('Starting execution of circuit', qc.name)
    configuration_dict = backend.configuration().to_dict()
    basis_gates = configuration_dict['basis_gates']
    print('Transpiling circuit')
    qc = transpile(qc, basis_gates=basis_gates, optimization_level=0)
    # Compose G G'^{-1}
    qcinv = qc.inverse()
    qccomp = qc.compose(qcinv)

    print('Starting setup')
    start_time = time.time()

    sim = ddsim.PathCircuitSimulator(circ=qccomp, mode=ddsim.PathSimulatorMode(mode))
    if mode == 'cotengra':
        max_time = options.get('cotengra_max_time', 60)
        max_repeats = options.get('cotengra_max_repeats', 1024)
        dump_path = options.get('cotengra_dump_path', False)
        plot_ring = options.get('cotengra_plot_ring', False)
        path = get_simulation_path(qccomp, max_time=max_time, max_repeats=max_repeats, dump_path=dump_path,
                                   plot_ring=plot_ring)
        sim.set_simulation_path(path, False)

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
    execute_verification(qc, backend, shots, 'alternating')
    if include_cotengra:
        execute_verification(qc, backend, shots, 'cotengra', cotengra_max_time=max_time,
                             cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)


if __name__ == '__main__':
    with open('results_verification.csv', 'a+') as file:
        file.write(
            datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S") + ';name;nqubits;ngates;mode;time_setup;time_sim\n')

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
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [48, 64, 96, 128]:
        qc = w_state(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [14, 15, 16, 17, 18, 19]:
        qc = qft_entangled(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [25, 26, 27, 28, 29]:
        qc = qpe_exact(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [15, 16, 17, 18]:
        qc = qpe_inexact(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [42, 44, 46, 48, 50, 52]:
        qc = graph_state(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [8, 9, 10]:
        qc = grover(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [24, 25, 26, 27, 28, 29, 30]:
        qc = grover(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=False,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for [i, j] in [[15, 7], [21, 2]]:
        qc = shor(i, j, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for [i, j] in [[35, 3], [65, 2], [129, 2]]:
        qc = shor(i, j, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=False,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    # # this most certainly fails due to the low PARAMETER_TOLERANCE configured in the QFR library
    for n in [16, 32, 48, 64]:
        qc = qft(n, include_measurements=False)
        execute_verification_all(qc=qc, backend=backend, basis_gates=basis_gates, shots=shots, include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)
