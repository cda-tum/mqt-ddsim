"""The main entry point for the MQT DDSIM package"""

from __future__ import annotations

from typing import TYPE_CHECKING, Any, Literal, Union

if TYPE_CHECKING:
    from os import PathLike

    from numpy.typing import NDArray
    from qiskit import QuantumCircuit

    from mqt.core import QuantumComputation

    CircuitInputType = Union[QuantumComputation, str, PathLike[str], QuantumCircuit]

    Simulator = Literal["default", "hsf", "simulation_path"]


import numpy as np

from mqt.core.io import load

from .simulators import (
    CircuitSimulator,
    HybridCircuitSimulator,
    PathCircuitSimulator,
    UnitarySimulator,
)
from .simulators.unitary_simulator import ConstructionMode, get_matrix


def sample(
    circ: CircuitInputType, shots: int = 1014, simulator: Simulator = "default", **kwargs: Any
) -> dict[str, int]:
    """Simulates `circ` with the given `simulator` and draws `shots` samples
    from its output distribution (i.e., *weak simulation*).

    Args:
        circ: The quantum circuit to simulate and sample from
        shots: The number of shots.
        simulator: The simulator to use. Can be one of:
            - "default": The default simulator.
            - "hybrid": The hybrid Schrödinger-Feynman simulator.
            - "path": The simulation path framework.

        **kwargs: Additional keyword arguments to pass to the simulator. See the
                 documentation of the individual simulators for details.

    Returns:
        The generated output samples.
    """
    qc = load(circ)

    sim: CircuitSimulator | HybridCircuitSimulator | PathCircuitSimulator
    if simulator == "default":
        sim = CircuitSimulator(qc, **kwargs)
    elif simulator == "hsf":
        sim = HybridCircuitSimulator(qc, **kwargs)
    elif simulator == "simulation_path":
        sim = PathCircuitSimulator(qc, **kwargs)

    return sim.simulate(shots=shots)


def simulate(circ: CircuitInputType, simulator: Simulator = "default", **kwargs: Any) -> list[complex]:
    """Simulates `circ` with the given `simulator` and returns the full
    statevector (i.e., *strong simulation*).

    Args:
        circ: The quantum circuit to simulate.
        simulator: The simulator to use. Can be one of:
            - "default": The default simulator.
            - "hybrid": The hybrid Schrödinger-Feynman simulator.
            - "path": The simulation path framework.

        **kwargs: Additional keyword arguments to pass to the simulator. See the
                 documentation of the individual simulators for details.

    Returns:
        The resulting statevector.
    """
    qc = load(circ)

    sim: CircuitSimulator | HybridCircuitSimulator | PathCircuitSimulator
    if simulator == "default":
        sim = CircuitSimulator(qc, **kwargs)
    elif simulator == "hsf":
        sim = HybridCircuitSimulator(qc, **kwargs)
    elif simulator == "simulation_path":
        sim = PathCircuitSimulator(qc, **kwargs)

    sim.simulate(shots=0)
    return sim.get_vector()


def construct_unitary(
    circ: CircuitInputType, construction_mode: ConstructionMode = ConstructionMode.recursive
) -> NDArray[np.complex128]:
    """Constructs the unitary matrix of `circ`.

    Args:
        circ: The quantum circuit to compute the unitary matrix of.
        construction_mode: The construction mode to use. Can be one of:
            - "recursive": The recursive construction mode.
            - "sequential": The sequential construction mode.

    Returns:
        The resulting unitary matrix.
    """
    qc = load(circ)
    sim = UnitarySimulator(circ=qc, mode=construction_mode)
    sim.construct()
    # Extract resulting matrix from final DD and write data
    unitary: NDArray[np.complex128] = np.zeros((2**qc.num_qubits, 2**qc.num_qubits), dtype=np.complex128)
    get_matrix(sim, unitary)
    return unitary
