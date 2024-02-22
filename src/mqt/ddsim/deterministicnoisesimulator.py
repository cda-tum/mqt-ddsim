"""Backend for DDSIM Density Matrix Simulator."""

from __future__ import annotations

import time
from qiskit.transpiler import Target

from qiskit import QuantumCircuit
from qiskit.result.models import ExperimentResult, ExperimentResultData

from mqt import ddsim

from .header import DDSIMHeader
from .qasmsimulator import QasmSimulatorBackend
from .pyddsim import DeterministicNoiseSimulator

from typing import Any


class DeterministicNoiseSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM deterministic noise-aware simulator."""

    _SHOW_STATE_VECTOR = True
    _DNS_SV_TARGET = Target(
        description="MQT DDSIM decision diagram-based deterministic noise-aware simulator target",
        # num_qubits=QasmSimulatorBackend.max_qubits(),
    )

    def __init__(
            self,
            name="density_matrix_dd_simulator",
            description="MQT DDSIM noise-aware density matrix simulator based on decision diagrams",
    ) -> None:
        super().__init__(name=name, description=description)

    @property
    def target(self) -> Target:
        return self._DNS_SV_TARGET

    def _run_experiment(self, qc: QuantumCircuit, **options: dict[str, Any]) -> ExperimentResult:
        start_time = time.time()
        noise_effect = options.get("noise_effects", "APD")
        noise_probability = options.get("noise_probability", 0.01)
        amp_damping_probability = options.get("amp_damping_probability", 0.02)
        multi_qubit_gate_factor = options.get("multi_qubit_gate_factor", 2)
        seed = options.get("seed", -1)
        shots = options.get("shots", 1024)

        sim = ddsim.DeterministicNoiseSimulator(
            qc,
            noise_effect,
            noise_probability,
            amp_damping_probability,
            multi_qubit_gate_factor
        )

        counts = sim.simulate(shots=shots)
        end_time = time.time()

        data = ExperimentResultData(
            counts={hex(int(result, 2)): count for result, count in counts.items()},
            statevector=None,
            time_taken=end_time - start_time,
        )

        return ExperimentResult(
            shots=shots,
            success=True,
            status="DONE",
            seed=seed,
            data=data,
            metadata=qc.metadata,
            header=DDSIMHeader(qc),
        )
