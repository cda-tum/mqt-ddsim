"""Backend for DDSIM Density Matrix Simulator."""

from __future__ import annotations

import time
from typing import TYPE_CHECKING, Any, cast

from qiskit.providers import Options
from qiskit.result.models import ExperimentResult, ExperimentResultData

from mqt import ddsim

from .header import DDSIMHeader
from .qasmsimulator import QasmSimulatorBackend

if TYPE_CHECKING:
    from qiskit import QuantumCircuit


class DeterministicNoiseSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM deterministic noise-aware simulator."""

    def __init__(
        self,
        name: str = "dd_simulator_density_matrix",
        description: str = "MQT DDSIM noise-aware density matrix simulator based on decision diagrams",
    ) -> None:
        super().__init__(name=name, description=description)

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            noise_effects="APD",
            noise_probability=0.01,
            amp_damping_probability=0.02,
            multi_qubit_gate_factor=2,
        )

    @staticmethod
    def _run_experiment(qc: QuantumCircuit, **options: dict[str, Any]) -> ExperimentResult:
        start_time = time.time()
        noise_effects = cast(str, options.get("noise_effects", "APD"))
        noise_probability = cast(float, options.get("noise_probability", 0.01))
        amp_damping_probability = cast(float, options.get("amp_damping_probability", 0.02))
        multi_qubit_gate_factor = cast(float, options.get("multi_qubit_gate_factor", 2))
        seed = cast(int, options.get("simulator_seed", -1))
        shots = cast(int, options.get("shots", 1024))

        sim = ddsim.DeterministicNoiseSimulator(
            circ=qc,
            seed=seed,
            noise_effects=noise_effects,
            noise_probability=noise_probability,
            amp_damping_probability=amp_damping_probability,
            multi_qubit_gate_factor=multi_qubit_gate_factor,
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
