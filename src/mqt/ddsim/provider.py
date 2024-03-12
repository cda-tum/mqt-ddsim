from __future__ import annotations

from typing import Any, Callable, cast

from qiskit.providers import BackendV2, ProviderV1
from qiskit.providers.providerutils import filter_backends

from .deterministicnoisesimulator import DeterministicNoiseSimulatorBackend
from .hybridqasmsimulator import HybridQasmSimulatorBackend
from .hybridstatevectorsimulator import HybridStatevectorSimulatorBackend
from .pathqasmsimulator import PathQasmSimulatorBackend
from .pathstatevectorsimulator import PathStatevectorSimulatorBackend
from .qasmsimulator import QasmSimulatorBackend
from .statevectorsimulator import StatevectorSimulatorBackend
from .stochasticnoisesimulator import StochasticNoiseSimulatorBackend
from .unitarysimulator import UnitarySimulatorBackend


class DDSIMProvider(ProviderV1):  # type: ignore[misc]
    _BACKENDS = (
        ("qasm_simulator", QasmSimulatorBackend),
        ("statevector_simulator", StatevectorSimulatorBackend),
        ("hybrid_qasm_simulator", HybridQasmSimulatorBackend),
        ("hybrid_statevector_simulator", HybridStatevectorSimulatorBackend),
        ("path_sim_qasm_simulator", PathQasmSimulatorBackend),
        ("path_sim_statevector_simulator", PathStatevectorSimulatorBackend),
        ("unitary_simulator", UnitarySimulatorBackend),
        ("stochastic_dd_simulator", StochasticNoiseSimulatorBackend),
        ("density_matrix_dd_simulator", DeterministicNoiseSimulatorBackend),
    )

    def get_backend(self, name: str | None = None, **kwargs: dict[str, Any]) -> BackendV2:
        return super().get_backend(name=name, **kwargs)

    def backends(
        self,
        name: str | None = None,
        filters: Callable[[list[BackendV2]], list[BackendV2]] | None = None,
        **kwargs: dict[str, Any],
    ) -> list[BackendV2]:
        backends = [
            backend_cls() for backend_name, backend_cls in self._BACKENDS if name is None or backend_name == name
        ]
        return cast(list[BackendV2], filter_backends(backends, filters=filters, **kwargs))

    def __str__(self) -> str:
        return "DDSIMProvider"
