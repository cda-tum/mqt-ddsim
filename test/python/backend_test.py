from qiskit import *

import jkq.ddsim

import logging

logging.basicConfig(level=logging.INFO)

circ = QuantumCircuit(3)
circ.h(0)
circ.cx(0, 1)
circ.cx(0, 2)

print(circ.draw(fold=-1))

print('Starting provider related stuff')

provider = jkq.ddsim.JKQProvider()

backend = provider.get_backend('circuit_simulator')

job = execute(circ, backend, shots=10000, return_statevector=True)
result = job.result()
outputstate = result.get_statevector(circ, decimals=3)
print(outputstate)

counts = result.get_counts(circ)
print(counts)
