[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PyPI](https://img.shields.io/pypi/v/mqt.ddsim?logo=pypi)](https://pypi.org/project/mqt.ddsim/)
[![CI](https://github.com/cda-tum/ddsim/actions/workflows/cmake.yml/badge.svg)](https://github.com/cda-tum/ddsim/actions/workflows/cmake.yml)
[![Documentation](https://img.shields.io/readthedocs/thapbi-pict.svg?logo=read-the-docs)](https://ddsim.readthedocs.io/en/latest/)
[![codecov](https://codecov.io/gh/cda-tum/ddsim/branch/master/graph/badge.svg)](https://codecov.io/gh/cda-tum/ddsim)
[![Language grade: Python](https://img.shields.io/lgtm/grade/python/github/cda-tum/ddsim?label=python&logo=lgtm)](https://lgtm.com/projects/g/cda-tum/ddsim/context:python)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/github/cda-tum/ddsim?label=c%2B%2B&logo=lgtm)](https://lgtm.com/projects/g/cda-tum/ddsim/context:cpp)


# MQT DDSIM - A quantum circuit simulator based on decision diagrams written in C++

A tool for classical quantum circuit simulation by the [Institute for Integrated Circuits](https://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at).
The simulator builds upon [our quantum functionality representation (QFR)](https://github.com/cda-tum/qfr.git) which in turns builds on [our decision diagram (DD) package](https://github.com/cda-tum/dd_package.git).

**Detailed documentation on all available formats, options, and algorithms is available at  [ReadTheDocs](https://ddsim.readthedocs.io/en/latest/).**

If you have any questions, feel free to contact us via [iic-quantum@jku.at](mailto:iic-quantum@jku.at) or by creating an [issue](https://github.com/cda-tum/ddsim/issues) on GitHub.

## Getting Started with the Python bindings

DDSIM bundled with the provider and backends for Qiskit is available via [PyPi](https://pypi.org/project/mqt.ddsim/) as wheel for Linux, Windows and macOS.

```console
(venv) $ pip install mqt.ddsim
```

The following code gives an example on the usage:

```python3
from qiskit import *
from mqt import ddsim

circ = QuantumCircuit(3)
circ.h(0)
circ.cx(0, 1)
circ.cx(0, 2)

print(circ.draw(fold=-1))

backend = ddsim.DDSIMProvider().get_backend('qasm_simulator')

job = execute(circ, backend, shots=10000)
counts = job.result().get_counts(circ)
print(counts)
```

## Clone, Build, and Run

Building (and running) is continuously tested under Linux, MacOS, and Windows using the [latest available system versions for GitHub Actions](https://github.com/actions/virtual-environments). 
The implementation should be compatible with any C++17 compiler and a minimum CMake version of 3.14.
The required dependencies are `OpenMP >= 4.0` and `Threads::Threads`.

Clone and build the repository with  

```console
$ git clone --recurse-submodules https://github.com/cda-tum/ddsim/
$ cd ddsim 
ddsim/ $ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
ddsim/ $ cmake --build build --config Release 
```

This builds the whole project including the library and executables, please refer to the [documentation](https://ddsim.readthedocs.io/en/latest/).

## References

If you use our tools for your research, we will be thankful if you refer to it by citing the appropriate publication.

- A. Zulehner and R. Wille, "[Advanced Simulation of Quantum Computations](https://iic.jku.at/files/eda/2018_tcad_advanced_simulation_quantum_computation.pdf)," Transactions on CAD of Integrated Circuits and Systems (TCAD), 2019
- S. Hillmich, I.L. Markov, and R. Wille, "[Just Like the Real Thing: Fast Weak Simulation of Quantum Computation](https://iic.jku.at/files/eda/2020_dac_weak_simulation_quantum_computation.pdf)," in Design Automation Conference (DAC), 2020
- T. Grurl, R. Kueng, J. Fuß, and R. Wille, "[Stochastic Quantum Circuit Simulation Using Decision Diagrams](https://iic.jku.at/files/eda/2021_stochastic_quantum_circuit_simulation_using_decision_diagrams.pdf)," in Design, Automation and Test in Europe (DATE), 2021
- S. Hillmich, R. Kueng, I. L. Markov, and R. Wille, "[As Accurate as Needed, as Efficient as Possible: Approximations in DD-based Quantum Circuit Simulation](https://iic.jku.at/files/eda/2021_date_approximations_dd_baed_quantum_circuit_simulation.pdf)," in Design, Automation and Test in Europe (DATE), 2021
- L. Burgholzer, H. Bauer, and R. Wille, "[Hybrid Schrödinger-Feynman Simulation of Quantum Circuits With Decision Diagrams](https://iic.jku.at/files/eda/2021_qce_hybrid_schrodinger_feynman_simulation_with_decision_diagrams.pdf)," Conference on Quantum Computing and Engineering (QCE), 2021
- L. Burgholzer, A.Ploier, and R. Wille, "[Exploiting Arbitrary Paths for the Simulation of Quantum Circuits with Decision Diagrams](https://iic.jku.at/files/eda/2022_date_exploiting_arbitrary_paths_simulation_quantum_circuits_decision_diagrams.pdf)," in Design, Automation and Test in Europe (DATE), 2022

