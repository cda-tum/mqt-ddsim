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

# Circuit Simulator

The _Circuit Simulator_ is a Schrödinger-style simulator based on decision diagrams (DDs) as a data structure that can be used to:

- obtain the full state vector of the quantum circuit ("strong simulation") as originally proposed in {cite:p}`zulehner2019advanced`
- sample from the output distribution of a quantum circuit ("weak simulation") as originally proposed in {cite:p}`DBLP:conf/dac/HillmichMW20`

To this end, it starts off with the decision diagram representation of the initial state (generally, $|0\dots 0\rangle$) and then applies the gates of the circuit one by one.
The DD representation of the state vector is updated in each step.
The simulator can handle (almost) arbitrary quantum circuits, including those with mid-circuit measurements and resets.
For circuits with no non-unitary operations (except for measurements at the end of the circuit) the simulation is only done once.
In that case, the requested number of samples is subsequently drawn from the final decision diagram, resulting in fast runtime.

For the purpose of this demonstration, we will use the following simple QASM circuit, which we save as `bell.qasm` in the current working directory:

```{code-cell} ipython3
from pathlib import Path

qasm = """
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0],q[1];
measure q[0] -> c[0];
measure q[1] -> c[1];
"""
filename = "bell.qasm"
with Path(filename).open("w", encoding="utf-8") as f:
    f.write(qasm)
```

This circuit creates a Bell state and measures the qubits.

## Simulating a simple circuit

Using the `CircuitSimulator` to simulate the circuit from the QASM file `bell.qasm` is straightforward.

```{code-cell} ipython3
from mqt.core import load

from mqt.ddsim import CircuitSimulator

# create a CircuitSimulator object
qc = load(filename)
sim = CircuitSimulator(qc)

# run the simulation
result = sim.simulate(shots=1024)

# print the result
print(result)
```

As expected, the output distribution is approximately 50% for the states $|00\rangle$ and $|11\rangle$ each.

If we would like to obtain the full state vector of the quantum circuit, we can query it after the simulation as follows:

```{code-cell} ipython3
import numpy as np

# get the final DD
dd = sim.get_constructed_dd()
# transform the DD to a vector
vec = dd.get_vector()
# transform to a numpy array (without copying)
sv = np.array(vec, copy=False)
print(sv)
```

Note that getting the full state vector is only feasible for small circuits, as the memory and time requirements grow exponentially with the number of qubits.
However, due to the nature of decision diagrams, the simulator can generally sample from the output distribution of much larger circuits than can be fully represented in memory.

If you want to inspect the final decision diagram, you can get a Graphviz representation of it.
For that, make sure that you have Graphviz installed and that the `graphviz` Python package is available.
Then, you can call the `export_dd_to_graphviz_str` method on the simulator to obtain a Graphviz representation of the decision diagram. The following shows the default configuration options for the export.

```{code-cell} ipython3
import graphviz

dot = dd.to_dot(colored=True, edge_labels=False, classic=False, memory=False, format_as_polar=True)

graphviz.Source(source=dot)
```

```{code-cell} ipython3
# clean up
Path(filename).unlink()
```

## Using Qiskit `QuantumCircuit` objects as input

The `CircuitSimulator` can also be directly used with Qiskit's `QuantumCircuit` objects, which can be more convenient for some users.
THe above computation can be done as follows:

```{code-cell} ipython3
from qiskit import QuantumCircuit

# create the Bell circuit
qc = QuantumCircuit(2, 2)
qc.h(0)
qc.cx(0, 1)
qc.measure([0, 1], [0, 1])

# create a CircuitSimulator object
circ = load(qc)
sim = CircuitSimulator(circ)

# run the simulation
result = sim.simulate(shots=1024)

# print the result
print(result)
```

## Simulating a circuit with mid-circuit measurements

The `CircuitSimulator` can also handle circuits with mid-circuit measurements and resets. The following shows an example of Iterative Quantum Phase Estimation (IQPE) with a mid-circuit measurement.
The circuit tries to iteratively estimate the phase of a unitary $U=p(3\pi/8)$ with 3-bit precision.
We seek $\theta=3/16=0.0011_2$, which is not exactly representable using 3 bits, so the best we can expect is high probabilities for $0.c_2 c_1 c_0 = 001$ and $010$.

```{code-cell} ipython3
import locale
from pathlib import Path

qasm = """
OPENQASM 3.0;
include "stdgates.inc";

// counting register
qubit[1] q;

// eigenstate register
qubit[1] psi;

// classical registers
bit[3] c;

// initialize eigenstate psi = |1>
x psi;

// start by computing LSB c_0
h q;
cp(4 * (3*pi/8)) psi, q;
h q;
c[0] = measure q[0];

// reset counting qubit and compute middle bit c_1
reset q;
h q;
cp(2 * (3*pi/8)) psi, q;
if (c == 1) p(-pi/2) q;
h q;
c[1] = measure q[0];

// reset counting qubit and compute MSB c_2
reset q;
h q;
cp(1 * (3*pi/8)) psi, q;
if (c == 1) p(1 * -pi/4) q;
if (c == 2) p(2 * -pi/4) q;
if (c == 3) p(3 * -pi/4) q;
h q;
c[2] = measure q[0];
"""
filename = "iqpe.qasm"
with Path(filename).open("w", encoding=locale.getpreferredencoding(False)) as f:
    f.write(qasm)
```

```{code-cell} ipython3
from qiskit import qasm3

qc = qasm3.loads(qasm)
qc.draw(output="mpl", style="iqp")
```

```{code-cell} ipython3
from mqt.core import load

from mqt.ddsim import CircuitSimulator

# create a CircuitSimulator object
circ = load(filename)
sim = CircuitSimulator(circ)

# run the simulation
result = sim.simulate(shots=1024)

# print the result
print(result)
```

```{code-cell} ipython3
# clean up
Path(filename).unlink()
```

## Usage as a Qiskit backend

The `CircuitSimulator` can also be easily used as a backend for Qiskit.
To this end, the `CircuitSimulator` class is wrapped to implement the `BackendV2` interface of Qiskit, which allows using it as a drop-in replacement for any other Qiskit backend, such as the `AerSimulator`.

All of the backends available from mqt-ddsim are available from the `DDSIMProvider` class, which can be used to obtain a backend instance.

```{code-cell} ipython3
from mqt.ddsim import DDSIMProvider

provider = DDSIMProvider()
```

The `CircuitSimulator` is offered as two separate backends, the `qasm_simulator` and the `statevector_simulator`, which perform weak and strong simulation, respectively.

Let's first create a simple quantum circuit again

```{code-cell} ipython3
from qiskit import QuantumCircuit

# create the Bell circuit
qc = QuantumCircuit(2, 2)
qc.h(0)
qc.cx(0, 1)
qc.measure([0, 1], [0, 1])

qc.draw(output="mpl", style="iqp")
```

We can now use the `qasm_simulator` backend to simulate the circuit.

```{code-cell} ipython3
# get the backend
backend = provider.get_backend("qasm_simulator")

# submit the job
job = backend.run(qc, shots=100000)

# get the result
result = job.result()
print(result.get_counts())
```

Again, we can also use the `statevector_simulator` backend to obtain the full state vector of the quantum circuit.

```{code-cell} ipython3
# get the backend
backend = provider.get_backend("statevector_simulator")

# submit the job
job = backend.run(qc)

# get the result
result = job.result()
print(result.get_statevector())
```

## Usage as standalone C++ executable

To build the standalone C++ simulator executable, build the `ddsim_simple` (the name stuck due to historical reasons ;) ) CMake target and run the resulting executable with options according to your needs.

```console
$ ./ddsim_simple --help
MQT DDSIM by https://www.cda.cit.tum.de/ -- Allowed options:
-h [ --help ]                         produce help message
--seed arg (=0)                       seed for random number generator (default zero is possibly directly used as seed!)
--shots arg (=0)                      number of measurements (if the algorithm does not contain non-unitary gates, weak simulation is used)
--pv                                  display the state vector as list of pairs (real and imaginary parts)
--ps                                  print simulation stats (applied gates, sim. time, and maximal size of the DD)
--pm                                  print measurement results
--pcomplex                            print print additional statistics on complex numbers
--verbose                             Causes some simulators to print additional information to STDERR
--simulate_file arg                   simulate a quantum circuit given by file (detection by the file extension)
--simulate_qft arg                    simulate Quantum Fourier Transform for given number of qubits
--simulate_ghz arg                    simulate state preparation of GHZ state for given number of qubits
--step_fidelity arg (=1)              target fidelity for each approximation run (>=1 = disable approximation)
--steps arg (=1)                      number of approximation steps
```

The output is JSON-formatted as shown below (with hopefully intuitive naming).

```bash
    $ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
    $ cmake --build build --config Release --target ddsim_simple
    $ ./build/ddsim_simple --simulate_ghz 4 --shots 1000 --ps --pm
    {
      "measurement_results": {
        "0000": 484,
        "1111": 516
      },
      "statistics": {
        "applied_gates": 4,
        "approximation_runs": "0",
        "benchmark": "entanglement_4",
        "distinct_results": 2,
        "final_fidelity": "1.000000",
        "max_nodes": 9,
        "n_qubits": 4,
        "seed": "0",
        "shots": 1000,
        "simulation_time": 0.00013726699398830533,
        "single_shots": "1",
        "step_fidelity": "1.000000"
      }
    }
```
