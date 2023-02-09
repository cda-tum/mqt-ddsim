"""
Exception for errors raised by DDSIM simulator.
"""

from qiskit import QiskitError


class DDSIMError(QiskitError):
    """Class for errors raised by the DDSIM simulator."""

    def __init__(self, *message):
        """Set the error message."""
        super().__init__(*message)
