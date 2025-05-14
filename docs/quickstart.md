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

# Quickstart

Assume you want to simulate the following quantum program:

```{code-cell} ipython3
from qiskit.circuit import QuantumCircuit

circ = QuantumCircuit(3)
circ.h(0)
circ.cx(0, 1)
circ.cx(0, 2)

circ.draw(output="mpl", style="iqp")
```

Then, using DDSIM to simulate the circuit is as easy as

```{code-cell} ipython3
from mqt import ddsim

provider = ddsim.DDSIMProvider()
backend = provider.get_backend("qasm_simulator")

job = backend.run(circ, shots=10000)
counts = job.result().get_counts(circ)
print(counts)
```

Check out the {doc}`reference documentation <api/mqt/ddsim/index>` for more information.
