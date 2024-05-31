#!/usr/bin/env python3

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
from __future__ import annotations

import locale

from pyqir import BasicQisBuilder, SimpleModule

# Create the module with two qubits and two results.
bell = SimpleModule("bell", num_qubits=2, num_results=2)
qis = BasicQisBuilder(bell.builder)

# Add instructions to the module to create a Bell pair and measure both qubits.
qis.h(bell.qubits[0])
qis.cx(bell.qubits[0], bell.qubits[1])
qis.mz(bell.qubits[0], bell.results[0])
qis.mz(bell.qubits[1], bell.results[1])

if __name__ == "__main__":
    with open("BellPair_py.ll", "w", encoding=locale.getpreferredencoding(False)) as f:
        f.write(bell.ir())
