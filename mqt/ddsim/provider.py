from __future__ import annotations

from qiskit.providers import ProviderV1
from qiskit.providers.providerutils import filter_backends

from mqt.ddsim.hybridqasmsimulator import HybridQasmSimulatorBackend
from mqt.ddsim.hybridstatevectorsimulator import HybridStatevectorSimulatorBackend
from mqt.ddsim.pathqasmsimulator import PathQasmSimulatorBackend
from mqt.ddsim.pathstatevectorsimulator import PathStatevectorSimulatorBackend
from mqt.ddsim.qasmsimulator import QasmSimulatorBackend
from mqt.ddsim.statevectorsimulator import StatevectorSimulatorBackend
from mqt.ddsim.unitarysimulator import UnitarySimulatorBackend


class DDSIMProvider(ProviderV1):
    _BACKENDS = (
        ("qasm_simulator", QasmSimulatorBackend),
        ("statevector_simulator", StatevectorSimulatorBackend),
        ("hybrid_qasm_simulator", HybridQasmSimulatorBackend),
        ("hybrid_statevector_simulator", HybridStatevectorSimulatorBackend),
        ("path_sim_qasm_simulator", PathQasmSimulatorBackend),
        ("path_sim_statevector_simulator", PathStatevectorSimulatorBackend),
        ("unitary_simulator", UnitarySimulatorBackend),
    )


    def get_backend(self, name=None, **kwargs):
        return super().get_backend(name=name, **kwargs)

    def backends(self, name=None, filters=None, **kwargs):
        backends = []
        for backend_name, backend_cls in self._BACKENDS:
            if name is None or backend_name == name:
                backends.append(backend_cls())
        return filter_backends(backends, filters=filters, **kwargs)

    def __str__(self) -> str:
        return "DDSIMProvider"
