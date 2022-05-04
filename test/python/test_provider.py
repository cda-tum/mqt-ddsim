import unittest

from mqt.ddsim import DDSIMProvider


class DDSIMProviderTestCase(unittest.TestCase):
    def setUp(self):
        self.provider = DDSIMProvider()
        self.backend_name = 'qasm_simulator'

    def test_backends(self):
        """Test the provider has backends."""
        backends = self.provider.backends()
        self.assertTrue(len(backends) > 0)
    
    def test_get_backend(self):
        """Test getting a backend from the provider."""
        backend = self.provider.get_backend(name=self.backend_name)
        self.assertEqual(backend.name(), self.backend_name)
