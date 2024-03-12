from __future__ import annotations

import unittest

from mqt.ddsim import DDSIMProvider


class DDSIMProviderTestCase(unittest.TestCase):
    def setUp(self) -> None:
        self.provider = DDSIMProvider()
        self.backend_name = "qasm_simulator"

    def test_backends(self) -> None:
        """Test the provider has backends."""
        backends = self.provider.backends()
        assert len(backends) > 0

    def test_get_backend(self) -> None:
        """Test getting a backend from the provider."""
        backend = self.provider.get_backend(name=self.backend_name)
        assert backend.name == self.backend_name
