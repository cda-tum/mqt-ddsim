# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

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
