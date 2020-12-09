[![Build Status](https://travis-ci.com/iic-jku/ddsim.svg?branch=master)](https://travis-ci.com/iic-jku/ddsim)
[![codecov](https://codecov.io/gh/iic-jku/ddsim/branch/master/graph/badge.svg)](https://codecov.io/gh/iic-jku/ddsim)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![toolset: JKQ](https://img.shields.io/badge/toolset-JKQ-blue)](https://github.com/iic-jku/jkq)

# JKQ DDSIM - A quantum simulator based on decision diagrams written in C++

A tool for quantum circuit simulation by the [Institute for Integrated Circuits](https://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at) 
and a part of the [JKQ toolset](https://github.com/iic-jku/jkq).

Developers: Stefan Hillmich, Lukas Burgholzer, and Robert Wille.

The tool builds upon [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git) which in turns builds on [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git).

For more information, please visit [iic.jku.at/eda/research/quantum_simulation](https://iic.jku.at/eda/research/quantum_simulation).

If you have any questions, feel free to contact us via [iic-quantum@jku.at](mailto:iic-quantum@jku.at) or by creating an [issue](https://github.com/iic-jku/ddsim/issues) on GitHub.

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

It can either be used as a **standalone executable** with command-line interface, or as a **library** for the incorporation in other projects.
- The standalone executable is launched in the following way, showing available options:
    ```commandline
    $ ./ddsim_simple --help
    JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options:
      -h [ --help ]                         produce help message
      --seed arg (=0)                       seed for random number generator (default zero is directly used as seed!) 
      --shots arg (=0)                      number of measurements on the final quantum state
      --display_vector                      display the state vector
      --ps                                  print simulation stats (applied gates, sim. time, and maximal size of the DD)
      --benchmark                           print simulation stats in a single CSV style line (overrides --ps and suppresses most other output)
      --simulate_file arg                   simulate a quantum circuit given by file (detection by the file extension)
      --simulate_qft arg                    simulate Quantum Fourier Transform for given number of qubits
      --simulate_ghz arg                    simulate state preparation of GHZ state for given number of qubits
      --simulate_grover arg                 simulate Grover's search for given number of qubits with random oracle
      --simulate_grover_emulated arg        simulate Grover's search for given number of qubits with random oracle and emulation
      --simulate_grover_oracle_emulated arg simulate Grover's search for given number of qubits with given oracle and emulation
    ```
   
- The library can be used by including, for example, the```QFRSimulator.hpp``` header file and
    ```c++
    std::string file1 = "PATH_TO_FILE_1.EXT";
    qc::QuantumComputation qc1(file1);
    
    qc::SimpleSimulator sim(qc1);
    sim.Simulate();
    auto samples = sim.MeasureAllNonCollapsing(1000);
    /* Use the results */
    ```
  
### System requirements

Building (and running) is continuously tested under Linux (Ubuntu 18.04) using gcc-7.4, gcc-9 and clang-9, MacOS (Catalina 10.15) using AppleClang, and Windows using MSVC 15.9. 
However, the implementation should be compatible with any current C++ compiler supporting C++14 and a minimum CMake version of 3.10.

`boost/program_options >= 1.50` is required for building the the commandline interface `ddsim_simple` to the simulator.

### Build and Run
For building the library alone the CMake target `ddsim` is available, i.e.,
```commandline
$ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
$ cmake --build build --config Release --target ddsim
```

Windows users need to configure CMake by calling 

```commandline
$ cmake -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release -S . -B build
```

instead.

To build the executable simulator, build the `ddsim_simple` CMake target (which requires `boost/program_options`) and run the resulting executable with options according to your needs. 
The output is JSON-formatted as shown below (with hopefully intuitive naming, the `dummy` object is just for easier handling of trailing commas).

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

The repository also includes some (rudimentary) unit tests (using GoogleTest), which aim to ensure the correct behaviour of the tool. They can be built and executed in the following way:
```commandline
$ cmake --build build --config Release --target ddsim_test
$ ./build/ddsim_test
[...]
```

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

## Reference

If you use our tool for your research, we will be thankful if you refer to it by citing the following publication:

```bibtex
@article{zulehner2019advanced,
    title = {Advanced Simulation of Quantum Computations},
    author = {Zulehner, Alwin and Wille, Robert},
    journal = {Trans. on {CAD} of Integrated Circuits and Systems},
    volume = {38},
    number = {5},
    pages = {848--859},
    year = {2019},
    doi = {10.1109/TCAD.2018.2834427}
}
```
