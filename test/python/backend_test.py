from qiskit import *

from jkq import ddsim

#import logging
#logging.basicConfig(level=logging.INFO)

circ = QuantumCircuit(3)
circ.h(0)
circ.cx(0, 1)
circ.cx(0, 2)

print(circ.draw(fold=-1))

provider = ddsim.JKQProvider()

backend = provider.get_backend('qasm_simulator')

job = execute(circ, backend, shots=100000)
result = job.result()

counts = result.get_counts(circ)
print(counts)

sim = ddsim.CircuitSimulator(circ)
print(sim.simulate(1000))
