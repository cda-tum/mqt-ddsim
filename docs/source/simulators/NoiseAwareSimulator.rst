Executable Noise-aware Simulator
================================

The tool also supports noise-aware quantum circuit simulation, based on a stochastic approach. It currently supports
global decoherence and gate error noise effects. A detailed summary of the simulator is presented
in :cite:p:`grurl2020stochastic`. Note that the simulator currently does not support simulating the integrated
algorithms.

Building the simulator requires :code:`Threads::Threads`. It can be built by executing

.. code-block:: console

    $ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
    $ cmake --build build --config Release --target ddsim_noise_aware


The simulator provides a help function which is called in the following way:

.. code-block:: console

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


An example run, with amplitude damping, phase flip, and depolarization error (each with a probability of 0.1% whenever a gate is applied) looks like this:

.. code-block:: console

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