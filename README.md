[>> Quickstart Guide](#quickstart-guide)

[![Build Status](https://github.com/iic-jku/ddsim/workflows/continuous%20integration/badge.svg)](https://github.com/iic-jku/ddsim/actions)
[![codecov](https://codecov.io/gh/iic-jku/ddsim/branch/master/graph/badge.svg)](https://codecov.io/gh/iic-jku/ddsim)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![toolset: JKQ](https://img.shields.io/badge/toolset-JKQ-blue)](https://github.com/iic-jku/jkq)

# JKQ DDSIM - A quantum simulator based on decision diagrams written in C++

A tool for quantum circuit simulation by the [Institute for Integrated Circuits](https://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at)
and a part of the [JKQ toolset](https://github.com/iic-jku/jkq).

The tool builds upon [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git) which in turns builds on [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git).

For more information, on our work on quantum circuit simulation please visit [iic.jku.at/eda/research/quantum_simulation](https://iic.jku.at/eda/research/quantum_simulation) or, for more information on our work on noise-aware quantum circuit simulation, please visit [iic.jku.at/eda/research/noise_aware_simulation](https://iic.jku.at/eda/research/noise_aware_simulation).

If you have any questions, feel free to contact us via [iic-quantum@jku.at](mailto:iic-quantum@jku.at) or by creating an [issue](https://github.com/iic-jku/ddsim/issues) on GitHub.

# Table of contents
<!--ts-->
- [Usage](#usage)
- [System Requirements](#system-requirements)
- [Build and Run](#build-and-run)
    * [Library](#library)
    * [Executable Simulator](#executable-simulator)
    * [Executable Noise-aware Simulator](#executable-noise-aware-simulator)
- [Running Tests](#running-tests)
- [Frequently Asked Questions](#frequently-asked-questions) 
- [Reference](#reference)
<!--te-->


## Usage

This tool can be used for simulating quantum circuits provided in any of the following formats:
* `Real` from [RevLib](http://revlib.org/documentation.php)
  * [Our set of circuits](https://github.com/iic-jku/quantum_circuits)
  * [RevLib](http://revlib.org)
* `OpenQASM` used by IBM's [Qiskit](https://github.com/Qiskit/qiskit)
  * [Our set of circuits](https://github.com/iic-jku/quantum_circuits)
  * [OpenQASM Repo](https://github.com/Qiskit/openqasm)
  * [QUEKO](https://github.com/tbcdebug/QUEKO-benchmark) (focus on mapping though)
* `GRCS`
  * [GRCS Repo](https://github.com/sboixo/GRCS)
* `TFC`
  * [Reversible Logic Synthesis Benchmarks Page](http://webhome.cs.uvic.ca/~dmaslov/mach-read.html)


The format is automatically detected through the file extension.


The following additional algorithms are integrated in [QFR](https://github.com/iic-jku/qfr.git) and hence available in the simulator as well:
* Quantum Fourier Transformation
* Bernstein-Vazirani
* GHZ / Entanglement
* Grover's search (see `--help` for different call options)

For details on the available methods we refer to [iic.jku.at/eda/research/quantum_simulation](https://iic.jku.at/eda/research/quantum_simulation).

The simulator is based on the references listed below and can either be used as a **standalone executable** with command-line interface, or as a **library** for the incorporation in other projects.

## Using the Python Bindings / Backend for Qiskit

The backend for Qiskit is available via PyPi. The following code gives an example on the usage:

```python
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

The simulator may be used in a more stand-alone fashion:

```python
...
sim = ddsim.CircuitSimulator(circ)
print(sim.simulate(1000))
```

## System Requirements

Building (and running) is continuously tested under Linux, MacOS, and Windows using the [latest available system versions for GitHub Actions](https://github.com/actions/virtual-environments). However, the implementation should be compatible
with any current C++ compiler supporting C++17 and a minimum CMake version of 3.14.

`OpenMP >= 4.0` is required for building the `ddsim` library. Additionally, `boost/program_options >= 1.50` is required for building the commandline interface for `ddsim_simple` and `ddsim_noise_aware`. The `ddsim_noise_aware` further
requires `Threads::Threads`.

## Clone, Build, and Run

The code uses quite a few submodules, which have to be initialized.
There are two ways to do this:

1. While cloning the repository with the `--recurse-submodules` option to `git clone`. For HTTPS access: `git clone --recurse-submodules https://github.com/iic-jku/ddsim/`.
2. After cloning with `git submodule update --init --recursive`.


### Library

For building the library alone the CMake target `ddsim` is available.
In CMake from version 3.13 you can use the following commands:
```commandline
$ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --config Release --target ddsim
```

Windows users need to configure CMake by calling

```commandline
$ cmake -A x64 -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --config Release --target ddsim
```

instead.

The library can be used by including, for example, the ``QFRSimulator.hpp`` header file and
```c++
std::string file1 = "PATH_TO_FILE_1.EXT";
qc::QuantumComputation qc1(file1);

qc::SimpleSimulator sim(qc1);
sim.Simulate();
auto samples = sim.MeasureAllNonCollapsing(1000);
/* Use the results */
```

### Executable Simulator

To build the executable simulator, build the `ddsim_simple` CMake target (which requires `boost/program_options`) and run the resulting executable with options according to your needs.

The standalone executable is launched in the following way, showing available options:
```commandline
$ ./ddsim_simple --help
JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options:
-h [ --help ]                         produce help message
--seed arg (=0)                       seed for random number generator (default zero is possibly directly used as seed!)
--shots arg (=0)                      number of measurements (if the algorithm does not contain non-unitary gates, weak simulation is used)
--pv                                  display the state vector as list of pairs (real and imaginary parts)
--ps                                  print simulation stats (applied gates, sim. time, and maximal size of the DD)
--pm                                  print measurement results
--verbose                             Causes some simulators to print additional information to STDERR
--simulate_file arg                   simulate a quantum circuit given by file (detection by the file extension)
--simulate_file_hybrid arg            simulate a quantum circuit given by file (detection by the file extension) using the hybrid Schrodinger-Feynman simulator
--hybrid_mode arg                     mode used for hybrid Schrodinger-Feynman simulation (*amplitude*, dd)
--nthreads arg (=2)                   #threads used for hybrid simulation
--simulate_qft arg                    simulate Quantum Fourier Transform for given number of qubits
--simulate_ghz arg                    simulate state preparation of GHZ state for given number of qubits
--step_fidelity arg (=1)              target fidelity for each approximation run (>=1 = disable approximation)
--steps arg (=1)                      number of approximation steps
--simulate_grover arg                 simulate Grover's search for given number of qubits with random oracle
--simulate_grover_emulated arg        simulate Grover's search for given number of qubits with random oracle and emulation
--simulate_grover_oracle_emulated arg simulate Grover's search for given number of qubits with given oracle and emulation
--simulate_shor arg                   simulate Shor's algorithm factoring this number
--simulate_shor_coprime arg (=0)      coprime number to use with Shor's algorithm (zero randomly generates a coprime)
--simulate_shor_no_emulation          Force Shor simulator to do modular exponentiation instead of using emulation (you'll usually want emulation)
--simulate_fast_shor arg              simulate Shor's algorithm factoring this number with intermediate measurements
--simulate_fast_shor_coprime arg (=0) coprime number to use with Shor's algorithm (zero randomly generates a coprime)
```


The output is JSON-formatted as shown below (with hopefully intuitive naming).

```commandline
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

#### Quickstart Guide

Execute the following lines to get the simulator running in no time:

```commandline
$ git clone --recurse-submodules https://github.com/iic-jku/ddsim/
[...]

$ cd ddsim

ddsim/ $ cmake -S . -B build
-- Build files have been written to: /.../build

ddsim/ $ cmake --build build --config Release --target ddsim_simple
[...]
[100%] Built target ddsim_simple

ddsim/ $ build/ddsim_simple --help                            
JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options:
  -h [ --help ]                         produce help message
[...]
```

From here on you can start simulating quantum circuits or run the integrated algorithms.

Note that you have to have the Boost program_options library installed.

### Executable Noise-aware Simulator

The tool also supports noise-aware quantum circuit simulation, based on a stochastic approach. It currently supports global decoherence and gate error noise effects. A detailed summary of the simulator is presented in [[2]](https://arxiv.org/abs/2012.05620). Note that the simulator currently does not support simulating the integrated algorithms.

Building the simulator requires `boost/program_options` and `Threads::Threads`. It can be built by executing

```commandline
$ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --config Release --target ddsim_noise_aware
```

The simulator provides a help function which is called in the following way:

```commandline
$ ./build/ddsim_noise_aware -h
JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options:
  -h [ --help ]                         produce help message
  --seed arg (=0)                       seed for random number generator (default zero is possibly directly used as seed!)
  --pm                                  print measurements
  --ps                                  print simulation stats (applied gates, sim. time, and maximal size of the DD)
  --verbose                             Causes some simulators to print additional information to STDERR
  --simulate_file arg                   simulate a quantum circuit given by file (detection by the file extension)
  --step_fidelity arg (=1)              target fidelity for each approximation run (>=1 = disable approximation)
  --steps arg (=1)                      number of approximation steps
  --noise_effects arg (=APD)            Noise effects (A (=amplitude damping),D (=depolarization),P (=phase flip)) in the form of a character string describing the noise effects
  --noise_prob arg (=0.001)             Probability for applying noise
  --confidence arg (=0.05)              Confidence in the error bound of the stochastic simulation
  --error_bound arg (=0.1)              Error bound of the stochastic simulation
  --stoch_runs arg (=0)                 Number of stochastic runs. When the value is 0 the value is calculated using the confidence, error_bound and number of tracked properties.
  --properties arg (=-3-1000)           Comma separated list of tracked properties. Note that -1 is the fidelity and "-" can be used to specify a range.

Process finished with exit code 0

```

An example run, with amplitude damping, phase flip, and depolarization error (each with a probability of 0.1% whenever a gate is applied) looks like this:

```commandline
$ ./build/ddsim_noise_aware --ps --noise_effects APD --stoch_runs 10000 --noise_prob 0.001 --simulate_file adder4.qasm
{
  "statistics": {
    "applied_gates": 23,
    "approximation_runs": "0.000000",
    "benchmark": "stoch_APD_adder_n4",
    "final_fidelity": "0.937343",
    "max_nodes": 10,
    "mean_stoch_run_time": "0.015796",
    "n_qubits": 4,
    "parallel_instances": "28",
    "perfect_run_time": "0.000066",
    "seed": "0",
    "simulation_time": 5.911194324493408,
    "step_fidelity": "1.000000",
    "stoch_runs": 10000,
    "stoch_wall_time": "5.911118",
    "threads": 28
  }
}
```

## Running Tests
The repository also includes some (rudimentary) unit tests (using GoogleTest), which aim to ensure the correct behavior of the tool. They can be built and executed in the following way:
```commandline
$ cmake --build build/ --config Release
$ ./build/test/ddsim_test
[...]
```

## Frequently Asked Questions

**Why is target `ddsim_simple` unavailable when I try to build it?**

To build the commandline interfaces such as `ddsim_simple` you require the Boost program_options library.
If it is missing, you will see the following message in the CMake generation step

> `-- Did not find Boost! Commandline interface will not be an available target!`

Under Ubuntu you can simply install [`libboost-program-options-dev`](https://packages.ubuntu.com/search?keywords=libboost-program-options-dev&searchon=names&exact=1&suite=all&section=all).

**Why does generation step of CMake fail?**

If you see the following error message
```
$ cmake -S . -B <build target directory>
CMake Error at CMakeLists.txt:27 (message):
qfr was not found.  Please init/update submodules (git submodule update --init --recursive) and try again.
```
Please run `git submodule update --init --recursive` and try again.

## References

If you use our tool for your research, we will be thankful if you refer to it by citing the appropriate publication:


<details open>
<summary>
  [1] <a href="https://iic.jku.at/files/eda/2018_tcad_advanced_simulation_quantum_computation.pdf">A. Zulehner and R. Wille, “Advanced Simulation of Quantum Computations,” Transactions on CAD of Integrated Circuits and Systems (TCAD), vol. 38, no. 5, pp. 848–859, 2019</a>
</summary>

```bibtex
@article{zulehner2019advanced,
    title = {Advanced Simulation of Quantum Computations},
    author = {Zulehner, Alwin and Wille, Robert},
    journal = {Transactions on {CAD} of Integrated Circuits and Systems},
    volume = {38},
    number = {5},
    pages = {848--859},
    year = {2019},
    doi = {10.1109/TCAD.2018.2834427}
}
```
</details>

<details open>
<summary>
  [2] <a href="https://iic.jku.at/files/eda/2020_dac_weak_simulation_quantum_computation.pdf">S. Hillmich, I. L. Markov, and R. Wille, “Just Like the Real Thing: Fast Weak Simulation of Quantum Computation,” in Design Automation Conference (DAC), 2020</a>
</summary>

```bibtex
@inproceedings{DBLP:conf/dac/HillmichMW20,
  author    = {Stefan Hillmich and
               Igor L. Markov and
               Robert Wille},
  title     = {Just Like the Real Thing: {F}ast Weak Simulation of Quantum Computation},
  booktitle = {Design Automation Conference},
  publisher = {{IEEE}},
  year      = {2020}
}
```
</details>



<details open>
<summary>
  [3] <a href="https://iic.jku.at/files/eda/2021_stochastic_quantum_circuit_simulation_using_decision_diagrams.pdf">T. Grurl, R. Kueng, J. Fuß, and R. Wille, “Stochastic Quantum Circuit Simulation Using Decision Diagrams,” in Design, Automation and Test in Europe (DATE), 2021</a>
</summary>

```bibtex
@inproceedings{Grurl2020,
    author = {Grurl, Thomas and Kueng, Richard and Fu{\ss}, J{\"{u}}rgen and Wille, Robert},
    booktitle = {Design, Automation and Test in Europe (DATE)},
    title = {{Stochastic Quantum Circuit Simulation Using Decision Diagrams}},
    url = {http://arxiv.org/abs/2012.05620},
    year = {2021}
}

```
</details>


<details open>
<summary>
  [4] <a href="https://iic.jku.at/files/eda/2021_date_approximations_dd_baed_quantum_circuit_simulation.pdf">S. Hillmich, R. Kueng, I. L. Markov, and R. Wille, "As Accurate as Needed, as Efficient as Possible: Approximations in DD-based Quantum Circuit Simulation," in Design, Automation and Test in Europe (DATE), 2021</a>
</summary>

```bibtex
@inproceedings{DBLP:conf/date/HillmichKMW21,
  author    = {Stefan Hillmich and
               Richard Kueng and
               Igor L. Markov and
               Robert Wille},
  title     = {As Accurate as Needed, as Efficient as Possible: Approximations in DD-based Quantum Circuit Simulation},
  booktitle = {Design, Automation and Test in Europe},
  year      = {2021}
}
```
</details>

<details open>
<summary>
  [5] <a href="">L. Burgholzer, H. Bauer, and R. Wille, "Hybrid Schrödinger-Feynman Simulation of Quantum Circuits With Decision Diagrams," arXiv:2105.07045, 2021</a>
</summary>

```bibtex
@misc{burgholzer2021hybrid,
      author={Lukas Burgholzer and
               Hartwig Bauer and
               Robert Wille},
      title={Hybrid Schrödinger-Feynman Simulation of Quantum Circuits With Decision Diagrams},
      year={2021},
      eprint={2105.07045},
      archivePrefix={arXiv},
      primaryClass={quant-ph}
}
```
</details>
