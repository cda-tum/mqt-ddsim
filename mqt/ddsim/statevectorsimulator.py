"""Backend for DDSIM."""

import logging

from qiskit.providers.models import BackendConfiguration

from mqt.ddsim import __version__
from mqt.ddsim.qasmsimulator import QasmSimulatorBackend

logger = logging.getLogger(__name__)


class StatevectorSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM"""

    SHOW_STATE_VECTOR = True

    def __init__(self, configuration=None, provider=None):
        conf = {
            "backend_name": "statevector_simulator",
            "backend_version": __version__,
            "url": "https://github.com/cda-tum/mqt-ddsim",
            "simulator": True,
            "local": True,
            "description": "MQT DDSIM C++ simulator",
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
                "snapshot",
            ],
            "memory": False,
            "n_qubits": 64,
            "coupling_map": None,
            "conditional": False,
            "max_shots": 1000000000,
            "open_pulse": False,
            "gates": [],
        }
        super().__init__(configuration=configuration or BackendConfiguration.from_dict(conf), provider=provider)

    def _validate(self, _quantum_circuit):
        return
