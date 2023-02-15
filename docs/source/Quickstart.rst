Quickstart
==========

Python
######

The python bindings are offered to give an easy start into using DDSIM.
Further, the integration with IBM Qiskit allows users to switch to DDSIM with just a few lines of code.

.. code-block:: python

    from qiskit import *
    from mqt import ddsim

    circ = QuantumCircuit(3)
    circ.h(0)
    circ.cx(0, 1)
    circ.cx(0, 2)

    print(circ.draw(fold=-1))

    provider = ddsim.DDSIMProvider()
    print([str(b) for b in provider.backends()])
    backend = provider.get_backend("qasm_simulator")

    job = execute(circ, backend, shots=10000)
    counts = job.result().get_counts(circ)
    print(counts)

The important lines are :code:`provider = ddsim.DDSIMProvider()` to get an instance of the provider and
:code:`backend = provider.get_backend("qasm_simulator")` to actually retrieve the backend that is passed to Qiskit.
The line in between prints all the available backends which are designed for use cases that are covered in the *Usage in Python* sections
of the :ref:`Simulators`.
Running the code above gives the following output:

.. code-block::

         ┌───┐
    q_0: ┤ H ├──■────■──
         └───┘┌─┴─┐  │
    q_1: ─────┤ X ├──┼──
              └───┘┌─┴─┐
    q_2: ──────────┤ X ├
                   └───┘
    ['qasm_simulator', 'statevector_simulator', 'hybrid_qasm_simulator', 'hybrid_statevector_simulator', 'path_sim_qasm_simulator', 'path_sim_statevector_simulator', 'unitary_simulator']
    {'0': 5050, '111': 4950}


Standalone
##########

The simulator also comes with several stand alone executables that take input via the command line and output their
results as json object.

The following example assumes the simulator has been build as described in :ref:`Building from Cloned Repository`.
Afterwards, there will be an executable :code:`ddsim_simple` in :code:`build/apps/` which can be used to simulate
(among others) OpenQASM files. All executables support the :code:`--help` parameter to list their options.

.. code-block:: console

    $ build/apps/ddsim_simple --help
    MQT DDSIM by https://www.cda.cit.tum.de/ -- Allowed options:
      -h [ --help ]          produce help message
      --shots arg            number of measurements (if the algorithm
                             does not contain non-unitary gates, weak
                             simulation is used) (default: 0)
      --ps                   print simulation stats (applied gates, sim.
                             time, and maximal size of the DD)
      --simulate_file arg    simulate a quantum circuit given by file
                             (detection by the file extension)
    [...]
    $ build/apps/ddsim_simple --shots 1000 --ps --simulate_file path/to/ghz_03.qasm
    {
      "statistics": {
        "applied_gates": 3,
        "approximation_runs": "0",
        "benchmark": "ghz_03",
        "distinct_results": 2,
        "final_fidelity": "1.000000",
        "max_nodes": 7,
        "n_qubits": 3,
        "seed": "0",
        "shots": 1000,
        "simulation_time": 0.00044452399015426636,
        "single_shots": "1",
        "step_fidelity": "1.000000"
      }
    }
