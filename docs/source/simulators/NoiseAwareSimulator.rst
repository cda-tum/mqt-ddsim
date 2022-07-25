Executable Noise-aware Simulator
================================

The tool also supports noise-aware quantum circuit simulation, based on a stochastic approach. It currently supports
global decoherence and gate error noise effects. A detailed summary of the simulator is presented
in :cite:p:`grurl2022`. Note that the simulator currently does not support simulating the integrated
algorithms.

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
	-h, --help                  produce help message
	  --seed arg              seed for random number generator (default zero is possibly directly used as seed!) (default: 0)
	  --pm                    print measurements
	  --ps                    print simulation stats (applied gates, sim. time, and maximal size of the DD)
	  --verbose               Causes some simulators to print additional  information to STDERR
	  --simulate_file arg     simulate a quantum circuit given by file (detection by the file extension)
	  --step_fidelity arg     target fidelity for each approximation run (>=1 = disable approximation) (default: 1.0)
	  --steps arg             number of approximation steps (default: 1)
	  --noise_effects arg     Noise effects (A (=amplitude damping),D (=depolarization),P (=phase flip)) in the form of a character string describing the noise effects (default: APD)
	  --noise_prob arg        Probability for applying noise. (default: 0.001)
	  --noise_prob_t1 arg     Probability for applying amplitude damping noise (default:2 x noise_prob)
	  --noise_prob_multi arg  Noise factor for multi qubit operations (default: 2)
	  --unoptimized_sim       Use unoptimized scheme for stochastic/deterministic noise-aware simulation
	  --stoch_runs arg        Number of stochastic runs. When the value is 0, the deterministic simulator is started. (default: 0)
	  --properties arg        Comma separated list of tracked amplitudes, when conducting a stochastic simulation. The "-" operator can be used to specify a range. (default: 0-100)


An example run of the stochastic simulator, with 1000 samples, amplitude damping, phase flip, and depolarization error enabled (each with a probability of 0.1% whenever a gate is applied) looks like this:

.. code-block:: console

    $./build/apps/ddsim_noise_aware --noise_effects APD --noise_prob 0.001 --stoch_runs 1000 --simulate_file adder_n4.qasm  --pm --ps
    {
      "measurement_results": {
        "0000": 0.010506209312687873,
        "0001": 0.011493790687312124,
        "0010": 0.002500492484500565,
        "0011": 0.0004995075154994353,
        "0100": 0.0015015015015015015,
        "0101": 0.0014984984984984986,
        "0110": 0.011499960617703234,
        "0111": 0.0005000393822967641,
        "1000": 0.04201375737631414,
        "1001": 0.8969862426236844,
        "1010": 0.002502502502502502,
        "1011": 0.0024974974974974976,
        "1100": 0.004003012087216088,
        "1101": 0.0049969879127839106,
        "1110": 0.006500446429090525,
        "1111": 0.0004995535709094751
      },
      "statistics": {
        "applied_gates": 23,
        "approximation_runs": "0.000000",
        "benchmark": "stoch_adder_n4",
        "max_matrix_nodes": 0,
        "max_nodes": 0,
        "mean_stoch_run_time": "0.000000",
        "n_qubits": 4,
        "parallel_instances": "8",
        "perfect_run_time": "0.000000",
        "seed": "0",
        "simulation_time": 0.6007186770439148,
        "step_fidelity": "1.000000",
        "stoch_runs": "1000",
        "stoch_wall_time": "0.600710",
        "threads": "8"
      }
    }
    
The deterministic simulator is run when "stochastic_runs" is set to 0. The same run from above, using the deterministic simulator would look like this:

.. code-block:: console

    ./build/apps/ddsim_noise_aware --noise_effects APD --noise_prob 0.001 --stoch_runs 0 --simulate_file adder_n4.qasm  --pm --ps
    {
      "measurement_results": {
        "0000": 0.013477634679595526,
        "0001": 0.013345991328932352,
        "1000": 0.03746879852829717,
        "1001": 0.9082983936899753
      },
      "statistics": {
        "active_matrix_nodes": 0,
        "active_nodes": 22,
        "applied_gates": 23,
        "benchmark": "adder_n4",
        "max_matrix_nodes": 0,
        "n_qubits": 4,
        "seed": "0",
        "simulation_time": 0.003795960918068886
      }
    }



