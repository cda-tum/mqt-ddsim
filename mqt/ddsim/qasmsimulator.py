"""QASM Simulator Backend for DDSIM."""

from __future__ import annotations

import time
import uuid
import warnings
from typing import Any

from qiskit import QuantumCircuit
from qiskit.circuit import Parameter
from qiskit.circuit.library import (
    MCMT,
    GlobalPhaseGate,
    MCPhaseGate,
    MCXGrayCode,
    MCXRecursive,
    MCXVChain,
    Measure,
    Reset,
    RXGate,
    RYGate,
    RZGate,
)
from qiskit.providers import BackendV2, Options
from qiskit.result import Result
from qiskit.transpiler import Target

from mqt.ddsim import CircuitSimulator, __version__
from mqt.ddsim.job import DDSIMJob

# Need to define new class for mcrx, mcry and mcrz. They cannot be found in qiskit.circuit.library


class MCRXGate(MCMT):
    def __init__(self, num_ctrl_qubits=None, theta=None):
        super().__init__(gate=RXGate(theta), num_ctrl_qubits=num_ctrl_qubits, num_target_qubits=1)
        QuantumCircuit.__init__(self)


class MCRYGate(MCMT):
    def __init__(self, num_ctrl_qubits=None, theta=None):
        super().__init__(gate=RYGate(theta), num_ctrl_qubits=num_ctrl_qubits, num_target_qubits=1)
        QuantumCircuit.__init__(self)


class MCRZGate(MCMT):
    def __init__(self, num_ctrl_qubits=None, theta=None):
        super().__init__(gate=RZGate(theta), num_ctrl_qubits=num_ctrl_qubits, num_target_qubits=1)
        QuantumCircuit.__init__(self)


class QasmSimulatorBackend(BackendV2):
    """Python interface to MQT DDSIM"""

    SHOW_STATE_VECTOR = False

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            approximation_step_fidelity=1.0,
            approximation_steps=0,
            approximation_strategy="fidelity",
        )

    def __init__(self):
        super().__init__(name="qasm_simulator", description="MQT DDSIM QASM Simulator", backend_version=__version__)

        custom_name_mapping_dict = {
            "gphase": GlobalPhaseGate(Parameter("ϴ")),
            "u0": GlobalPhaseGate(Parameter("ϴ")),
            "mcphase": MCPhaseGate,
            "mcx_gray": MCXGrayCode,
            "mcx_recursive": MCXRecursive,
            "mcx_vchain": MCXVChain,
            "mcrx": MCRXGate,
            "mcry": MCRYGate,
            "mcrz": MCRZGate,
            "reset": Reset,
        }

        self._target = Target.from_configuration(
            basis_gates=[
                "gphase",
                "id",
                "u0",
                "u1",
                "u2",
                "u3",
                "cu3",
                "x",
                "cx",
                "ccx",
                "mcx_gray",
                "mcx_recursive",
                "mcx_vchain",
                "y",
                "cy",
                "z",
                "cz",
                "h",
                "ch",
                "s",
                "sdg",
                "t",
                "tdg",
                "rx",
                "crx",
                "mcrx",
                "ry",
                "cry",
                "mcry",
                "rz",
                "crz",
                "mcrz",
                "p",
                "cp",
                "cu1",
                "mcphase",
                "sx",
                "csx",
                "sxdg",
                "swap",
                "cswap",
                "iswap",
                "dcx",
                "ecr",
                "rxx",
                "ryy",
                "rzz",
                "rzx",
                "reset",
                "xx_minus_yy",
                "xx_plus_yy",
            ],
            coupling_map=None,
            num_qubits=64,
            custom_name_mapping=custom_name_mapping_dict,
        )

        self.target.add_instruction(Measure())

    @property
    def target(self):
        return self._target

    @property
    def max_circuits(self):
        return None

    def run(self, quantum_circuits: QuantumCircuit | list[QuantumCircuit], **options) -> DDSIMJob:
        if not isinstance(quantum_circuits, list):
            quantum_circuits = [quantum_circuits]

        out_options = {}
        for key in options:
            if not hasattr(self.options, key):
                warnings.warn("Option %s is not used by this backend" % key, UserWarning, stacklevel=2)
            else:
                out_options[key] = options[key]

        job_id = str(uuid.uuid4())
        local_job = DDSIMJob(self, job_id, self._run_job, quantum_circuits, **options)
        local_job.submit()
        return local_job

    def _run_job(self, job_id: int, quantum_circuits: list[QuantumCircuit], **options) -> Result:
        start = time.time()
        result_list = [self.run_experiment(q_circ, **options) for q_circ in quantum_circuits]
        end = time.time()

        result = {
            "backend_name": self.name,
            "backend_version": self.backend_version,
            "qobj_id": None,
            "job_id": job_id,
            "results": result_list,
            "status": "COMPLETED",
            "success": True,
            "time_taken": (end - start),
            "header": {"backend_name": self.name, "backend_version": self.backend_version},
        }

        return Result.from_dict(result)

    def run_experiment(self, q_circ: QuantumCircuit, **options) -> dict[str, Any]:
        start_time = time.time()
        approximation_step_fidelity = options.get("approximation_step_fidelity", 1.0)
        approximation_steps = options.get("approximation_steps", 1)
        approximation_strategy = options.get("approximation_strategy", "fidelity")
        seed = options.get("seed", -1)

        sim = CircuitSimulator(
            q_circ,
            approximation_step_fidelity=approximation_step_fidelity,
            approximation_steps=approximation_steps,
            approximation_strategy=approximation_strategy,
            seed=seed,
        )
        counts = sim.simulate(options.get("shots", 1024))
        end_time = time.time()
        counts_hex = {hex(int(result, 2)): count for result, count in counts.items()}

        qubit_labels = []
        clbit_labels = []
        qreg_sizes = []
        creg_sizes = []

        for qreg in q_circ.qregs:
            qreg_sizes.append([qreg.name, qreg.size])
            for j in range(qreg.size):
                qubit_labels.append([qreg.name, j])

        for creg in q_circ.cregs:
            creg_sizes.append([creg.name, creg.size])
            for j in range(creg.size):
                clbit_labels.append([creg.name, j])

        metadata = q_circ.metadata
        if metadata is None:
            metadata = {}

        header_dict = {
            "clbit_labels": clbit_labels,
            "qubit_labels": qubit_labels,
            "creg_sizes": creg_sizes,
            "qreg_sizes": qreg_sizes,
            "n_qubits": q_circ.num_qubits,
            "memory_slots": q_circ.num_clbits,
            "name": q_circ.name,
            "global_phase": q_circ.global_phase,
            "metadata": metadata,
        }

        result = {
            "header": header_dict,
            "name": q_circ.name,
            "status": "DONE",
            "time_taken": end_time - start_time,
            "seed": options.get("seed", -1),
            "shots": options.get("shots", 1024),
            "data": {"counts": counts_hex},
            "success": True,
        }

        if self.SHOW_STATE_VECTOR:
            result["data"]["statevector"] = sim.get_vector()
        return result
