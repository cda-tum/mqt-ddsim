import datetime
import os.path
import time

from mqt.ddsim.pathqasmsimulator import PathQasmSimulator, get_simulation_path
from mqt import ddsim
from qiskit import *
from mqt.bench import get_one_benchmark
from pathlib import Path

from typing import Union


def execute_verification(qc: QuantumCircuit, qcog: QuantumCircuit, gatecost, backend, shots: int,
                         mode: Union[str, ddsim.PathSimulatorMode] = 'sequential', **options):
    print('Starting execution of circuit', qc.name)
    print('Starting setup')
    start_time = time.time()
    if mode == 'alternating':
        print('number of gates in the original circuit and starting point', qcog.size())
        alt_start = qcog.size()
        sim = ddsim.PathCircuitSimulator(circ=qc, mode=ddsim.PathSimulatorMode(mode), alternating_start=alt_start,
                                         gate_cost=gatecost)
    elif mode == 'gatecost':
        print('number of gates in the original circuit and starting point', qcog.size())
        alt_start = qcog.size()
        sim = ddsim.PathCircuitSimulator(circ=qc, mode=ddsim.PathSimulatorMode(mode), alternating_start=alt_start,
                                         gate_cost=gatecost)
    else:
        sim = ddsim.PathCircuitSimulator(circ=qc, mode=ddsim.PathSimulatorMode(mode),
                                         gate_cost=gatecost)  # ccomp changed to qc
    if mode == 'cotengra':
        max_time = options.get('cotengra_max_time', 60)
        max_repeats = options.get('cotengra_max_repeats', 1024)
        dump_path = options.get('cotengra_dump_path', False)
        plot_ring = options.get('cotengra_plot_ring', False)
        path = get_simulation_path(qc, max_time=max_time, max_repeats=max_repeats, dump_path=dump_path,
                                   plot_ring=plot_ring)  # qccomp changed to qc
        sim.set_simulation_path(path, False)

    setup_time = time.time()
    # print(qc.count_ops())
    print('Simulating circuit')
    results = sim.simulate(shots)
    print("results are in: ", results)
    end_time = time.time()
    run_results = {'time_setup': setup_time - start_time, 'time_sim': end_time - setup_time}

    # print resulting csv string
    p = Path(__file__).with_name('results_verification.csv')
    with p.open('a+') as file:
        file.write(';{};{};{};{};{};{}\n'.format(qc.name, qc.num_qubits, qc.size(), mode, run_results['time_setup'],
                                                 run_results['time_sim']))
    print(qc.name, qc.num_qubits, qc.size(), mode, run_results['time_setup'], run_results['time_sim'],
          sep=';')


def execute_verification_all(qc, qcog, gatecosts, backend, shots, include_cotengra, max_time, max_repeats, plot_ring):
    print('Running sequential')
    execute_verification(qc, qcog, gatecosts, backend, shots)
    # print('Running naive')
    # execute_verification(qc, qcog, gatecosts, backend, shots, 'alternating')
    print('Running gatecost')
    execute_verification(qc, qcog, gatecosts, backend, shots, 'gatecost')
    print('Running CoTenGra')
    if include_cotengra:
        execute_verification(qc, qcog, gatecosts, backend, shots, 'cotengra', cotengra_max_time=max_time,
                             cotengra_max_repeats=max_repeats, cotengra_plot_ring=plot_ring)


def generate_lookup_table(lookup_table):
    # loading the Gatecost LUT
    script_path = os.path.abspath(__file__)
    script_dir = os.path.split(script_path)[0]
    print(script_dir)
    rel_path = "qiskit_O2_nonancilla.profile"
    abs_file_path = os.path.join(script_dir, rel_path)
    with open(
            abs_file_path,
            'r') as f:
        for line in f.readlines():
            line = line.split(" ")
            lookup_table[line[0] + line[1]] = str(line[2][:-1])


if __name__ == '__main__':
    p = Path(__file__).with_name('results_verification.csv')
    with p.open('a+') as file:
        file.write(
            datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S") + ';name;nqubits;ngates;mode;time_setup;time_sim\n')

    # settings to create to versions of the same quantum circuit
    backend = PathQasmSimulator()
    configuration_dict = backend.configuration().to_dict()
    basis_gates = ["id", "rz", "sx", "x", "cx", "reset"]
    basis_gates_transpile = configuration_dict['basis_gates']

    # generating the lookup table
    lut_gatecost = {}
    generate_lookup_table(lut_gatecost)

    shots = 0
    max_time = 3600
    max_repeats = 256
    plot_ring = False
    # run benchmarks
    for n in [10, 11, 12, 13, 14]:
        gatecosts = []
        qc = get_one_benchmark("ae", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        # print(qc.draw(fold=-1))
        for i in range(qc.size()):
            if qc[i][0].name == 'u3':
                index = 'u' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'cx' or qc[i][0].name == 'mcx' or qc[i][0].name == 'mcx_gray':
                index = 'x' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'mcphase' or qc[i][0].name == 'cp':
                index = 'p' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "ae_" + str(n)
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qc=qccomp, qcog=qc, backend=backend, gatecosts=gatecosts, shots=shots,
                                 include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [14, 15, 16, 17, 18]:
        gatecosts = []
        qc = get_one_benchmark("qpeinexact", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        for i in range(qc.size()):
            if qc[i][0].name == 'cp':
                index = 'p' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "qpeinexacte_" + str(n)
        # execute_circuit(qccomp, backend, shots)
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qc=qccomp, qcog=qc, gatecosts=gatecosts, backend=backend, shots=shots,
                                 include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [46, 48, 50, 52, 54, 56]:
        gatecosts = []
        qc = get_one_benchmark("graphstate", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        for i in range(qc.size()):
            if qc[i][0].name == 'cz':
                index = 'z' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        # print(qctwo.draw())
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "graphstate_" + str(n)
        # print(qccomp.draw(fold=-1))
        # print(qccomp.size())
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qccomp, qc, gatecosts, backend, shots, include_cotengra=True, max_time=max_time,
                                 max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [64, 96, 128]:
        gatecosts = []
        qc = get_one_benchmark("ghz", "alg", n)
        qc.remove_final_measurements(inplace=True)
        for i in range(qc.size()):
            if qc[i][0].name == 'cx':
                index = 'x' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "ghz_" + str(n)
        # print(qccomp.size())
        # print(qccomp.draw(fold=-1))
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qccomp, qc, gatecosts, backend, shots, include_cotengra=True, max_time=max_time,
                                 max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [64, 96, 128]:
        gatecosts = []
        qc = get_one_benchmark("wstate", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        # print(qc.draw(fold=-1))
        for i in range(qc.size()):
            if qc[i][0].name == 'cz':
                index = 'z' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'cx':
                index = 'x' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "wstate_" + str(n)
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        # print(qccomp.draw(fold=-1))
        execute_verification_all(qccomp, qc, gatecosts, backend, shots, include_cotengra=True, max_time=max_time,
                                 max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [64, 96, 128]:
        gatecosts = []
        qc = get_one_benchmark("dj", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        # print(qc.draw(fold=-1))
        for i in range(qc.size()):
            if qc[i][0].name == 'u3':
                index = 'u' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'cx' or qc[i][0].name == 'mcx' or qc[i][0].name == 'mcx_gray':
                index = 'x' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'mcphase' or qc[i][0].name == 'cp':
                index = 'p' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "dj_" + str(n)
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qc=qccomp, qcog=qc, backend=backend, gatecosts=gatecosts, shots=shots,
                                 include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [14, 15, 16, 17, 18]:
        gatecosts = []
        qc = get_one_benchmark("qftentangled", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = qc.decompose()
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        for i in range(qc.size()):
            if qc[i][0].name == 'u2':
                index = 'u' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'cx':
                index = 'x' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'cp':
                index = 'p' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "qftentangled_" + str(n)
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qc=qccomp, qcog=qc, gatecosts=gatecosts, backend=backend, shots=shots,
                                 include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [10, 11, 12, 13, 14, 15, 16, 17]:
        gatecosts = []
        qc = get_one_benchmark("su2random", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        # print(qc.draw(fold=-1))
        for i in range(qc.size()):
            if qc[i][0].name == 'u3':
                index = 'u' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'cx' or qc[i][0].name == 'mcx' or qc[i][0].name == 'mcx_gray' or qc[i][
                0].name == 'ccx' or qc[i][0].name == 'mcx_vchain':
                index = 'x' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'mcphase' or qc[i][0].name == 'cp':
                index = 'p' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "su2random_" + str(n)
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qc=qccomp, qcog=qc, backend=backend, gatecosts=gatecosts, shots=shots,
                                 include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [10, 11, 12, 13, 14, 15, 16, 17]:
        gatecosts = []
        qc = get_one_benchmark("realamprandom", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        # print(qc.draw(fold=-1))
        for i in range(qc.size()):
            if qc[i][0].name == 'u3':
                index = 'u' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'cx' or qc[i][0].name == 'mcx' or qc[i][0].name == 'mcx_gray' or qc[i][
                0].name == 'ccx' or qc[i][0].name == 'mcx_vchain':
                index = 'x' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'mcphase' or qc[i][0].name == 'cp':
                index = 'p' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "realamprandom_" + str(n)
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qc=qccomp, qcog=qc, backend=backend, gatecosts=gatecosts, shots=shots,
                                 include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)

    for n in [14, 15, 16, 17]:
        gatecosts = []
        qc = get_one_benchmark("twolocalrandom", "alg", n)
        qc.remove_final_measurements(inplace=True)
        qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
        # print(qc.draw(fold=-1))
        for i in range(qc.size()):
            if qc[i][0].name == 'u3':
                index = 'u' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'cx' or qc[i][0].name == 'mcx' or qc[i][0].name == 'mcx_gray' or qc[i][
                0].name == 'ccx' or qc[i][0].name == 'mcx_vchain':
                index = 'x' + str(len(qc[i][1]) - 1)
            elif qc[i][0].name == 'mcphase' or qc[i][0].name == 'cp':
                index = 'p' + str(len(qc[i][1]) - 1)
            else:
                index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)
            # print(index)
            # print(lut_gatecost[index])
            gatecosts.append(lut_gatecost[index])
        # print(gatecosts)
        for i in range(0, len(gatecosts)):
            gatecosts[i] = int(gatecosts[i])
        qctwo = transpile(qc, basis_gates=basis_gates, optimization_level=2)
        qcinv = qctwo.inverse()
        qccomp = qc.compose(qcinv)
        qccomp.name = "twolocalrandom_" + str(n)
        # sim = ddsim.PathCircuitSimulator(qccomp)
        # result = sim.simulate(1000)
        # print(result)
        execute_verification_all(qc=qccomp, qcog=qc, backend=backend, gatecosts=gatecosts, shots=shots,
                                 include_cotengra=True,
                                 max_time=max_time, max_repeats=max_repeats, plot_ring=plot_ring)
