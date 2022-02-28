"""
Exception for errors raised by JKQ simulator.
"""

from qiskit import QiskitError


class DDSIMError(QiskitError):
    """Class for errors raised by the JKQ simulator."""

    def __init__(self, *message):
        """Set the error message."""
        super().__init__(*message)
        self.message = ' '.join(message)

    def __str__(self):
        """Return the message."""
        return repr(self.message)
