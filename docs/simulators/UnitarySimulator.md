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

# Unitary Simulator

The _Unitary Simulator_ uses the same underlying techniques as the _Circuit Simulator_, but instead of computing the final state vector, it computes the unitary matrix that represents the (functionality of the) quantum circuit.
Specifically, given a quantum circuit $G=g_0g_1\ldots g_{|G|-1}$, the unitary simulator computes the matrix $U=U_{{|G|-1}}\ldots U_{1}U_{0}$, where $U_g$ is the unitary matrix that represents the functionality of the gate $g$.

To this end, it starts off with the decision diagram representation of the identity $I$ (which is maximally compact as a decision diagram) and then applies the gates of the circuit one by one.
The DD representation of the unitary is updated in each step.
The final result is a decision diagram that represents the unitary matrix $U$.
Note that, by definition, this simulator can only handle circuits composed of unitary operations.

In general, the unitary matrix for an $n$-qubit circuit is a $2^n \times 2^n$ matrix.
The decision diagram representation of such a matrix can be exponentially more compact than the full matrix representation.
Hence, as the other simulators, the unitary simulator can take advantage of the decision diagram representation to efficiently compute a representation of the functionality of the quantum circuit, even in cases where the full matrix representation would be infeasible due to its exponential size.

## Computing a simple unitary

Let us start by computing the unitary matrix of a simple quantum circuit. Out of convenience, the following will use the `QuantumCircuit` class from Qiskit to define the circuit.
However, the unitary simulator generally accepts the same input types as all other simulators (e.g., OpenQASM).

```{code-cell} ipython3
from qiskit import QuantumCircuit

qc = QuantumCircuit(1)
qc.x(0)

qc.draw(output="mpl", style="iqp")
```

```{code-cell} ipython3
import graphviz
from mqt.core import load

from mqt.ddsim import UnitarySimulator

# Create the simulator
circ = load(qc)
sim = UnitarySimulator(circ)

# Construct the decision diagram representation of the unitary
sim.construct()

# Get the decision diagram representation of the unitary
dd = sim.get_constructed_dd()
dot = dd.to_dot(colored=True, edge_labels=True, classic=False)

graphviz.Source(source=dot)
```

```{code-cell} ipython3
import numpy as np

# Get the matrix representation of the unitary
mat = dd.get_matrix(qc.num_qubits)
unitary = np.array(mat, copy=False)

print(unitary)
```

## Examples

The following examples demonstrate a couple of different aspects about the unitary simulator.

### Multiple qubits and qubit ordering

```{code-cell} ipython3
from qiskit import QuantumCircuit

qc = QuantumCircuit(2)
qc.x(0)

qc.draw(output="mpl", style="iqp", wire_order=[1, 0])
```

```{code-cell} ipython3
import graphviz
from mqt.core import load

from mqt.ddsim import UnitarySimulator

# Create the simulator
circ = load(qc)
sim = UnitarySimulator(circ)

# Construct the decision diagram representation of the unitary
sim.construct()

# Get the decision diagram representation of the unitary
dd = sim.get_constructed_dd()
dot = dd.to_dot(colored=True, edge_labels=True, classic=False)

graphviz.Source(source=dot)
```

```{code-cell} ipython3
import numpy as np

mat = dd.get_matrix(qc.num_qubits)
unitary = np.array(mat, copy=False)

unitary
```

Now, consider applying the gate to the other qubit instead.

```{code-cell} ipython3
from qiskit import QuantumCircuit

qc = QuantumCircuit(2)
qc.x(1)

qc.draw(output="mpl", style="iqp", wire_order=[1, 0])
```

```{code-cell} ipython3
import graphviz
from mqt.core import load

from mqt.ddsim import UnitarySimulator

# Create the simulator
circ = load(qc)
sim = UnitarySimulator(circ)

# Construct the decision diagram representation of the unitary
sim.construct()

# Get the decision diagram representation of the unitary
dd = sim.get_constructed_dd()
dot = dd.to_dot(colored=True, edge_labels=True, classic=False)

graphviz.Source(source=dot)
```

```{code-cell} ipython3
import numpy as np

mat = dd.get_matrix(qc.num_qubits)
unitary = np.array(mat, copy=False)

unitary
```

### Multi-controlled quantum operations

The following shows an example of how efficiently decision diagrams can represent multi-controlled quantum operations.

```{code-cell} ipython3
from qiskit import QuantumCircuit

num_qubits = 8
qc = QuantumCircuit(num_qubits)
qc.mcx(control_qubits=list(reversed(range(1, num_qubits))), target_qubit=0)

qc.draw(output="mpl", style="iqp", wire_order=list(reversed(range(num_qubits))))
```

```{code-cell} ipython3
import graphviz
from mqt.core import load

from mqt.ddsim import UnitarySimulator

# Create the simulator
circ = load(qc)
sim = UnitarySimulator(circ)

# Construct the decision diagram representation of the unitary
sim.construct()

# Get the decision diagram representation of the unitary
dd = sim.get_constructed_dd()
dot = dd.to_dot(colored=True, edge_labels=True, classic=False)

graphviz.Source(source=dot)
```

```{code-cell} ipython3
import numpy as np

mat = dd.get_matrix(qc.num_qubits)
unitary = np.array(mat, copy=False)

unitary
```

### Unitary of a complete circuit

The following computes the unitary for a circuit consisting of multiple gates.

```{code-cell} ipython3
from qiskit import QuantumCircuit

num_qubits = 5
qc = QuantumCircuit(num_qubits)
qc.h(num_qubits - 1)
for i in reversed(range(num_qubits - 1)):
    qc.cx(num_qubits - 1, i)

qc.draw(output="mpl", style="iqp", wire_order=list(reversed(range(num_qubits))))
```

```{code-cell} ipython3
import graphviz
from mqt.core import load

from mqt.ddsim import UnitarySimulator

# Create the simulator
circ = load(qc)
sim = UnitarySimulator(circ)

# Construct the decision diagram representation of the unitary
sim.construct()

# Get the decision diagram representation of the unitary
dd = sim.get_constructed_dd()
dot = dd.to_dot(colored=True, edge_labels=True, classic=False)

graphviz.Source(source=dot)
```

```{code-cell} ipython3
import numpy as np

mat = dd.get_matrix(qc.num_qubits)
unitary = np.array(mat, copy=False)

unitary
```

### Decision diagrams are not always compact

The following example aims to demonstrate that decision diagrams are not a holy grail to constructing unitaries for circuits. In the worst case, they are still exponentially large. At that point, a plain array representation most likely becomes more performant.

```{code-cell} ipython3
import numpy as np
from qiskit import QuantumCircuit

qc = QuantumCircuit(3)
qc.h(2)
qc.cp(np.pi / 2, 1, 2)
qc.cp(np.pi / 4, 0, 2)
qc.h(1)
qc.cp(np.pi / 2, 0, 1)
qc.h(0)
qc.swap(0, 2)

qc.draw(output="mpl", style="iqp", wire_order=[2, 1, 0])
```

```{code-cell} ipython3
import graphviz
from mqt.core import load

from mqt.ddsim import UnitarySimulator

# Create the simulator
circ = load(qc)
sim = UnitarySimulator(circ)

# Construct the decision diagram representation of the unitary
sim.construct()

# Get the decision diagram representation of the unitary
dd = sim.get_constructed_dd()
dot = dd.to_dot(colored=True, edge_labels=True, classic=False)

graphviz.Source(source=dot)
```

```{code-cell} ipython3
import numpy as np

mat = dd.get_matrix(qc.num_qubits)
unitary = np.array(mat, copy=False)

unitary
```

## Usage as a Qiskit backend

Similar to the circuit simulator, the unitary simulator can be conveniently used via a Qiskit backend.

```{code-cell} ipython3
from qiskit import QuantumCircuit

from mqt.ddsim import DDSIMProvider

qc = QuantumCircuit(2)
qc.h(0)
qc.cx(0, 1)

# get the DDSIM provider
provider = DDSIMProvider()

# get the backend
backend = provider.get_backend("unitary_simulator")

# submit the job
job = backend.run(qc)

# get the result
result = job.result()
print(result.get_unitary(qc))
```

Note that this only gives access to the final unitary and not the underlying decision diagram representing the unitary. As a consequence, this approach is inherently limited by the amount of memory available on your system.
If you need access to the underlying decision diagram and/or do not need the final unitary matrix, consider using the standalone `UnitarySimulator` as described above.

## Alternative construction sequence

```{raw-cell}
:raw_mimetype: text/restructuredtext

It is well known, that the sequence in which the individual operations of a quantum circuit are applied can have a significant impact on the efficiency of the decision diagram representation.
As a result, the straight-forward, sequential application of gates may not always yield the most compact intermediate decision diagram representations.
The unitary simulator also supports an alternative construction sequence, which recursively groups gates and applies them in a tree-like fashion as described in :cite:p:`burgholzer2021efficient`.
```

Using the alternative construction sequence is as simple as setting `mode="recursive"` when creating the simulator or passing the `mode` argument to the `backend.run` method when using the Qiskit backend.

```{code-cell} ipython3
import graphviz
from mqt.core import load
from qiskit import QuantumCircuit

from mqt.ddsim import ConstructionMode, UnitarySimulator

qc = QuantumCircuit(3)
qc.h(2)
qc.h(1)
qc.h(0)
qc.cx(2, 1)

# Create the simulator
circ = load(qc)
sim = UnitarySimulator(circ, mode=ConstructionMode.recursive)

# Construct the decision diagram representation of the unitary
sim.construct()

# Get the decision diagram representation of the unitary
dd = sim.get_constructed_dd()
dot = dd.to_dot(colored=True, edge_labels=True, classic=False)

graphviz.Source(source=dot)
```
