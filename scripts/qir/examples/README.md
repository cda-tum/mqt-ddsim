# QIR Integration

## Options

There are two inherently different approaches to support QIR with a simulator such as our DDsim.
One way is to provide the QIR file (.ll) together with definitions of the missing functions to clang compiler that cannot only handle C/C++ code but also LLVM IR where QIR is a dialect of.
This approach allows from the beginning to support the full profile of QIR.
Another way of supporting QIR is to parse the respective file during runtime either by a custom parser or by utilizing the LLVM infrastructure.

We here focus for now on the first method.

## Overview of Examples

Here we list 3 examples that work as test cases. So far only the BellPair example is fully complete such that we can recreate it.
For the BellPair example the .py file is transformed as described in the repository into a working QIR (.ll) file.
We use this file as input.

The examples are taken from the corresponding repositories.

| Status | File                                                                               | Project                                                                                                      |
| ------ | ---------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------ |
| ❌     | [`SmallSeparation.ll`](SmallSeparation.ll)<sup>1</sup>                             | [qat](https://github.com/qir-alliance/qat/blob/main/qir/demos/SmallSeparation/input.ll)                      |
| ❌     | [`VariationalQuantumEigensolver.ll`](VariationalQuantumEigensolver.ll)<sup>2</sup> | [nwqsim](https://github.com/qir-alliance/nwqsim/blob/main/dmsim/qsharp/vqe/VariationalQuantumEigensolver.ll) |
| ✅     | [`BellPair.py`](BellPair.py)<sup>3</sup>                                           | [pyqir](https://github.com/qir-alliance/pyqir/blob/main/examples/bell_pair.py)                               |

---

<sup>1</sup> This circuit has no main function.<br>
<sup>2</sup> The circuit itself has no main function. The [driver](VariationalQuantumEigensolver_driver.cc) provides the main function; however, I cannot find the headers `QirRuntimeApi_I.hpp` and `QirContext.hpp` to compile it. I wrote a minimal wrapper that strips out everything unnecessary from the previous file. Compilation works this way; however, leads to a segmentation fault due to not implemented functions in [`DDsimQir.cpp`](../../../src/qir/DDsimQir.cpp).<br>
<sup>3</sup> This works like a charm, one can build the circuit in a Python script, and executing it generates a [`.ll` file](BellPair.ll) that can be linked with the library and produces an executable.

## BellPair Example

### Instructions to run QIR code with DDSim by Hand

0. If you have written your quantum circuit in C or C++ and want
to compile it to QIR (LLVM IR), use:

        clang -S -emit-llvm examples/BellPair.c

1. **Build a shared library** with a QIR interface (commands run from the `/build/` directory):

        clang -c -o DDsimQir.o ../src/qir/DDsimQir.cpp -I ../include -I ../extern/mqt-core/include/ -fPIC -std=c++17

        clang \
        -c  # Only run preprocess, compile, and assemble steps \
        -o DDsimQir.o  # Write output to <file> \
        ../src/DDsimQir.cpp  # input file \
        -I ../include  # Add directory to the end of the list of include search paths \
        -fPIC  # Enable Position Independent Code \

        clang -shared -o DDsimQir.so DDsimQir.o -L src/extern/mqt-core/src/ -lmqt-core -lc++


        clang \
        -shared  # Build a shared library \
        -o DDsimQir.so  # Write output to <file> \
        DDsimQir.o  # input file



2. **Link the shared library** to the QIR file and compile, use (run from `project-root/` directory):

        clang scripts/qir/examples/BellPair.ll build/DDsimQir.so -L build/src/extern/mqt-core/src/ -lmqt-core -lc++  -I include -I extern/mqt-core/include -o ./build/bellPair

    The executable `bellPair` must be executed from the `/build/` repository.

        cd build/ && ./bellPair

### Run Code with Cmake

As of now when building the whole DDSim project with cmake, an executable called `bellPair` is created under `build/src/qir/`.
This executable is the same as when created by hand as in the steps above but the idea was to include this in the cmake process, eliminating
any step required by hand.

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    
    cmake --build build --config Release

**NOTE** it turned out that both variants depend on the clang version used. It has been tested with clang 15.0.0 and 
worked. However, with the newer version 18.6.0 there was an error for an "Unsupported stack probing method"
and the request to submit a pull request with this error.

## Further Steps

With this set-up given it is now possible to start implementing the actual qir functions inside the ddsim-qir library.

- [ ] write definitions of QIR functions
- [ ] discuss how to make the qir file usage available for the end user

