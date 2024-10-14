Noise-aware Simulator
=====================

The tool also supports noise-aware quantum circuit simulation, based on a stochastic approach. It currently supports
global decoherence and gate error noise effects. A detailed summary of the simulator is presented
in :cite:p:`grurl2022`. Note that the simulator currently does not support simulating the integrated
algorithms.

Usage in Python
###############

.. warning::
    Currently the noise-aware simulator is not exposed via the python bindings.

Usage as Standalone Executable
##############################

Building the simulator requires :code:`Threads::Threads`. It can be built by executing

.. code-block:: console

    $ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
    $ cmake --build build --config Release --target ddsim_noise_aware


The simulator provides a help function which is called in the following way:

.. code-block:: console

    $ ./build/apps/ddsim_noise_aware --help
      see for more information https://www.cda.cit.tum.de/
      Usage:
        MQT DDSIM [OPTION...]

        -h, --help                              produce help message
            --seed arg                          seed for random number generator (default zero is possibly directly used as seed!)(default: 0)
            --pm                                print measurements
            --ps                                print simulation stats (applied gates, sim. time, and maximal size of the DD)
            --verbose                           Causes some simulators to print additional information to STDERR
            --simulate_file arg                 simulate a quantum circuit given by file (detection by the file extension)
            --step_fidelity arg                 target fidelity for each approximation run (>=1 = disable approximation) (default: 1.0)
            --steps arg                         number of approximation steps (default: 1)
            --noise_effects arg                 Noise effects (A (=amplitude damping),D (=depolarization),P (=phase flip)) in the form of a character string describing the noise effects (default: APD)
            --noise_prob arg                    Probability for applying noise. (default: 0.001)
            --noise_prob_t1 arg                 Probability for applying amplitude damping noise (default:2 x noise_prob)
            --noise_prob_multi arg              Noise factor for multi qubit operations (default: 2)
            --use_density_matrix_simulator      Set this flag to use the density matrix simulator. Per default the stochastic simulator is used
            --shots arg                         Specify the number of shots that shall be generated (default: 0)


An example run of the stochastic simulator, with 1000 samples, amplitude damping, phase flip, and depolarization error enabled (each with a probability of 0.1% whenever a gate is applied) looks like this:

.. code-block:: console

    $./build/apps/ddsim_noise_aware ./build/apps/ddsim_noise_aware --noise_effects APD --noise_prob 0.001 --shots 1000 --simulate_file adder_n4.qasm  --pm --ps
   {
     "measurement_results": {
       "0000": 15,
       "0001": 36,
       "0010": 3,
       "0011": 3,
       "0100": 3,
       "0110": 4,
       "0111": 4,
       "1000": 9,
       "1001": 915,
       "1010": 2,
       "1011": 1,
       "1101": 4,
       "1111": 1
     },
     "statistics": {
       "applied_gates": 27,
       "approximation_runs": "0.000000",
       "benchmark": "stoch_adder_n4",
       "max_matrix_nodes": 0,
       "max_nodes": 0,
       "mean_stoch_run_time": "0.000000",
       "n_qubits": 4,
       "parallel_instances": "8",
       "perfect_run_time": "0.000000",
       "seed": "0",
       "simulation_time": 0.2757418751716614,
       "step_fidelity": "1.000000",
       "stoch_runs": "1000",
       "stoch_wall_time": "0.275722",
       "threads": "8"
     }
   }

The deterministic simulator is run when the flag "--use_density_matrix_simulator" is set. The same run from above, using the deterministic simulator would look like this:

.. code-block:: console

    $ ./build/apps/ddsim_noise_aware --noise_effects APD --noise_prob 0.001 --shots 1000 --simulate_file adder_n4.qasm  --pm --ps --use_density_matrix_simulator
  {
    "measurement_results": {
      "0000": 12,
      "0001": 40,
      "0010": 1,
      "0011": 5,
      "0100": 3,
      "0101": 1,
      "0110": 1,
      "0111": 7,
      "1000": 12,
      "1001": 912,
      "1010": 3,
      "1011": 1,
      "1101": 2
    },
    "statistics": {
      "active_matrix_nodes": 0,
      "active_nodes": 22,
      "applied_gates": 27,
      "benchmark": "adder_n4",
      "max_matrix_nodes": 0,
      "n_qubits": 4,
      "seed": "0",
      "simulation_time": 0.0007002829806879163
    }
  }
