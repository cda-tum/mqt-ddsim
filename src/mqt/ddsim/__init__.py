"""MQT DDSim Python Package."""

from __future__ import annotations

import sys

# under Windows, make sure to add the appropriate DLL directory to the PATH
if sys.platform == "win32":

    def _dll_patch() -> None:
        """Add the DLL directory to the PATH."""
        import os
        import sysconfig
        from pathlib import Path

        site_packages = Path(sysconfig.get_paths()["purelib"])
        bin_dir = site_packages / "mqt" / "core" / "bin"
        os.add_dll_directory(str(bin_dir))

    _dll_patch()
    del _dll_patch

from ._version import version as __version__
from .provider import DDSIMProvider
from .pyddsim import (
    CircuitSimulator,
    ConstructionMode,
    DeterministicNoiseSimulator,
    HybridCircuitSimulator,
    HybridMode,
    PathCircuitSimulator,
    PathSimulatorConfiguration,
    PathSimulatorMode,
    StochasticNoiseSimulator,
    UnitarySimulator,
    get_matrix,
)

__all__ = [
    "CircuitSimulator",
    "ConstructionMode",
    "DDSIMProvider",
    "DeterministicNoiseSimulator",
    "HybridCircuitSimulator",
    "HybridMode",
    "PathCircuitSimulator",
    "PathSimulatorConfiguration",
    "PathSimulatorMode",
    "StochasticNoiseSimulator",
    "UnitarySimulator",
    "__version__",
    "get_matrix",
]
