Installation
============

MQT DDSIM is mainly developed as a *C++* library that builds upon `our decision diagram (DD) package <https://github.com/cda-tum/dd_package.git>`_ as well as `our quantum functionality representation (QFR) <https://github.com/cda-tum/qfr.git>`_.
In order to make the tool as accessible as possible, it comes with an easy-to-use Python interface.


User
####

We encourage installing DDSIM via pip (preferably in a `virtual environment <https://docs.python.org/3/library/venv.html>`_):

    .. code-block:: console

        (venv) $ pip install mqt.ddsim

In most practical cases (under 64-bit Linux, MacOS, and Windows), this requires no compilation and merely downloads and installs a platform-specific pre-built wheel.

However, in order to get the best performance out of QCEC and enable platform-specific compiler optimizations that cannot be enabled on portable wheels, it is recommended to build the package from source via:

    .. code-block:: console

        (venv) $ pip install mqt.ddsim --no-binary mqt.ddsim

This requires a C++ compiler supporting C++17, a minimum CMake version of 3.14 and OpenMP.

The library is continuously tested under Linux, MacOS, and Windows using the `latest available system versions for GitHub Actions <https://github.com/actions/virtual-environments>`_.
In order to access the latest build logs, visit `ddsim/actions/workflows/ci.yml <https://github.com/cda-tum/ddsim/actions/workflows/ci.yml>`_.

**Disclaimer**: We noticed some issues when compiling with Microsoft's MSCV compiler toolchain. If you want to start development on this project under Windows, consider using the *clang* compiler toolchain. A detailed description of how to set this up can be found `here <https://docs.microsoft.com/en-us/cpp/build/clang-support-msbuild?view=msvc-160>`_.

eveloper
#########

In order to start developing, clone the DDSIM repository using

    .. code-block:: console

        $ git clone --recurse-submodules https://github.com/cda-tum/ddsim

Note the :code:`--recurse-submodules` flag. It is required to also clone all the required submodules. If you happen to forget passing the flag on your initial clone, you can initialize all the submodules by executing :code:`git submodule update --init --recursive` in the main project directory.

A C++ compiler supporting C++17, a minimum CMake version of 3.14 and OpenMP is required to build the project.
The :code:`ddsim_noise_aware simulator` further requires :code:`Threads::Threads`.

Working on the core C++ library
---------------------------------

Our projects use CMake as the main build configuration tool.
Building a project using CMake is a two-stage process.
First, CMake needs to be configured by calling

    .. code-block:: console

        $ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

This tells CMake to search the current directory :code:`.` (passed via :code:`-S` for source) for a :code:`CMakeLists.txt` file and process it into a directory :code:`build` (passed via :code:`-B`).
The flag :code:`-DCMAKE_BUILD_TYPE=Release` tells CMake to configure a *Release* build (as opposed to, e.g., a *Debug* build).

After configuring with CMake, the project can be built by calling

    .. code-block:: console

        $ cmake --build build --config Release

This tries to build the project in the :code:`build` directory (passed via :code:`--build`).
Some operating systems and developer environments explicitly require a configuration to be set, which is why the :code:`--config` flag is also passed to the build command. The flag :code:`--parallel <NUMBER_OF_THREADS>` may be added to trigger a parallel build.

Building the project this way generates

- the main library :code:`libddsim.a` (Unix) / :code:`ddsim.lib` (Windows) in the :code:`build/src` directory
- a test executable :code:`ddsim_test` containing unit tests in the :code:`build/test` directory (this requires passing :code:`-DBUILD_DDSIM_TESTS=ON` to CMake during configuration)
- the Python bindings library :code:`pyddsim.<...>` in the :code:`build/mqt/ddsim` directory (this requires passing :code:`-DBINDINGS=ON` to CMake during configuration)

Working on the Python module
----------------------------

The :code:`mqt.ddsim` Python module can be conveniently built locally by calling

    .. code-block:: console

        (venv) $ pip install --editable .

The :code:`--editable` flag ensures that changes in the Python code are instantly available without re-running the command.

`Pybind11 <https://pybind11.readthedocs.io/>`_ is used for providing bindings of the C++ core library to Python (see `bindings.cpp <https://github.com/cda-tum/qcec/tree/master/mqt/ddsim/bindings.cpp>`_).
If parts of the C++ code have been changed, the above command has to be run again to make the changes visible in Python.
