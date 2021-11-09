"""Backend for DDSIM Task-Based Simulator."""

import logging
import os

import time
import uuid
import warnings
from typing import Union, List

from qiskit.providers import BackendV1, Options
from qiskit import QiskitError, QuantumCircuit
from qiskit.providers.models import BackendConfiguration, BackendStatus
from qiskit.qobj import QasmQobjExperiment, Qobj, QasmQobj, PulseQobj
from qiskit.result import Result
from qiskit.compiler import assemble

from .jkqjob import JKQJob
from .jkqerror import JKQSimulatorError
from jkq import ddsim

logger = logging.getLogger(__name__)


def read_tensor_network_file(filename):
    import pandas as pd
    import numpy as np
    import quimb.tensor as qtn

    df = pd.read_json(filename)
    tensors = []

    for i in range(len(df['tensors'])):
        tens_data = [complex(real, imag) for [real, imag] in df['tensors'][i][3]]
        tens_shape = df['tensors'][i][2]
        data = np.array(tens_data).reshape(tens_shape)
        inds = df['tensors'][i][1]
        tags = df['tensors'][i][0]
        tensors.append(qtn.Tensor(data, inds, tags, left_inds=inds[:len(inds) // 2]))
    return tensors


def create_tensor_network(qc):
    import quimb.tensor as qtn
    import sparse
    from jkq.ddsim import dump_tensor_network

    if isinstance(qc, QuantumCircuit):
        filename = qc.name + '_' + str(qc.num_qubits) + ".tensor"
        nqubits = qc.num_qubits
    else:
        filename = 'tensor.tensor'
        nqubits = qc.header.n_qubits

    dump_tensor_network(qc, filename)
    tensors = read_tensor_network_file(filename)
    os.remove(filename)

    # add the zero state tensor |0...0> at the beginning
    shape = [2] * nqubits
    data = sparse.COO(coords=[[0]] * nqubits, data=1, shape=shape, sorted=True, has_duplicates=False)
    inds = ['q' + str(i) + '_0' for i in range(nqubits)]
    tags = ['Q' + str(i) for i in range(nqubits)]
    tensors.insert(0, qtn.Tensor(data=data, inds=inds, tags=tags))

    # using the following lines instead would allow much greater flexibility,
    # but is not supported for DD-based simulation at the moment

    # add the zero state tensor |0>...|0> at the beginning
    # shape = [2]
    # data = np.zeros(2)
    # data[0] = 1.0
    # for i in range(qc.num_qubits):
    #     inds = ['q'+str(i)+'_0']
    #     tags = ['Q'+str(i)]
    #     tensors.insert(0, qtn.Tensor(data=data.reshape(shape), inds=inds, tags=tags))

    return qtn.TensorNetwork(tensors)


def get_contraction_path(qc, max_time: int = 60, max_repeats: int = 1024, dump_path: bool = True,
                         plot_ring: bool = False):
    import cotengra as ctg
    from opt_einsum.paths import linear_to_ssa

    tn = create_tensor_network(qc)

    opt = ctg.HyperOptimizer(max_time=max_time,
                             max_repeats=max_repeats,
                             progbar=True,
                             parallel=4,
                             minimize='flops')
    info = tn.contract(all, get='path-info', optimize=opt)
    path = linear_to_ssa(info.path)

    if dump_path:
        if isinstance(qc, QuantumCircuit):
            filename = qc.name + '_' + str(qc.num_qubits) + ".path"
        else:
            filename = 'contraction.path'
        with open(filename, 'w') as file:
            file.write(str(path))

    if plot_ring:
        fig = opt.get_tree().plot_ring(return_fig=True)
        fig.savefig("contraction_ring.svg", bbox_inches='tight')

    return path


class PathQasmSimulator(BackendV1):
    """Python interface to JKQ DDSIM Task-Based Simulator"""

    SHOW_STATE_VECTOR = False

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            mode="sequential",
            nGatesQC1=0,
            nGatesQC2=0,
            cotengra_max_time=60,
            cotengra_max_repeats=1024,
            cotengra_plot_ring=False,
            cotengra_dump_path=True,
            nthreads=1  # local_hardware_info()['cpus']
        )

    def __init__(self, configuration=None, provider=None):
        conf = {
            'backend_name': 'path_sim_qasm_simulator',
            'backend_version': ddsim.__version__,
            'url': 'https://github.com/iic-jku/ddsim',
            'simulator': True,
            'local': True,
            'description': 'JKQ DDSIM C++ path simulator',
            'basis_gates': ['id', 'u0', 'u1', 'u2', 'u3', 'cu3',
                            'x', 'cx', 'ccx', 'mcx_gray', 'mcx_recursive', 'mcx_vchain', 'mcx',
                            'y', 'cy',
                            'z', 'cz',
                            'h', 'ch',
                            's', 'sdg', 't', 'tdg',
                            'rx', 'crx', 'mcrx',
                            'ry', 'cry', 'mcry',
                            'rz', 'crz', 'mcrz',
                            'p', 'cp', 'cu1', 'mcphase',
                            'sx', 'csx',
                            'sxdg',
                            'swap', 'cswap', 'iswap',
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
        local_job = JKQJob(self, job_id, self._run_job, circuit_qobj, **options)
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
        mode = options.get('mode', 'sequential')
        nthreads = int(options.get('nthreads', 1))
        # TODO: Add new strategies here
        if mode == 'sequential':
            task_based_mode = ddsim.PathSimulatorMode.sequential
        elif mode == 'pairwise_recursive':
            task_based_mode = ddsim.PathSimulatorMode.pairwise_recursive
        elif mode == 'cotengra':
            task_based_mode = ddsim.PathSimulatorMode.cotengra
        elif mode == 'bracket':
            task_based_mode = ddsim.PathSimulatorMode.bracket
        elif mode == 'alternating':
            task_based_mode = ddsim.PathSimulatorMode.alternating
        else:
            raise JKQSimulatorError('Simulation mode', mode,
                                    'not supported by JKQ path simulator. Available modes are \'sequential\', \'pairwise_recursive\', \'cotengra\', \'bracket\' and \'verification\'')

        sim = ddsim.PathCircuitSimulator(qobj_experiment, seed, task_based_mode, nthreads)

        # determine the best case contraction path for verification purposes
        if mode == 'alternating':
            n_gates_1 = options.get('nGatesQC1', 0)
            n_gates_2 = options.get('nGatesQC2', 0)
            if n_gates_1 != n_gates_2:
                print('Warning: Circuits do not contain the same number of operations')

            # add first task to path
            path = [(n_gates_1, n_gates_1 + 1)]

            left_id = n_gates_1 - 1
            left_end = 0
            right_id = n_gates_1 + 2
            right_end = n_gates_1 + n_gates_2 + 1
            next_id = n_gates_1 + n_gates_2 + 1

            # alternate between applications from the left and the right
            while left_id != left_end and right_id != right_end:
                path.append((left_id, next_id))
                next_id += 1
                path.append((next_id, right_id))
                next_id += 1
                left_id -= 1
                right_id += 1

            # finish the left circuit
            while left_id != left_end:
                path.append((left_id, next_id))
                next_id += 1
                left_id -= 1

            # finish the right circuit
            while right_id != right_end:
                path.append((next_id, right_id))
                next_id += 1
                right_id += 1

            # add the remaining matrix-vector multiplication
            path.append((0, next_id))

            # set contraction path
            sim.set_contraction_path(path, True)

        # determine the contraction path using cotengra in case this is requested
        if mode == 'cotengra':
            max_time = options.get('cotengra_max_time', 60)
            max_repeats = options.get('cotengra_max_repeats', 1024)
            dump_path = options.get('cotengra_dump_path', False)
            plot_ring = options.get('cotengra_plot_ring', False)
            path = get_contraction_path(qobj_experiment, max_time=max_time, max_repeats=max_repeats,
                                        dump_path=dump_path, plot_ring=plot_ring)
            sim.set_contraction_path(path, False)

        shots = options['shots']
        setup_time = time.time()
        counts = sim.simulate(shots)
        end_time = time.time()
        counts_hex = {hex(int(result, 2)): count for result, count in counts.items()}

        result = {'header': qobj_experiment.header.to_dict(),
                  'name': qobj_experiment.header.name,
                  'status': 'DONE',
                  'time_taken': end_time - start_time,
                  'time_setup': setup_time - start_time,
                  'time_sim': end_time - setup_time,
                  'seed': seed,
                  'mode': mode,
                  'nthreads': nthreads,
                  'shots': shots,
                  'data': {'counts': counts_hex},
                  'success': True,
                  }
        if self.SHOW_STATE_VECTOR:
            result['data']['statevector'] = sim.get_vector()

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
