#include "DDsimQir.h"

#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"

#include <stdio.h>
#include <stdlib.h>

extern "C" {

// TODO: replace the static 10; Problem: where do we get the information from for those illformated QIRs?
#define NUM_QUBITS 10

// AUXILIARY VARIABLES AND FUNCTIONS

qc::QuantumComputation* qc = NULL;

void create_quantum_computation(unsigned long long int n) {
    qc = new qc::QuantumComputation(n);
}

void delete_quantum_computation() {
    delete qc;
    qc = NULL;
}

// Creates if necessary a new QuantumComputation object and returns a pointer to it.
qc::QuantumComputation* get_quantum_computation_handle() {
    if (qc == NULL) {
        create_quantum_computation(NUM_QUBITS);
    }
    return qc;
}

const char* simulate(qc::QuantumComputation* qc) {
    CircuitSimulator ddsim(std::move(qc), ApproximationInfo(1, 1, ApproximationInfo::FidelityDriven));
    ddsim.simulate(1);
    return ddsim.measureAll(false).c_str();
}

// *** MEASUREMENT RESULTS ***
Result* __quantum__rt__result_get_zero() {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Result* __quantum__rt__result_get_one() {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Bool __quantum__rt__result_equal(Result*, Result*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return FALSE;
}

void __quantum__rt__result_update_reference_count(Result*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

// *** STRINGS ***
String* __quantum__rt__string_create(const char*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

const char* __quantum__rt__string_get_data(String*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

long int __quantum__rt__string_get_length(String*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

void __quantum__rt__string_update_reference_count(String*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

String* __quantum__rt__string_concatenate(String*, String*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Bool __quantum__rt__string_equal(String*, String*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return FALSE;
}

String* __quantum__rt__int_to_string(Int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

String* __quantum__rt__double_to_string(Double) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

String* __quantum__rt__bool_to_string(Bool) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

String* __quantum__rt__result_to_string(Result*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

String* __quantum__rt__pauli_to_string(Pauli) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

String* __quantum__rt__qubit_to_string(Qubit*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

String* __quantum__rt__range_to_string(Range) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

String* __quantum__rt__bigint_to_string(BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

// *** BIG INTEGERS ***
BigInt* __quantum__rt__bigint_create_i64(Int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_create_array(long int, char*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

char* __quantum__rt__bigint_get_data(BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

long int __quantum__rt__bigint_get_length(BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

void __quantum__rt__bigint_update_reference_count(BigInt*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

BigInt* __quantum__rt__bigint_negate(BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_add(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_subtract(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_multiply(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_divide(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_modulus(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_power(BigInt*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_bitand(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_bitor(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_bitxor(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_bitnot(BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_shiftleft(BigInt*, Int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

BigInt* __quantum__rt__bigint_shiftright(BigInt*, Int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Bool __quantum__rt__bigint_equal(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Bool __quantum__rt__bigint_greater(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Bool __quantum__rt__bigint_greater_eq(BigInt*, BigInt*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

// *** TUPLES ***
Tuple* __quantum__rt__tuple_create(Int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Tuple* __quantum__rt__tuple_copy(Tuple*, Bool force) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

void __quantum__rt__tuple_update_reference_count(Tuple*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__tuple_update_alias_count(Tuple*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

// *** ARRAYS ***
Array* __quantum__rt__array_create_1d(long int size, Int n) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    Array* a = malloc(size * n);
    for (Int i = 0; i < n; i++) {
        ((char*)a)[i] = '0';
    }
    return a;
}

Array* __quantum__rt__array_copy(Array*, Bool) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Array* __quantum__rt__array_concatenate(Array*, Array*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Array* __quantum__rt__array_slice_1d(Array*, Range, Bool) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Int __quantum__rt__array_get_size_1d(Array*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

char* __quantum__rt__array_get_element_ptr_1d(Array*, Int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

void __quantum__rt__array_update_reference_count(Array*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__array_update_alias_count(Array*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

Array* __quantum__rt__array_create(long int, long int, Int*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

long int __quantum__rt__array_get_dim(Array*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

Int __quantum__rt__array_get_size(Array*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

char* __quantum__rt__array_get_element_ptr(Array*, Int*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Array* __quantum__rt__array_slice(Array*, long int, Range, Bool) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Array* __quantum__rt__array_project(Array*, long int, Int, Bool) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

// *** CALLABLES ***
Callable* __quantum__rt__callable_create(void (*f[4])(Tuple*, Tuple*, Tuple*), void (*c[2])(Tuple*, Tuple*, Tuple*), Tuple*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Callable* __quantum__rt__callable_copy(Callable*, Bool) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

void __quantum__rt__callable_invoke(Callable*, Tuple*, Tuple*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__callable_make_adjoint(Callable*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__callable_make_controlled(Callable*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__callable_update_reference_count(Callable*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__callable_update_alias_count(Callable*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__capture_update_reference_count(Callable*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__capture_update_alias_count(Callable*, long int) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

// *** CLASSICAL RUNTIME ***
void __quantum__rt__message(String* msg) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

_Noreturn void __quantum__rt__fail(String* msg) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    exit(1);
}

// *** QUANTUM INSTRUCTION SET AND RUNTIME ***
Qubit* __quantum__rt__qubit_allocate() {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return NULL;
}

Array* __quantum__rt__qubit_allocate_array(Int n) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    return malloc(sizeof(Qubit) * n);
}

void __quantum__rt__qubit_release(Qubit*) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
}

void __quantum__rt__qubit_release_array(Array* a) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    free(a);
}

// QUANTUM INSTRUCTION SET
void __quantum__qis__x__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->x(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__y__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->y(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__z__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->z(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__h__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->h(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__sqrtx__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->sx(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__sqrtxdg__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->sxdg(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__s__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->s(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__sdg__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->sdg(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__t__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->t(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__tdg__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->tdg(reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__rx__body(Double phi, Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->rx(phi, reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__ry__body(Double phi, Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->ry(phi, reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__rz__body(Double phi, Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->rz(phi, reinterpret_cast<qc::Qubit>(q));
}

void __quantum__qis__cnot__body(Qubit* c, Qubit* t) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->cx(reinterpret_cast<qc::Qubit>(c), reinterpret_cast<qc::Qubit>(t));
}

void __quantum__qis__cx__body(Qubit* c, Qubit* t) {
    printf("%s:%s:%d \nredirects to: ", __FILE__, __FUNCTION__, __LINE__);
    __quantum__qis__cnot__body(c, t);
}

void __quantum__qis__cz__body(Qubit* c, Qubit* t) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->cz(reinterpret_cast<qc::Qubit>(c), reinterpret_cast<qc::Qubit>(t));
}

void __quantum__qis__ccx__body(Qubit* c1, Qubit* c2, Qubit* t) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->mcx({reinterpret_cast<qc::Qubit>(c1), reinterpret_cast<qc::Qubit>(c2)}, reinterpret_cast<qc::Qubit>(t));
}

void __quantum__qis__ccz__body(Qubit* c1, Qubit* c2, Qubit* t) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->mcz({reinterpret_cast<qc::Qubit>(c1), reinterpret_cast<qc::Qubit>(c2)}, reinterpret_cast<qc::Qubit>(t));
}

void __quantum__qis__mz__body(Qubit* q, Result* r) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    // TODO
}

void __quantum__qis__m__body(Qubit* q, Result* r) {
    printf("%s:%s:%d \nredirects to: ", __FILE__, __FUNCTION__, __LINE__);
    __quantum__qis__mz__body(q, r);
}

void __quantum__qis__reset__body(Qubit* q) {
    printf("%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
    qc->reset(reinterpret_cast<qc::Qubit>(q));
}

} // extern "C"
