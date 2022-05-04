"""Backend for DDSIM."""

import logging

from qiskit.providers.models import BackendConfiguration

from mqt import ddsim
from mqt.ddsim.pathqasmsimulator import PathQasmSimulator

logger = logging.getLogger(__name__)


class PathStatevectorSimulator(PathQasmSimulator):
    """Python interface to MQT DDSIM Simulation Path Framework"""

    SHOW_STATE_VECTOR = True

    def __init__(self, configuration=None, provider=None):
        conf = {
            'backend_name': 'path_sim_statevector_simulator',
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
                            'sx', 'csx', 'sxdg',
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

    def _validate(self, quantum_circuit):
        return
