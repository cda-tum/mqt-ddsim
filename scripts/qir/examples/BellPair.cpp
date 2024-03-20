#include "DDsimQir.h"

// run `clang++ -S -emit-llvm BellPair.cpp -I ../../../include/qir` to produce the .ll file.

int main() {
    __quantum__qis__h__body(nullptr);
    __quantum__qis__cnot__body(nullptr, reinterpret_cast<Qubit*>(1));
    __quantum__qis__mz__body(nullptr, nullptr);
    __quantum__qis__mz__body(reinterpret_cast<Qubit*>(1), reinterpret_cast<Qubit*>(1));
    return 0;
}
