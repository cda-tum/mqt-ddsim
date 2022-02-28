"""Backend for DDSIM Hybrid Schrodinger-Feynman Simulator."""

import logging
import time
import uuid
from math import log2
import warnings
from typing import Union, List

from qiskit.providers import BackendV1, Options
from qiskit import QiskitError, QuantumCircuit
from qiskit.providers.models import BackendConfiguration, BackendStatus
from qiskit.qobj import QasmQobjExperiment, Qobj, QasmQobj, PulseQobj
from qiskit.result import Result
from qiskit.compiler import assemble
from qiskit.utils.multiprocessing import local_hardware_info

from .job import DDSIMJob
from .error import DDSIMError
from mqt import ddsim

logger = logging.getLogger(__name__)


class HybridQasmSimulator(BackendV1):
    """Python interface to MQT DDSIM Hybrid Schrodinger-Feynman Simulator"""

    SHOW_STATE_VECTOR = False

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            mode="amplitude",
            nthreads=local_hardware_info()['cpus']
        )

    def __init__(self, configuration=None, provider=None):
        conf = {
            'backend_name': 'hybrid_qasm_simulator',
            'backend_version': ddsim.__version__,
            'url': 'https://github.com/cda-tum/ddsim',
            'simulator': True,
            'local': True,
            'description': 'MQT DDSIM Hybrid Schrodinger-Feynman C++ simulator',
            'basis_gates': ['id', 'u0', 'u1', 'u2', 'u3', 'cu3',
                            'x', 'cx',
                            'y', 'cy',
                            'z', 'cz',
                            'h', 'ch',
                            's', 'sdg', 't', 'tdg',
                            'rx', 'crx',
                            'ry', 'cry',
                            'rz', 'crz',
                            'p', 'cp', 'cu1',
                            'sx', 'csx', 'sxdg',
                            # 'swap', 'cswap', 'iswap',
                            'snapshot'],
            'memory': False,
            'n_qubits': 128,
            'coupling_map': None,
            'conditional': False,
            'max_shots': 1000000000,
            'open_pulse': False,
            'gates': []
        }
        super().__init__(configuration=configuration or BackendConfiguration.from_dict(conf), provider=provider)

    def run(self, quantum_circuits: Union[QuantumCircuit, List[QuantumCircuit]], **options):
        if isinstance(quantum_circuits, QasmQobj) or isinstance(quantum_circuits, PulseQobj):
            raise QiskitError('QasmQobj and PulseQobj are not supported.')

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

        result = {'backend_name': self.configuration().backend_name,
                  'backend_version': self.configuration().backend_version,
                  'qobj_id': qobj_instance.qobj_id,
                  'job_id': job_id,
                  'results': result_list,
                  'status': 'COMPLETED',
                  'success': True,
                  'time_taken': (end - start),
                  'header': qobj_instance.header.to_dict()
                  }
        return Result.from_dict(result)

    def run_experiment(self, qobj_experiment: QasmQobjExperiment, **options):
        start_time = time.time()
        seed = options.get('seed', -1)
        mode = options.get('mode', 'amplitude')
        nthreads = int(options.get('nthreads', local_hardware_info()['cpus']))
        if mode == 'amplitude':
            hybrid_mode = ddsim.HybridMode.amplitude
            max_qubits = int(log2(local_hardware_info()['memory'] * (1024 ** 3) / 16))
            algorithm_qubits = qobj_experiment.header.n_qubits
            if algorithm_qubits > max_qubits:
                raise DDSIMError('Not enough memory available to simulate the circuit even on a single thread')
            qubit_diff = max_qubits - algorithm_qubits
            nthreads = int(min(2 ** qubit_diff, nthreads))
        elif mode == 'dd':
            hybrid_mode = ddsim.HybridMode.DD
        else:
            raise DDSIMError('Simulation mode', mode, 'not supported by MQT hybrid simulator. Available modes are \'amplitude\' and \'dd\'')

        sim = ddsim.HybridCircuitSimulator(qobj_experiment, seed, hybrid_mode, nthreads)

        shots = options.get('shots', 1024)
        if self.SHOW_STATE_VECTOR and shots > 0:
            logger.info('Statevector can only be shown if shots == 0 when using the amplitude hybrid simulation mode. Setting shots=0.')
            shots = 0

        counts = sim.simulate(shots)
        end_time = time.time()
        counts_hex = {hex(int(result, 2)): count for result, count in counts.items()}

        result = {'header': qobj_experiment.header.to_dict(),
                  'name': qobj_experiment.header.name,
                  'status': 'DONE',
                  'time_taken': end_time - start_time,
                  'seed': seed,
                  'mode': mode,
                  'nthreads': nthreads,
                  'shots': shots,
                  'data': {'counts': counts_hex},
                  'success': True,
                  }
        if self.SHOW_STATE_VECTOR:
            if sim.get_mode() == ddsim.HybridMode.DD:
                result['data']['statevector'] = sim.get_vector()
            else:
                result['data']['statevector'] = sim.get_final_amplitudes()

        return result

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
