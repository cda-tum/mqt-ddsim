"""Backend for DDSIM Task-Based Simulator."""

from __future__ import annotations

import pathlib
import time
from typing import TYPE_CHECKING, Any, cast

if TYPE_CHECKING:
    from qiskit import QuantumCircuit
    from quimb.tensor import Tensor, TensorNetwork

    from mqt.core.ir import QuantumComputation

import locale

from qiskit.providers import Options
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target

from mqt.core import load

from .header import DDSIMHeader
from .pyddsim import PathCircuitSimulator, PathSimulatorConfiguration, PathSimulatorMode
from .qasmsimulator import QasmSimulatorBackend
from .target import DDSIMTargetBuilder


def read_tensor_network_file(filename: str) -> list[Tensor]:
    """Read a tensor network from a file.

    Args:
        filename: The name of the file to read the tensor network from.

    Returns:
        The tensor network read from the file.
    """
    import numpy as np
    import pandas as pd
    import quimb.tensor as qtn

    df = pd.read_json(filename)
    tensors = []

    for i in range(len(df["tensors"])):
        tens_data = [complex(real, imag) for [real, imag] in df["tensors"][i][3]]
        tens_shape = df["tensors"][i][2]
        data = np.array(tens_data).reshape(tens_shape)
        inds = df["tensors"][i][1]
        tags = df["tensors"][i][0]
        tensors.append(qtn.Tensor(data, inds, tags, left_inds=inds[: len(inds) // 2]))
    return tensors


def create_tensor_network(qc: QuantumComputation) -> TensorNetwork:
    """Create a tensor network from a quantum circuit.

    Args:
        qc: The quantum circuit to be simulated.

    Returns:
        The tensor network representing the quantum circuit.
    """
    import quimb.tensor as qtn
    import sparse

    from mqt.ddsim import dump_tensor_network

    filename = qc.name + "_" + str(qc.num_qubits) + ".tensor"
    nqubits = qc.num_qubits

    dump_tensor_network(qc, filename)
    tensors = read_tensor_network_file(filename)
    pathlib.Path(filename).unlink()

    # add the zero state tensor |0...0> at the beginning
    shape = [2] * nqubits
    data = sparse.COO(coords=[[0]] * nqubits, data=1, shape=shape, sorted=True, has_duplicates=False)
    inds = ["q" + str(i) + "_0" for i in range(nqubits)]
    tags = ["Q" + str(i) for i in range(nqubits)]
    tensors.insert(0, qtn.Tensor(data=data, inds=inds, tags=tags))

    # using the following lines instead would allow much greater flexibility,
    # but is not supported for DD-based simulation at the moment

    # add the zero state tensor |0>...|0> at the beginning
    # shape = [2]
    # data = np.zeros(2)
    # data[0] = 1.0
    # for i in range(qc.num_qubits):
    #     inds = ['q'+str(i)+'_0']
    #     tags = ['Q'+str(i)]
    #     tensors.insert(0, qtn.Tensor(data=data.reshape(shape), inds=inds, tags=tags))

    return qtn.TensorNetwork(tensors)


def get_simulation_path(
    qc: QuantumComputation,
    max_time: int = 60,
    max_repeats: int = 1024,
    parallel_runs: int = 1,
    dump_path: bool = True,
    plot_ring: bool = False,
) -> list[tuple[int, int]]:
    """Determine a simulation path via computing a contraction path using cotengra.

    Args:
        qc: The quantum circuit to be simulated.
        max_time: The maximum time in seconds to spend on optimization.
        max_repeats: The maximum number of repetitions for optimization.
        parallel_runs: The number of parallel runs for optimization.
        dump_path: Whether to dump the path to a file.
        plot_ring: Whether to plot the contraction tree as a ring.

    Returns:
        The simulation path as a list of tuples.
    """
    import cotengra as ctg
    from opt_einsum.paths import linear_to_ssa

    tn = create_tensor_network(qc)

    opt = ctg.HyperOptimizer(
        max_time=max_time,
        max_repeats=max_repeats,
        progbar=True,
        parallel=parallel_runs,
        minimize="flops",
    )
    info = tn.contract(all, get="path-info", optimize=opt)
    path = cast("list[tuple[int, int]]", linear_to_ssa(info.path))

    if dump_path:
        filename = qc.name + "_" + str(qc.num_qubits) + ".path"
        with pathlib.Path(filename).open("w", encoding=locale.getpreferredencoding(False)) as file:
            file.write(str(path))

    if plot_ring:
        fig, _ = opt.get_tree().plot_ring()
        fig.savefig("simulation_ring.svg", bbox_inches="tight")

    return path


class PathQasmSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM Simulation Path Framework."""

    _PATH_TARGET = Target(description="MQT DDSIM Simulation Path Framework Target", num_qubits=128)

    @staticmethod
    def _add_operations_to_target(target: Target) -> None:
        DDSIMTargetBuilder.add_0q_gates(target)
        DDSIMTargetBuilder.add_1q_gates(target)
        DDSIMTargetBuilder.add_2q_gates(target)
        DDSIMTargetBuilder.add_3q_gates(target)
        DDSIMTargetBuilder.add_multi_qubit_gates(target)
        DDSIMTargetBuilder.add_barrier(target)
        DDSIMTargetBuilder.add_measure(target)

    def __init__(
        self,
        name: str = "path_sim_qasm_simulator",
        description: str = "MQT DDSIM Simulation Path Framework",
    ) -> None:
        """Constructor for the DDSIM Simulation Path Framework QASM Simulator backend.

        Args:
            name: The name of the backend.
            description: The description of the backend.
        """
        super().__init__(name=name, description=description)

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            pathsim_configuration=PathSimulatorConfiguration(),
            mode=None,
            bracket_size=None,
            alternating_start=None,
            gate_cost=None,
            seed=None,
            cotengra_max_time=60,
            cotengra_max_repeats=1024,
            cotengra_plot_ring=False,
            cotengra_dump_path=True,
        )

    @property
    def target(self) -> Target:
        """Return the target of the backend."""
        return self._PATH_TARGET

    def _run_experiment(self, qc: QuantumCircuit, **options: Any) -> ExperimentResult:
        start_time = time.time()

        pathsim_configuration = options.get("pathsim_configuration", PathSimulatorConfiguration())

        mode = options.get("mode")
        if mode is not None:
            pathsim_configuration.mode = PathSimulatorMode(mode)

        bracket_size = options.get("bracket_size")
        if bracket_size is not None:
            pathsim_configuration.bracket_size = bracket_size

        alternating_start = options.get("alternating_start")
        if alternating_start is not None:
            pathsim_configuration.alternating_start = alternating_start

        gate_cost = options.get("gate_cost")
        if gate_cost is not None:
            pathsim_configuration.gate_cost = gate_cost

        seed: int | None = options.get("seed")
        if seed is not None:
            pathsim_configuration.seed = seed

        circuit = load(qc)
        sim = PathCircuitSimulator(circuit, config=pathsim_configuration)

        # determine the contraction path using cotengra in case this is requested
        if pathsim_configuration.mode == PathSimulatorMode.cotengra:
            max_time = options.get("cotengra_max_time", 60)
            max_repeats = options.get("cotengra_max_repeats", 1024)
            dump_path = options.get("cotengra_dump_path", False)
            plot_ring = options.get("cotengra_plot_ring", False)
            path = get_simulation_path(
                circuit,
                max_time=max_time,
                max_repeats=max_repeats,
                dump_path=dump_path,
                plot_ring=plot_ring,
            )
            sim.set_simulation_path(path, False)

        shots = options.get("shots", 1024)
        setup_time = time.time()
        counts = sim.simulate(shots)
        end_time = time.time()

        data = ExperimentResultData(
            counts={hex(int(result, 2)): count for result, count in counts.items()},
            statevector=None if not self._SHOW_STATE_VECTOR else sim.get_vector(),
            time_taken=end_time - start_time,
            time_setup=setup_time - start_time,
            time_sim=end_time - setup_time,
        )

        return ExperimentResult(
            shots=shots,
            success=True,
            status="DONE",
            config=pathsim_configuration,
            seed=seed,
            data=data,
            metadata=qc.metadata,
            header=DDSIMHeader(qc),
        )
