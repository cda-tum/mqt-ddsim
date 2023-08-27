"""Backend for DDSIM Unitary Simulator."""
from __future__ import annotations

import logging
import time
import uuid
import warnings
from math import log2, sqrt

import numpy as np
import numpy.typing as npt
from qiskit import QiskitError, QuantumCircuit
from qiskit.compiler import assemble
from qiskit.providers import BackendV1, Options
from qiskit.providers.models import QasmBackendConfiguration
from qiskit.qobj import PulseQobj, QasmQobj, QasmQobjExperiment, Qobj
from qiskit.result import Result
from qiskit.utils.multiprocessing import local_hardware_info

from . import __version__
from .error import DDSIMError
from .job import DDSIMJob
from .pyddsim import ConstructionMode, UnitarySimulator, get_matrix

logger = logging.getLogger(__name__)


class UnitarySimulatorBackend(BackendV1):
    """Decision diagram-based unitary simulator."""

    def __init__(self, configuration=None, provider=None, **fields) -> None:
        conf = {
            "backend_name": "unitary_simulator",
            "backend_version": __version__,
            "n_qubits": min(24, int(log2(sqrt(local_hardware_info()["memory"] * (1024**3) / 16)))),
            "url": "https://github.com/cda-tum/mqt-ddsim",
            "simulator": True,
            "local": True,
            "conditional": False,
            "open_pulse": False,
            "memory": False,
            "max_shots": 1000000000,
            "coupling_map": None,
            "description": "MQT DDSIM C++ Unitary Simulator",
            "basis_gates": [
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
                "xx_minus_yy",
                "xx_plus_yy",
            ],
            "gates": [],
        }
        super().__init__(
            configuration=(configuration or QasmBackendConfiguration.from_dict(conf)), provider=provider, **fields
        )

    @classmethod
    def _default_options(cls):
        return Options(shots=1, mode="recursive", parameter_binds=None)

    def run(self, quantum_circuits: QuantumCircuit | list[QuantumCircuit], **options):
        if isinstance(quantum_circuits, (QasmQobj, PulseQobj)):
            msg = "QasmQobj and PulseQobj are not supported."
            raise QiskitError(msg)

        if not isinstance(quantum_circuits, list):
            quantum_circuits = [quantum_circuits]

        out_options = {}
        for key in options:
            if not hasattr(self.options, key):
                warnings.warn("Option %s is not used by this backend" % key, UserWarning, stacklevel=2)
            else:
                out_options[key] = options[key]
        circuit_qobj = assemble(quantum_circuits, self, **out_options)

        job_id = str(uuid.uuid4())
        local_job = DDSIMJob(self, job_id, self._run_job, circuit_qobj, **options)
        local_job.submit()
        return local_job

    def _run_job(self, job_id, qobj_instance: Qobj, **options):
        self._validate(qobj_instance)

        start = time.time()
        result_list = [self.run_experiment(qobj_exp, **options) for qobj_exp in qobj_instance.experiments]
        end = time.time()

        result = {
            "backend_name": self.configuration().backend_name,
            "backend_version": self.configuration().backend_version,
            "qobj_id": qobj_instance.qobj_id,
            "job_id": job_id,
            "results": result_list,
            "status": "COMPLETED",
            "success": True,
            "time_taken": (end - start),
            "header": qobj_instance.header.to_dict(),
        }
        return Result.from_dict(result)

    def run_experiment(self, qobj_experiment: QasmQobjExperiment, **options):
        start = time.time()
        seed = options.get("seed", -1)
        mode = options.get("mode", "recursive")

        if mode == "sequential":
            construction_mode = ConstructionMode.sequential
        elif mode == "recursive":
            construction_mode = ConstructionMode.recursive
        else:
            msg = (
                f"Construction mode {mode} not supported by DDSIM unitary simulator. Available modes are "
                "'recursive' and 'sequential'"
            )
            raise DDSIMError(msg)

        sim = UnitarySimulator(qobj_experiment, seed=seed, mode=construction_mode)
        sim.construct()
        # Add extract resulting matrix from final DD and write data
        unitary: npt.NDArray[np.complex_] = np.zeros(
            (2**qobj_experiment.header.n_qubits, 2**qobj_experiment.header.n_qubits), dtype=np.complex_
        )
        get_matrix(sim, unitary)
        data = {
            "unitary": unitary,
            "construction_time": sim.get_construction_time(),
            "max_dd_nodes": sim.get_max_node_count(),
            "dd_nodes": sim.get_final_node_count(),
        }
        end = time.time()
        return {
            "name": qobj_experiment.header.name,
            "shots": 1,
            "data": data,
            "status": "DONE",
            "success": True,
            "time_taken": (end - start),
            "header": qobj_experiment.header.to_dict(),
        }

    def _validate(self, qobj):
        """Semantic validations of the qobj which cannot be done via schemas.
        Some of these may later move to backend schemas.
        1. No shots
        2. No measurements in the middle.
        """
        n_qubits = qobj.config.n_qubits
        max_qubits = self.configuration().n_qubits
        if n_qubits > max_qubits:
            msg = f"Number of qubits {n_qubits} is greater than maximum ({max_qubits}) for '{self.name()}'."
            raise DDSIMError(msg)
        if hasattr(qobj.config, "shots") and qobj.config.shots != 1:
            logger.info('"%s" only supports 1 shot. Setting shots=1.', self.name())
            qobj.config.shots = 1
        for experiment in qobj.experiments:
            name = experiment.header.name
            if getattr(experiment.config, "shots", 1) != 1:
                logger.info('"%s" only supports 1 shot. Setting shots=1 for circuit "%s".', self.name(), name)
                experiment.config.shots = 1
            for operation in experiment.instructions:
                if operation.name in ["measure", "reset"]:
                    msg = f"Unsupported '{self.name()}' instruction '{operation.name}' in circuit '{name}'."
                    raise DDSIMError(msg)
