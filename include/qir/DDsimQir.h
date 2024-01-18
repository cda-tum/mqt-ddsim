// initially taken from https://github.com/qir-alliance/qir-runner/blob/main/stdlib/include/qir_stdlib.h
// and adopted to match the QIR specification https://github.com/qir-alliance/qir-spec/tree/main/specification/v0.1

// Instructions to wrap a C++ class with a C interface are taken from
// [https://stackoverflow.com/a/11971205](https://stackoverflow.com/a/11971205)
#include <stdbool.h>

#pragma once
#ifndef QIR_H
    #define QIR_H

    #define FALSE 0
    #define TRUE 1

    #ifdef __cplusplus
extern "C" {
    #endif

// *** SIMPLE TYPES ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/1_Data_Types.md#simple-types
typedef signed long long int Int;
typedef double               Double;
typedef bool                 Bool;
typedef enum {
    PauliId_I = 0,
    PauliId_X = 1,
    PauliId_Z = 2,
    PauliId_Y = 3,
} Pauli;
typedef struct {
    Int start;
    Int step;
    Int end;
} Range;

// moved up, because it is required for Strings already, see BigInt section for more
typedef void BigInt;

// *** MEASUREMENT RESULTS ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/1_Data_Types.md#measurement-results

typedef void Result;

// Returns a constant representing a measurement result zero.
Result* __quantum__rt__result_get_zero();

// Returns a constant representing a measurement result one.
Result* __quantum__rt__result_get_one();

// Returns true if the two results are the same, and false if they are different.
Bool __quantum__rt__result_equal(Result*, Result*);

// Adds the given integer value to the reference count for the result. Deallocates the result if the reference count
// becomes 0.
void __quantum__rt__result_update_reference_count(Result*, long int);

// *** QUBITS ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/1_Data_Types.md#qubits
typedef void Qubit;

// *** STRINGS ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/1_Data_Types.md#strings

typedef char String;

// Creates a string from an array of UTF-8 bytes.
String* __quantum__rt__string_create(const char*);

// Returns a pointer to the zero-terminated array of UTF-8 bytes.
const char* __quantum__rt__string_get_data(String*);

// Returns the length of the byte array that contains the string data.
long int __quantum__rt__string_get_length(String*);

// Adds the given integer value to the reference count for the string. Deallocates the string if the reference count
// becomes 0.
void __quantum__rt__string_update_reference_count(String*, long int);

// Creates a new string that is the concatenation of the two argument strings.
String* __quantum__rt__string_concatenate(String*, String*);

// Returns true if the two strings are equal, false otherwise.
Bool __quantum__rt__string_equal(String*, String*);

// Returns a string representation of the integer.
String* __quantum__rt__int_to_string(Int);

// Returns a string representation of the double.
String* __quantum__rt__double_to_string(Double);

// Returns a string representation of the Boolean.
String* __quantum__rt__bool_to_string(Bool);

// Returns a string representation of the result.
String* __quantum__rt__result_to_string(Result*);

// Returns a string representation of the Pauli.
String* __quantum__rt__pauli_to_string(Pauli);

// Returns a string representation of the qubit.
String* __quantum__rt__qubit_to_string(Qubit*);

// Returns a string representation of the range.
String* __quantum__rt__range_to_string(Range);

// Returns a string representation of the big integer.
String* __quantum__rt__bigint_to_string(BigInt*);

// *** BIG INTEGERS ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/1_Data_Types.md#big-integers

// Creates a big integer with the specified initial value.
BigInt* __quantum__rt__bigint_create_i64(Int);

// Creates a big integer with the initial value specified by the i8 array. The 0-th element of the array is the
// highest-order byte, followed by the first element, etc.
BigInt* __quantum__rt__bigint_create_array(long int, char*);

// Returns a pointer to the i8 array containing the value of the big integer.
char* __quantum__rt__bigint_get_data(BigInt*);

// Returns the length of the i8 array that represents the big integer value.
long int __quantum__rt__bigint_get_length(BigInt*);

// Adds the given integer value to the reference count for the big integer. Deallocates the big integer if the
// reference count becomes 0. The behavior is undefined if the reference count becomes negative.
void __quantum__rt__bigint_update_reference_count(BigInt*, long int);

// Returns the negative of the big integer.
BigInt* __quantum__rt__bigint_negate(BigInt*);

// Adds two big integers and returns their sum.
BigInt* __quantum__rt__bigint_add(BigInt*, BigInt*);

// Subtracts the second big integer from the first and returns their difference.
BigInt* __quantum__rt__bigint_subtract(BigInt*, BigInt*);

// Multiplies two big integers and returns their product.
BigInt* __quantum__rt__bigint_multiply(BigInt*, BigInt*);

// Divides the first big integer by the second and returns their quotient.
BigInt* __quantum__rt__bigint_divide(BigInt*, BigInt*);

// Returns the first big integer modulo the second.
BigInt* __quantum__rt__bigint_modulus(BigInt*, BigInt*);

// Returns the big integer raised to the integer power.
BigInt* __quantum__rt__bigint_power(BigInt*, long int);

// Returns the bitwise-AND of two big integers.
BigInt* __quantum__rt__bigint_bitand(BigInt*, BigInt*);

// Returns the bitwise-OR of two big integers.
BigInt* __quantum__rt__bigint_bitor(BigInt*, BigInt*);

// Returns the bitwise-XOR of two big integers.
BigInt* __quantum__rt__bigint_bitxor(BigInt*, BigInt*);

// Returns the bitwise complement of the big integer.
BigInt* __quantum__rt__bigint_bitnot(BigInt*);

// Returns the big integer arithmetically shifted left by the (positive) integer amount of bits.
BigInt* __quantum__rt__bigint_shiftleft(BigInt*, Int);

// Returns the big integer arithmetically shifted right by the (positive) integer amount of bits.
BigInt* __quantum__rt__bigint_shiftright(BigInt*, Int);

// Returns true if the two big integers are equal, false otherwise.
Bool __quantum__rt__bigint_equal(BigInt*, BigInt*);

// Returns true if the first big integer is greater than the second, false otherwise.
Bool __quantum__rt__bigint_greater(BigInt*, BigInt*);

// Returns true if the first big integer is greater than or equal to the second, false otherwise.
Bool __quantum__rt__bigint_greater_eq(BigInt*, BigInt*);

// *** TUPLES ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/1_Data_Types.md#tuples-and-user-defined-types

typedef void Tuple;

// Allocates space for a tuple requiring the given number of bytes and sets the reference count to 1.
Tuple* __quantum__rt__tuple_create(Int);

// Creates a shallow copy of the tuple if the user count is larger than 0 or the second argument is `true`.
Tuple* __quantum__rt__tuple_copy(Tuple*, Bool force);

// Adds the given integer value to the reference count for the tuple. Deallocates the tuple if the reference count
// becomes 0. The behavior is undefined if the reference count becomes negative.
void __quantum__rt__tuple_update_reference_count(Tuple*, long int);

// Adds the given integer value to the alias count for the tuple. Fails if the count becomes negative.
void __quantum__rt__tuple_update_alias_count(Tuple*, long int);

// *** ARRAYS ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/1_Data_Types.md#arrays

typedef void Array;

// Creates a new 1-dimensional array. The int is the size of each element in bytes. The Int is the length
// of the array. The bytes of the new array should be set to zero.
Array* __quantum__rt__array_create_1d(long int, Int);

// Creates a shallow copy of the array if the user count is larger than 0 or the second argument is `true`.
Array* __quantum__rt__array_copy(Array*, Bool);

// Returns a new array which is the concatenation of the two passed-in arrays.
Array* __quantum__rt__array_concatenate(Array*, Array*);

// Creates and returns an array that is a slice of an existing 1-dimensional array. The slice may be accessing the
// same memory as the given array unless its alias count is larger than 0 or the last argument is true.
// The Range specifies the indices that should be the elements of the returned array.
Array* __quantum__rt__array_slice_1d(Array*, Range, Bool);

// Returns the length of a dimension of the array. The int is the zero-based dimension to return the length of; it
// must be 0 for a 1-dimensional array.
Int __quantum__rt__array_get_size_1d(Array*);

// Returns a pointer to the element of the array at the zero-based index given by the Int.
char* __quantum__rt__array_get_element_ptr_1d(Array*, Int);

// Adds the given integer value to the reference count for the array. Deallocates the array if the reference count
// becomes 0. The behavior is undefined if the reference count becomes negative.
void __quantum__rt__array_update_reference_count(Array*, long int);

// Adds the given integer value to the alias count for the array. Fails if the count becomes negative.
void __quantum__rt__array_update_alias_count(Array*, long int);

// Creates a new array. The first i32 is the size of each element in bytes. The second i32 is the dimension count.
// The i64* should point to an array of i64s contains the length of each dimension. The bytes of the new array
// should be set to zero. If any length is zero, the result should be an empty array with the given number of
// dimensions.
Array* __quantum__rt__array_create(long int, long int, Int*);

// Returns the number of dimensions in the array.
long int __quantum__rt__array_get_dim(Array*);

// Returns the length of a dimension of the array. The i32 is the zero-based dimension to return the length of; it
// must be smaller than the number of dimensions in the array.
Int __quantum__rt__array_get_size(Array*, long int);

// Returns a pointer to the indicated element of the array. The i64* should point to an array of i64s that are the
// indices for each dimension.
char* __quantum__rt__array_get_element_ptr(Array*, Int*);

// Creates and returns an array that is a slice of an existing array. The slice may be accessing the same memory as
// the given array unless its alias count is larger than 0 or the last argument is true. The i32 indicates which
// dimension the slice is on, and must be smaller than the number of dimensions in the array. The %Range specifies
// the indices in that dimension that should be the elements of the returned array. The reference count of the
// elements remains unchanged.
Array* __quantum__rt__array_slice(Array*, long int, Range, Bool);

// Creates and returns an array that is a projection of an existing array. The projection may be accessing the same
// memory as the given array unless its alias count is larger than 0 or the last argument is true. The i32 indicates
// which dimension the projection is on, and the i64 specifies the index in that dimension to project. The reference
// count of all array elements remains unchanged. If the existing array is one-dimensional then a runtime failure
// should occur.
Array* __quantum__rt__array_project(Array*, long int, Int, Bool);

// *** CALLABLES ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/2_Callables.md

typedef void Callable;

// Initializes the callable with the provided function table and capture tuple. The capture tuple pointer
// should be null if there is no capture.
Callable* __quantum__rt__callable_create(void (*f[4])(Tuple*, Tuple*, Tuple*), void (*m[2])(Tuple*, Tuple*, Tuple*), Tuple*);

// Creates a shallow copy of the callable if the alias count is larger than 0 or the second argument is `true`.
// Returns the given callable pointer otherwise, after increasing its reference count by 1.
Callable* __quantum__rt__callable_copy(Callable*, Bool);

// Invokes the callable with the provided argument tuple and fills in the result tuple.
void __quantum__rt__callable_invoke(Callable*, Tuple*, Tuple*);

// Updates the callable by applying the Adjoint functor.
void __quantum__rt__callable_make_adjoint(Callable*);

// Updates the callable by applying the Controlled functor.
void __quantum__rt__callable_make_controlled(Callable*);

// Adds the given integer value to the reference count for the callable. Deallocates the callable if the reference
// count becomes 0. The behavior is undefined if the reference count becomes negative.
void __quantum__rt__callable_update_reference_count(Callable*, long int);

// Adds the given integer value to the alias count for the callable. Fails if the count becomes negative.
void __quantum__rt__callable_update_alias_count(Callable*, long int);

// Invokes the function at index 0 in the memory management table of the callable with the capture tuple and the
// given 32-bit integer.
void __quantum__rt__capture_update_reference_count(Callable*, long int);

// Invokes the function at index 1 in the memory management table of the callable with the capture tuple and the
// given 32-bit integer.
void __quantum__rt__capture_update_alias_count(Callable*, long int);

// *** CLASSICAL RUNTIME ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/3_Classical_Runtime.md

// Include the given message in the computation's execution log or equivalent.
void __quantum__rt__message(String* msg);

// Fail the computation with the given error message.
_Noreturn void __quantum__rt__fail(String* msg);

// *** QUANTUM INSTRUCTIONSET AND RUNTIME ***
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/v0.1/4_Quantum_Runtime.md

// Allocates a single qubit.
Qubit* __quantum__rt__qubit_allocate();
// Creates an array of the given size and populates it with newly-allocated qubits.

Array* __quantum__rt__qubit_allocate_array(Int);

// Releases a single qubit. Passing a null pointer as argument should cause a runtime failure.
void __quantum__rt__qubit_release(Qubit*);

// Releases an array of qubits; each qubit in the array is released, and the array itself is unreferenced.
// Passing a null pointer as argument should cause a runtime failure.
void __quantum__rt__qubit_release_array(Array*);

// QUANTUM INSTRUCTION SET
// cf. https://github.com/qir-alliance/qir-spec/blob/main/specification/under_development/profiles/Base_Profile.md#base-profile
// WARNING: This refers to the unstable version of the specification under develpoment.

void __quantum__qis__x__body(Qubit*);
void __quantum__qis__y__body(Qubit*);
void __quantum__qis__z__body(Qubit*);
void __quantum__qis__h__body(Qubit*);
void __quantum__qis__sqrtx__body(Qubit*);
void __quantum__qis__sqrtxdg__body(Qubit*);
void __quantum__qis__s__body(Qubit*);
void __quantum__qis__sdg__body(Qubit*);
void __quantum__qis__t__body(Qubit*);
void __quantum__qis__tdg__body(Qubit*);
void __quantum__qis__rx__body(Double, Qubit*);
void __quantum__qis__ry__body(Double, Qubit*);
void __quantum__qis__rz__body(Double, Qubit*);
void __quantum__qis__cx__body(Qubit*, Qubit*);
void __quantum__qis__cnot__body(Qubit*, Qubit*);
void __quantum__qis__cz__body(Qubit*, Qubit*);
void __quantum__qis__ccx__body(Qubit*, Qubit*, Qubit*);
void __quantum__qis__ccz__body(Qubit*, Qubit*, Qubit*);
void __quantum__qis__mz__body(Qubit*, Result*);
void __quantum__qis__m__body(Qubit*, Result*);
void __quantum__qis__reset__body(Qubit*);

    #ifdef __cplusplus
} // extern "C"
    #endif

#endif
