from __future__ import annotations

from qiskit.providers import ProviderV1
from qiskit.providers.providerutils import filter_backends

from .hybridqasmsimulator import HybridQasmSimulatorBackend
from .hybridstatevectorsimulator import HybridStatevectorSimulatorBackend
from .pathqasmsimulator import PathQasmSimulatorBackend
from .pathstatevectorsimulator import PathStatevectorSimulatorBackend
from .qasmsimulator import QasmSimulatorBackend
from .statevectorsimulator import StatevectorSimulatorBackend
from .unitarysimulator import UnitarySimulatorBackend


class DDSIMProvider(ProviderV1):
    _BACKENDS = None

    def __init__(self) -> None:
        if DDSIMProvider._BACKENDS is None:
            DDSIMProvider._BACKENDS = [
                ("qasm_simulator", QasmSimulatorBackend, None, None),
                ("statevector_simulator", StatevectorSimulatorBackend, None, None),
                ("hybrid_qasm_simulator", HybridQasmSimulatorBackend, None, None),
                ("hybrid_statevector_simulator", HybridStatevectorSimulatorBackend, None, None),
                ("path_sim_qasm_simulator", PathQasmSimulatorBackend, None, None),
                ("path_sim_statevector_simulator", PathStatevectorSimulatorBackend, None, None),
                ("unitary_simulator", UnitarySimulatorBackend, None, None),
            ]

    def get_backend(self, name=None, **kwargs):
        return super().get_backend(name=name, **kwargs)

    def backends(self, name=None, filters=None, **kwargs):
        # pylint: disable=arguments-differ
        # Instantiate a new backend instance so if config options
        # are set they will only last as long as that backend object exists
        backends = []
        for backend_name, backend_cls, method, device in self._BACKENDS:
            opts = {"provider": self}
            if method is not None:
                opts["method"] = method
            if device is not None:
                opts["device"] = device
            if name is None or backend_name == name:
                backends.append(backend_cls(**opts))
        return filter_backends(backends, filters=filters, **kwargs)

    def __str__(self) -> str:
        return "DDSIMProvider"
