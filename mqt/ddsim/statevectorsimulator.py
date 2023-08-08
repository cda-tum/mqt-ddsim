"""Backend for DDSIM."""

import logging

from mqt.ddsim import __version__
from mqt.ddsim.qasmsimulator import QasmSimulatorBackend


class StatevectorSimulatorBackend(QasmSimulatorBackend):
    """Python interface to MQT DDSIM"""

    SHOW_STATE_VECTOR = True

    def __init__(self, provider=None, name=None, description=None, online_date=None, backend_version=None):
        super().__init__(
            provider=provider,
            name=name or "statevector_simulator",
            description=description or "MQT DDSIM C++ simulator",
            online_date=online_date or None,
            backend_version=backend_version or __version__,
        )

    def _validate(self, _quantum_circuit):
        return
