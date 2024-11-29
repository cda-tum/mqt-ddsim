"""Provider for DDSIM backends."""

from __future__ import annotations

from typing import TYPE_CHECKING, Any, cast

from qiskit.providers.exceptions import QiskitBackendNotFoundError
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

if TYPE_CHECKING:
    from collections.abc import Callable

    from qiskit.providers import BackendV2


class DDSIMProvider:
    """Provider for DDSIM backends."""

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

    def get_backend(self, name: str | None = None, **kwargs: Any) -> BackendV2:
        """Return a backend matching the specified criteria.

        Args:
            name: Name of the backend.
            kwargs: Additional filtering criteria.
        """
        backends = self.backends(name, **kwargs)
        if len(backends) > 1:
            msg = "More than one backend matches the criteria"
            raise QiskitBackendNotFoundError(msg)
        if not backends:
            msg = "No backend matches the criteria"
            raise QiskitBackendNotFoundError(msg)

        return backends[0]

    def backends(
        self,
        name: str | None = None,
        filters: Callable[[list[BackendV2]], list[BackendV2]] | None = None,
        **kwargs: dict[str, Any],
    ) -> list[BackendV2]:
        """Return a list of backends matching the specified criteria.

        Args:
            name: Name of the backend.
            filters: Additional filtering criteria.
            kwargs: Additional filtering criteria.
        """
        backends = [
            backend_cls() for backend_name, backend_cls in self._BACKENDS if name is None or backend_name == name
        ]
        return cast("list[BackendV2]", filter_backends(backends, filters=filters, **kwargs))

    def __str__(self) -> str:
        """Return the provider name."""
        return "DDSIMProvider"
