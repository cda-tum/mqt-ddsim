---
file_format: mystnb
kernelspec:
  name: python3
mystnb:
  number_source_lines: true
---

```{code-cell} ipython3
:tags: [remove-cell]
%config InlineBackend.figure_formats = ['svg']
```

# Hybrid Schrödinger-Feynman Simulator

Hybrid Schrödinger-Feynman approaches strive to use all the available memory and processing units in order to efficiently simulate quantum circuits which would

1. run into memory bottlenecks using Schrödinger-style simulation, or
2. take exceedingly long using Feynman-style path summation,

eventually trading-off the respective memory and runtime requirements.

DDSIM offers such a simulator based on decision diagrams as proposed in {cite:p}`DBLP:conf/qce/BurgholzerBW21`.

It currently assumes that no non-unitary operations (besides measurements at the end of the circuit) are present in the circuit.
Furthermore it always measures all qubits at the end of the circuit in the order they were defined.

The backend provides two different modes that can be set using the `mode` option:

- `dd`: all computations are conducted on decision diagrams and the requested number of shots are sampled from the final decision diagram
- `amplitude`: all individual paths in the hybrid simulation scheme are simulated using decision diagrams, while subsequent computations (addition of all results) is conducted using arrays. This requires more memory but can lead to significantly better runtime performance in many cases. The requested shots are sampled from the final statevector array.

The number of threads to use can be set using the `nthreads` option. Note that the number of threads may be reduced when using the `amplitude` mode in order to fit the computation in the available memory.

```{code-cell} ipython3
from qiskit import QuantumCircuit

qc = QuantumCircuit(4)

# create a simple graph state
qc.h(range(4))
qc.cz(0, 2)
qc.cz(1, 3)

qc.draw(output="mpl", style="iqp")
```

```{code-cell} ipython3
from mqt.ddsim import DDSIMProvider

provider = DDSIMProvider()

# get the backend
backend = provider.get_backend("hybrid_qasm_simulator")

# submit the job
job = backend.run(qc, shots=1000, mode="dd", nthreads=2)

# get the result
result = job.result()
print(result.get_counts())
```

Similar to the circuit simulator, there is also a statevector simulator available. The statevector simulator can be used to obtain the full statevector of the quantum circuit at the end of the simulation.

Note that `shots` has to be set to `0` when using the `amplitude` mode as the statevector array is modified in-place for sampling and, hence, the state vector is no longer available afterwards.

```{code-cell} ipython3
from mqt.ddsim import DDSIMProvider

provider = DDSIMProvider()

# get the backend
backend = provider.get_backend("hybrid_statevector_simulator")

# submit the job
job = backend.run(qc, shots=0, mode="amplitude", nthreads=2)

# get the result
result = job.result()
print(result.get_statevector())
```
