"""Backend for DDSIM Unitary Simulator."""
from __future__ import annotations

import time
from typing import TYPE_CHECKING, Sequence

import numpy as np
import numpy.typing as npt
from qiskit import QiskitError, AncillaRegister, ClassicalRegister, QuantumCircuit, QuantumRegister, execute
from qiskit.providers import Options
from qiskit.result.models import ExperimentResult, ExperimentResultData
from qiskit.transpiler import Target


from mqt.ddsim.pyddsim import StochasticNoiseSimulator

from mqt.ddsim.target import DDSIMTargetBuilder

from mqt.ddsim.header import DDSIMHeader
from mqt.ddsim.qasmsimulator import QasmSimulatorBackend
from mqt.ddsim.pathqasmsimulator import PathQasmSimulatorBackend
from qiskit_aer import AerSimulator

if TYPE_CHECKING:
    from qiskit import QuantumCircuit


class StochasticNoiseSimulatorBackend(QasmSimulatorBackend):
    """Decision diagram-based unitary simulator."""

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

    def __init__(self, name="path_sim_qasm_simulator", description="MQT DDSIM Simulation Path Framework") -> None:
        super().__init__(name=name, description=description)

    @property
    def target(self):
        return self._PATH_TARGET

    def _run_experiment(self, qc: QuantumCircuit, **options) -> ExperimentResult:
        start_time = time.time()

        sim = StochasticNoiseSimulator(qc, noiseEffects="APD", noiseProbability=0.5)

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

        metadata = qc.metadata
        if metadata is None:
            metadata = {}

        return ExperimentResult(
            shots=shots,
            success=True,
            status="DONE",
            data=data,
            metadata=metadata,
            header=DDSIMHeader(qc),
        )



if __name__ == '__main__':
    shots = 100
    backend = StochasticNoiseSimulatorBackend()
    # backend = QasmSimulatorBackend()
    # backend = PathQasmSimulatorBackend()
    # backend = AerSimulator()


    c2 = ClassicalRegister(2, name="c2")
    q2 = QuantumRegister(2, name="q2")

    circuit_2 = QuantumCircuit(q2, c2, name="q2")

    circuit_2.x(q2[0])
    circuit_2.x(q2[1])
    circuit_2.measure(q2[0], c2[0])
    circuit_2.measure(q2[1], c2[1])

    result = execute(circuit_2, backend, shots=shots).result()



    # assert result.success
    #
    counts_2 = result.get_counts(circuit_2.name)
    print(counts_2)
    # assert counts_2 == {"11": shots}
