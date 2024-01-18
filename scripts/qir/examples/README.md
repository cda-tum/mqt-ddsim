# Examples

The examples are taken from the corresponding repositories.

Status | File | Project
---|---|---
❌ | [`SmallSeparation.ll`](SmallSeparation.ll)<sup>1</sup> | [qat](https://github.com/qir-alliance/qat/blob/main/qir/demos/SmallSeparation/input.ll)
❌ | [`VariationalQuantumEigensolver.ll`](VariationalQuantumEigensolver.ll)<sup>2</sup> | [nwqsim](https://github.com/qir-alliance/nwqsim/blob/main/dmsim/qsharp/vqe/VariationalQuantumEigensolver.ll)
✅ | [`BellPair.py`](BellPair.py)<sup>3</sup> | [pyqir](https://github.com/qir-alliance/pyqir/blob/main/examples/bell_pair.py)

---
<sup>1</sup> This circuit has no main function.<br>
<sup>2</sup> The circuit itself has no main function. The [driver](VariationalQuantumEigensolver_driver.cc) provides the main function; however, I cannot find the headers `QirRuntimeApi_I.hpp` and `QirContext.hpp` to compile it. I wrote a minimal wrapper that strips out everything unnecessary from the previous file. Compilation works this way; however, leads to a segmentation fault due to not implemented functions in [`DDsimQir.cpp`](../../../src/qir/DDsimQir.cpp).<br>
<sup>3</sup> This works like a charm, one can build the circuit in a Python script, and executing it generates a [`.ll` file](BellPair.ll) that can be linked with the library and produces an executable.



# Instructions

Build a shared library with a QIR interface:

    clang \
    -c  # Only run preprocess, compile, and assemble steps \
    -o DDsimQir.o  # Write output to <file> \
    ../src/DDsimQir.c  # input file \
    -I ../include  # Add directory to the end of the list of include search paths \
    -fPIC  # Enable Position Independent Code \
    -Wnp-c2x-extensions  # supress warning because of omitted parameter names

    clang \
    -shared  # Build a shared library \
    -o DDsimQir.so  # Write output to <file> \
    DDsimQir.o  # input file

If you have written your quantum circuit in C and want to compile it to QIR (LLVM IR), use:

    clang -S -emit-llvm examples/BellPair.c

To link the shared library to the QIR file and compile, use:

    clang -o .build/bellPair examples/BellPair.ll .build/DDsimQir.so
