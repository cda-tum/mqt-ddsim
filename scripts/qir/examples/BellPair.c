#include <stdlib.h>
#include "DDsimQir.h"

// run `clang -S -emit-llvm BellPair.c -I ../../../include/qir` to produce the .ll file.

int main() {
    __quantum__qis__h__body(NULL);
    __quantum__qis__cnot__body(NULL, (Qubit*)1);
    __quantum__qis__mz__body(NULL, NULL);
    __quantum__qis__mz__body((Qubit*)1, (Qubit*)1);
    return 0;
}
