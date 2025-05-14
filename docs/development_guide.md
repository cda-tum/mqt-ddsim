# Development Guide

Ready to contribute to the project? This guide will get you started.

## Initial Setup

1. Get the code

   ::::{tab-set}
   :::{tab-item} External Contribution
   If you do not have write access to the [munich-quantum-toolkit/ddsim](https://github.com/munich-quantum-toolkit/ddsim) repository,
   fork the repository on GitHub (see <https://docs.github.com/en/get-started/quickstart/fork-a-repo>)
   and clone your fork locally.

   ```console
   $ git clone git@github.com:your_name_here/ddsim.git mqt-ddsim
   ```

   :::
   :::{tab-item} Internal Contribution
   If you do have write access to the [munich-quantum-toolkit/ddsim](https://github.com/munich-quantum-toolkit/ddsim) repository,
   clone the repository locally.

   ```console
   $ git clone git@github.com/munich-quantum-toolkit/ddsim.git mqt-ddsim
   ```

   :::
   ::::

2. Change into the project directory

   ```console
   $ cd mqt-ddsim
   ```

3. Create a branch for local development

   ```console
   $ git checkout -b name-of-your-bugfix-or-feature
   ```

   Now you can make your changes locally.

4. If you plan to [work on the Python package](#working-on-the-python-package), we highly recommend using [`uv`](https://docs.astral.sh/uv/).
   It is an extremely fast Python package and project manager, written in Rust and developed by [Astral](https://astral.sh/) (the same team behind [`ruff`](https://docs.astral.sh/ruff/)).
   It can act as a drop-in replacement for `pip` and `virtualenv`, and provides a more modern and faster alternative to the traditional Python package management tools.
   It automatically handles the creation of virtual environments and the installation of packages, and is much faster than `pip`.
   Additionally, it can even set up Python for you if it is not installed yet.

   If you do not have `uv` installed yet, you can install it via:

   ::::{tab-set}
   :::{tab-item} macOS and Linux

   ```console
   $ curl -LsSf https://astral.sh/uv/install.sh | sh
   ```

   :::
   :::{tab-item} Windows

   ```console
   $ powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"
   ```

   :::
   ::::
   Check out their excellent [documentation](https://docs.astral.sh/uv/) for more information.

5. We also highly recommend to install and set up [pre-commit](https://pre-commit.com/) to automatically
   run a set of checks before each commit.

   ::::{tab-set}
   :::{tab-item} via `uv`
   :sync: uv
   The easiest way to install pre-commit is via [uv](https://docs.astral.sh/uv/).

   ```console
   $ uv tool install pre-commit
   ```

   :::
   :::{tab-item} via `brew`
   :sync: brew
   If you use macOS, then pre-commit is in Homebrew, and you can use

   ```console
   $ brew install pre-commit
   ```

   :::
   :::{tab-item} via `pipx`
   :sync: pipx
   If you prefer to use [pipx](https://pypa.github.io/pipx/), you can install pre-commit with

   ```console
   $ pipx install pre-commit
   ```

   :::
   :::{tab-item} via `pip`
   :sync: pip
   If you prefer to use regular `pip` (preferably in a virtual environment), you can install pre-commit with

   ```console
   $ pip install pre-commit
   ```

   :::
   ::::

   Afterwards, you can install the pre-commit hooks with

   ```console
   $ pre-commit install
   ```

## Working on the C++ library

Building the project requires a C++ compiler supporting _C++17_ and CMake with a minimum version of _3.24_.
As of 2025, our CI pipeline on GitHub continuously tests the library under the following matrix of systems and compilers:

- Ubuntu 24.04 with GCC 13 or Clang 18 on x86_64 and arm64
- Ubuntu 22.04 with GCC 11 and Clang 14 on x86_64 and arm64
- macOS 13 with AppleClang 15 or GCC 14 on x86_64
- macOS 14 with AppleClang 15 or GCC 14 on arm64
- macOS 15 with AppleClang 16 or GCC 14 on arm64
- Windows 2022 with MSVC 19.42 or Clang 18 on x86_64
- Windows 2025 with MSVC 19.42 or Clang 18 on x86_64

To access the latest build logs, visit the [GitHub Actions page](https://github.com/munich-quantum-toolkit/ddsim/actions/workflows/ci.yml).

We are not aware of any issues with other compilers or operating systems.
If you encounter any problems, please [open an issue](https://github.com/munich-quantum-toolkit/ddsim/issues) and let us know.

### Configure and Build

:::{tip}
We recommend using an IDE like [CLion](https://www.jetbrains.com/clion/) or [Visual Studio Code](https://code.visualstudio.com/) for development.
Both IDEs have excellent support for CMake projects and provide a convenient way to run CMake and build the project.
If you prefer to work on the command line, the following instructions will guide you through the process.
:::

Our projects use _CMake_ as the main build configuration tool.
Building a project using CMake is a two-stage process.
First, CMake needs to be _configured_ by calling

```console
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

This tells CMake to

- search the current directory {code}`.` (passed via {code}`-S`) for a {code}`CMakeLists.txt` file.
- process it into a directory {code}`build` (passed via {code}`-B`).
- the flag {code}`-DCMAKE_BUILD_TYPE=Release` tells CMake to configure a _Release_ build (as opposed to, e.g., a _Debug_ build).

After configuring with CMake, the project can be built by calling

```console
$ cmake --build build --config Release
```

This tries to build the project in the {code}`build` directory (passed via {code}`--build`).
Some operating systems and development environments explicitly require a configuration to be set, which is why the {code}`--config` flag is also passed to the build command. The flag {code}`--parallel <NUMBER_OF_THREADS>` may be added to trigger a parallel build.

Building the project this way generates

- the main project libraries in the {code}`build/src` directory
- some test executables in the {code}`build/test` directory

:::{note}
The project uses CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) module to download and build its dependencies.
As such, the configuration step requires an active internet connection (at least the first time it is run) to download the dependencies.
Specifically, the project downloads

- [nlohmann/json](https://github.com/nlohmann/json) for JSON serialization and deserialization (roughly 100 KB)
- [Boost Multiprecision](https://github.com/boostorg/multiprecision) for arbitrary precision arithmetic (roughly 4 MB)
- [GoogleTest](https://github.com/google/googletest) for unit testing (roughly 1 MB)

There are multiple ways to avoid these downloads:

- If you have the dependencies installed on your system, `FetchContent` will use them and not download anything.
- If you have a local copy of the dependencies, for example, from a previous build or another project, you can point `FetchContent`
  to them by passing [`-DFETCHCONTENT_SOURCE_DIR_<uppercaseName>`](https://cmake.org/cmake/help/latest/module/FetchContent.html#variable:FETCHCONTENT_SOURCE_DIR_%3CuppercaseName%3E) to the CMake configure step.
- You can pass `-DUSE_SYSTEM_BOOST=ON` to the CMake configure step to use the Boost installation on your system and avoid downloading Boost multiprecision.
  :::

### C++ Testing and Code Coverage

We use the [GoogleTest](https://google.github.io/googletest/primer.html) framework for unit testing of the C++ library.
All tests are contained in the {code}`test` directory, which is further divided into subdirectories for different parts of the library.
You are expected to write tests for any new features you implement and ensure that all tests pass.
Our CI pipeline on GitHub will also run the tests and check for any failures.
It will also collect code coverage information and upload it to [Codecov](https://codecov.io/gh/munich-quantum-toolkit/ddsim).
Our goal is to have new contributions at least maintain the current code coverage level, while striving for covering as much of the code as possible.
Try to write meaningful tests that actually test the correctness of the code and not just exercise the code paths.

Most IDEs like [CLion](https://www.jetbrains.com/clion/) or [Visual Studio Code](https://code.visualstudio.com/) provide a convenient way to run the tests directly from the IDE.
If you prefer to run the tests from the command line, you can use CMake's test runner [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html).
To run the tests, call

```console
$ ctest -C Release --test-dir build
```

from the main project directory after building the project (as described above).

:::{tip}
If you want to disable configuring and building the C++ tests, you can pass `-DBUILD_MQT_DDSIM_TESTS=OFF` to the CMake configure step.
:::

### C++ Code Formatting and Linting

This project mostly follows the [LLVM Coding Standard](https://llvm.org/docs/CodingStandards.html), which is a set of guidelines for writing C++ code.
To ensure the quality of the code and that it conforms to these guidelines, we use

- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) -- a static analysis tool that checks for common mistakes in C++ code, and
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html) -- a tool that automatically formats C++ code according to a given style guide.

Common IDEs like [CLion](https://www.jetbrains.com/clion/) or [Visual Studio Code](https://code.visualstudio.com/) have plugins that can automatically run clang-tidy on the code and automatically format it with clang-format.

- If you are using CLion, you can configure the project to use the {code}`.clang-tidy` and {code}`.clang-format` files in the project root directory.
- If you are using Visual Studio Code, you can install the [clangd extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd).

They will automatically execute clang-tidy on your code and highlight any issues.
In many cases, they also provide quick-fixes for these issues.
Furthermore, they provide a command to automatically format your code according to the given style.

:::{note}
After configuring CMake, you can run clang-tidy on a file by calling

```console
$ clang-tidy <FILE> -- -I <PATH_TO_INCLUDE_DIRECTORY>
```

where {code}`<FILE>` is the file you want to analyze and {code}`<PATH_TO_INCLUDE_DIRECTORY>` is the path to the {code}`include` directory of the project.
:::

Our pre-commit configuration also includes clang-format.
If you have installed pre-commit, it will automatically run clang-format on your code before each commit.
If you do not have pre-commit setup, the [pre-commit.ci](https://pre-commit.ci) bot will run clang-format on your code
and automatically format it according to the style guide.

:::{tip}
Remember to pull the changes back into your local repository after the bot has formatted your code to avoid merge conflicts.
:::

Our CI pipeline will also run clang-tidy over the changes in your pull request and report any issues it finds.
Due to technical limitations, the workflow can only post pull request comments if the changes are not coming from a fork.
If you are working on a fork, you can still see the clang-tidy results either in the GitHub Actions logs,
on the workflow summary page, or in the "Files changed" tab of the pull request.

### C++ Documentation

Historically, the C++ part of the code base has not been sufficiently documented.
Given the substantial size of the code base, we have set ourselves the goal to improve the documentation over time.
We expect any new additions to the code base to be documented using [Doxygen](https://www.doxygen.nl/index.html) comments.
When touching existing code, we encourage you to add Doxygen comments to the code you touch or refactor.

For some tips on how to write good Doxygen comments, see the [Doxygen Manual](https://www.doxygen.nl/manual/docblocks.html).

The C++ API documentation is integrated into the overall documentation that we host on ReadTheDocs using the
[breathe](https://breathe.readthedocs.io/en/latest/) extension for Sphinx.
See the [Working on the Documentation](#working-on-the-documentation) section for more information on how to build the documentation.

## Working on the Python package

We use [pybind11](https://pybind11.readthedocs.io/en/stable) to expose large parts of the C++ core library to Python.
This allows to keep the performance critical parts of the code in C++ while providing a convenient interface for Python users.
All code related to C++-Python bindings is contained in the {code}`src/python` directory.
The Python package itself lives in the {code}`src/mqt/ddsim` directory.

::::::{tab-set}
:sync-group: installer

:::::{tab-item} uv _(recommended)_
:sync: uv
Getting the project up and running locally using `uv` is as simple as running:

```console
$ uv sync
```

This will

- download a suitable version of Python for you (if you don't have it installed yet),
- create a virtual environment,
- install all the project's dependencies into the virtual environment with known-good versions, and
- build and install the project itself into the virtual environment.
  :::::

:::::{tab-item} pip
:sync: pip
The whole process is a lot more tedious and manual if you use `pip` directly.
Once you have Python installed, you can first create a virtual environment with:
::::{tab-set}
:::{tab-item} macOS and Linux

```console
$ python3 -m venv .venv
$ source .venv/bin/activate
```

:::
:::{tab-item} Windows

```console
$ python3 -m venv .venv
$ .venv\Scripts\activate.bat
```

:::
::::
Then, you can install the project via:

```console
(.venv) $ pip install -ve.
```

:::::
::::::

:::{tip}
While the above commands install the project in editable mode, so that changes to the Python code are immediately reflected in the installed package,
any changes to the C++ code will require a rebuild of the Python package.
:::

The way the Python package build process in the above commands works is that a wheel for the project is built
in an isolated environment and then installed into the virtual environment.
Due to the build isolation, the corresponding C++ build directory cannot be reused for subsequent builds.
This can make rapid iteration on the Python package cumbersome.
However, one can work around this by pre-installing the build dependencies in the virtual environment and then building the package without isolation.

Since the overall process can be quite involved, we recommend using [nox](https://nox.thea.codes/en/stable/) to automate the build process.
Nox is a Python automation tool that allows you to define tasks in a `noxfile.py` and then run them with a single command.

::::{tab-set}
:::{tab-item} via `uv`
:sync: uv
The easiest way to install nox is via [uv](https://docs.astral.sh/uv/).

```console
$ uv tool install nox
```

:::
:::{tab-item} via `brew`
:sync: brew
If you use macOS, then nox is in Homebrew, and you can use

```console
$ brew install nox
```

:::
:::{tab-item} via `pipx`
:sync: pipx
If you prefer to use [pipx](https://pypa.github.io/pipx/), you can install nox with

```console
$ pipx install nox
```

:::
:::{tab-item} via `pip`
:sync: pip
If you prefer to use regular `pip` (preferably in a virtual environment), you can install nox with

```console
$ pip install nox
```

:::
::::

We define four convenient nox sessions in the `noxfile.py`:

- `tests` to run the Python tests
- `minimums` to run the Python tests with the minimum dependencies
- `lint` to run the Python code formatting and linting
- `docs` to build the documentation

These are explained in more detail in the following sections.

:::{tip}
If you just want to build the Python bindings themselves, you can pass `-DBUILD_MQT_DDSIM_BINDINGS=ON` to the CMake configure step.
CMake will then try to find Python and the necessary dependencies (`pybind11`) on your system and configure the respective targets.
In [CLion](https://www.jetbrains.com/clion/), you can enable an option to pass the current Python interpreter to CMake.
Go to `Preferences` -> `Build, Execution, Deployment` -> `CMake` -> `Python Integration` and check the box `Pass Python Interpreter to CMake`.
Alternatively, you can pass `-DPython_ROOT_DIR=<PATH_TO_PYTHON>` to the configure step to point CMake to a specific Python installation.
:::

### Running Python Tests

The Python part of the code base is tested by unit tests using the [pytest](https://docs.pytest.org/en/latest/) framework.
The corresponding test files can be found in the {code}`test/python` directory.
A {code}`nox` session is provided to conveniently run the Python tests.

```console
$ nox -s tests
```

The above command will automatically build the project and run the tests on all supported Python versions.
For each Python version, it will create a virtual environment (in the {code}`.nox` directory) and install the project into it.
We take extra care to install the project without build isolation so that rebuilds are typically very fast.

If you only want to run the tests on a specific Python version, you can pass the desired Python version to the {code}`nox` command.

```console
$ nox -s tests-3.12
```

:::{note}
If you don't want to use {code}`nox`, you can also run the tests directly using {code}`pytest`.

```console
(.venv) $ pytest test/python
```

This requires that you have the project installed in the virtual environment and the test dependency group installed.
:::

We provide an additional nox session {code}`minimums` that makes use of `uv`'s `--resolution=lowest-direct` flag to
install the lowest possible versions of the direct dependencies.
This ensures that the project can still be built and the tests pass with the minimum required versions of the dependencies.

```console
$ nox -s minimums
```

### Python Code Formatting and Linting

The Python code is formatted and linted using a collection of [pre-commit hooks](https://pre-commit.com/).
This collection includes:

- [ruff](https://docs.astral.sh/ruff/) -- an extremely fast Python linter and formatter, written in Rust.
- [mypy](https://mypy-lang.org/) -- a static type checker for Python code

There are two ways of using these hooks:

- You can install the hooks manually by running

  ```console
  $ pre-commit install
  ```

  in the project root directory.
  This will install the hooks in the {code}`.git/hooks` directory of the repository.
  The hooks will then be executed automatically when committing changes.

- You can use the {code}`nox` session {code}`lint` to run the hooks manually.

  ```console
  $ nox -s lint
  ```

  :::{note}
  If you don't want to use {code}`nox`, you can also run the hooks directly using {code}`pre-commit`.

  ```console
  $ pre-commit run --all-files
  ```

  :::

### Python Documentation

The Python part of the code base is documented using [Google-style docstrings](https://google.github.io/styleguide/pyguide.html#s3.8-comments-and-docstrings).
Every public function, class, and module should have a docstring that explains what it does and how to use it.
Ruff will check for missing docstrings and will explicitly warn you if you forget to add one.

We heavily rely on [type hints](https://docs.python.org/3/library/typing.html) to document the expected types of function arguments and return values.
For the compiled parts of the code base, we provide type hints in the form of stub files in the {code}`src/mqt/ddsim` directory.

The Python API documentation is integrated into the overall documentation that we host on ReadTheDocs using the
[sphinx-autoapi](https://sphinx-autoapi.readthedocs.io/en/latest/) extension for Sphinx.

## Working on the Documentation

The documentation is written in [MyST](https://myst-parser.readthedocs.io/en/latest/index.html) (a flavour of Markdown) and built using [Sphinx](https://www.sphinx-doc.org/en/master/).
The documentation source files can be found in the {code}`docs/` directory.

On top of the API documentation, we provide a set of tutorials and examples that demonstrate how to use the library.
These are written in Markdown using [myst-nb](https://myst-nb.readthedocs.io/en/latest/), which allows to execute Python code blocks in the documentation.
The code blocks are executed during the documentation build process, and the output is included in the documentation.
This allows us to provide up-to-date examples and tutorials that are guaranteed to work with the latest version of the library.

You can build the documentation using the {code}`nox` session {code}`docs`.

```console
$ nox -s docs
```

This will install all dependencies for building the documentation in an isolated environment, build the Python package, and then build the documentation.
Finally, it will host the documentation on a local web server for you to view.

:::{note}
If you don't want to use {code}`nox`, you can also build the documentation directly using {code}`sphinx-build`.

```console
(.venv) $ sphinx-build -b html docs/ docs/_build
```

The docs can then be found in the {code}`docs/_build` directory.
:::
