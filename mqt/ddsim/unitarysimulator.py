"""Backend for DDSIM Unitary Simulator."""

import logging
import uuid
import time
from math import log2, sqrt
import warnings
from typing import Union, List

from qiskit.providers import BackendV1, Options
from qiskit import QiskitError, QuantumCircuit
from qiskit.providers.models import QasmBackendConfiguration
from qiskit.qobj import QasmQobjExperiment, Qobj, QasmQobj, PulseQobj
from qiskit.result import Result
from qiskit.compiler import assemble
from qiskit.utils.multiprocessing import local_hardware_info

from .job import DDSIMJob
from .error import DDSIMError
from mqt import ddsim

import numpy as np

logger = logging.getLogger(__name__)


class UnitarySimulator(BackendV1):
    """Decision diagram-based unitary simulator."""

    def __init__(self, configuration=None, provider=None, **fields):
        conf = {
            'backend_name': 'unitary_simulator',
            'backend_version': ddsim.__version__,
            'n_qubits': min(24, int(log2(sqrt(local_hardware_info()['memory'] * (1024 ** 3) / 16)))),
            'url': 'https://github.com/cda-tum/ddsim',
            'simulator': True,
            'local': True,
            'conditional': False,
            'open_pulse': False,
            'memory': False,
            'max_shots': 1000000000,
            'coupling_map': None,
            'description': 'MQT DDSIM C++ Unitary Simulator',
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
                            'swap', 'cswap', 'iswap'],
            'gates': []
        }
        super().__init__(configuration=(configuration or QasmBackendConfiguration.from_dict(conf)), provider=provider, **fields)

    @classmethod
    def _default_options(cls):
        return Options(shots=1,
                       mode='recursive',
                       parameter_binds=None)

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
        start = time.time()
        seed = options.get('seed', -1)
        mode = options.get('mode', 'recursive')

        if mode == 'sequential':
            construction_mode = ddsim.ConstructionMode.sequential
        elif mode == 'recursive':
            construction_mode = ddsim.ConstructionMode.recursive
        else:
            raise DDSIMError('Construction mode', mode, 'not supported by JKQ unitary simulator. Available modes are \'recursive\' and \'sequential\'')

        sim = ddsim.UnitarySimulator(qobj_experiment, seed, construction_mode)
        sim.construct()
        # Add extract resulting matrix from final DD and write data
        unitary = np.zeros((2 ** qobj_experiment.header.n_qubits, 2 ** qobj_experiment.header.n_qubits), dtype=complex)
        ddsim.get_matrix(sim, unitary)
        data = {
            'unitary': unitary,
            'construction_time': sim.get_construction_time(),
            'max_dd_nodes': sim.get_max_node_count(),
            'dd_nodes': sim.get_final_node_count()
        }
        end = time.time()
        return {'name': qobj_experiment.header.name,
                'shots': 1,
                'data': data,
                'status': 'DONE',
                'success': True,
                'time_taken': (end - start),
                'header': qobj_experiment.header.to_dict()}

    def _validate(self, qobj):
        """Semantic validations of the qobj which cannot be done via schemas.
        Some of these may later move to backend schemas.
        1. No shots
        2. No measurements in the middle
        """
        n_qubits = qobj.config.n_qubits
        max_qubits = self.configuration().n_qubits
        if n_qubits > max_qubits:
            raise DDSIMError('Number of qubits {} '.format(n_qubits) +
                                    'is greater than maximum ({}) '.format(max_qubits) +
                                    'for "{}".'.format(self.name()))
        if hasattr(qobj.config, 'shots') and qobj.config.shots != 1:
            logger.info('"%s" only supports 1 shot. Setting shots=1.',
                        self.name())
            qobj.config.shots = 1
        for experiment in qobj.experiments:
            name = experiment.header.name
            if getattr(experiment.config, 'shots', 1) != 1:
                logger.info('"%s" only supports 1 shot. '
                            'Setting shots=1 for circuit "%s".',
                            self.name(), name)
                experiment.config.shots = 1
            for operation in experiment.instructions:
                if operation.name in ['measure', 'reset']:
                    raise DDSIMError('Unsupported "%s" instruction "%s" ' +
                                            'in circuit "%s" ', self.name(),
                                            operation.name, name)
