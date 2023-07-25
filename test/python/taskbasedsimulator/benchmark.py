from __future__ import annotations

import datetime
import time
from pathlib import Path

from qiskit import QuantumCircuit, transpile

from mqt import ddsim
from mqt.bench import get_benchmark
from mqt.ddsim.pathqasmsimulator import PathQasmSimulatorBackend, get_simulation_path


def execute_circuit(
    qc: QuantumCircuit, backend, shots: int, mode: str | ddsim.PathSimulatorMode = "sequential", **options
):
    print("Starting execution of circuit", qc.name)
    result = execute_verification(qc, backend, shots=shots, mode=mode, optimization_level=0, **options).result()
    counts = result.get_counts()
    # just for debugging
    print("Counts:", counts)
    result_dict = result.to_dict()
    run_results = result_dict["results"][0]

    # print resulting csv string
    with Path("results_comparison.csv").open("a+") as file:
        file.write(
            ";{};{};{};{};{}\n".format(qc.name, qc.num_qubits, mode, run_results["time_setup"], run_results["time_sim"])
        )

    print(
        qc.name,
        qc.num_qubits,
        mode,
        run_results["time_taken"],
        run_results["time_setup"],
        run_results["time_sim"],
        sep=";",
    )


def execute_verification(
    qc: QuantumCircuit,
    qcog: QuantumCircuit,
    gatecost,
    backend,
    shots: int,
    mode: str | ddsim.PathSimulatorMode = "sequential",
    **options,
):
    print("Starting execution of circuit", qc.name)
    configuration_dict = backend.configuration().to_dict()
    basis_gates = configuration_dict["basis_gates"]
    print("Transpiling circuit")
    qc = transpile(qc, basis_gates=basis_gates, optimization_level=0)

    print("Starting setup")
    start_time = time.time()
    if mode in ("alternating", "gatecost"):
        print("number of gates in the original circuit and starting point", qcog.size())
        alt_start = qcog.size()
        sim = ddsim.PathCircuitSimulator(
            circ=qc, mode=ddsim.PathSimulatorMode(mode), starting_point=alt_start, gate_cost=gatecost
        )
    else:
        sim = ddsim.PathCircuitSimulator(
            circ=qc, mode=ddsim.PathSimulatorMode(mode), gate_cost=gatecost
        )  # ccomp changed to qc
    if mode == "cotengra":
        max_time = options.get("cotengra_max_time", 60)
        max_repeats = options.get("cotengra_max_repeats", 1024)
        dump_path = options.get("cotengra_dump_path", False)
        plot_ring = options.get("cotengra_plot_ring", False)
        path = get_simulation_path(
            qc, max_time=max_time, max_repeats=max_repeats, dump_path=dump_path, plot_ring=plot_ring
        )  # qccomp changed to qc
        sim.set_simulation_path(path, False)

    setup_time = time.time()
    # print(qc.count_ops())
    print("Simulating circuit")
    results = sim.simulate(shots)
    print("results are in: ", results)
    end_time = time.time()
    run_results = {"time_setup": setup_time - start_time, "time_sim": end_time - setup_time}

    # print resulting csv string
    p = Path(__file__).with_name("results_verification.csv")
    with p.open("a+") as file:
        file.write(
            ";{};{};{};{};{};{}\n".format(
                qc.name, qc.num_qubits, qc.size(), mode, run_results["time_setup"], run_results["time_sim"]
            )
        )
    print(qc.name, qc.num_qubits, qc.size(), mode, run_results["time_setup"], run_results["time_sim"], sep=";")


def execute_verification_all(
    qc: QuantumCircuit,
    qcog: QuantumCircuit,
    gatecosts,
    backend,
    shots,
    include_cotengra,
    max_time,
    max_repeats,
    plot_ring,
):
    print("Running sequential")
    execute_verification(qc, qcog, gatecosts, backend, shots)
    # print('Running naive')
    # execute_verification(qc, qcog, gatecosts, backend, shots, 'alternating')
    print("Running gatecost")
    execute_verification(qc, qcog, gatecosts, backend, shots, "gatecost")
    print("Running CoTenGra")
    if include_cotengra:
        execute_verification(
            qc,
            qcog,
            gatecosts,
            backend,
            shots,
            "cotengra",
            cotengra_max_time=max_time,
            cotengra_max_repeats=max_repeats,
            cotengra_plot_ring=plot_ring,
        )


def generate_lookup_table(profile_path: Path) -> dict:
    # loading the Gatecost LUT
    lookup_table = {}
    with profile_path.open("r") as f:
        for raw_line in f.readlines():
            line = raw_line.split(" ")
            lookup_table[line[0] + line[1]] = int(line[2][:-1])
    return lookup_table


def run_benchmark(
    benchmark_name: str,
    circuit_size: int,
    lut_gatecost: dict,
    backend,
    basis_gates_transpile: list,
    basis_gates_optimize: list,
    shots: int = 0,
    max_time: int = 3600,
    max_repeats: int = 256,
    plot_ring: bool = False,
):
    gatecosts = []
    qc = get_benchmark(benchmark_name, "alg", circuit_size)
    qc.remove_final_measurements(inplace=True)
    qc = transpile(qc, basis_gates=basis_gates_transpile, optimization_level=0)
    for i in range(qc.size()):
        if qc[i][0].name == "u2" or qc[i][0].name == "u3":  # noqa: PLR1714
            index = "u" + str(len(qc[i][1]) - 1)
        elif qc[i][0].name == "cx" or qc[i][0].name == "ccx" or qc[i][0].name.startswith("mcx"):
            index = "x" + str(len(qc[i][1]) - 1)
        elif qc[i][0].name == "mcphase" or qc[i][0].name == "cp":  # noqa: PLR1714
            index = "p" + str(len(qc[i][1]) - 1)
        elif qc[i][0].name == "cz":
            index = "z" + str(len(qc[i][1]) - 1)
        else:
            index = str(qc[i][0].name) + str(len(qc[i][1]) - 1)

        gatecosts.append(lut_gatecost[index])

    qctwo = transpile(qc, basis_gates=basis_gates_optimize, optimization_level=2)
    qcinv = qctwo.inverse()
    qccomp = qc.compose(qcinv)
    qccomp.name = f"{benchmark_name}_{circuit_size}"

    execute_verification_all(
        qc=qccomp,
        qcog=qc,
        gatecosts=gatecosts,
        backend=backend,
        shots=shots,
        include_cotengra=True,
        max_time=max_time,
        max_repeats=max_repeats,
        plot_ring=plot_ring,
    )


if __name__ == "__main__":
    p = Path(__file__).with_name("results_verification.csv")
    with p.open("a+") as file:
        file.write(
            datetime.datetime.now().strftime("%Y-%m-%d_%H:%M:%S") + ";name;nqubits;ngates;mode;time_setup;time_sim\n"
        )

    # settings to create to versions of the same quantum circuit
    backend = PathQasmSimulatorBackend()
    configuration_dict = backend.configuration().to_dict()
    basis_gates_transpile = configuration_dict["basis_gates"]
    basis_gates_optimize = ["id", "rz", "sx", "x", "cx", "reset"]

    # generating the lookup table
    profile_path = Path(__file__).with_name("qiskit_O2_nonancilla.profile").absolute()
    lut_gatecost = generate_lookup_table(profile_path)

    # run benchmarks
    benchmarks = {
        "ae": [10, 11, 12, 13, 14],
        "qpeinexact": [14, 15, 16, 17, 18],
        "graphstate": [46, 48, 50, 52, 54, 56],
        "ghz": [64, 96, 128],
        "wstate": [64, 96, 128],
        "dj": [64, 96, 128],
        "qftentangled": [14, 15, 16, 17, 18],
        "su2random": [10, 11, 12, 13, 14, 15, 16, 17],
        "realamprandom": [10, 11, 12, 13, 14, 15, 16, 17],
        "twolocalrandom": [14, 15, 16, 17],
    }

    for benchmark_name, qubit_counts in benchmarks.items():
        for qubits in qubit_counts:
            run_benchmark(benchmark_name, qubits, lut_gatecost, backend, basis_gates_transpile, basis_gates_optimize)
