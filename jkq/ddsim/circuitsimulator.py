"""Backend for DDSIM."""

import logging
import time
import uuid

from qiskit.providers import BackendV1, BaseBackend, Options
from qiskit.providers.models import BackendConfiguration, BackendStatus
from qiskit.result import Result

from .jkqjob import JKQJob
from .jkqerror import JKQSimulatorError

from jkq.ddsim import pyddsim

logger = logging.getLogger(__name__)


class CircuitSimulator(BackendV1):
    """Python interface to JKQ DDSIM"""

    DEFAULT_CONFIGURATION = {
        'backend_name': 'qasm_simulator',
        'backend_version': '1.4.0',
        'url': 'https://github.com/iic-jku/ddsim',
        'simulator': True,
        'local': True,
        'description': 'JKQ DDSIM C++ simulator',
        'basis_gates': ['id', 'u0', 'u1', 'u2', 'u3', 'cx', 'x', 'y', 'z', 'h', 's', 't', 'snapshot'],
        'memory': False,
        'n_qubits': 64,
        'coupling_map': None,
        'conditional': False,
        'max_shots': 1000000,
        'open_pulse': False,
        'gates': [
            {
                'name': 'TODO',
                'parameters': [],
                'qasm_def': 'TODO'
            }
        ]
    }

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            return_statevector=True
        )

    def __init__(self, configuration=None, provider=None):
        conf = self.DEFAULT_CONFIGURATION.copy()
        if configuration:
            conf.update(configuration)
        super().__init__(configuration=BackendConfiguration.from_dict(conf), provider=provider)
        print(self.options)

    def run(self, quantum_circuit, **options):
        job_id = str(uuid.uuid4())
        local_job = JKQJob(self, job_id, self._run_job, quantum_circuit, **options)
        local_job.submit()
        return local_job

    def _run_job(self, job_id, quantum_circuit, **options):
        """Run circuits in q_job"""

        self._validate(quantum_circuit)

        start = time.time()
        result_list = [
            self.run_experiment(quantum_circuit, **options)
        ]
        end = time.time()
        result = {'backend_name': self._configuration.backend_name,
                  'backend_version': '1.4.0',
                  'qobj_id': '0',
                  'job_id': job_id,
                  'results': result_list,
                  'status': 'COMPLETED',
                  'success': True,
                  'time_taken': (end - start)}
        return Result.from_dict(result)

    def run_experiment(self, quantum_circuit, **options):
        print(options)
        start_time = time.time()

        sim = pyddsim.CircuitSimulator(quantum_circuit)
        counts = sim.simulate(options['shots'])

        end_time = time.time()

        result_dict = {'header': {'name': quantum_circuit.name},
                       'status': 'DONE',
                       'time_taken': end_time - start_time,
                       'seed': options['shots'],
                       'shots': 1,
                       'data': {
                           'counts': counts
                       },
                       'success': True
                       }
        if options.get('return_statevector', True):
            result_dict['data']['statevector'] = sim.get_vector()
        return result_dict

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
