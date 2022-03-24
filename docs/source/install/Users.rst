Installation for Users
======================

MQT DDSIM is mainly developed as a C++ library that builds upon `our decision diagram (DD) package <https://github.com/cda-tum/dd_package.git>`_ as well as `our quantum functionality representation (QFR) <https://github.com/cda-tum/qfr.git>`_.
In order to make the tool as accessible as possible, it comes with an easy-to-use Python interface.


We encourage installing DDSIM via pip (preferably in a `virtual environment <https://docs.python.org/3/library/venv.html>`_):

    .. code-block:: console

        (venv) $ pip install mqt.ddsim

In most practical cases (under 64-bit Linux, MacOS, and Windows), this requires no compilation and merely downloads and installs a platform-specific pre-built wheel.

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

        backend = ddsim.DDSIMProvider().get_backend('qasm_simulator')

        job = execute(circ, backend, shots=10000)
        counts = job.result().get_counts(circ)
        print(counts)

Linux
-----

The following snippet shows the installation process on Linux (more specifically Ubuntu 20.04 LTS) from setting up the virtual environment to running a small example program.

    .. code-block:: console

        $ python3 -m venv venv
        $ . venv/bin/activate
        (venv) $ pip install -U pip setuptools wheel
        (venv) $ pip install mqt.ddsim qiskit-terra
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

Possibly requires the installation of :code:`python3-venv` (:code:`sudo apt install python3-venv`).

macOS
-----

The following snippet shows the installation process on macOS from setting up the virtual environment to running a small example program.

    .. code-block:: console

        $ python3 -m venv venv
        $ . venv/bin/activate
        (venv) $ pip install -U pip setuptools wheel
        (venv) $ pip install mqt.ddsim qiskit-terra
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

Windows
-------
The following snippet shows the installation process on Windows from setting up the virtual environment to running a small example program.

    .. code-block:: console

        > python3 -m venv venv
        > .\venv\bin\activate
        (venv) > pip install -U pip setuptools wheel
        (venv) > pip install mqt.ddsim qiskit-terra
        (venv) > python3 ghz_3.py
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

Building from Scratch for Performance
######################################

However, in order to get the best performance out of DDSIM and enable platform-specific compiler optimizations that cannot be enabled on portable wheels, it is recommended to build the package from source via:

    .. code-block:: console

        (venv) $ pip install mqt.ddsim --no-binary mqt.ddsim

This requires a C++ compiler supporting C++17, a minimum `CMake <https://cmake.org/>`_ version of 3.14 and `OpenMP <https://www.openmp.org/>`_.

The library is continuously tested under Linux, MacOS, and Windows using the `latest available system versions for GitHub Actions <https://github.com/actions/virtual-environments>`_.
In order to access the latest build logs, visit `ddsim/actions/workflows/ci.yml <https://github.com/cda-tum/ddsim/actions/workflows/ci.yml>`_.

**Disclaimer**: We noticed some issues when compiling with Microsoft's MSCV compiler toolchain.
If you want to start development on this project under Windows, consider using the *clang* compiler toolchain.
A detailed description of how to set this up can be found `here <https://docs.microsoft.com/en-us/cpp/build/clang-support-msbuild?view=msvc-160>`_.