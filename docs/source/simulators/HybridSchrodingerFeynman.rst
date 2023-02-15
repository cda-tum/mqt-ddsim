Hybrid Schrödinger-Feynman Simulator
====================================

Hybrid Schrödinger-Feynman approaches strive to use all the available memory and processing units in order to efficiently simulate quantum circuits which would
(1) run into memory bottlenecks using Schrödinger-style simulation, or
(2) take exceedingly long using Feynman-style path summation—eventually trading-off the respective memory and runtime requirements.

Usage in Python
###############

Available backends in the provider :code:`DDSIMProvider`:

- :code:`hybrid_qasm_simulator` to simulate and sample from the resulting state
- :code:`hybrid_statevector_simulator` to simulate and explicitly return the state vector (which will require an exponential amount of memory in the number of qubits)


Usage as Standalone Executable
##############################

Please see :doc:`CircuitSimulator` on how to build the :code:`ddsim_simple` (the name stuck due to historical reasons ;) ) CMake target.

Launching the standalone executable with :code:`-h` parameter shows the available options (truncated to highlight the hybrid settings):

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
    [...]

If you are using this simulator, please cite :cite:p:`DBLP:conf/qce/BurgholzerBW21`.
