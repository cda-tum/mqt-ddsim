[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PyPI](https://img.shields.io/pypi/v/jkq.ddsim?logo=pypi)](https://pypi.org/project/jkq.ddsim/)
[![CI](https://github.com/iic-jku/ddsim/actions/workflows/cmake.yml/badge.svg)](https://github.com/iic-jku/ddsim/actions/workflows/cmake.yml)
[![Documentation](https://img.shields.io/readthedocs/thapbi-pict.svg?logo=read-the-docs)](https://readthedocs.io)
[![codecov](https://codecov.io/gh/iic-jku/ddsim/branch/master/graph/badge.svg)](https://codecov.io/gh/iic-jku/ddsim)
[![Language grade: Python](https://img.shields.io/lgtm/grade/python/github/iic-jku/ddsim?label=python&logo=lgtm)](https://lgtm.com/projects/g/iic-jku/ddsim/context:python)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/github/iic-jku/ddsim?label=c%2B%2B&logo=lgtm)](https://lgtm.com/projects/g/iic-jku/ddsim/context:cpp)


# JKQ DDSIM - A quantum circuit simulator based on decision diagrams written in C++

A tool for quantum circuit simulation by the [Institute for Integrated Circuits](https://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at).
The simulator builds upon [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git) which in turns builds on [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git).

If you have any questions, feel free to contact us via [iic-quantum@jku.at](mailto:iic-quantum@jku.at) or by creating an [issue](https://github.com/iic-jku/ddsim/issues) on GitHub.

---

This tool can be used for simulating quantum circuits provided in multiple format such as OpenQASM (see our [our set of circuits](https://github.com/iic-jku/quantum_circuits)) and Qiskit QuantumCircuit objects.
Additional algorithms are integrated in [QFR](https://github.com/iic-jku/qfr.git) and hence available in the simulator.
A complete list of supported formats and algorithms is available at [RtD]().

**You can find the detailed documention at [ReadTheDocs]().** (Here the button might make sense ;) )

## Using the Python Bindings / Backend for Qiskit

The simulator bundled with the backend for Qiskit is available via [PyPi](https://pypi.org/project/jkq.ddsim/) as wheel for Linux, Windows and MacOS.

```console
(venv) $ pip install jkq.ddsim
```

The following code gives an example on the usage:

```python3
from qiskit import *
from jkq import ddsim

circ = QuantumCircuit(3)
circ.h(0)
circ.cx(0, 1)
circ.cx(0, 2)

print(circ.draw(fold=-1))

provider = ddsim.JKQProvider()
backend = provider.get_backend('qasm_simulator')

job = execute(circ, backend, shots=10000)
result = job.result()

counts = result.get_counts(circ)
print(counts)
```

Detailed documentation on the available backends and their usage is available on [RtD]().

## Clone, Build, and Run

Building (and running) is continuously tested under Linux, MacOS, and Windows using the [latest available system versions for GitHub Actions](https://github.com/actions/virtual-environments). 
The implementation should be compatible with any C++17 compiler and a minimum CMake version of 3.14.
The required dependencies are `OpenMP >= 4.0` and `Threads::Threads`.

Clone and build the repository with  

```console
$ git clone --recurse-submodules https://github.com/iic-jku/ddsim/
$ cd ddsim 
ddsim $ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
ddsim $ cmake --build build --config Release --target ddsim
```

Documentation on where to go from here is available on [RtD]().

## References

If you use our tool for your research, we will be thankful if you refer to it by citing the appropriate publication.
A more elaborate description of the papers is available at [RtD]().

- A. Zulehner and R. Wille, "[Advanced Simulation of Quantum Computations](https://iic.jku.at/files/eda/2018_tcad_advanced_simulation_quantum_computation.pdf)," Transactions on CAD of Integrated Circuits and Systems (TCAD), 2019
- S. Hillmich, I.L. Markov, and R. Wille, "[Just Like the Real Thing: Fast Weak Simulation of Quantum Computation](https://iic.jku.at/files/eda/2020_dac_weak_simulation_quantum_computation.pdf)," in Design Automation Conference (DAC), 2020
- T. Grurl, R. Kueng, J. Fuß, and R. Wille, "[Stochastic Quantum Circuit Simulation Using Decision Diagrams](https://iic.jku.at/files/eda/2021_stochastic_quantum_circuit_simulation_using_decision_diagrams.pdf)," in Design, Automation and Test in Europe (DATE), 2021
- S. Hillmich, R. Kueng, I. L. Markov, and R. Wille, "[As Accurate as Needed, as Efficient as Possible: Approximations in DD-based Quantum Circuit Simulation](https://iic.jku.at/files/eda/2021_date_approximations_dd_baed_quantum_circuit_simulation.pdf)," in Design, Automation and Test in Europe (DATE), 2021
- L. Burgholzer, H. Bauer, and R. Wille, "[Hybrid Schrödinger-Feynman Simulation of Quantum Circuits With Decision Diagrams](https://arxiv.org/pdf/2105.07045.pdf)," arXiv:2105.07045, 2021
- L. Burgholzer, A.Ploier, and R. Wille, "[Exploiting Arbitrary Paths for the Simulation of Quantum Circuits with Decision Diagrams](https://iic.jku.at/files/eda/2022_date_exploiting_arbitrary_paths_simulation_quantum_circuits_decision_diagrams.pdf)," in Design, Automation and Test in Europe (DATE), 2022

