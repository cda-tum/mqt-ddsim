"""Backend for DDSIM."""

import logging
import time
import uuid

from qiskit.providers import BackendV1, Options
from qiskit import qobj, QiskitError
from qiskit.providers.models import BackendConfiguration, BackendStatus
from qiskit.result import Result

from .jkqjob import JKQJob
from .jkqerror import JKQSimulatorError

from jkq import ddsim

logger = logging.getLogger(__name__)


class QasmSimulator(BackendV1):
    """Python interface to JKQ DDSIM"""

    @classmethod
    def _default_options(cls) -> Options:
        return Options(shots=None)

    def __init__(self, configuration=None, provider=None):
        conf = {
            'backend_name': 'qasm_simulator',
            'backend_version': '1.5.0',
            'url': 'https://github.com/iic-jku/ddsim',
            'simulator': True,
            'local': True,
            'description': 'JKQ DDSIM C++ simulator',
            'basis_gates': ['id', 'u0', 'u1', 'u2', 'u3', 'cu3',
                            'x', 'cx', 'ccx', 'mcx_gray', 'mcx_recursive', 'mcx_vchain',
                            'y', 'cy',
                            'z', 'cz',
                            'h', 'ch',
                            's', 'sdg', 't', 'tdg',
                            'rx', 'crx', 'mcrx',
                            'ry', 'cry', 'mcry',
                            'rz', 'crz', 'mcrz',
                            'p', 'cp', 'cu1', 'mcphase',
                            'sx', 'csx', 'sxdg',
                            'swap', 'cswap', 'iswap',
                            'snapshot'],
            'memory': False,
            'n_qubits': 64,
            'coupling_map': None,
            'conditional': False,
            'max_shots': 1000000000,
            'open_pulse': False,
            'gates': []
        }
        if configuration:
            conf.update(configuration)
        super().__init__(configuration=BackendConfiguration.from_dict(conf), provider=provider)

    def run(self, quantum_circuit, **options):
        if isinstance(quantum_circuit, qobj.QasmQobj) or isinstance(quantum_circuit, qobj.PulseQobj):
            raise QiskitError('QasmQobj and PulseQobj are not supported.')
        job_id = str(uuid.uuid4())
        local_job = JKQJob(self, job_id, self._run_job, quantum_circuit, **options)
        local_job.submit()
        return local_job

    def _run_job(self, job_id, experiments, **options):
        self._validate(experiments)

        if not isinstance(experiments, list):
            experiments = [experiments]

        start = time.time()
        result_list = [self.run_experiment(quantum_circuit, **options) for quantum_circuit in experiments]
        end = time.time()
        result = {'backend_name': self.configuration().backend_name,
                  'backend_version': self.configuration().backend_version,
                  'qobj_id': '0',
                  'job_id': job_id,
                  'results': result_list,
                  'status': 'COMPLETED',
                  'success': True,
                  'time_taken': (end - start)}
        return Result.from_dict(result)

    def run_experiment(self, quantum_circuit, **options):
        print(quantum_circuit)
        start_time = time.time()

        sim = ddsim.CircuitSimulator(quantum_circuit)
        counts = sim.simulate(options['shots'])
        end_time = time.time()

        return {'header': {'name': quantum_circuit.name},
                'name': quantum_circuit.name,
                'status': 'DONE',
                'time_taken': end_time - start_time,
                'seed': options['shots'],
                'shots': 1,
                'data': {'counts': counts},
                'success': True
                }

    def _validate(self, quantum_circuit):
        return

    def status(self):
        """Return backend status.
        Returns:
            BackendStatus: the status of the backend.
        """
        return BackendStatus(backend_name=self.name(),
                             backend_version=self.configuration().backend_version,
                             operational=True,
                             pending_jobs=0,
                             status_msg='')
