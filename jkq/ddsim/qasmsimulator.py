"""Backend for DDSIM."""

import logging
import time
import uuid
import warnings

from qiskit.providers import BackendV1, Options
from qiskit import qobj, QiskitError
from qiskit.providers.models import BackendConfiguration, BackendStatus
from qiskit.result import Result
from qiskit.compiler import assemble

from .jkqjob import JKQJob
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
            'backend_version': ddsim.__version__,
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

    def run(self, quantum_circuits, **options):
        if isinstance(quantum_circuits, qobj.QasmQobj) or isinstance(quantum_circuits, qobj.PulseQobj):
            raise QiskitError('QasmQobj and PulseQobj are not supported.')

        if not isinstance(quantum_circuits, list):
            quantum_circuits = [quantum_circuits]

        out_options = {}
        for key in options:
            if not hasattr(self.options, key):
                warnings.warn(
                    "Option %s is not used by this backend" % key, UserWarning, stacklevel=2
                )
            else:
                out_options[key] = options[key]
        circuit_qobj = assemble(quantum_circuits, self, **out_options)

        job_id = str(uuid.uuid4())
        local_job = JKQJob(self, job_id, self._run_job, quantum_circuits, circuit_qobj, **options)
        local_job.submit()
        return local_job

    def _run_job(self, job_id, quantum_circuits, qobj, **options):
        self._validate(qobj)

        start = time.time()
        result_list = []
        for circ, exp in zip(quantum_circuits, qobj.experiments):
            result_list.append(self.run_experiment(circ, exp, **options))
        end = time.time()
        result = {'backend_name': self.configuration().backend_name,
                  'backend_version': self.configuration().backend_version,
                  'qobj_id': qobj.qobj_id,
                  'job_id': job_id,
                  'results': result_list,
                  'status': 'COMPLETED',
                  'success': True,
                  'time_taken': (end - start),
                  'header': qobj.header.to_dict()
                  }
        return Result.from_dict(result)

    def run_experiment(self, circuit, experiment, **options):
        start_time = time.time()

        sim = ddsim.CircuitSimulator(circuit)
        counts = sim.simulate(options['shots'])
        end_time = time.time()
        counts_hex = {hex(int(result, 2)): count for result, count in counts.items()}
        return {'header': experiment.header.to_dict(),
                'name': experiment.header.name,
                'status': 'DONE',
                'time_taken': end_time - start_time,
                'seed': 1,
                'shots': options['shots'],
                'data': {'counts': counts_hex},
                'success': True,
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
