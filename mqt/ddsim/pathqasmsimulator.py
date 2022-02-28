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

from .job import DDSIMJob
from mqt import ddsim

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
    from mqt.ddsim import dump_tensor_network

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


def get_simulation_path(qc, max_time: int = 60, max_repeats: int = 1024, parallel_runs: int = 1, dump_path: bool = True,
                        plot_ring: bool = False):
    import cotengra as ctg
    from opt_einsum.paths import linear_to_ssa

    tn = create_tensor_network(qc)

    opt = ctg.HyperOptimizer(max_time=max_time,
                             max_repeats=max_repeats,
                             progbar=True,
                             parallel=parallel_runs,
                             minimize='flops')
    info = tn.contract(all, get='path-info', optimize=opt)
    path = linear_to_ssa(info.path)

    if dump_path:
        if isinstance(qc, QuantumCircuit):
            filename = qc.name + '_' + str(qc.num_qubits) + ".path"
        else:
            filename = 'simulation.path'
        with open(filename, 'w') as file:
            file.write(str(path))

    if plot_ring:
        fig = opt.get_tree().plot_ring(return_fig=True)
        fig.savefig("simulation_ring.svg", bbox_inches='tight')

    return path


class PathQasmSimulator(BackendV1):
    """Python interface to JKQ DDSIM Simulation Path Framework"""

    SHOW_STATE_VECTOR = False

    @classmethod
    def _default_options(cls) -> Options:
        return Options(
            shots=None,
            parameter_binds=None,
            simulator_seed=None,
            pathsim_configuration=ddsim.PathSimulatorConfiguration(),
            mode=None,
            bracket_size=None,
            alternating_start=None,
            seed=None,
            cotengra_max_time=60,
            cotengra_max_repeats=1024,
            cotengra_plot_ring=False,
            cotengra_dump_path=True
        )

    def __init__(self, configuration=None, provider=None):
        conf = {
            'backend_name': 'path_sim_qasm_simulator',
            'backend_version': ddsim.__version__,
            'url': 'https://github.com/cda-tum/ddsim',
            'simulator': True,
            'local': True,
            'description': 'MQT DDSIM C++ simulation path framework',
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

        pathsim_configuration = options.get('pathsim_configuration', ddsim.PathSimulatorConfiguration())

        mode = options.get('mode')
        if mode is not None:
            pathsim_configuration.mode = ddsim.PathSimulatorMode(mode)

        bracket_size = options.get('bracket_size')
        if bracket_size is not None:
            pathsim_configuration.bracket_size = bracket_size

        alternating_start = options.get('alternating_start')
        if alternating_start is not None:
            pathsim_configuration.alternating_start = alternating_start

        seed = options.get('seed')
        if seed is not None:
            pathsim_configuration.seed = seed

        sim = ddsim.PathCircuitSimulator(qobj_experiment, config=pathsim_configuration)

        # determine the contraction path using cotengra in case this is requested
        if pathsim_configuration.mode == ddsim.PathSimulatorMode.cotengra:
            max_time = options.get('cotengra_max_time', 60)
            max_repeats = options.get('cotengra_max_repeats', 1024)
            dump_path = options.get('cotengra_dump_path', False)
            plot_ring = options.get('cotengra_plot_ring', False)
            path = get_simulation_path(qobj_experiment, max_time=max_time, max_repeats=max_repeats,
                                       dump_path=dump_path, plot_ring=plot_ring)
            sim.set_simulation_path(path, False)

        shots = options.get('shots', 1024)
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
                  'config': pathsim_configuration,
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
