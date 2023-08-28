"""Backend for DDSIM Hybrid Schrodinger-Feynman Simulator."""
from __future__ import annotations

import time
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from qiskit import QuantumCircuit

from qiskit import QiskitError
from qiskit.providers import Options
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target
from qiskit.utils.multiprocessing import local_hardware_info

from .header import DDSIMHeader
from .pyddsim import HybridCircuitSimulator, HybridMode
from .qasmsimulator import QasmSimulatorBackend
from .target import DDSIMTargetBuilder


class HybridQasmSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM Hybrid Schrodinger-Feynman Simulator."""

    _HSF_TARGET = Target(description="MQT DDSIM HSF Simulator Target", num_qubits=128)

    @staticmethod
    def _add_operations_to_target(target: Target) -> None:
        DDSIMTargetBuilder.add_0q_gates(target)
        DDSIMTargetBuilder.add_1q_gates(target)
        DDSIMTargetBuilder.add_2q_controlled_gates(target)
        DDSIMTargetBuilder.add_barrier(target)
        DDSIMTargetBuilder.add_measure(target)

    def __init__(self) -> None:
        super().__init__()
        self.name = "hybrid_qasm_simulator"
        self.description = ("MQT DDSIM Hybrid Schrodinger-Feynman simulator",)

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            mode="amplitude",
            nthreads=local_hardware_info()["cpus"],
        )

    @property
    def target(self):
        return self._HSF_TARGET

    def _run_experiment(self, qc: QuantumCircuit, **options) -> ExperimentResult:
        start_time = time.time()
        seed = options.get("seed", -1)
        mode = options.get("mode", "amplitude")
        nthreads = int(options.get("nthreads", local_hardware_info()["cpus"]))
        if mode == "amplitude":
            hybrid_mode = HybridMode.amplitude
            max_qubits = self.max_qubits()
            algorithm_qubits = qc.num_qubits
            if algorithm_qubits > max_qubits:
                msg = "Not enough memory available to simulate the circuit even on a single thread"
                raise QiskitError(msg)
            qubit_diff = max_qubits - algorithm_qubits
            nthreads = int(min(2**qubit_diff, nthreads))
        elif mode == "dd":
            hybrid_mode = HybridMode.DD
        else:
            msg = f"Simulation mode{mode} not supported by hybrid simulator. Available modes are 'amplitude' and 'dd'."
            raise QiskitError(msg)

        sim = HybridCircuitSimulator(qc, seed=seed, mode=hybrid_mode, nthreads=nthreads)

        shots = options.get("shots", 1024)
        if self._SHOW_STATE_VECTOR and shots > 0:
            print("Statevector can only be shown if shots == 0 when using the amplitude hybrid simulation mode.")
            shots = 0

        counts = sim.simulate(shots)
        end_time = time.time()

        data = ExperimentResultData(
            counts={hex(int(result, 2)): count for result, count in counts.items()},
            statevector=None
            if not self._SHOW_STATE_VECTOR
            else sim.get_vector()
            if sim.get_mode() == HybridMode.DD
            else sim.get_final_amplitudes(),
            time_taken=end_time - start_time,
            mode=mode,
            nthreads=nthreads,
        )

        metadata = qc.metadata
        if metadata is None:
            metadata = {}

        return ExperimentResult(
            shots=shots,
            success=True,
            status="DONE",
            seed=seed,
            data=data,
            metadata=metadata,
            header=DDSIMHeader(qc),
        )
