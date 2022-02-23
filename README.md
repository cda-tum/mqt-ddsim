[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![toolset: JKQ](https://img.shields.io/badge/toolset-JKQ-blue)](https://github.com/iic-jku/jkq)
[![PyPI](https://img.shields.io/pypi/v/jkq.ddsim?logo=pypi)](https://pypi.org/project/jkq.ddsim/)
[![CI](https://github.com/iic-jku/ddsim/actions/workflows/cmake.yml/badge.svg)](https://github.com/iic-jku/ddsim/actions/workflows/cmake.yml)
[![codecov](https://codecov.io/gh/iic-jku/ddsim/branch/master/graph/badge.svg)](https://codecov.io/gh/iic-jku/ddsim)
[![Language grade: Python](https://img.shields.io/lgtm/grade/python/github/iic-jku/ddsim?label=python&logo=lgtm)](https://lgtm.com/projects/g/iic-jku/ddsim/context:python)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/github/iic-jku/ddsim?label=c%2B%2B&logo=lgtm)](https://lgtm.com/projects/g/iic-jku/ddsim/context:cpp)


# JKQ DDSIM - A quantum circuit simulator based on decision diagrams written in C++

A tool for quantum circuit simulation by the [Institute for Integrated Circuits](https://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at)
and a part of the [JKQ toolset](https://github.com/iic-jku/jkq).

The tool builds upon [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git) which in turns builds on [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git).

For more information, on our work on quantum circuit simulation please visit [iic.jku.at/eda/research/quantum_simulation](https://iic.jku.at/eda/research/quantum_simulation) or, for more information on our work on noise-aware quantum circuit simulation, please visit [iic.jku.at/eda/research/noise_aware_simulation](https://iic.jku.at/eda/research/noise_aware_simulation).

If you have any questions, feel free to contact us via [iic-quantum@jku.at](mailto:iic-quantum@jku.at) or by creating an [issue](https://github.com/iic-jku/ddsim/issues) on GitHub.

# Table of contents
<!--ts-->
- [Usage](#usage)
- [Using the Python Bindings / Backend for Qiskit](#using-the-python-bindings--backend-for-qiskit) 
- [System Requirements](#system-requirements)
- [Build and Run](#build-and-run)
    * [Library](#library)
    * [Executable Simulator](#executable-simulator)
    * [Executable Noise-aware Simulator](#executable-noise-aware-simulator)
    * [Simulation Path Framework](#simulation-path-framework)
- [Running Tests](#running-tests)
- [Frequently Asked Questions](#frequently-asked-questions) 
- [References](#references)
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
* Qiskit QuantumCircuits
  * As a backend for qiskit
  * "Standalone" by directly passing the quantum circuit


The format is automatically detected through the file extension.


The following additional algorithms are integrated in [QFR](https://github.com/iic-jku/qfr.git) and hence available in the simulator as well:
* Quantum Fourier Transformation
* Bernstein-Vazirani
* GHZ / Entanglement
* Grover's search (see `--help` for different call options)

For details on the available methods we refer to [iic.jku.at/eda/research/quantum_simulation](https://iic.jku.at/eda/research/quantum_simulation).

The simulator is based on the references listed below and can either be used as a **standalone executable** with command-line interface, or as a **library** for the incorporation in other projects.

## Using the Python Bindings / Backend for Qiskit

The backend for Qiskit is available via [PyPi](https://pypi.org/project/jkq.ddsim/) as wheel for Linux, Windows and MacOS.
In order to make the library as easy to use as possible (without compilation), we provide pre-built wheels for most common platforms (64-bit Linux, MacOS, Windows). (We strongly recommend using a [virtual environment](https://docs.python.org/3/tutorial/venv.html).)

```console
(venv) $ pip install jkq.ddsim
```

However, in order to get the best performance out of DDSIM, it is recommended to build it locally from the source distribution via
```console
(venv) $ pip install --no-binary jkq.ddsim jkq.ddsim
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

The provider currently has five backends:

* `qasm_simulator` which simulates the circuit and returns the requested number of shots
* `statevector_simulator` which also simulates the circuit and returns the statevector along the requested number of shots
* `hybrid_qasm_simulator` which simulates the circuit in parallel using a hybrid Schrodinger-Feynman technique and returns the requested number of shots
* `hybrid_statevector_simulator` which also simulates the circuit in parallel using a hybrid Schrodinger-Feynman technique and returns the statevector
* `unitary_simulator` which constructs the unitary functionality of a circuit and returns the corresponding unitary matrix

A slightly more elaborate example can be found in the notebook [ddsim.ipynb](ddsim.ipynb).

## System Requirements

Building (and running) is continuously tested under Linux, MacOS, and Windows using the [latest available system versions for GitHub Actions](https://github.com/actions/virtual-environments). However, the implementation should be compatible
with any current C++ compiler supporting C++17 and a minimum CMake version of 3.14.

`OpenMP >= 4.0` is required for building the `ddsim` library. The `ddsim_noise_aware` simulator further requires `Threads::Threads`.

## Clone, Build, and Run

The code uses quite a few submodules, which have to be initialized.
There are two ways to do this:

1. While cloning the repository with the `--recurse-submodules` option to `git clone`. For HTTPS access: `git clone --recurse-submodules https://github.com/iic-jku/ddsim/`.
2. After cloning with `git submodule update --init --recursive`.


### Library

For building the library alone the CMake target `ddsim` is available.
In CMake from version 3.13 you can use the following commands:
```console
$ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --config Release --target ddsim
```

Windows users need to configure CMake by calling

```console
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

To build the executable simulator, build the `ddsim_simple` CMake target and run the resulting executable with options according to your needs.

The standalone executable is launched in the following way, showing available options:
```console
$ ./ddsim_simple --help
JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options:
-h [ --help ]                         produce help message
--seed arg (=0)                       seed for random number generator (default zero is possibly directly used as seed!)
--shots arg (=0)                      number of measurements (if the algorithm does not contain non-unitary gates, weak simulation is used)
--pv                                  display the state vector as list of pairs (real and imaginary parts)
--ps                                  print simulation stats (applied gates, sim. time, and maximal size of the DD)
--pm                                  print measurement results
--pcomplex                            print print additional statistics on complex numbers
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

```console
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

```console
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

### Executable Noise-aware Simulator

The tool also supports noise-aware quantum circuit simulation, based on a stochastic approach. It currently supports
global decoherence and gate error noise effects. A detailed summary of the simulator is presented
in [[3]](https://arxiv.org/abs/2012.05620). Note that the simulator currently does not support simulating the integrated
algorithms.

Building the simulator requires `Threads::Threads`. It can be built by executing

```console
$ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --config Release --target ddsim_noise_aware
```

The simulator provides a help function which is called in the following way:

```console
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

```console
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

### Simulation Path Framework

The tool also supports a simulation path framework that allows the usage of different strategies for quantum circuit
simulation, the related publication can be found
at [[6]](https://iic.jku.at/files/eda/2022_date_exploiting_arbitrary_paths_simulation_quantum_circuits_decision_diagrams.pdf)
. It allows speedups of up to several orders of magnitude when simulating quantum circuits and is inspired by work
previously done at [[7]](https://github.com/taskflow/taskflow). The framework supports strategies implemented
directly `sequential, pairwise_recursive, bracket, alternating` and strategies from the tensor network domain `cotengra`
. If no additional strategie is declared `sequential` is used.

**Basic Example**

This example shall serve as a showcase on how to work with the simulation path framework and what exactly it does. At
first one has to have a quantum circuit.

```
auto qc = std::make_unique<qc::QuantumComputation>(2);
qc->h(1U);
qc->x(0U, 1_pc);
qc->x(0U, 1_pc);
qc->x(0U, 1_pc);
```

This is a two qubit quantum circuit with a hadamard gate on the first qubit and then three CNOT gates coming afterwards.
In this setup the hadamard gate would correspond to the number `1`, the CNOTs to `2,3,4` and the initial state to the
number `0` respectivly. Applying the sequential strategie to this quantum circuit would first combine the numbers `0`
and `1` and save the result as number `5` on the first position in the circuit, deleting `0` and `1` in the process. The
next step is to combine `5` and `2`. Again saving the result in `6` and putting it in front. This goes on until only one
single number remains.

```
auto config        = PathSimulator::Configuration{};
config.mode        = PathSimulator::Configuration::Mode::Sequential;
```

Is used to create the configuration object and set the mode of the simulation path strategie. The next step is to create
a **PathSimulator** object and simulate it.

```
PathSimulator tbs(std::move(qc), config);
auto counts = tbs.Simulate(1024);
```

**CoTenGra**

For a deeper dive into what CoTenGra does, we refer to [[8]](https://github.com/jcmgray/cotengra). It is also possible
to have the visualization and path available when working with the CoTenGra mode. This is achieved by setting the
boolean parameters for `dump_path` and `plot_ring` to true

```
if pathsim_configuration.mode == ddsim.PathSimulatorMode.cotengra:
            max_time = options.get('cotengra_max_time', 60)
            max_repeats = options.get('cotengra_max_repeats', 1024)
            dump_path = options.get('cotengra_dump_path', True)
            plot_ring = options.get('cotengra_plot_ring', True)
            path = get_simulation_path(qobj_experiment, max_time=max_time, max_repeats=max_repeats,
                                       dump_path=dump_path, plot_ring=plot_ring)
```

**The configuration**

To make use of the functionality, one can call it as an additional argument when simulating quantum circuits with
decision diagrams. Either via a **configuration** object itself

```
 auto config        = PathSimulator::Configuration{};
 config.mode        = PathSimulator::Configuration::Mode::BracketGrouping;
 config.bracketSize = 3;
 PathSimulator tbs(std::move(qc), config);
```

or as a set of individual parameters directly in the function call.

```
PathSimulator tbs(std::move(qc), PathSimulator::Configuration::Mode::Sequential, 2, 0, 12345U);
```

This can be done in c++, as seen above, or python. For the latter the call looks like this

```
sim = ddsim.PathCircuitSimulator(circ, seed=0, mode=ddsim.PathSimulatorMode.bracket, bracket_size=2)
```

**Adding new strategies**

Shall be done in `PathSimulator.cpp`, `PathSimulator.hpp` and in `bindings.cpp` respectivly. The following is an example
on how such an implementation can look like

```
void PathSimulator::generateSequentialSimulationPath() {
    SimulationPath::Components components{};
    components.reserve(qc->getNops());

    for (std::size_t i = 0; i < qc->getNops(); ++i) {
        if (i == 0)
            components.emplace_back(0, 1);
        else
            components.emplace_back(qc->getNops() + i, i + 1);
    }
    setSimulationPath(components, true);
}
```

The basic idea is to add numbers in the components list where the numbers represent either gates of the quantum circuit
or the result of multipliying two gates together. Again we refer
to [[6]](https://iic.jku.at/files/eda/2022_date_exploiting_arbitrary_paths_simulation_quantum_circuits_decision_diagrams.pdf)
for a deeper look into the specifics.

## Running Tests

The repository also includes some (rudimentary) unit tests (using GoogleTest), which aim to ensure the correct behavior
of the tool. They can be built and executed in the following way:

```console
$ cmake -DBUILD_DDSIM_TESTS=ON -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build/ --config Release
$ ./build/test/ddsim_test
[...]
```

## Frequently Asked Questions


**Why does generation step of CMake fail?**

If you see the following error message
```console
$ cmake -S . -B <build target directory>
CMake Error at CMakeLists.txt:27 (message):
qfr was not found.  Please init/update submodules (git submodule update --init --recursive) and try again.
```
Please run `git submodule update --init --recursive` and try again.

**Why do I get a linking error at the end of the build process?**

If you are using gcc for building, and you get the error message 
```
lto1: internal compiler error: Segmentation fault
Please submit a full bug report,
with preprocessed source if appropriate.
See <file:///usr/share/doc/gcc-9/README.Bugs> for instructions.
lto-wrapper: fatal error: /usr/bin/c++ returned 1 exit status
compilation terminated.
/usr/bin/ld: error: lto-wrapper failed
collect2: error: ld returned 1 exit status
make[2]: *** [test/CMakeFiles/ddsim_test.dir/build.make:166: test/ddsim_test] Error 1
make[1]: *** [CMakeFiles/Makefile2:464: test/CMakeFiles/ddsim_test.dir/all] Error 2
```

Configure the simulator again and add the parameter `-DBINDINGS=ON`

## References

If you use our tool for your research, we will be thankful if you refer to it by citing the appropriate publication:


<details>
<summary>
  [1] A. Zulehner and R. Wille, “<a href="https://iic.jku.at/files/eda/2018_tcad_advanced_simulation_quantum_computation.pdf">Advanced Simulation of Quantum Computations</a>,” Transactions on CAD of Integrated Circuits and Systems (TCAD), vol. 38, no. 5, pp. 848–859, 2019
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

<details>
<summary>
  [2] S. Hillmich, I.L. Markov, and R. Wille, “<a href="https://iic.jku.at/files/eda/2020_dac_weak_simulation_quantum_computation.pdf">Just Like the Real Thing: Fast Weak Simulation of Quantum Computation</a>,” in Design Automation Conference (DAC), 2020
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



<details>
<summary>
  [3] T. Grurl, R. Kueng, J. Fuß, and R. Wille, “<a href="https://iic.jku.at/files/eda/2021_stochastic_quantum_circuit_simulation_using_decision_diagrams.pdf">Stochastic Quantum Circuit Simulation Using Decision Diagrams</a>,” in Design, Automation and Test in Europe (DATE), 2021
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


<details>
<summary>
  [4] S. Hillmich, R. Kueng, I. L. Markov, and R. Wille, "<a href="https://iic.jku.at/files/eda/2021_date_approximations_dd_baed_quantum_circuit_simulation.pdf">As Accurate as Needed, as Efficient as Possible: Approximations in DD-based Quantum Circuit Simulation</a>," in Design, Automation and Test in Europe (DATE), 2021
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

<details>
<summary>
  [5] L. Burgholzer, H. Bauer, and R. Wille, "<a href="https://arxiv.org/pdf/2105.07045.pdf">Hybrid Schrödinger-Feynman Simulation of Quantum Circuits With Decision Diagrams</a>," arXiv:2105.07045, 2021
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

<details>
<summary>
  [6] L. Burgholzer, A.Ploier, and R. Wille, "<a href="https://iic.jku.at/files/eda/2022_date_exploiting_arbitrary_paths_simulation_quantum_circuits_decision_diagrams.pdf">Exploiting Arbitrary Paths for the Simulation of Quantum Circuits with Decision Diagrams</a>," 2021
</summary>

```bibtex
@misc{burgholzer2021hybrid,
      author={Lukas Burgholzer and
               Alexander Ploier and
               Robert Wille},
      title={Exploiting Arbitrary Paths for the Simulation of Quantum Circuits with Decision Diagrams},
      year={2021},
      eprint={2105.07045},
      archivePrefix={arXiv},
      primaryClass={quant-ph}
}
```

</details>

<details>
<summary>
  [7] https://github.com/taskflow/taskflow
</summary>

</details>

<details>
<summary>
  [8] https://github.com/jcmgray/cotengra
</summary>
</details>