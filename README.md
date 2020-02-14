[![Build Status](https://travis-ci.com/iic-jku/ddsim.svg?branch=master)](https://travis-ci.com/iic-jku/ddsim)
[![codecov](https://codecov.io/gh/iic-jku/ddsim/branch/master/graph/badge.svg)](https://codecov.io/gh/iic-jku/ddsim)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# DDSIM - A quantum simulator based on decision diagrams written in C++

A tool for quantum circuit simulation by the [Institute for Integrated Circuits](http://iic.jku.at/eda/) at the [Johannes Kepler University Linz](https://jku.at).

Developers: Stefan Hillmich, Lukas Burgholzer, Alwin Zulehner, and Robert Wille.

The tool builds upon [our quantum functionality representation (QFR)](https://github.com/iic-jku/qfr.git) which in turns builds on [our decision diagram (DD) package](https://github.com/iic-jku/dd_package.git).

For more information, please visit [iic.jku.at/eda/research/quantum_simulation](http://iic.jku.at/eda/research/quantum_simulation).

If you have any questions, feel free to contact us via [iic_quantum@jku.at](mailto:iic_quantum@jku.at) or by creating an issue on GitHub.

## Usage

This tool can be used for simulating quantum circuits provided in any of the following formats:
* `Real` from [RevLib](http://revlib.org),
* `OpenQASM` used by IBM's [Qiskit](https://github.com/Qiskit/qiskit) (currently, we do not support intermediate measurements!),
* `GRCS` as available on https://github.com/sboixo/GRCS
The format is automatically detected through the file extension.


The following additional algorithms are integrated in [QFR](https://github.com/iic-jku/qfr.git) and hence available in the simulator as well:
* Quantum Fourier Transformation
* Grover's search

For details on the available methods we refer to [iic.jku.at/eda/research/quantum_simulation](http://iic.jku.at/eda/research/quantum_simulation).

It can either be used as a **standalone executable** with command-line interface, or as a **library** for the incorporation in other projects.
- The standalone executable is launched in the following way, showing available options:
    ```commandline
    ddsim_simple --help
    ```
   
- The library can be used by including, for example, the```SimpleSimulator.hpp``` header file and
    ```c++
    std::string file1 = "PATH_TO_FILE_1.EXT";
    qc::QuantumComputation qc1(file1);
    
    qc::SimpleSimulator sim(qc1);
    sim.Simulate();
    auto samples = sim.MeasureAllNonCollapsing(1000);
    /* Use the results */
    ```
  
### System requirements

Building (and running) is continuously tested under Linux (Ubuntu 18.04) using gcc-7.4, gcc-9 and clang-9, MacOS (Mojave 10.14) using AppleClang and gcc-9, and Windows using MSVC 15.9. 
However, the implementation should be compatible with any current C++ compiler supporting C++14 and a minimum CMake version of 3.10.

`boost/program_options >= 1.50` is required for building the executable simulator.

### Build and Run
For building the library alone the CMake target `ddsim` is available, i.e.,
```commandline
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target ddsim --config Release
```

Windows users need to configure CMake by calling 

`cmake .. -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release`

instead.

To build the executable simulator, build the `ddsim_simple` CMake target (which requires `boost/program_options`) and run the resulting executable with options according to your needs. 
The output is JSON-formatted as shown below (with hopefully intuitive naming, the `dummy` object is just for easier handling of trailing commas).

```commandline
# (still in the build directory after building the ddsim target from above)
cmake --build . --target ddsim_simple --config Release
./ddsim_simple --simulate_file entanglement_4.real --display_vector --shots 1000 --ps
{
  "measurements": {
    "0000": 504,
    "1111": 496
  },
  "state_vector": [
    √½,
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
    √½
  ],
  "statistics": {
    "simulation_time": 0.000147604,
    "measurement_time": 0.000715286,
    "shots": 1000,
    "benchmark": "simple",
    "n_qubits": 4,
    "applied_gates": 4,
    "max_nodes": 9,
    "seed": 0
  },
  "dummy": 0
}

Process finished with exit code 0

```

The repository also includes some (rudimentary) unit tests (using GoogleTest), which aim to ensure the correct behaviour of the tool. They can be built and executed in the following way:
```commandline
cmake --build . --target ddsim_test --config Release
./ddsim_test
```

## Reference

If you use our tool for your research, we will be thankful if you refer to it by citing the following publications:

```bibtex
@article{zulehner2019package,
    title={How to Efficiently Handle Complex Values? Implementing Decision Diagrams for Quantum Computing},
    author={Zulehner, Alwin and Hillmich, Stefan and Wille, Robert},
    journal={International Conference on Computer-Aided Design (ICCAD)},
    year={2019}
}

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
