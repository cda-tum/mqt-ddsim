"""Backend for DDSIM Hybrid Schrodinger-Feynman Simulator."""
from __future__ import annotations

import logging
from math import log2

from qiskit.utils.multiprocessing import local_hardware_info

from mqt.ddsim.hybridqasmsimulator import HybridQasmSimulatorBackend

logger = logging.getLogger(__name__)


class HybridStatevectorSimulatorBackend(HybridQasmSimulatorBackend):
    """Python interface to MQT DDSIM Hybrid Schrodinger-Feynman Simulator."""

    SHOW_STATE_VECTOR = True

    def __init__(self) -> None:
        super().__init__()
        self.name = "hybrid_statevector_simulator"
        self.description = "MQT DDSIM C++ simulator",
        self.TARGET.num_qubits= int(log2(local_hardware_info()["memory"] * (1024**3) / 16))
        
        


