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

# Simulation Path Framework

DDSIM also provides a framework for exploiting arbitrary simulation paths (using the [taskflow](https://github.com/taskflow/taskflow) library) based on the methods proposed in {cite:p}`burgholzer2022simulation`.

```{note}
As of mqt-ddsim v2.0.0, the Cotengra-based workflow for translating circuits to tensor networks and deriving contraction strategies has been removed. If you want to check out the respective code, please checkout prior versions of mqt-ddsim.
```

A _simulation path_ describes the order in which the individual MxV or MxM multiplications are conducted during the simulation of a quantum circuit.
It can be described as a list of tuples that identify the individual states/operations to be multiplied.
The initial state is always assigned `ID 0`, while the i-th operation is assigned `ID i`.
The result of a multiplication is assigned the next highest, unused ID, e.g., in a circuit with `|G|` gates the result of the first multiplication is assigned `ID |G|+1`.

The framework comes with several pre-defined simulation path strategies:

- `sequential` (_default_): simulate the circuit sequentially from left to right using only MxV multiplications
- `pairwise_recursive`: recursively group pairs of states and operations to form a binary tree of MxV/MxM multiplications
- `bracket`: group certain number of operations according to a given `bracket_size`
- `alternating`: start the simulation in the middle of the circuit and alternate between applications of gates "from the left" and "from the right" (useful for equivalence checking).

## Simulating a simple circuit

This example shall serve as a showcase on how to use the simulation path framework (via Python).
First, create the circuit to be simulated using qiskit, e.g., in this case a three-qubit GHZ state:

```{code-cell} ipython3
from qiskit import QuantumCircuit

circ = QuantumCircuit(3)
# the initial state corresponds to ID 0
circ.h(0)  # corresponds to ID 1
circ.cx(0, 1)  # corresponds to ID 2
circ.cx(0, 2)  # corresponds to ID 3

circ.draw(output="mpl", style="iqp")
```

Then, obtain the simulation path framework qiskit backend. You can choose between the `path_sim_qasm_simulator` and the `path_sim_statevector_simulator`.
The first just yields a dictionary with the counts of the measurements, while the latter also provides the complete statevector (which, depending on the amount of qubits, might not fit in the available memory).

```{code-cell} ipython3
from mqt.ddsim import DDSIMProvider

provider = DDSIMProvider()
backend = provider.get_backend("path_sim_qasm_simulator")
```

Per default, the backend uses the `sequential` strategy. For this particular example, this means:

- first, the Hadamard operation is applied to the initial state (`[0, 1] -> 4`)
- then, the first CNOT is applied (`[4, 2] -> 5`)
- finally, the last CNOT is applied (`[5, 3] -> 6`)

The corresponding simulation path is thus described by `[[0, 1], [4, 2], [5, 3]]` and the final state is the one with `ID 6`.

The simulation is started by calling the `run` method on the backend, which takes several optional configuration parameters (such as the simulation path strategy).
For a complete list of configuration options see [here](#configuration).

```{code-cell} ipython3
job = backend.run(circ)
result = job.result()
print(result.get_counts())
```

## Configuration

The framework can be configured using multiple options (which can be passed to the `backend.run()` method):

- `mode`: the simulation path mode to use (`sequential`, `pairwise_recursive`, `bracket`, `alternating`))
- `bracket_size`: the bracket size used for the `bracket` mode (default: `2`)
- `alternating_start`: the id of the operation to start with in the `alternating` mode (default: `0`)
- `seed`: the random seed used for the simulator (default `0`, i.e., no particular seed).
