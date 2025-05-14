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

# Qiskit Primitives

The existing Qiskit interface to backends (`backend.run()`) was originally designed to accept a list of circuits and return counts for every job. Over time, it became clear that users have diverse purposes for quantum computing, and therefore the ways in which they define the requirements for their computing jobs are expanding.

To address this issue, Qiskit introduced the `qiskit.primitives` module in version 0.35.0 (Qiskit Terra 0.20.0) to provide a simplified way for end users to compute outputs of interest from a QuantumCircuit and to use backends. Qiskit's primitives provide methods that make it easier to build modular algorithms and other higher-order programs. Rather than simply returning counts, they return more immediately meaningful information.

The two currently available primitives are the `Sampler` and the `Estimator`. The first one computes quasi-probability distributions from circuit measurements, while the second one calculates and interprets expectation values of quantum operators that are required for many near-term quantum algorithms.

DDSIM provides its own version of these Qiskit Primitives that leverage the default circuit simulator based on decision diagrams, while preserving the methods and functionality of the original Qiskit primitives.

+++

## Sampler

+++

The `Sampler` takes a list of `QuantumCircuit` objects and simulates them using the `QasmSimulatorBackend` from MQT DDSIM. It then computes the quasi-probability distributions of the circuits in the list and encapsulates the results, along with the job's metadata, within a `SamplerResult` object.

Furthermore, it also handles compilation and parameter binding when working with parametrized circuits.

Here we show an example on how to use this submodule:

```{code-cell} ipython3
import numpy as np
from qiskit import QuantumCircuit
from qiskit.circuit import Parameter

from mqt.ddsim.primitives import Sampler

# Parametrized circuit to create an X-gate
theta_a = Parameter("theta_a")
circ = QuantumCircuit(1)
circ.ry(theta_a, 0)
circ.measure_all()

# Show circuit
circ.draw("mpl")
```

```{code-cell} ipython3
# Initialize sampler
sampler = Sampler()
```

The first argument in the `run` method is the circuit to simulate. The second argument contains the parameter values, if needed. The number of shots can be set as an additional argument. If not, the default value is 1024.

```{code-cell} ipython3
job = sampler.run(circ, np.pi, shots=int(1e4))
result = job.result()
```

The `result()` method of the job returns a `SamplerResult` object, which includes both the quasi-probability distribution and job metadata.

```{code-cell} ipython3
print(f">>> {result}")
print(f"  > Quasi-probability distribution: {result.quasi_dists[0]}")
```

It is also possible to simulate multiple circuits in one job:

```{code-cell} ipython3
# Create a second circuit
circ_2 = QuantumCircuit(1)
circ_2.measure_all()
circ_2.draw("mpl")
```

In this case, both the circuits and the parameter values must be entered as a `Sequence`.

```{code-cell} ipython3
job = sampler.run((circ, circ_2), [[np.pi], []], shots=int(1e4))
result = job.result()

print(f">>> {result}")
print(f"  > Quasi-probability distribution for circuit 1: {result.quasi_dists[0]}")
print(f"  > Quasi-probability distribution for circuit 2: {result.quasi_dists[1]}")
```

Now let's use the `Sampler` on a more complex circuit. For this example, we will test Grover's algorithm on a 4 qubit circuit where the marked states in the binary representation are "0110" and "1010".

A similar implementation of Grover's algorithm using Qiskit's sampler can be found here: https://qiskit.org/ecosystem/ibm-runtime/tutorials/grover_with_sampler.html

```{code-cell} ipython3
# Built-in modules
import math

# Imports from Qiskit
from qiskit.circuit.library import MCMT, GroverOperator, ZGate
from qiskit.visualization import plot_distribution


# Define an oracle circuit that marks the input states, which are represented as a list of bitstrings
def grover_oracle(marked_states: list) -> QuantumCircuit:
    if not isinstance(marked_states, list):
        marked_states = [marked_states]
    # Compute the number of qubits in circuit
    num_qubits = len(marked_states[0])

    qc = QuantumCircuit(num_qubits)
    # Mark each target state in the input list
    for target in marked_states:
        # Flip target bit-string to match Qiskit bit-ordering
        rev_target = target[::-1]
        # Find the indices of all the '0' elements in bit-string
        zero_inds = [ind for ind in range(num_qubits) if rev_target.startswith("0", ind)]
        # Add a multi-controlled Z-gate with pre- and post-applied X-gates (open-controls)
        # where the target bit-string has a '0' entry
        qc.x(zero_inds)
        qc.compose(MCMT(ZGate(), num_qubits - 1, 1), inplace=True)
        qc.x(zero_inds)
    return qc


# Enter the states to be marked
marked_states = ["0110", "1010"]
oracle = grover_oracle(marked_states)
oracle.draw("mpl")
```

```{code-cell} ipython3
# Build circuit that amplifies the marked states by the oracle

grover_op = GroverOperator(oracle)
grover_op.decompose().draw("mpl")
```

```{code-cell} ipython3
# Compute the optimal number of repetitions of the GroverOperator to amplify the probabilities of the marked states
optimal_num_iterations = math.floor(math.pi / 4 * math.sqrt(2**grover_op.num_qubits / len(marked_states)))


# Build the full Grover circuit
qc = QuantumCircuit(grover_op.num_qubits)
# Create even superposition of all basis states
qc.h(range(grover_op.num_qubits))
# Apply Grover operator the optimal number of times
qc.compose(grover_op.power(optimal_num_iterations), inplace=True)
# Measure all qubits
qc.measure_all()
qc.draw("mpl")
```

After having built the circuit, use the `Sampler` to get the probability distribution.

```{code-cell} ipython3
dist = sampler.run(qc, shots=int(1e4)).result().quasi_dists[0]
plot_distribution(dist.binary_probabilities())
```

## Estimator

+++

The `Estimator` calculates the expectation value of an observable with respect to a certain quantum state (described by a quantum circuit). In contrast to Qiskit's estimator, the DDSIM version exactly computes the expectation value using its simulator based on decision diagrams instead of sampling.

The `Estimator` also handles parameter binding when dealing with parametrized circuits.

Here we show an example on how to use it:

+++

First, we build the observable and the quantum state. The observable is given as a `SparsePauliOp` object, while the quantum state is described by a `QuantumCircuit`.

In this example, our observable is the Pauli matrix $\sigma_{x}$ and the quantum state is $\frac{1}{\sqrt{2}}(|0\rangle + |1\rangle)$.

```{code-cell} ipython3
import numpy as np
from qiskit import QuantumCircuit
from qiskit.circuit import Parameter
from qiskit.quantum_info import Pauli

from mqt.ddsim.primitives.estimator import Estimator

# Build quantum state
circ = QuantumCircuit(1)
circ.ry(np.pi / 2, 0)
circ.measure_all()

# Build observable
pauli_x = Pauli("X")

# Show circuit
circ.draw("mpl")
```

```{code-cell} ipython3
# Initialize estimator

estimator = Estimator()
```

The next step involves running the estimation using the `run()` method. This method requires three arguments: a sequence of `QuantumCircuit` objects representing quantum states, a sequence of `SparsePauliOp` objects representing observables, and optionally, a parameter sequence if we are dealing with parametrized circuits.

The user has to ensure that the number of circuits matches the number of observables, as the estimator pairs corresponding elements from both lists sequentially.

```{code-cell} ipython3
# Enter observable and circuit as a sequence

job = estimator.run([circ], [pauli_x])
result = job.result()
```

The `result()` method of the job returns a `EstimatorResult` object, which contains the computed expectation values.

```{code-cell} ipython3
print(f">>> {result}")
print(f"  > Expectation values: {result.values}")
```

Now we explore how the `Estimator` works with multiple circuits and observables. For this example, we will calculate the expectation value of $\sigma_{x}$ and $\sigma_{y}$ with respect to the quantum state $|1\rangle$. Since our observable list has a length of 2, we need to enter two copies of the `QuantumCircuit` object representing $|1\rangle$ as a sequence:

```{code-cell} ipython3
# Build quantum state
circ = QuantumCircuit(1)
circ.ry(np.pi, 0)
circ.measure_all()

# Build observables
pauli_x = Pauli("X")
pauli_y = Pauli("Y")

# Construct input arguments
observables = [pauli_x, pauli_y]
quantum_states = [circ, circ]

# Run estimator
job = estimator.run(quantum_states, observables)
result = job.result()
```

```{code-cell} ipython3
print(f">>> {result}")
print(f"  > Expectation values: {result.values}")
```

The first and second entries of the list of values are the expectation value of $\sigma_{x}$ and $\sigma_{y}$ respectively.

+++

Let's now calculate the expectation values of $\sigma_{x}$ with respect to $\frac{1}{\sqrt{2}}(|0\rangle + |1\rangle)$ and $\frac{1}{\sqrt{2}}(|0\rangle - |1\rangle)$. For this example we will use parametrized circuits to build the quantum state.

```{code-cell} ipython3
theta = Parameter("theta")
circ_2 = QuantumCircuit(1)
circ_2.ry(theta, 0)
circ_2.measure_all()

# Show circuit
circ_2.draw("mpl")
```

```{code-cell} ipython3
# Construct input arguments
observables = [pauli_x, pauli_x]
quantum_states = [circ_2, circ_2]
parameters = [[np.pi / 2], [-np.pi / 2]]

# Run estimator
job = estimator.run(quantum_states, observables, parameters)
result = job.result()
```

```{code-cell} ipython3
print(f">>> {result}")
print(f"  > Expectation values: {result.values}")
```
