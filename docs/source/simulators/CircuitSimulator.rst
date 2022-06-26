CircuitSimulator
================

To build the simulator execute, build the :code:`ddsim_simple` (the name stuck due to historical reasons ;) ) CMake target and run the resulting executable with options according to your needs.

The standalone executable is launched in the following way, showing available options:

.. code-block:: console

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

The output is JSON-formatted as shown below (with hopefully intuitive naming).

.. code-block:: console

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


Quickstart Guide
################

Execute the following lines to get the simulator running in no time:

.. code-block:: console

    $ git clone --recurse-submodules https://github.com/cda-tum/ddsim/
    [...]

    $ cd ddsim

    ddsim/ $ cmake -S . -B build
    -- Build files have been written to: /.../build

    ddsim/ $ cmake --build build --config Release --target ddsim_simple
    [...]
    [100%] Built target ddsim_simple

    ddsim/ $ build/ddsim_simple --help
    MQT DDSIM by https://www.cda.cit.tum.de/ -- Allowed options:
      -h [ --help ]                         produce help message
    [...]

If you are using this simulator, please cite :cite:p:`zulehner2019advanced`.
