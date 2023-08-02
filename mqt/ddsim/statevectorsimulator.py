"""Backend for DDSIM."""

import logging

from mqt.ddsim import __version__
from qiskit import QiskitError, QuantumCircuit
from mqt.ddsim.qasmsimulator import QasmSimulatorBackend

logger = logging.getLogger(__name__)


class StatevectorSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM"""

    SHOW_STATE_VECTOR = True

    def __init__(self, provider=None, name=None, description=None, online_date=None, backend_version=None):

        super().__init__(
            provider=provider,
            name= name,
            description= description,
            online_date= None,
            backend_version= backend_version,
        )
        
        
        self.name= "statevector_simulator"      
        self.description= "MQT DDSIM C++ simulator"
        self.backend_version= __version__

    def _validate(self, _quantum_circuit):
        return
        
