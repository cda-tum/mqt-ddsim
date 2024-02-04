Installation
============

MQT DDSIM is mainly developed as a C++ library that builds upon `MQT Core <https://github.com/cda-tum/mqt-core.git>`_, which forms the backbone of the `MQT <https://mqt.readthedocs.io>`_.
In order to make the tool as accessible as possible, it comes with an easy-to-use Python interface.

We encourage installing DDSIM via pip (preferably in a `virtual environment <https://docs.python.org/3/library/venv.html>`_):

    .. code-block:: console

        (venv) $ pip install mqt.ddsim

In most practical cases (under 64-bit Linux, MacOS, and Windows), this requires no compilation and merely downloads and installs a platform-specific pre-built wheel.

.. note::
    In order to set up a virtual environment on Linux or MacOS, you can use the following commands:

    .. code-block:: console

        $ python3 -m venv venv
        $ source venv/bin/activate

    If you are using Windows, you can use the following commands instead:

    .. code-block:: console

        $ python3 -m venv venv
        $ venv\Scripts\activate.bat

    It is recommended to make sure that you are using the latest version of pip, setuptools, and wheel before trying to install the project:

    .. code-block:: console

        (venv) $ pip install --upgrade pip setuptools wheel

.. warning::
    Python 3.7 will reach end-of-life in June 2023 and MQT phases out support starting now.
    We strongly recommend that users upgrade to a more recent version of Python to ensure compatibility and continue receiving updates and support.
    Thank you for your understanding.


A Detailed Walk Through
#######################
First, save the following lines as :code:`ghz_3.py` in a folder where you want to install the simulator and run the example.
Then proceed as described in the section on your operating system.

    .. code-block:: python

        from qiskit import *
        from mqt import ddsim

        circ = QuantumCircuit(3)
        circ.h(0)
        circ.cx(0, 1)
        circ.cx(0, 2)

        print(circ.draw(fold=-1))

        backend = ddsim.DDSIMProvider().get_backend("qasm_simulator")

        job = backend.run(circ, shots=10000)
        counts = job.result().get_counts(circ)
        print(counts)

The following snippet shows the installation process on Linux (more specifically Ubuntu 20.04 LTS) from setting up the virtual environment to running a small example program.

    .. code-block:: console

        $ python3 -m venv venv
        $ . venv/bin/activate
        (venv) $ pip install -U pip setuptools wheel
        (venv) $ pip install mqt.ddsim
        (venv) $ python3 ghz_3.py
                ┌───┐           ░ ┌─┐
           q_0: ┤ H ├──■────■───░─┤M├──────
                └───┘┌─┴─┐  │   ░ └╥┘┌─┐
           q_1: ─────┤ X ├──┼───░──╫─┤M├───
                     └───┘┌─┴─┐ ░  ║ └╥┘┌─┐
           q_2: ──────────┤ X ├─░──╫──╫─┤M├
                          └───┘ ░  ║  ║ └╥┘
        meas: 3/═══════════════════╩══╩══╩═
                                   0  1  2
        {'000': 50149, '111': 49851}


Building Wheel from Scratch for Performance
###########################################

In order to get the best performance out of DDSIM and enable platform-specific compiler optimizations that cannot be enabled on portable wheels, it is recommended to build the package from source via:

    .. code-block:: console

        (venv) $ pip install mqt.ddsim --no-binary mqt.ddsim

This requires a `C++ compiler <https://en.wikipedia.org/wiki/List_of_compilers#C++_compilers>`_ supporting C++17 and a minimum `CMake <https://cmake.org/>`_ version of 3.19.

The library is continuously tested under Linux, MacOS, and Windows using the `latest available system versions for GitHub Actions <https://github.com/actions/virtual-environments>`_.
In order to access the latest build logs, visit `mqt-ddsim/actions/workflows/ci.yml <https://github.com/cda-tum/mqt-ddsim/actions/workflows/ci.yml>`_.

.. note::
    We noticed some issues when compiling with Microsoft's MSCV compiler toolchain.
    If you want to start development on this project under Windows, consider using the *clang* compiler toolchain.
    A detailed description of how to set this up can be found `here <https://docs.microsoft.com/en-us/cpp/build/clang-support-msbuild?view=msvc-160>`_.


.. _Building from Cloned Repository:

Building from Cloned Repository
###############################

If you want to have the latest features that may not be released to PyPI yet, you can also clone the repository and
build from source.

The following snippet clones the repository including the submodules and builds all targets (library, standalone apps, and
python bindings) in *Release* mode for DDSIM.

.. code-block:: console

    $ git clone --recurse-submodules https://github.com/cda-tum/mqt-ddsim
    [...]
    $ cd mqt-ddsim
    $ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBINDINGS=ON -DBUILD_DDSIM_TESTS=ON
    [...]
    $ cmake --build build --config Release
    [...]

Afterwards the standalone executables can be found in :code:`build/apps/`.
For the python bindings it's recommended to install via pip in a virtual environment.
To do so, run :code:`pip install .` instead of the cmake calls above.
