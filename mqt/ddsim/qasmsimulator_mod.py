"""Backend for DDSIM."""

import logging
import time
import uuid
import warnings
from typing import Dict, List, Union

from qiskit import QiskitError, QuantumCircuit
from qiskit.providers import BackendV2, Options
from qiskit.providers.models import BackendStatus
from qiskit.qobj import PulseQobj, QasmQobj
from qiskit.result import Result

from mqt.ddsim import CircuitSimulator
from mqt.ddsim.job import DDSIMJob

logger = logging.getLogger(__name__)


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

    def __init__(self, provider=None, name=None, description=None, online_date=None, backend_version=None):
        super().__init__(provider=provider, name=name, description=None, online_date=None, backend_version=None)

    def run(self, quantum_circuits: Union[QuantumCircuit, List[QuantumCircuit]], **options) -> DDSIMJob:
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

        job_id = str(uuid.uuid4())
        local_job = DDSIMJob(self, job_id, self._run_job, quantum_circuits, **options)
        local_job.submit()
        return local_job

    def _run_job(self, job_id, quantum_circuits: list, **options) -> Result:
        self._validate(quantum_circuits)

        start = time.time()
        result_list = [self.run_experiment(q_circ, **options) for q_circ in quantum_circuits]
        end = time.time()

        result = {
            "backend_name": self.name,
            "backend_version": self.backend_version,
            "qobj_id": None,  # Before it was  "qobj_id": qobj_instance.qobj_id   (Label of the Qobj)
            "job_id": job_id,
            "results": result_list,
            "status": "COMPLETED",
            "success": True,
            "time_taken": (end - start),
            "header": None,  # Before it was  "header": qobj_instance.header.to_dict()  (Info about the backend name and backend version)
        }
        return Result.from_dict(result)

    def run_experiment(self, q_circ: QuantumCircuit, **options) -> Dict:
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

        result = {
            "header": None,  # Before it was   "header": qobj_experiment.header.to_dict()   (Info about the gates and measurements in the circuit)
            "name": None,  # Before it was   "name": qobj_experiment.header.name          (Circuit's name)
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

    def _validate(self, _quantum_circuit):
        return

    def status(self) -> BackendStatus:
        """Return backend status.
        Returns:
            BackendStatus: the status of the backend.
        """
        return BackendStatus(
            backend_name=self.name,
            backend_version=self.backend_version,
            operational=True,
            pending_jobs=0,
            status_msg="",
        )
