Simulators
==========

DDSIM offers different kinds of simulators to use. The following documentation gives an overview of the different simulator
followed by brief demonstatrations of the usage in Python and as standalone executable.

All simulators immediately or with steps in between inherit from the abstract :code:`Simulator` class that provides
basic functionality used by multiple actual simulators.

.. toctree::
   :maxdepth: 4

   simulators/CircuitSimulator
   simulators/NoiseAwareSimulator
   simulators/SimulationPathFramework
   simulators/HybridSchrodingerFeynman
