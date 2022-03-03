Hybrid Schrödinger-Feynman Simulator
====================================

Please see :doc:`CircuitSimulator` on how to build the :code:`ddsim_simple` (the name stuck due to historical reasons ;) ) CMake target.

Launching the standalone executable with :code:`-h` parameter shows the available options (truncated to highlight the hybrid settings):

.. code-block:: console

    $ ./ddsim_simple --help
    MQT DDSIM by https://iic.jku.at/eda/ -- Allowed options:
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