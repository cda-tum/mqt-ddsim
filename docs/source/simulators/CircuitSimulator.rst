Circuit Simulator
=================

The *Circuit Simulator* is the "default" simulator in DDSIM.
It can handle all input supplied as :code:`QuantumComputation` from the QFR. This includes, but is not limited to,
OpenQASM and real files as well as pre-defined algorithm like QFT, Grover, and random clifford circuits.

Capabilities:

- Exact Simulation :cite:p:`zulehner2019advanced,DBLP:conf/iccad/ZulehnerHW19`
- Weak Simulation :cite:p:`DBLP:conf/dac/HillmichMW20`
- Approximate Simulation with different strategies :cite:p:`10.1145/3530776,DBLP:conf/date/HillmichKMW21`

    - Fidelity-driven
    - Memory-driven


Usage in Python
###############

Available backends in the provider :code:`DDSIMProvider`:

- :code:`qasm_simulator` to simulate and sample from the resulting state
- :code:`statevector_simulator` to simulate and explicitly return the state vector (which will require an exponential amount of memory in the number of qubits)

Using the Circuit Simulator without Qiskit is also possible by passing a file name like in the following example:

.. code-block:: python

    from mqt import ddsim

    sim = ddsim.CircuitSimulator("ghz_03.qasm")
    result = sim.simulate(1000)  # 1000 shots
    # results will contain a dictional like {'000': 510, '111': 490}

This of course assumes a file :code:`ghz_03.qasm` exists.
Alternatively, Qiskit :code:`QuantumCircuit` objects can be passed directly as well.

Usage as Standalone Executable
##############################

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
    --simulate_qft arg                    simulate Quantum Fourier Transform for given number of qubits
    --simulate_ghz arg                    simulate state preparation of GHZ state for given number of qubits
    --step_fidelity arg (=1)              target fidelity for each approximation run (>=1 = disable approximation)
    --steps arg (=1)                      number of approximation steps

.. note::
    The parameter list is abbreviated to show only the parameters relevant for the Circuit Simulator.

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



If you are using this simulator, please cite :cite:p:`zulehner2019advanced`.
