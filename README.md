[>> Quickstart Guide](#quickstart-guide)

[![Build Status](https://github.com/iic-jku/ddsim/workflows/continuous%20integration/badge.svg)](https://github.com/iic-jku/ddsim/actions)
[![codecov](https://codecov.io/gh/iic-jku/ddsim/branch/master/graph/badge.svg)](https://codecov.io/gh/iic-jku/ddsim)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![toolset: JKQ](https://img.shields.io/badge/toolset-JKQ-blue)](https://github.com/iic-jku/jkq)

# JKQ DDSIM - A quantum simulator based on decision diagrams written in C++

A tool for quantum circuit simulation by the [Institute for Integrated Circuits](https://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at)
and a part of the [JKQ toolset](https://github.com/iic-jku/jkq).

Developers: Stefan Hillmich, Lukas Burgholzer, Thomas Grurl, and Robert Wille.

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
    * [Installation](#installation)
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

The simulator is based on [[1]](https://iic.jku.at/files/eda/2018_tcad_advanced_simulation_quantum_computation.pdf) and can either be used as a **standalone executable** with command-line interface, or as a **library** for the incorporation in other projects.

## System Requirements

Building (and running) is continuously tested under Linux, MacOS, and Windows using the [latest available system versions for GitHub Actions](https://github.com/actions/virtual-environments). 
However, the implementation should be compatible with any current C++ compiler supporting C++17 and a minimum CMake version of 3.13.

`boost/program_options >= 1.50` is required for building the the commandline interface for `ddsim_simple` and `ddsim_noise_aware`. The `ddsim_noise_aware` further requires `Threads::Threads`.

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
$ cmake -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --config Release --target ddsim
```

instead.

Older CMake versions not supporting the above syntax (< 3.13) may be used with
```commandline
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release --target ddsim
```

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
--display_vector                      display the state vector
--ps                                  print simulation stats (applied gates, sim. time, and maximal size of the DD)
--verbose                             Causes some simulators to print additional information to STDERR
--benchmark                           print simulation stats in a single CSV style line (overrides --ps and  suppresses most other output, please don't rely on the format across versions)
--simulate_file arg                   simulate a quantum circuit given by file (detection by the file extension)
--simulate_qft arg                    simulate Quantum Fourier Transform for given number of qubits
--simulate_ghz arg                    simulate state preparation of GHZ state for given number of qubits
--step_fidelity arg (=1)              target fidelity for each approximation run (>=1 = disable approximation)
--steps arg (=1)                      number of approximation steps
--initial_reorder arg (=0)            Try to find a good initial variable order (0=None, 1=Most affected qubits to the top, 2=Most affected targets to the top)
--dynamic_reorder arg (=0)            Apply reordering strategy during simulation (0=None, 1=Sifting, 2=Move2Top)
--post_reorder arg (=0)               Apply a reordering strategy after simulation (0=None, 1=Sifting)
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
$ ./build/ddsim_simple --simulate_ghz 4 --display_vector --shots 1000 --ps
{
  "measurements": {
    "0000": 484,
    "1111": 516
  },
  "state_vector": [
    +0.707107+0i,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    +0.707107+0i
  ],
  "non_zero_entries": 2,
  "statistics": {
    "simulation_time": 0.000104,
    "measurement_time": 0.000104,
    "benchmark": "",
    "shots": 1000,
    "distinct_results": 2,
    "n_qubits": 4,
    "applied_gates": 4,
    "max_nodes": 9,
    "path_of_least_resistance": "1111",
    "seed": 0
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
$ ./build/ddsim_noise_aware --help
JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options:
  -h [ --help ]                         produce help message
  --seed arg (=0)                       seed for random number generator 
                                        (default zero is possibly directly used
                                        as seed!)
  --shots arg (=0)                      number of measurements (if the 
                                        algorithm does not contain non-unitary 
                                        gates, weak simulation is used)
  --display_vector                      display the state vector
  --ps                                  print simulation stats (applied gates, 
                                        sim. time, and maximal size of the DD)
  --verbose                             Causes some simulators to print 
                                        additional information to STDERR
  --benchmark                           print simulation stats in a single CSV 
                                        style line (overrides --ps and 
                                        suppresses most other output, please 
                                        don't rely on the format across 
                                        versions)
  --simulate_file arg                   simulate a quantum circuit given by 
                                        file (detection by the file extension)
    .
    .  
    .
  --noise_effects arg                   Noise effects (A (=amplitude damping),D
                                        (=depolarization),P (=phase flip)) in 
                                        the form of a character string 
                                        describing the noise effects (default="
                                        ")
  --noise_prob arg                      Probability for applying noise 
                                        (default=0.001)
  --confidence arg                      Confidence in the error bound of the 
                                        stochastic simulation (default= 0.05)
  --error_bound arg                     Error bound of the stochastic 
                                        simulation (default=0.1)
  --stoch_runs arg (=0)                 Number of stochastic runs. When the 
                                        value is 0 the value is calculated 
                                        using the confidence, error_bound and 
                                        number of tracked properties. (default 
                                        = 0)
  --properties arg                      Comma separated list of tracked 
                                        properties. Note that -1 is the 
                                        fidelity and "-" can be used to specify
                                        a range.  (default="0-1000")
```

An example run, with amplitude damping, phase flip, and depolarization error (each with a probability of 0.1% whenever a gate is applied) looks like this:

```commandline
$ ./build/ddsim_noise_aware --noise_effects APD --stoch_runs 10000 --noise_prob 0.001 --simulate_file /home/user/adder4.qasm
Conducting perfect run ...
Conducting 10000 runs using 4 cores ...
Starting 4 threads
Calculating amplitudes from all runs ...
Probabilities are ... (probabilities < 0.001 are omitted)
state=|0000> proba=0.00975488
state=|0001> proba=0.00784512
state=|0100> proba=0.00175135
state=|0101> proba=0.00194865
state=|0110> proba=0.00404999
state=|1000> proba=0.0229565
state=|1001> proba=0.937243
state=|1011> proba=0.00229939
state=|1100> proba=0.00215185
state=|1101> proba=0.00284815
state=|1110> proba=0.00505003
```

### Installation

The DDSIM library and tool may be installed on the system by executing

```commandline
$ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --config Release --target install
```

It can then also be included in other projects using the following CMake snippet

```cmake
find_package(ddsim)
target_link_libraries(${TARGET_NAME} PRIVATE JKQ::ddsim)
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
<summary>[1] A. Zulehner and R. Wille, “Advanced Simulation of Quantum Computations,” Transactions on CAD of Integrated Circuits and Systems (TCAD), vol. 38, no. 5, pp. 848–859, 2019</summary>

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
<summary>[2] T. Grurl, R. Kueng, J. Fuß, and R. Wille, “Stochastic Quantum Circuit Simulation Using Decision Diagrams,” in Design, Automation and Test in Europe (DATE), 2021</summary>

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



