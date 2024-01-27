from __future__ import annotations

import sys

# under Windows, make sure to add the appropriate DLL directory to the PATH
if sys.platform == "win32":  # pragma: no cover
    import os
    import sysconfig
    from pathlib import Path

    bin_dir = Path(sysconfig.get_paths()["purelib"]) / "mqt" / "core" / "bin"
    os.add_dll_directory(str(bin_dir))

from ._version import version as __version__
from .provider import DDSIMProvider
from .pyddsim import (
    CircuitSimulator,
    ConstructionMode,
    HybridCircuitSimulator,
    HybridMode,
    PathCircuitSimulator,
    PathSimulatorConfiguration,
    PathSimulatorMode,
    UnitarySimulator,
    dump_tensor_network,
    get_matrix,
)

__all__ = [
    "CircuitSimulator",
    "ConstructionMode",
    "DDSIMProvider",
    "HybridCircuitSimulator",
    "HybridMode",
    "PathCircuitSimulator",
    "PathSimulatorConfiguration",
    "PathSimulatorMode",
    "UnitarySimulator",
    "__version__",
    "dump_tensor_network",
    "get_matrix",
]
