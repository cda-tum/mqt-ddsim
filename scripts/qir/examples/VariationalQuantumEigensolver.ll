
%Range = type { i64, i64, i64 }
%Tuple = type opaque
%Array = type opaque
%Qubit = type opaque
%Result = type opaque
%String = type opaque
%Callable = type opaque

@PauliI = internal constant i2 0
@PauliX = internal constant i2 1
@PauliY = internal constant i2 -1
@PauliZ = internal constant i2 -2
@EmptyRange = internal constant %Range { i64 0, i64 1, i64 -1 }
@0 = internal constant [18 x i8] c"Unsupported input\00"
@Microsoft__Quantum__Intrinsic__CNOT__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__CNOT__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__CNOT__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__CNOT__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__CNOT__ctladj__wrapper]
@Microsoft__Quantum__Intrinsic__H__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__H__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__H__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__H__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__H__ctladj__wrapper]
@Microsoft__Quantum__Intrinsic__Rx__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Rx__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Rx__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Rx__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Rx__ctladj__wrapper]
@Microsoft__Quantum__Intrinsic__Ry__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Ry__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Ry__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Ry__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Ry__ctladj__wrapper]
@Microsoft__Quantum__Intrinsic__Rz__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Rz__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Rz__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Rz__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Rz__ctladj__wrapper]
@Microsoft__Quantum__Intrinsic__S__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__S__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__S__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__S__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__S__ctladj__wrapper]
@Microsoft__Quantum__Intrinsic__T__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__T__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__T__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__T__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__T__ctladj__wrapper]
@Microsoft__Quantum__Intrinsic__X__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__X__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__X__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__X__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__X__ctladj__wrapper]
@Microsoft__Quantum__Intrinsic__Z__FunctionTable = internal constant [4 x void (%Tuple*, %Tuple*, %Tuple*)*] [void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Z__body__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Z__adj__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Z__ctl__wrapper, void (%Tuple*, %Tuple*, %Tuple*)* @Microsoft__Quantum__Intrinsic__Z__ctladj__wrapper]

define internal void @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__StatePrep__PrepareTrialState__body({ i64, %Array* }* %stateData, %Array* %qubits) {
entry:
  %0 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %stateData, i32 0, i32 1
  %terms = load %Array*, %Array** %0, align 8
  %1 = call i64 @__quantum__rt__array_get_size_1d(%Array* %terms)
  %2 = sub i64 %1, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %3 = phi i64 [ 0, %entry ], [ %14, %exiting__1 ]
  %4 = icmp sle i64 %3, %2
  br i1 %4, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %5 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %terms, i64 %3)
  %6 = bitcast i8* %5 to { { double, double }*, %Array* }**
  %7 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %6, align 8
  %8 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %7, i32 0, i32 0
  %9 = load { double, double }*, { double, double }** %8, align 8
  %10 = bitcast { double, double }* %9 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %10, i32 1)
  %11 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %7, i32 0, i32 1
  %12 = load %Array*, %Array** %11, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %12, i32 1)
  %13 = bitcast { { double, double }*, %Array* }* %7 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %13, i32 1)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %14 = add i64 %3, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  call void @__quantum__rt__array_update_alias_count(%Array* %terms, i32 1)
  %15 = bitcast { i64, %Array* }* %stateData to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %15, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %qubits, i32 1)
  %16 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %stateData, i32 0, i32 0
  %stateType = load i64, i64* %16, align 4
  %17 = sub i64 %1, 1
  br label %header__2

header__2:                                        ; preds = %exiting__2, %exit__1
  %18 = phi i64 [ 0, %exit__1 ], [ %29, %exiting__2 ]
  %19 = icmp sle i64 %18, %17
  br i1 %19, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %20 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %terms, i64 %18)
  %21 = bitcast i8* %20 to { { double, double }*, %Array* }**
  %22 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %21, align 8
  %23 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %22, i32 0, i32 0
  %24 = load { double, double }*, { double, double }** %23, align 8
  %25 = bitcast { double, double }* %24 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %25, i32 1)
  %26 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %22, i32 0, i32 1
  %27 = load %Array*, %Array** %26, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %27, i32 1)
  %28 = bitcast { { double, double }*, %Array* }* %22 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %28, i32 1)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %29 = add i64 %18, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %terms, i32 1)
  %30 = sub i64 %1, 1
  br label %header__3

header__3:                                        ; preds = %exiting__3, %exit__2
  %31 = phi i64 [ 0, %exit__2 ], [ %43, %exiting__3 ]
  %32 = icmp sle i64 %31, %30
  br i1 %32, label %body__3, label %exit__3

body__3:                                          ; preds = %header__3
  %33 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %terms, i64 %31)
  %34 = bitcast i8* %33 to { { double, double }*, %Array* }**
  %term = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %34, align 8
  %35 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %term, i32 0, i32 0
  %coefficient = load { double, double }*, { double, double }** %35, align 8
  %36 = bitcast { double, double }* %coefficient to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %36, i32 1)
  %37 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %term, i32 0, i32 1
  %excitation = load %Array*, %Array** %37, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %excitation, i32 1)
  %38 = bitcast { { double, double }*, %Array* }* %term to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %38, i32 1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %36, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %excitation, i32 1)
  %39 = getelementptr inbounds { double, double }, { double, double }* %coefficient, i32 0, i32 0
  %theta = load double, double* %39, align 8
  %40 = getelementptr inbounds { double, double }, { double, double }* %coefficient, i32 0, i32 1
  %phi = load double, double* %40, align 8
  %41 = call i64 @__quantum__rt__array_get_size_1d(%Array* %excitation)
  %42 = sub i64 %41, 1
  br label %header__4

exiting__3:                                       ; preds = %continue__1
  %43 = add i64 %31, 1
  br label %header__3

exit__3:                                          ; preds = %header__3
  %44 = sub i64 %1, 1
  br label %header__6

header__4:                                        ; preds = %exiting__4, %body__3
  %45 = phi i64 [ 0, %body__3 ], [ %52, %exiting__4 ]
  %46 = icmp sle i64 %45, %42
  br i1 %46, label %body__4, label %exit__4

body__4:                                          ; preds = %header__4
  %47 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %excitation, i64 %45)
  %48 = bitcast i8* %47 to i64*
  %i = load i64, i64* %48, align 4
  %49 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %qubits, i64 %i)
  %50 = bitcast i8* %49 to %Qubit**
  %51 = load %Qubit*, %Qubit** %50, align 8
  call void @Microsoft__Quantum__Intrinsic__X__body(%Qubit* %51)
  br label %exiting__4

exiting__4:                                       ; preds = %body__4
  %52 = add i64 %45, 1
  br label %header__4

exit__4:                                          ; preds = %header__4
  %53 = fcmp olt double %theta, 1.000000e+00
  br i1 %53, label %then0__1, label %continue__1

then0__1:                                         ; preds = %exit__4
  %54 = call i64 @__quantum__rt__array_get_size_1d(%Array* %qubits)
  %55 = sub i64 %54, 1
  br label %header__5

continue__1:                                      ; preds = %exit__5, %exit__4
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %36, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %excitation, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %38, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %36, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %excitation, i32 -1)
  br label %exiting__3

header__5:                                        ; preds = %exiting__5, %then0__1
  %56 = phi i64 [ 0, %then0__1 ], [ %60, %exiting__5 ]
  %57 = icmp sle i64 %56, %55
  br i1 %57, label %body__5, label %exit__5

body__5:                                          ; preds = %header__5
  %58 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %qubits, i64 %56)
  %59 = bitcast i8* %58 to %Qubit**
  %qubit = load %Qubit*, %Qubit** %59, align 8
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__Rx__body(double %theta, %Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__S__adj(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %theta, %Qubit* %qubit)
  br label %exiting__5

exiting__5:                                       ; preds = %body__5
  %60 = add i64 %56, 1
  br label %header__5

exit__5:                                          ; preds = %header__5
  br label %continue__1

header__6:                                        ; preds = %exiting__6, %exit__3
  %61 = phi i64 [ 0, %exit__3 ], [ %72, %exiting__6 ]
  %62 = icmp sle i64 %61, %44
  br i1 %62, label %body__6, label %exit__6

body__6:                                          ; preds = %header__6
  %63 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %terms, i64 %61)
  %64 = bitcast i8* %63 to { { double, double }*, %Array* }**
  %65 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %64, align 8
  %66 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %65, i32 0, i32 0
  %67 = load { double, double }*, { double, double }** %66, align 8
  %68 = bitcast { double, double }* %67 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %68, i32 -1)
  %69 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %65, i32 0, i32 1
  %70 = load %Array*, %Array** %69, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %70, i32 -1)
  %71 = bitcast { { double, double }*, %Array* }* %65 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %71, i32 -1)
  br label %exiting__6

exiting__6:                                       ; preds = %body__6
  %72 = add i64 %61, 1
  br label %header__6

exit__6:                                          ; preds = %header__6
  call void @__quantum__rt__array_update_alias_count(%Array* %terms, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %15, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %qubits, i32 -1)
  %73 = sub i64 %1, 1
  br label %header__7

header__7:                                        ; preds = %exiting__7, %exit__6
  %74 = phi i64 [ 0, %exit__6 ], [ %85, %exiting__7 ]
  %75 = icmp sle i64 %74, %73
  br i1 %75, label %body__7, label %exit__7

body__7:                                          ; preds = %header__7
  %76 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %terms, i64 %74)
  %77 = bitcast i8* %76 to { { double, double }*, %Array* }**
  %78 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %77, align 8
  %79 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %78, i32 0, i32 0
  %80 = load { double, double }*, { double, double }** %79, align 8
  %81 = bitcast { double, double }* %80 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %81, i32 -1)
  %82 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %78, i32 0, i32 1
  %83 = load %Array*, %Array** %82, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %83, i32 -1)
  %84 = bitcast { { double, double }*, %Array* }* %78 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %84, i32 -1)
  br label %exiting__7

exiting__7:                                       ; preds = %body__7
  %85 = add i64 %74, 1
  br label %header__7

exit__7:                                          ; preds = %header__7
  call void @__quantum__rt__array_update_alias_count(%Array* %terms, i32 -1)
  ret void
}

declare i64 @__quantum__rt__array_get_size_1d(%Array*)

declare i8* @__quantum__rt__array_get_element_ptr_1d(%Array*, i64)

declare void @__quantum__rt__tuple_update_alias_count(%Tuple*, i32)

declare void @__quantum__rt__array_update_alias_count(%Array*, i32)

define internal void @Microsoft__Quantum__Intrinsic__X__body(%Qubit* %qubit) {
entry:
  call void @__quantum__qis__x__body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledS____body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rx__body(double %theta, %Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledRx____body(double %theta, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__S__adj(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledSAdj____body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rz__body(double %theta, %Qubit* %qubit) {
entry:
  call void @__quantum__qis__rz__body(double %theta, %Qubit* %qubit)
  ret void
}

define internal double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__VariationalQuantumEigensolver__EstimateEnergy__body(i64 %nQubits, { %Array*, %Array*, %Array*, %Array* }* %hamiltonianTermList, { i64, %Array* }* %inputState, double %energyOffset, i64 %nSamples) {
entry:
  %0 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %hamiltonianTermList, i32 0, i32 0
  %ZData = load %Array*, %Array** %0, align 8
  %1 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ZData)
  %2 = sub i64 %1, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %3 = phi i64 [ 0, %entry ], [ %13, %exiting__1 ]
  %4 = icmp sle i64 %3, %2
  br i1 %4, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %5 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZData, i64 %3)
  %6 = bitcast i8* %5 to { %Array*, %Array* }**
  %7 = load { %Array*, %Array* }*, { %Array*, %Array* }** %6, align 8
  %8 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %7, i32 0, i32 0
  %9 = load %Array*, %Array** %8, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %9, i32 1)
  %10 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %7, i32 0, i32 1
  %11 = load %Array*, %Array** %10, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %11, i32 1)
  %12 = bitcast { %Array*, %Array* }* %7 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %12, i32 1)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %13 = add i64 %3, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ZData, i32 1)
  %14 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %hamiltonianTermList, i32 0, i32 1
  %ZZData = load %Array*, %Array** %14, align 8
  %15 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ZZData)
  %16 = sub i64 %15, 1
  br label %header__2

header__2:                                        ; preds = %exiting__2, %exit__1
  %17 = phi i64 [ 0, %exit__1 ], [ %27, %exiting__2 ]
  %18 = icmp sle i64 %17, %16
  br i1 %18, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %19 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZZData, i64 %17)
  %20 = bitcast i8* %19 to { %Array*, %Array* }**
  %21 = load { %Array*, %Array* }*, { %Array*, %Array* }** %20, align 8
  %22 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %21, i32 0, i32 0
  %23 = load %Array*, %Array** %22, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %23, i32 1)
  %24 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %21, i32 0, i32 1
  %25 = load %Array*, %Array** %24, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %25, i32 1)
  %26 = bitcast { %Array*, %Array* }* %21 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %26, i32 1)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %27 = add i64 %17, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %ZZData, i32 1)
  %28 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %hamiltonianTermList, i32 0, i32 2
  %PQandPQQRData = load %Array*, %Array** %28, align 8
  %29 = call i64 @__quantum__rt__array_get_size_1d(%Array* %PQandPQQRData)
  %30 = sub i64 %29, 1
  br label %header__3

header__3:                                        ; preds = %exiting__3, %exit__2
  %31 = phi i64 [ 0, %exit__2 ], [ %41, %exiting__3 ]
  %32 = icmp sle i64 %31, %30
  br i1 %32, label %body__3, label %exit__3

body__3:                                          ; preds = %header__3
  %33 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %PQandPQQRData, i64 %31)
  %34 = bitcast i8* %33 to { %Array*, %Array* }**
  %35 = load { %Array*, %Array* }*, { %Array*, %Array* }** %34, align 8
  %36 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %35, i32 0, i32 0
  %37 = load %Array*, %Array** %36, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %37, i32 1)
  %38 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %35, i32 0, i32 1
  %39 = load %Array*, %Array** %38, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %39, i32 1)
  %40 = bitcast { %Array*, %Array* }* %35 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %40, i32 1)
  br label %exiting__3

exiting__3:                                       ; preds = %body__3
  %41 = add i64 %31, 1
  br label %header__3

exit__3:                                          ; preds = %header__3
  call void @__quantum__rt__array_update_alias_count(%Array* %PQandPQQRData, i32 1)
  %42 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %hamiltonianTermList, i32 0, i32 3
  %h0123Data = load %Array*, %Array** %42, align 8
  %43 = call i64 @__quantum__rt__array_get_size_1d(%Array* %h0123Data)
  %44 = sub i64 %43, 1
  br label %header__4

header__4:                                        ; preds = %exiting__4, %exit__3
  %45 = phi i64 [ 0, %exit__3 ], [ %55, %exiting__4 ]
  %46 = icmp sle i64 %45, %44
  br i1 %46, label %body__4, label %exit__4

body__4:                                          ; preds = %header__4
  %47 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %h0123Data, i64 %45)
  %48 = bitcast i8* %47 to { %Array*, %Array* }**
  %49 = load { %Array*, %Array* }*, { %Array*, %Array* }** %48, align 8
  %50 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %49, i32 0, i32 0
  %51 = load %Array*, %Array** %50, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %51, i32 1)
  %52 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %49, i32 0, i32 1
  %53 = load %Array*, %Array** %52, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %53, i32 1)
  %54 = bitcast { %Array*, %Array* }* %49 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %54, i32 1)
  br label %exiting__4

exiting__4:                                       ; preds = %body__4
  %55 = add i64 %45, 1
  br label %header__4

exit__4:                                          ; preds = %header__4
  call void @__quantum__rt__array_update_alias_count(%Array* %h0123Data, i32 1)
  %56 = bitcast { %Array*, %Array*, %Array*, %Array* }* %hamiltonianTermList to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %56, i32 1)
  %57 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %inputState, i32 0, i32 1
  %inputStateTerms = load %Array*, %Array** %57, align 8
  %58 = call i64 @__quantum__rt__array_get_size_1d(%Array* %inputStateTerms)
  %59 = sub i64 %58, 1
  br label %header__5

header__5:                                        ; preds = %exiting__5, %exit__4
  %60 = phi i64 [ 0, %exit__4 ], [ %71, %exiting__5 ]
  %61 = icmp sle i64 %60, %59
  br i1 %61, label %body__5, label %exit__5

body__5:                                          ; preds = %header__5
  %62 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %inputStateTerms, i64 %60)
  %63 = bitcast i8* %62 to { { double, double }*, %Array* }**
  %64 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %63, align 8
  %65 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %64, i32 0, i32 0
  %66 = load { double, double }*, { double, double }** %65, align 8
  %67 = bitcast { double, double }* %66 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %67, i32 1)
  %68 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %64, i32 0, i32 1
  %69 = load %Array*, %Array** %68, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %69, i32 1)
  %70 = bitcast { { double, double }*, %Array* }* %64 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %70, i32 1)
  br label %exiting__5

exiting__5:                                       ; preds = %body__5
  %71 = add i64 %60, 1
  br label %header__5

exit__5:                                          ; preds = %header__5
  call void @__quantum__rt__array_update_alias_count(%Array* %inputStateTerms, i32 1)
  %72 = bitcast { i64, %Array* }* %inputState to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %72, i32 1)
  %energy = alloca double, align 8
  store double 0.000000e+00, double* %energy, align 8
  %73 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %inputState, i32 0, i32 0
  %inputStateType = load i64, i64* %73, align 4
  %74 = sub i64 %58, 1
  br label %header__6

header__6:                                        ; preds = %exiting__6, %exit__5
  %75 = phi i64 [ 0, %exit__5 ], [ %86, %exiting__6 ]
  %76 = icmp sle i64 %75, %74
  br i1 %76, label %body__6, label %exit__6

body__6:                                          ; preds = %header__6
  %77 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %inputStateTerms, i64 %75)
  %78 = bitcast i8* %77 to { { double, double }*, %Array* }**
  %79 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %78, align 8
  %80 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %79, i32 0, i32 0
  %81 = load { double, double }*, { double, double }** %80, align 8
  %82 = bitcast { double, double }* %81 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %82, i32 1)
  %83 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %79, i32 0, i32 1
  %84 = load %Array*, %Array** %83, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %84, i32 1)
  %85 = bitcast { { double, double }*, %Array* }* %79 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %85, i32 1)
  br label %exiting__6

exiting__6:                                       ; preds = %body__6
  %86 = add i64 %75, 1
  br label %header__6

exit__6:                                          ; preds = %header__6
  call void @__quantum__rt__array_update_alias_count(%Array* %inputStateTerms, i32 1)
  %87 = sub i64 %1, 1
  br label %header__7

header__7:                                        ; preds = %exiting__7, %exit__6
  %88 = phi i64 [ 0, %exit__6 ], [ %98, %exiting__7 ]
  %89 = icmp sle i64 %88, %87
  br i1 %89, label %body__7, label %exit__7

body__7:                                          ; preds = %header__7
  %90 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZData, i64 %88)
  %91 = bitcast i8* %90 to { %Array*, %Array* }**
  %92 = load { %Array*, %Array* }*, { %Array*, %Array* }** %91, align 8
  %93 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %92, i32 0, i32 0
  %94 = load %Array*, %Array** %93, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %94, i32 1)
  %95 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %92, i32 0, i32 1
  %96 = load %Array*, %Array** %95, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %96, i32 1)
  %97 = bitcast { %Array*, %Array* }* %92 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %97, i32 1)
  br label %exiting__7

exiting__7:                                       ; preds = %body__7
  %98 = add i64 %88, 1
  br label %header__7

exit__7:                                          ; preds = %header__7
  call void @__quantum__rt__array_update_alias_count(%Array* %ZData, i32 1)
  %99 = sub i64 %15, 1
  br label %header__8

header__8:                                        ; preds = %exiting__8, %exit__7
  %100 = phi i64 [ 0, %exit__7 ], [ %110, %exiting__8 ]
  %101 = icmp sle i64 %100, %99
  br i1 %101, label %body__8, label %exit__8

body__8:                                          ; preds = %header__8
  %102 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZZData, i64 %100)
  %103 = bitcast i8* %102 to { %Array*, %Array* }**
  %104 = load { %Array*, %Array* }*, { %Array*, %Array* }** %103, align 8
  %105 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %104, i32 0, i32 0
  %106 = load %Array*, %Array** %105, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %106, i32 1)
  %107 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %104, i32 0, i32 1
  %108 = load %Array*, %Array** %107, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %108, i32 1)
  %109 = bitcast { %Array*, %Array* }* %104 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %109, i32 1)
  br label %exiting__8

exiting__8:                                       ; preds = %body__8
  %110 = add i64 %100, 1
  br label %header__8

exit__8:                                          ; preds = %header__8
  call void @__quantum__rt__array_update_alias_count(%Array* %ZZData, i32 1)
  %111 = sub i64 %29, 1
  br label %header__9

header__9:                                        ; preds = %exiting__9, %exit__8
  %112 = phi i64 [ 0, %exit__8 ], [ %122, %exiting__9 ]
  %113 = icmp sle i64 %112, %111
  br i1 %113, label %body__9, label %exit__9

body__9:                                          ; preds = %header__9
  %114 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %PQandPQQRData, i64 %112)
  %115 = bitcast i8* %114 to { %Array*, %Array* }**
  %116 = load { %Array*, %Array* }*, { %Array*, %Array* }** %115, align 8
  %117 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %116, i32 0, i32 0
  %118 = load %Array*, %Array** %117, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %118, i32 1)
  %119 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %116, i32 0, i32 1
  %120 = load %Array*, %Array** %119, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %120, i32 1)
  %121 = bitcast { %Array*, %Array* }* %116 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %121, i32 1)
  br label %exiting__9

exiting__9:                                       ; preds = %body__9
  %122 = add i64 %112, 1
  br label %header__9

exit__9:                                          ; preds = %header__9
  call void @__quantum__rt__array_update_alias_count(%Array* %PQandPQQRData, i32 1)
  %123 = sub i64 %43, 1
  br label %header__10

header__10:                                       ; preds = %exiting__10, %exit__9
  %124 = phi i64 [ 0, %exit__9 ], [ %134, %exiting__10 ]
  %125 = icmp sle i64 %124, %123
  br i1 %125, label %body__10, label %exit__10

body__10:                                         ; preds = %header__10
  %126 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %h0123Data, i64 %124)
  %127 = bitcast i8* %126 to { %Array*, %Array* }**
  %128 = load { %Array*, %Array* }*, { %Array*, %Array* }** %127, align 8
  %129 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %128, i32 0, i32 0
  %130 = load %Array*, %Array** %129, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %130, i32 1)
  %131 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %128, i32 0, i32 1
  %132 = load %Array*, %Array** %131, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %132, i32 1)
  %133 = bitcast { %Array*, %Array* }* %128 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %133, i32 1)
  br label %exiting__10

exiting__10:                                      ; preds = %body__10
  %134 = add i64 %124, 1
  br label %header__10

exit__10:                                         ; preds = %header__10
  call void @__quantum__rt__array_update_alias_count(%Array* %h0123Data, i32 1)
  %hamiltonianTermArray = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 4)
  %135 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTermArray, i64 0)
  %136 = bitcast i8* %135 to %Array**
  %137 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTermArray, i64 1)
  %138 = bitcast i8* %137 to %Array**
  %139 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTermArray, i64 2)
  %140 = bitcast i8* %139 to %Array**
  %141 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTermArray, i64 3)
  %142 = bitcast i8* %141 to %Array**
  %143 = sub i64 %1, 1
  br label %header__11

header__11:                                       ; preds = %exiting__11, %exit__10
  %144 = phi i64 [ 0, %exit__10 ], [ %154, %exiting__11 ]
  %145 = icmp sle i64 %144, %143
  br i1 %145, label %body__11, label %exit__11

body__11:                                         ; preds = %header__11
  %146 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZData, i64 %144)
  %147 = bitcast i8* %146 to { %Array*, %Array* }**
  %148 = load { %Array*, %Array* }*, { %Array*, %Array* }** %147, align 8
  %149 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %148, i32 0, i32 0
  %150 = load %Array*, %Array** %149, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %150, i32 1)
  %151 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %148, i32 0, i32 1
  %152 = load %Array*, %Array** %151, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %152, i32 1)
  %153 = bitcast { %Array*, %Array* }* %148 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %153, i32 1)
  br label %exiting__11

exiting__11:                                      ; preds = %body__11
  %154 = add i64 %144, 1
  br label %header__11

exit__11:                                         ; preds = %header__11
  call void @__quantum__rt__array_update_reference_count(%Array* %ZData, i32 1)
  %155 = sub i64 %15, 1
  br label %header__12

header__12:                                       ; preds = %exiting__12, %exit__11
  %156 = phi i64 [ 0, %exit__11 ], [ %166, %exiting__12 ]
  %157 = icmp sle i64 %156, %155
  br i1 %157, label %body__12, label %exit__12

body__12:                                         ; preds = %header__12
  %158 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZZData, i64 %156)
  %159 = bitcast i8* %158 to { %Array*, %Array* }**
  %160 = load { %Array*, %Array* }*, { %Array*, %Array* }** %159, align 8
  %161 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %160, i32 0, i32 0
  %162 = load %Array*, %Array** %161, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %162, i32 1)
  %163 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %160, i32 0, i32 1
  %164 = load %Array*, %Array** %163, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %164, i32 1)
  %165 = bitcast { %Array*, %Array* }* %160 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %165, i32 1)
  br label %exiting__12

exiting__12:                                      ; preds = %body__12
  %166 = add i64 %156, 1
  br label %header__12

exit__12:                                         ; preds = %header__12
  call void @__quantum__rt__array_update_reference_count(%Array* %ZZData, i32 1)
  %167 = sub i64 %29, 1
  br label %header__13

header__13:                                       ; preds = %exiting__13, %exit__12
  %168 = phi i64 [ 0, %exit__12 ], [ %178, %exiting__13 ]
  %169 = icmp sle i64 %168, %167
  br i1 %169, label %body__13, label %exit__13

body__13:                                         ; preds = %header__13
  %170 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %PQandPQQRData, i64 %168)
  %171 = bitcast i8* %170 to { %Array*, %Array* }**
  %172 = load { %Array*, %Array* }*, { %Array*, %Array* }** %171, align 8
  %173 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %172, i32 0, i32 0
  %174 = load %Array*, %Array** %173, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %174, i32 1)
  %175 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %172, i32 0, i32 1
  %176 = load %Array*, %Array** %175, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %176, i32 1)
  %177 = bitcast { %Array*, %Array* }* %172 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %177, i32 1)
  br label %exiting__13

exiting__13:                                      ; preds = %body__13
  %178 = add i64 %168, 1
  br label %header__13

exit__13:                                         ; preds = %header__13
  call void @__quantum__rt__array_update_reference_count(%Array* %PQandPQQRData, i32 1)
  %179 = sub i64 %43, 1
  br label %header__14

header__14:                                       ; preds = %exiting__14, %exit__13
  %180 = phi i64 [ 0, %exit__13 ], [ %190, %exiting__14 ]
  %181 = icmp sle i64 %180, %179
  br i1 %181, label %body__14, label %exit__14

body__14:                                         ; preds = %header__14
  %182 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %h0123Data, i64 %180)
  %183 = bitcast i8* %182 to { %Array*, %Array* }**
  %184 = load { %Array*, %Array* }*, { %Array*, %Array* }** %183, align 8
  %185 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %184, i32 0, i32 0
  %186 = load %Array*, %Array** %185, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %186, i32 1)
  %187 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %184, i32 0, i32 1
  %188 = load %Array*, %Array** %187, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %188, i32 1)
  %189 = bitcast { %Array*, %Array* }* %184 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %189, i32 1)
  br label %exiting__14

exiting__14:                                      ; preds = %body__14
  %190 = add i64 %180, 1
  br label %header__14

exit__14:                                         ; preds = %header__14
  call void @__quantum__rt__array_update_reference_count(%Array* %h0123Data, i32 1)
  store %Array* %ZData, %Array** %136, align 8
  store %Array* %ZZData, %Array** %138, align 8
  store %Array* %PQandPQQRData, %Array** %140, align 8
  store %Array* %h0123Data, %Array** %142, align 8
  br label %header__15

header__15:                                       ; preds = %exiting__15, %exit__14
  %191 = phi i64 [ 0, %exit__14 ], [ %198, %exiting__15 ]
  %192 = icmp sle i64 %191, 3
  br i1 %192, label %body__15, label %exit__15

body__15:                                         ; preds = %header__15
  %193 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTermArray, i64 %191)
  %194 = bitcast i8* %193 to %Array**
  %195 = load %Array*, %Array** %194, align 8
  %196 = call i64 @__quantum__rt__array_get_size_1d(%Array* %195)
  %197 = sub i64 %196, 1
  br label %header__16

exiting__15:                                      ; preds = %exit__16
  %198 = add i64 %191, 1
  br label %header__15

exit__15:                                         ; preds = %header__15
  call void @__quantum__rt__array_update_alias_count(%Array* %hamiltonianTermArray, i32 1)
  %199 = add i64 %1, %15
  %200 = add i64 %199, %29
  %nTerms = add i64 %200, %43
  br label %header__17

header__16:                                       ; preds = %exiting__16, %body__15
  %201 = phi i64 [ 0, %body__15 ], [ %211, %exiting__16 ]
  %202 = icmp sle i64 %201, %197
  br i1 %202, label %body__16, label %exit__16

body__16:                                         ; preds = %header__16
  %203 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %195, i64 %201)
  %204 = bitcast i8* %203 to { %Array*, %Array* }**
  %205 = load { %Array*, %Array* }*, { %Array*, %Array* }** %204, align 8
  %206 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %205, i32 0, i32 0
  %207 = load %Array*, %Array** %206, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %207, i32 1)
  %208 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %205, i32 0, i32 1
  %209 = load %Array*, %Array** %208, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %209, i32 1)
  %210 = bitcast { %Array*, %Array* }* %205 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %210, i32 1)
  br label %exiting__16

exiting__16:                                      ; preds = %body__16
  %211 = add i64 %201, 1
  br label %header__16

exit__16:                                         ; preds = %header__16
  call void @__quantum__rt__array_update_alias_count(%Array* %195, i32 1)
  br label %exiting__15

header__17:                                       ; preds = %exiting__17, %exit__15
  %termType = phi i64 [ 0, %exit__15 ], [ %217, %exiting__17 ]
  %212 = icmp sle i64 %termType, 3
  br i1 %212, label %body__17, label %exit__17

body__17:                                         ; preds = %header__17
  %213 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTermArray, i64 %termType)
  %214 = bitcast i8* %213 to %Array**
  %hamiltonianTerms = load %Array*, %Array** %214, align 8
  %215 = call i64 @__quantum__rt__array_get_size_1d(%Array* %hamiltonianTerms)
  %216 = sub i64 %215, 1
  br label %header__18

exiting__17:                                      ; preds = %exit__23
  %217 = add i64 %termType, 1
  br label %header__17

exit__17:                                         ; preds = %header__17
  %218 = load double, double* %energy, align 8
  %219 = sub i64 %1, 1
  br label %header__24

header__18:                                       ; preds = %exiting__18, %body__17
  %220 = phi i64 [ 0, %body__17 ], [ %230, %exiting__18 ]
  %221 = icmp sle i64 %220, %216
  br i1 %221, label %body__18, label %exit__18

body__18:                                         ; preds = %header__18
  %222 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTerms, i64 %220)
  %223 = bitcast i8* %222 to { %Array*, %Array* }**
  %224 = load { %Array*, %Array* }*, { %Array*, %Array* }** %223, align 8
  %225 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %224, i32 0, i32 0
  %226 = load %Array*, %Array** %225, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %226, i32 1)
  %227 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %224, i32 0, i32 1
  %228 = load %Array*, %Array** %227, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %228, i32 1)
  %229 = bitcast { %Array*, %Array* }* %224 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %229, i32 1)
  br label %exiting__18

exiting__18:                                      ; preds = %body__18
  %230 = add i64 %220, 1
  br label %header__18

exit__18:                                         ; preds = %header__18
  call void @__quantum__rt__array_update_alias_count(%Array* %hamiltonianTerms, i32 1)
  %231 = sub i64 %215, 1
  br label %header__19

header__19:                                       ; preds = %exiting__19, %exit__18
  %232 = phi i64 [ 0, %exit__18 ], [ %241, %exiting__19 ]
  %233 = icmp sle i64 %232, %231
  br i1 %233, label %body__19, label %exit__19

body__19:                                         ; preds = %header__19
  %234 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTerms, i64 %232)
  %235 = bitcast i8* %234 to { %Array*, %Array* }**
  %hamiltonianTerm = load { %Array*, %Array* }*, { %Array*, %Array* }** %235, align 8
  %236 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %hamiltonianTerm, i32 0, i32 0
  %qubitIndices = load %Array*, %Array** %236, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %qubitIndices, i32 1)
  %237 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %hamiltonianTerm, i32 0, i32 1
  %coefficient = load %Array*, %Array** %237, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %coefficient, i32 1)
  %238 = bitcast { %Array*, %Array* }* %hamiltonianTerm to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %238, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %qubitIndices, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %coefficient, i32 1)
  %measOps = call %Array* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__Utils__VQEMeasurementOperators__body(i64 %nQubits, %Array* %qubitIndices, i64 %termType)
  %239 = call i64 @__quantum__rt__array_get_size_1d(%Array* %measOps)
  %240 = sub i64 %239, 1
  br label %header__20

exiting__19:                                      ; preds = %exit__22
  %241 = add i64 %232, 1
  br label %header__19

exit__19:                                         ; preds = %header__19
  %242 = sub i64 %215, 1
  br label %header__23

header__20:                                       ; preds = %exiting__20, %body__19
  %243 = phi i64 [ 0, %body__19 ], [ %248, %exiting__20 ]
  %244 = icmp sle i64 %243, %240
  br i1 %244, label %body__20, label %exit__20

body__20:                                         ; preds = %header__20
  %245 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %measOps, i64 %243)
  %246 = bitcast i8* %245 to %Array**
  %247 = load %Array*, %Array** %246, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %247, i32 1)
  br label %exiting__20

exiting__20:                                      ; preds = %body__20
  %248 = add i64 %243, 1
  br label %header__20

exit__20:                                         ; preds = %header__20
  call void @__quantum__rt__array_update_alias_count(%Array* %measOps, i32 1)
  %coefficients = call %Array* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__Utils__ExpandedCoefficients__body(%Array* %coefficient, i64 %termType)
  call void @__quantum__rt__array_update_alias_count(%Array* %coefficients, i32 1)
  %jwTermEnergy = call double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__EstimateEnergy__SumTermExpectation__body({ i64, %Array* }* %inputState, %Array* %measOps, %Array* %coefficients, i64 %nQubits, i64 %nSamples)
  %249 = load double, double* %energy, align 8
  %250 = fadd double %249, %jwTermEnergy
  store double %250, double* %energy, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %qubitIndices, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %coefficient, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %238, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %qubitIndices, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %coefficient, i32 -1)
  %251 = sub i64 %239, 1
  br label %header__21

header__21:                                       ; preds = %exiting__21, %exit__20
  %252 = phi i64 [ 0, %exit__20 ], [ %257, %exiting__21 ]
  %253 = icmp sle i64 %252, %251
  br i1 %253, label %body__21, label %exit__21

body__21:                                         ; preds = %header__21
  %254 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %measOps, i64 %252)
  %255 = bitcast i8* %254 to %Array**
  %256 = load %Array*, %Array** %255, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %256, i32 -1)
  br label %exiting__21

exiting__21:                                      ; preds = %body__21
  %257 = add i64 %252, 1
  br label %header__21

exit__21:                                         ; preds = %header__21
  call void @__quantum__rt__array_update_alias_count(%Array* %measOps, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %coefficients, i32 -1)
  %258 = sub i64 %239, 1
  br label %header__22

header__22:                                       ; preds = %exiting__22, %exit__21
  %259 = phi i64 [ 0, %exit__21 ], [ %264, %exiting__22 ]
  %260 = icmp sle i64 %259, %258
  br i1 %260, label %body__22, label %exit__22

body__22:                                         ; preds = %header__22
  %261 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %measOps, i64 %259)
  %262 = bitcast i8* %261 to %Array**
  %263 = load %Array*, %Array** %262, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %263, i32 -1)
  br label %exiting__22

exiting__22:                                      ; preds = %body__22
  %264 = add i64 %259, 1
  br label %header__22

exit__22:                                         ; preds = %header__22
  call void @__quantum__rt__array_update_reference_count(%Array* %measOps, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %coefficients, i32 -1)
  br label %exiting__19

header__23:                                       ; preds = %exiting__23, %exit__19
  %265 = phi i64 [ 0, %exit__19 ], [ %275, %exiting__23 ]
  %266 = icmp sle i64 %265, %242
  br i1 %266, label %body__23, label %exit__23

body__23:                                         ; preds = %header__23
  %267 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTerms, i64 %265)
  %268 = bitcast i8* %267 to { %Array*, %Array* }**
  %269 = load { %Array*, %Array* }*, { %Array*, %Array* }** %268, align 8
  %270 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %269, i32 0, i32 0
  %271 = load %Array*, %Array** %270, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %271, i32 -1)
  %272 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %269, i32 0, i32 1
  %273 = load %Array*, %Array** %272, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %273, i32 -1)
  %274 = bitcast { %Array*, %Array* }* %269 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %274, i32 -1)
  br label %exiting__23

exiting__23:                                      ; preds = %body__23
  %275 = add i64 %265, 1
  br label %header__23

exit__23:                                         ; preds = %header__23
  call void @__quantum__rt__array_update_alias_count(%Array* %hamiltonianTerms, i32 -1)
  br label %exiting__17

header__24:                                       ; preds = %exiting__24, %exit__17
  %276 = phi i64 [ 0, %exit__17 ], [ %286, %exiting__24 ]
  %277 = icmp sle i64 %276, %219
  br i1 %277, label %body__24, label %exit__24

body__24:                                         ; preds = %header__24
  %278 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZData, i64 %276)
  %279 = bitcast i8* %278 to { %Array*, %Array* }**
  %280 = load { %Array*, %Array* }*, { %Array*, %Array* }** %279, align 8
  %281 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %280, i32 0, i32 0
  %282 = load %Array*, %Array** %281, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %282, i32 -1)
  %283 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %280, i32 0, i32 1
  %284 = load %Array*, %Array** %283, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %284, i32 -1)
  %285 = bitcast { %Array*, %Array* }* %280 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %285, i32 -1)
  br label %exiting__24

exiting__24:                                      ; preds = %body__24
  %286 = add i64 %276, 1
  br label %header__24

exit__24:                                         ; preds = %header__24
  call void @__quantum__rt__array_update_alias_count(%Array* %ZData, i32 -1)
  %287 = sub i64 %15, 1
  br label %header__25

header__25:                                       ; preds = %exiting__25, %exit__24
  %288 = phi i64 [ 0, %exit__24 ], [ %298, %exiting__25 ]
  %289 = icmp sle i64 %288, %287
  br i1 %289, label %body__25, label %exit__25

body__25:                                         ; preds = %header__25
  %290 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZZData, i64 %288)
  %291 = bitcast i8* %290 to { %Array*, %Array* }**
  %292 = load { %Array*, %Array* }*, { %Array*, %Array* }** %291, align 8
  %293 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %292, i32 0, i32 0
  %294 = load %Array*, %Array** %293, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %294, i32 -1)
  %295 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %292, i32 0, i32 1
  %296 = load %Array*, %Array** %295, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %296, i32 -1)
  %297 = bitcast { %Array*, %Array* }* %292 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %297, i32 -1)
  br label %exiting__25

exiting__25:                                      ; preds = %body__25
  %298 = add i64 %288, 1
  br label %header__25

exit__25:                                         ; preds = %header__25
  call void @__quantum__rt__array_update_alias_count(%Array* %ZZData, i32 -1)
  %299 = sub i64 %29, 1
  br label %header__26

header__26:                                       ; preds = %exiting__26, %exit__25
  %300 = phi i64 [ 0, %exit__25 ], [ %310, %exiting__26 ]
  %301 = icmp sle i64 %300, %299
  br i1 %301, label %body__26, label %exit__26

body__26:                                         ; preds = %header__26
  %302 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %PQandPQQRData, i64 %300)
  %303 = bitcast i8* %302 to { %Array*, %Array* }**
  %304 = load { %Array*, %Array* }*, { %Array*, %Array* }** %303, align 8
  %305 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %304, i32 0, i32 0
  %306 = load %Array*, %Array** %305, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %306, i32 -1)
  %307 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %304, i32 0, i32 1
  %308 = load %Array*, %Array** %307, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %308, i32 -1)
  %309 = bitcast { %Array*, %Array* }* %304 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %309, i32 -1)
  br label %exiting__26

exiting__26:                                      ; preds = %body__26
  %310 = add i64 %300, 1
  br label %header__26

exit__26:                                         ; preds = %header__26
  call void @__quantum__rt__array_update_alias_count(%Array* %PQandPQQRData, i32 -1)
  %311 = sub i64 %43, 1
  br label %header__27

header__27:                                       ; preds = %exiting__27, %exit__26
  %312 = phi i64 [ 0, %exit__26 ], [ %322, %exiting__27 ]
  %313 = icmp sle i64 %312, %311
  br i1 %313, label %body__27, label %exit__27

body__27:                                         ; preds = %header__27
  %314 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %h0123Data, i64 %312)
  %315 = bitcast i8* %314 to { %Array*, %Array* }**
  %316 = load { %Array*, %Array* }*, { %Array*, %Array* }** %315, align 8
  %317 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %316, i32 0, i32 0
  %318 = load %Array*, %Array** %317, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %318, i32 -1)
  %319 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %316, i32 0, i32 1
  %320 = load %Array*, %Array** %319, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %320, i32 -1)
  %321 = bitcast { %Array*, %Array* }* %316 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %321, i32 -1)
  br label %exiting__27

exiting__27:                                      ; preds = %body__27
  %322 = add i64 %312, 1
  br label %header__27

exit__27:                                         ; preds = %header__27
  call void @__quantum__rt__array_update_alias_count(%Array* %h0123Data, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %56, i32 -1)
  %323 = sub i64 %58, 1
  br label %header__28

header__28:                                       ; preds = %exiting__28, %exit__27
  %324 = phi i64 [ 0, %exit__27 ], [ %335, %exiting__28 ]
  %325 = icmp sle i64 %324, %323
  br i1 %325, label %body__28, label %exit__28

body__28:                                         ; preds = %header__28
  %326 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %inputStateTerms, i64 %324)
  %327 = bitcast i8* %326 to { { double, double }*, %Array* }**
  %328 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %327, align 8
  %329 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %328, i32 0, i32 0
  %330 = load { double, double }*, { double, double }** %329, align 8
  %331 = bitcast { double, double }* %330 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %331, i32 -1)
  %332 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %328, i32 0, i32 1
  %333 = load %Array*, %Array** %332, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %333, i32 -1)
  %334 = bitcast { { double, double }*, %Array* }* %328 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %334, i32 -1)
  br label %exiting__28

exiting__28:                                      ; preds = %body__28
  %335 = add i64 %324, 1
  br label %header__28

exit__28:                                         ; preds = %header__28
  call void @__quantum__rt__array_update_alias_count(%Array* %inputStateTerms, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %72, i32 -1)
  %336 = sub i64 %58, 1
  br label %header__29

header__29:                                       ; preds = %exiting__29, %exit__28
  %337 = phi i64 [ 0, %exit__28 ], [ %348, %exiting__29 ]
  %338 = icmp sle i64 %337, %336
  br i1 %338, label %body__29, label %exit__29

body__29:                                         ; preds = %header__29
  %339 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %inputStateTerms, i64 %337)
  %340 = bitcast i8* %339 to { { double, double }*, %Array* }**
  %341 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %340, align 8
  %342 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %341, i32 0, i32 0
  %343 = load { double, double }*, { double, double }** %342, align 8
  %344 = bitcast { double, double }* %343 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %344, i32 -1)
  %345 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %341, i32 0, i32 1
  %346 = load %Array*, %Array** %345, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %346, i32 -1)
  %347 = bitcast { { double, double }*, %Array* }* %341 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %347, i32 -1)
  br label %exiting__29

exiting__29:                                      ; preds = %body__29
  %348 = add i64 %337, 1
  br label %header__29

exit__29:                                         ; preds = %header__29
  call void @__quantum__rt__array_update_alias_count(%Array* %inputStateTerms, i32 -1)
  %349 = sub i64 %1, 1
  br label %header__30

header__30:                                       ; preds = %exiting__30, %exit__29
  %350 = phi i64 [ 0, %exit__29 ], [ %360, %exiting__30 ]
  %351 = icmp sle i64 %350, %349
  br i1 %351, label %body__30, label %exit__30

body__30:                                         ; preds = %header__30
  %352 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZData, i64 %350)
  %353 = bitcast i8* %352 to { %Array*, %Array* }**
  %354 = load { %Array*, %Array* }*, { %Array*, %Array* }** %353, align 8
  %355 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %354, i32 0, i32 0
  %356 = load %Array*, %Array** %355, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %356, i32 -1)
  %357 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %354, i32 0, i32 1
  %358 = load %Array*, %Array** %357, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %358, i32 -1)
  %359 = bitcast { %Array*, %Array* }* %354 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %359, i32 -1)
  br label %exiting__30

exiting__30:                                      ; preds = %body__30
  %360 = add i64 %350, 1
  br label %header__30

exit__30:                                         ; preds = %header__30
  call void @__quantum__rt__array_update_alias_count(%Array* %ZData, i32 -1)
  %361 = sub i64 %15, 1
  br label %header__31

header__31:                                       ; preds = %exiting__31, %exit__30
  %362 = phi i64 [ 0, %exit__30 ], [ %372, %exiting__31 ]
  %363 = icmp sle i64 %362, %361
  br i1 %363, label %body__31, label %exit__31

body__31:                                         ; preds = %header__31
  %364 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ZZData, i64 %362)
  %365 = bitcast i8* %364 to { %Array*, %Array* }**
  %366 = load { %Array*, %Array* }*, { %Array*, %Array* }** %365, align 8
  %367 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %366, i32 0, i32 0
  %368 = load %Array*, %Array** %367, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %368, i32 -1)
  %369 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %366, i32 0, i32 1
  %370 = load %Array*, %Array** %369, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %370, i32 -1)
  %371 = bitcast { %Array*, %Array* }* %366 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %371, i32 -1)
  br label %exiting__31

exiting__31:                                      ; preds = %body__31
  %372 = add i64 %362, 1
  br label %header__31

exit__31:                                         ; preds = %header__31
  call void @__quantum__rt__array_update_alias_count(%Array* %ZZData, i32 -1)
  %373 = sub i64 %29, 1
  br label %header__32

header__32:                                       ; preds = %exiting__32, %exit__31
  %374 = phi i64 [ 0, %exit__31 ], [ %384, %exiting__32 ]
  %375 = icmp sle i64 %374, %373
  br i1 %375, label %body__32, label %exit__32

body__32:                                         ; preds = %header__32
  %376 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %PQandPQQRData, i64 %374)
  %377 = bitcast i8* %376 to { %Array*, %Array* }**
  %378 = load { %Array*, %Array* }*, { %Array*, %Array* }** %377, align 8
  %379 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %378, i32 0, i32 0
  %380 = load %Array*, %Array** %379, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %380, i32 -1)
  %381 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %378, i32 0, i32 1
  %382 = load %Array*, %Array** %381, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %382, i32 -1)
  %383 = bitcast { %Array*, %Array* }* %378 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %383, i32 -1)
  br label %exiting__32

exiting__32:                                      ; preds = %body__32
  %384 = add i64 %374, 1
  br label %header__32

exit__32:                                         ; preds = %header__32
  call void @__quantum__rt__array_update_alias_count(%Array* %PQandPQQRData, i32 -1)
  %385 = sub i64 %43, 1
  br label %header__33

header__33:                                       ; preds = %exiting__33, %exit__32
  %386 = phi i64 [ 0, %exit__32 ], [ %396, %exiting__33 ]
  %387 = icmp sle i64 %386, %385
  br i1 %387, label %body__33, label %exit__33

body__33:                                         ; preds = %header__33
  %388 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %h0123Data, i64 %386)
  %389 = bitcast i8* %388 to { %Array*, %Array* }**
  %390 = load { %Array*, %Array* }*, { %Array*, %Array* }** %389, align 8
  %391 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %390, i32 0, i32 0
  %392 = load %Array*, %Array** %391, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %392, i32 -1)
  %393 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %390, i32 0, i32 1
  %394 = load %Array*, %Array** %393, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %394, i32 -1)
  %395 = bitcast { %Array*, %Array* }* %390 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %395, i32 -1)
  br label %exiting__33

exiting__33:                                      ; preds = %body__33
  %396 = add i64 %386, 1
  br label %header__33

exit__33:                                         ; preds = %header__33
  call void @__quantum__rt__array_update_alias_count(%Array* %h0123Data, i32 -1)
  br label %header__34

header__34:                                       ; preds = %exiting__34, %exit__33
  %397 = phi i64 [ 0, %exit__33 ], [ %404, %exiting__34 ]
  %398 = icmp sle i64 %397, 3
  br i1 %398, label %body__34, label %exit__34

body__34:                                         ; preds = %header__34
  %399 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTermArray, i64 %397)
  %400 = bitcast i8* %399 to %Array**
  %401 = load %Array*, %Array** %400, align 8
  %402 = call i64 @__quantum__rt__array_get_size_1d(%Array* %401)
  %403 = sub i64 %402, 1
  br label %header__35

exiting__34:                                      ; preds = %exit__35
  %404 = add i64 %397, 1
  br label %header__34

exit__34:                                         ; preds = %header__34
  call void @__quantum__rt__array_update_alias_count(%Array* %hamiltonianTermArray, i32 -1)
  br label %header__36

header__35:                                       ; preds = %exiting__35, %body__34
  %405 = phi i64 [ 0, %body__34 ], [ %415, %exiting__35 ]
  %406 = icmp sle i64 %405, %403
  br i1 %406, label %body__35, label %exit__35

body__35:                                         ; preds = %header__35
  %407 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %401, i64 %405)
  %408 = bitcast i8* %407 to { %Array*, %Array* }**
  %409 = load { %Array*, %Array* }*, { %Array*, %Array* }** %408, align 8
  %410 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %409, i32 0, i32 0
  %411 = load %Array*, %Array** %410, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %411, i32 -1)
  %412 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %409, i32 0, i32 1
  %413 = load %Array*, %Array** %412, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %413, i32 -1)
  %414 = bitcast { %Array*, %Array* }* %409 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %414, i32 -1)
  br label %exiting__35

exiting__35:                                      ; preds = %body__35
  %415 = add i64 %405, 1
  br label %header__35

exit__35:                                         ; preds = %header__35
  call void @__quantum__rt__array_update_alias_count(%Array* %401, i32 -1)
  br label %exiting__34

header__36:                                       ; preds = %exiting__36, %exit__34
  %416 = phi i64 [ 0, %exit__34 ], [ %423, %exiting__36 ]
  %417 = icmp sle i64 %416, 3
  br i1 %417, label %body__36, label %exit__36

body__36:                                         ; preds = %header__36
  %418 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %hamiltonianTermArray, i64 %416)
  %419 = bitcast i8* %418 to %Array**
  %420 = load %Array*, %Array** %419, align 8
  %421 = call i64 @__quantum__rt__array_get_size_1d(%Array* %420)
  %422 = sub i64 %421, 1
  br label %header__37

exiting__36:                                      ; preds = %exit__37
  %423 = add i64 %416, 1
  br label %header__36

exit__36:                                         ; preds = %header__36
  call void @__quantum__rt__array_update_reference_count(%Array* %hamiltonianTermArray, i32 -1)
  ret double %218

header__37:                                       ; preds = %exiting__37, %body__36
  %424 = phi i64 [ 0, %body__36 ], [ %434, %exiting__37 ]
  %425 = icmp sle i64 %424, %422
  br i1 %425, label %body__37, label %exit__37

body__37:                                         ; preds = %header__37
  %426 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %420, i64 %424)
  %427 = bitcast i8* %426 to { %Array*, %Array* }**
  %428 = load { %Array*, %Array* }*, { %Array*, %Array* }** %427, align 8
  %429 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %428, i32 0, i32 0
  %430 = load %Array*, %Array** %429, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %430, i32 -1)
  %431 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %428, i32 0, i32 1
  %432 = load %Array*, %Array** %431, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %432, i32 -1)
  %433 = bitcast { %Array*, %Array* }* %428 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %433, i32 -1)
  br label %exiting__37

exiting__37:                                      ; preds = %body__37
  %434 = add i64 %424, 1
  br label %header__37

exit__37:                                         ; preds = %header__37
  call void @__quantum__rt__array_update_reference_count(%Array* %420, i32 -1)
  br label %exiting__36
}

declare %Array* @__quantum__rt__array_create_1d(i32, i64)

declare void @__quantum__rt__array_update_reference_count(%Array*, i32)

declare void @__quantum__rt__tuple_update_reference_count(%Tuple*, i32)

define internal %Array* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__Utils__VQEMeasurementOperators__body(i64 %nQubits, %Array* %indices, i64 %termType) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %indices, i32 1)
  %nOps = alloca i64, align 8
  store i64 0, i64* %nOps, align 4
  %0 = icmp eq i64 %termType, 2
  br i1 %0, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  store i64 2, i64* %nOps, align 4
  br label %continue__1

test1__1:                                         ; preds = %entry
  %1 = icmp eq i64 %termType, 3
  br i1 %1, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  store i64 8, i64* %nOps, align 4
  br label %continue__1

else__1:                                          ; preds = %test1__1
  store i64 1, i64* %nOps, align 4
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  %2 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 0)
  %3 = load i64, i64* %nOps, align 4
  %4 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 %3)
  %5 = sub i64 %3, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %continue__1
  %6 = phi i64 [ 0, %continue__1 ], [ %10, %exiting__1 ]
  %7 = icmp sle i64 %6, %5
  br i1 %7, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %8 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %4, i64 %6)
  %9 = bitcast i8* %8 to %Array**
  store %Array* %2, %Array** %9, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %2, i32 1)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %10 = add i64 %6, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %ops = alloca %Array*, align 8
  store %Array* %4, %Array** %ops, align 8
  %11 = sub i64 %3, 1
  br label %header__2

header__2:                                        ; preds = %exiting__2, %exit__1
  %12 = phi i64 [ 0, %exit__1 ], [ %17, %exiting__2 ]
  %13 = icmp sle i64 %12, %11
  br i1 %13, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %14 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %4, i64 %12)
  %15 = bitcast i8* %14 to %Array**
  %16 = load %Array*, %Array** %15, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %16, i32 1)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %17 = add i64 %12, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %4, i32 1)
  %18 = icmp eq i64 %termType, 0
  %19 = xor i1 %18, true
  br i1 %19, label %condTrue__1, label %condContinue__1

condTrue__1:                                      ; preds = %exit__2
  %20 = icmp eq i64 %termType, 1
  br label %condContinue__1

condContinue__1:                                  ; preds = %condTrue__1, %exit__2
  %21 = phi i1 [ %20, %condTrue__1 ], [ %18, %exit__2 ]
  br i1 %21, label %then0__2, label %test1__2

then0__2:                                         ; preds = %condContinue__1
  %22 = load i2, i2* @PauliI, align 1
  %23 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 %nQubits)
  %24 = sub i64 %nQubits, 1
  br label %header__3

test1__2:                                         ; preds = %condContinue__1
  %25 = icmp eq i64 %termType, 3
  br i1 %25, label %then1__2, label %test2__1

then1__2:                                         ; preds = %test1__2
  %compactOps = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 8)
  %26 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 0)
  %27 = bitcast i8* %26 to %Array**
  %28 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 1)
  %29 = bitcast i8* %28 to %Array**
  %30 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 2)
  %31 = bitcast i8* %30 to %Array**
  %32 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 3)
  %33 = bitcast i8* %32 to %Array**
  %34 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 4)
  %35 = bitcast i8* %34 to %Array**
  %36 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 5)
  %37 = bitcast i8* %36 to %Array**
  %38 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 6)
  %39 = bitcast i8* %38 to %Array**
  %40 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 7)
  %41 = bitcast i8* %40 to %Array**
  %42 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 4)
  %43 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %42, i64 0)
  %44 = bitcast i8* %43 to i2*
  %45 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %42, i64 1)
  %46 = bitcast i8* %45 to i2*
  %47 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %42, i64 2)
  %48 = bitcast i8* %47 to i2*
  %49 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %42, i64 3)
  %50 = bitcast i8* %49 to i2*
  %51 = load i2, i2* @PauliX, align 1
  %52 = load i2, i2* @PauliX, align 1
  %53 = load i2, i2* @PauliX, align 1
  %54 = load i2, i2* @PauliX, align 1
  store i2 %51, i2* %44, align 1
  store i2 %52, i2* %46, align 1
  store i2 %53, i2* %48, align 1
  store i2 %54, i2* %50, align 1
  %55 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 4)
  %56 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %55, i64 0)
  %57 = bitcast i8* %56 to i2*
  %58 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %55, i64 1)
  %59 = bitcast i8* %58 to i2*
  %60 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %55, i64 2)
  %61 = bitcast i8* %60 to i2*
  %62 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %55, i64 3)
  %63 = bitcast i8* %62 to i2*
  %64 = load i2, i2* @PauliY, align 1
  %65 = load i2, i2* @PauliY, align 1
  %66 = load i2, i2* @PauliY, align 1
  %67 = load i2, i2* @PauliY, align 1
  store i2 %64, i2* %57, align 1
  store i2 %65, i2* %59, align 1
  store i2 %66, i2* %61, align 1
  store i2 %67, i2* %63, align 1
  %68 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 4)
  %69 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %68, i64 0)
  %70 = bitcast i8* %69 to i2*
  %71 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %68, i64 1)
  %72 = bitcast i8* %71 to i2*
  %73 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %68, i64 2)
  %74 = bitcast i8* %73 to i2*
  %75 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %68, i64 3)
  %76 = bitcast i8* %75 to i2*
  %77 = load i2, i2* @PauliX, align 1
  %78 = load i2, i2* @PauliX, align 1
  %79 = load i2, i2* @PauliY, align 1
  %80 = load i2, i2* @PauliY, align 1
  store i2 %77, i2* %70, align 1
  store i2 %78, i2* %72, align 1
  store i2 %79, i2* %74, align 1
  store i2 %80, i2* %76, align 1
  %81 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 4)
  %82 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %81, i64 0)
  %83 = bitcast i8* %82 to i2*
  %84 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %81, i64 1)
  %85 = bitcast i8* %84 to i2*
  %86 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %81, i64 2)
  %87 = bitcast i8* %86 to i2*
  %88 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %81, i64 3)
  %89 = bitcast i8* %88 to i2*
  %90 = load i2, i2* @PauliY, align 1
  %91 = load i2, i2* @PauliY, align 1
  %92 = load i2, i2* @PauliX, align 1
  %93 = load i2, i2* @PauliX, align 1
  store i2 %90, i2* %83, align 1
  store i2 %91, i2* %85, align 1
  store i2 %92, i2* %87, align 1
  store i2 %93, i2* %89, align 1
  %94 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 4)
  %95 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %94, i64 0)
  %96 = bitcast i8* %95 to i2*
  %97 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %94, i64 1)
  %98 = bitcast i8* %97 to i2*
  %99 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %94, i64 2)
  %100 = bitcast i8* %99 to i2*
  %101 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %94, i64 3)
  %102 = bitcast i8* %101 to i2*
  %103 = load i2, i2* @PauliX, align 1
  %104 = load i2, i2* @PauliY, align 1
  %105 = load i2, i2* @PauliX, align 1
  %106 = load i2, i2* @PauliY, align 1
  store i2 %103, i2* %96, align 1
  store i2 %104, i2* %98, align 1
  store i2 %105, i2* %100, align 1
  store i2 %106, i2* %102, align 1
  %107 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 4)
  %108 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %107, i64 0)
  %109 = bitcast i8* %108 to i2*
  %110 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %107, i64 1)
  %111 = bitcast i8* %110 to i2*
  %112 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %107, i64 2)
  %113 = bitcast i8* %112 to i2*
  %114 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %107, i64 3)
  %115 = bitcast i8* %114 to i2*
  %116 = load i2, i2* @PauliY, align 1
  %117 = load i2, i2* @PauliX, align 1
  %118 = load i2, i2* @PauliY, align 1
  %119 = load i2, i2* @PauliX, align 1
  store i2 %116, i2* %109, align 1
  store i2 %117, i2* %111, align 1
  store i2 %118, i2* %113, align 1
  store i2 %119, i2* %115, align 1
  %120 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 4)
  %121 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %120, i64 0)
  %122 = bitcast i8* %121 to i2*
  %123 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %120, i64 1)
  %124 = bitcast i8* %123 to i2*
  %125 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %120, i64 2)
  %126 = bitcast i8* %125 to i2*
  %127 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %120, i64 3)
  %128 = bitcast i8* %127 to i2*
  %129 = load i2, i2* @PauliY, align 1
  %130 = load i2, i2* @PauliX, align 1
  %131 = load i2, i2* @PauliX, align 1
  %132 = load i2, i2* @PauliY, align 1
  store i2 %129, i2* %122, align 1
  store i2 %130, i2* %124, align 1
  store i2 %131, i2* %126, align 1
  store i2 %132, i2* %128, align 1
  %133 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 4)
  %134 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %133, i64 0)
  %135 = bitcast i8* %134 to i2*
  %136 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %133, i64 1)
  %137 = bitcast i8* %136 to i2*
  %138 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %133, i64 2)
  %139 = bitcast i8* %138 to i2*
  %140 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %133, i64 3)
  %141 = bitcast i8* %140 to i2*
  %142 = load i2, i2* @PauliX, align 1
  %143 = load i2, i2* @PauliY, align 1
  %144 = load i2, i2* @PauliY, align 1
  %145 = load i2, i2* @PauliX, align 1
  store i2 %142, i2* %135, align 1
  store i2 %143, i2* %137, align 1
  store i2 %144, i2* %139, align 1
  store i2 %145, i2* %141, align 1
  store %Array* %42, %Array** %27, align 8
  store %Array* %55, %Array** %29, align 8
  store %Array* %68, %Array** %31, align 8
  store %Array* %81, %Array** %33, align 8
  store %Array* %94, %Array** %35, align 8
  store %Array* %107, %Array** %37, align 8
  store %Array* %120, %Array** %39, align 8
  store %Array* %133, %Array** %41, align 8
  br label %header__5

test2__1:                                         ; preds = %test1__2
  %146 = icmp eq i64 %termType, 2
  br i1 %146, label %then2__1, label %continue__2

then2__1:                                         ; preds = %test2__1
  %compactOps__1 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %147 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps__1, i64 0)
  %148 = bitcast i8* %147 to %Array**
  %149 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps__1, i64 1)
  %150 = bitcast i8* %149 to %Array**
  %151 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 2)
  %152 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %151, i64 0)
  %153 = bitcast i8* %152 to i2*
  %154 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %151, i64 1)
  %155 = bitcast i8* %154 to i2*
  %156 = load i2, i2* @PauliX, align 1
  %157 = load i2, i2* @PauliX, align 1
  store i2 %156, i2* %153, align 1
  store i2 %157, i2* %155, align 1
  %158 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 2)
  %159 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %158, i64 0)
  %160 = bitcast i8* %159 to i2*
  %161 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %158, i64 1)
  %162 = bitcast i8* %161 to i2*
  %163 = load i2, i2* @PauliY, align 1
  %164 = load i2, i2* @PauliY, align 1
  store i2 %163, i2* %160, align 1
  store i2 %164, i2* %162, align 1
  store %Array* %151, %Array** %148, align 8
  store %Array* %158, %Array** %150, align 8
  br label %header__13

continue__2:                                      ; preds = %exit__18, %test2__1, %exit__12, %exit__4
  %165 = load %Array*, %Array** %ops, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %indices, i32 -1)
  %166 = call i64 @__quantum__rt__array_get_size_1d(%Array* %165)
  %167 = sub i64 %166, 1
  br label %header__19

header__3:                                        ; preds = %exiting__3, %then0__2
  %168 = phi i64 [ 0, %then0__2 ], [ %172, %exiting__3 ]
  %169 = icmp sle i64 %168, %24
  br i1 %169, label %body__3, label %exit__3

body__3:                                          ; preds = %header__3
  %170 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %23, i64 %168)
  %171 = bitcast i8* %170 to i2*
  store i2 %22, i2* %171, align 1
  br label %exiting__3

exiting__3:                                       ; preds = %body__3
  %172 = add i64 %168, 1
  br label %header__3

exit__3:                                          ; preds = %header__3
  %op = alloca %Array*, align 8
  store %Array* %23, %Array** %op, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %23, i32 1)
  %173 = call i64 @__quantum__rt__array_get_size_1d(%Array* %indices)
  %174 = sub i64 %173, 1
  br label %header__4

header__4:                                        ; preds = %exiting__4, %exit__3
  %175 = phi i64 [ 0, %exit__3 ], [ %184, %exiting__4 ]
  %176 = icmp sle i64 %175, %174
  br i1 %176, label %body__4, label %exit__4

body__4:                                          ; preds = %header__4
  %177 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 %175)
  %178 = bitcast i8* %177 to i64*
  %idx = load i64, i64* %178, align 4
  %179 = load %Array*, %Array** %op, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %179, i32 -1)
  %180 = call %Array* @__quantum__rt__array_copy(%Array* %179, i1 false)
  %181 = load i2, i2* @PauliZ, align 1
  %182 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %180, i64 %idx)
  %183 = bitcast i8* %182 to i2*
  store i2 %181, i2* %183, align 1
  call void @__quantum__rt__array_update_alias_count(%Array* %180, i32 1)
  store %Array* %180, %Array** %op, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %179, i32 -1)
  br label %exiting__4

exiting__4:                                       ; preds = %body__4
  %184 = add i64 %175, 1
  br label %header__4

exit__4:                                          ; preds = %header__4
  call void @__quantum__rt__array_update_alias_count(%Array* %4, i32 -1)
  %185 = call %Array* @__quantum__rt__array_copy(%Array* %4, i1 false)
  %186 = load %Array*, %Array** %op, align 8
  %187 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %185, i64 0)
  %188 = bitcast i8* %187 to %Array**
  call void @__quantum__rt__array_update_alias_count(%Array* %186, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %186, i32 1)
  %189 = load %Array*, %Array** %188, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %189, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %189, i32 -1)
  store %Array* %186, %Array** %188, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %185, i32 1)
  store %Array* %185, %Array** %ops, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %186, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %4, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %186, i32 -1)
  br label %continue__2

header__5:                                        ; preds = %exiting__5, %then1__2
  %190 = phi i64 [ 0, %then1__2 ], [ %195, %exiting__5 ]
  %191 = icmp sle i64 %190, 7
  br i1 %191, label %body__5, label %exit__5

body__5:                                          ; preds = %header__5
  %192 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 %190)
  %193 = bitcast i8* %192 to %Array**
  %194 = load %Array*, %Array** %193, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %194, i32 1)
  br label %exiting__5

exiting__5:                                       ; preds = %body__5
  %195 = add i64 %190, 1
  br label %header__5

exit__5:                                          ; preds = %header__5
  call void @__quantum__rt__array_update_alias_count(%Array* %compactOps, i32 1)
  br label %header__6

header__6:                                        ; preds = %exiting__6, %exit__5
  %iOp = phi i64 [ 0, %exit__5 ], [ %203, %exiting__6 ]
  %196 = icmp sle i64 %iOp, 7
  br i1 %196, label %body__6, label %exit__6

body__6:                                          ; preds = %header__6
  %197 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 %iOp)
  %198 = bitcast i8* %197 to %Array**
  %199 = load %Array*, %Array** %198, align 8
  %compactOp = alloca %Array*, align 8
  store %Array* %199, %Array** %compactOp, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %199, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %199, i32 1)
  %200 = load i2, i2* @PauliI, align 1
  %201 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 %nQubits)
  %202 = sub i64 %nQubits, 1
  br label %header__7

exiting__6:                                       ; preds = %exit__10
  %203 = add i64 %iOp, 1
  br label %header__6

exit__6:                                          ; preds = %header__6
  br label %header__11

header__7:                                        ; preds = %exiting__7, %body__6
  %204 = phi i64 [ 0, %body__6 ], [ %208, %exiting__7 ]
  %205 = icmp sle i64 %204, %202
  br i1 %205, label %body__7, label %exit__7

body__7:                                          ; preds = %header__7
  %206 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %201, i64 %204)
  %207 = bitcast i8* %206 to i2*
  store i2 %200, i2* %207, align 1
  br label %exiting__7

exiting__7:                                       ; preds = %body__7
  %208 = add i64 %204, 1
  br label %header__7

exit__7:                                          ; preds = %header__7
  %op__1 = alloca %Array*, align 8
  store %Array* %201, %Array** %op__1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %201, i32 1)
  %209 = call i64 @__quantum__rt__array_get_size_1d(%Array* %indices)
  %210 = sub i64 %209, 1
  br label %header__8

header__8:                                        ; preds = %exiting__8, %exit__7
  %i = phi i64 [ 0, %exit__7 ], [ %221, %exiting__8 ]
  %211 = icmp sle i64 %i, %210
  br i1 %211, label %body__8, label %exit__8

body__8:                                          ; preds = %header__8
  %212 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 %i)
  %213 = bitcast i8* %212 to i64*
  %idx__1 = load i64, i64* %213, align 4
  %214 = load %Array*, %Array** %compactOp, align 8
  %215 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %214, i64 %i)
  %216 = bitcast i8* %215 to i2*
  %pauli = load i2, i2* %216, align 1
  %217 = load %Array*, %Array** %op__1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %217, i32 -1)
  %218 = call %Array* @__quantum__rt__array_copy(%Array* %217, i1 false)
  %219 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %218, i64 %idx__1)
  %220 = bitcast i8* %219 to i2*
  store i2 %pauli, i2* %220, align 1
  call void @__quantum__rt__array_update_alias_count(%Array* %218, i32 1)
  store %Array* %218, %Array** %op__1, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %217, i32 -1)
  br label %exiting__8

exiting__8:                                       ; preds = %body__8
  %221 = add i64 %i, 1
  br label %header__8

exit__8:                                          ; preds = %header__8
  %222 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 0)
  %223 = bitcast i8* %222 to i64*
  %224 = load i64, i64* %223, align 4
  %225 = add i64 %224, 1
  %226 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 1)
  %227 = bitcast i8* %226 to i64*
  %228 = load i64, i64* %227, align 4
  %229 = sub i64 %228, 1
  br label %header__9

header__9:                                        ; preds = %exiting__9, %exit__8
  %i__1 = phi i64 [ %225, %exit__8 ], [ %236, %exiting__9 ]
  %230 = icmp sle i64 %i__1, %229
  br i1 %230, label %body__9, label %exit__9

body__9:                                          ; preds = %header__9
  %231 = load %Array*, %Array** %op__1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %231, i32 -1)
  %232 = call %Array* @__quantum__rt__array_copy(%Array* %231, i1 false)
  %233 = load i2, i2* @PauliZ, align 1
  %234 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %232, i64 %i__1)
  %235 = bitcast i8* %234 to i2*
  store i2 %233, i2* %235, align 1
  call void @__quantum__rt__array_update_alias_count(%Array* %232, i32 1)
  store %Array* %232, %Array** %op__1, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %231, i32 -1)
  br label %exiting__9

exiting__9:                                       ; preds = %body__9
  %236 = add i64 %i__1, 1
  br label %header__9

exit__9:                                          ; preds = %header__9
  %237 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 2)
  %238 = bitcast i8* %237 to i64*
  %239 = load i64, i64* %238, align 4
  %240 = add i64 %239, 1
  %241 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 3)
  %242 = bitcast i8* %241 to i64*
  %243 = load i64, i64* %242, align 4
  %244 = sub i64 %243, 1
  br label %header__10

header__10:                                       ; preds = %exiting__10, %exit__9
  %i__2 = phi i64 [ %240, %exit__9 ], [ %251, %exiting__10 ]
  %245 = icmp sle i64 %i__2, %244
  br i1 %245, label %body__10, label %exit__10

body__10:                                         ; preds = %header__10
  %246 = load %Array*, %Array** %op__1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %246, i32 -1)
  %247 = call %Array* @__quantum__rt__array_copy(%Array* %246, i1 false)
  %248 = load i2, i2* @PauliZ, align 1
  %249 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %247, i64 %i__2)
  %250 = bitcast i8* %249 to i2*
  store i2 %248, i2* %250, align 1
  call void @__quantum__rt__array_update_alias_count(%Array* %247, i32 1)
  store %Array* %247, %Array** %op__1, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %246, i32 -1)
  br label %exiting__10

exiting__10:                                      ; preds = %body__10
  %251 = add i64 %i__2, 1
  br label %header__10

exit__10:                                         ; preds = %header__10
  %252 = load %Array*, %Array** %ops, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %252, i32 -1)
  %253 = call %Array* @__quantum__rt__array_copy(%Array* %252, i1 false)
  %254 = load %Array*, %Array** %op__1, align 8
  %255 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %253, i64 %iOp)
  %256 = bitcast i8* %255 to %Array**
  call void @__quantum__rt__array_update_alias_count(%Array* %254, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %254, i32 1)
  %257 = load %Array*, %Array** %256, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %257, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %257, i32 -1)
  store %Array* %254, %Array** %256, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %253, i32 1)
  store %Array* %253, %Array** %ops, align 8
  %258 = load %Array*, %Array** %compactOp, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %258, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %254, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %252, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %258, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %254, i32 -1)
  br label %exiting__6

header__11:                                       ; preds = %exiting__11, %exit__6
  %259 = phi i64 [ 0, %exit__6 ], [ %264, %exiting__11 ]
  %260 = icmp sle i64 %259, 7
  br i1 %260, label %body__11, label %exit__11

body__11:                                         ; preds = %header__11
  %261 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 %259)
  %262 = bitcast i8* %261 to %Array**
  %263 = load %Array*, %Array** %262, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %263, i32 -1)
  br label %exiting__11

exiting__11:                                      ; preds = %body__11
  %264 = add i64 %259, 1
  br label %header__11

exit__11:                                         ; preds = %header__11
  call void @__quantum__rt__array_update_alias_count(%Array* %compactOps, i32 -1)
  br label %header__12

header__12:                                       ; preds = %exiting__12, %exit__11
  %265 = phi i64 [ 0, %exit__11 ], [ %270, %exiting__12 ]
  %266 = icmp sle i64 %265, 7
  br i1 %266, label %body__12, label %exit__12

body__12:                                         ; preds = %header__12
  %267 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps, i64 %265)
  %268 = bitcast i8* %267 to %Array**
  %269 = load %Array*, %Array** %268, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %269, i32 -1)
  br label %exiting__12

exiting__12:                                      ; preds = %body__12
  %270 = add i64 %265, 1
  br label %header__12

exit__12:                                         ; preds = %header__12
  call void @__quantum__rt__array_update_reference_count(%Array* %compactOps, i32 -1)
  br label %continue__2

header__13:                                       ; preds = %exiting__13, %then2__1
  %271 = phi i64 [ 0, %then2__1 ], [ %276, %exiting__13 ]
  %272 = icmp sle i64 %271, 1
  br i1 %272, label %body__13, label %exit__13

body__13:                                         ; preds = %header__13
  %273 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps__1, i64 %271)
  %274 = bitcast i8* %273 to %Array**
  %275 = load %Array*, %Array** %274, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %275, i32 1)
  br label %exiting__13

exiting__13:                                      ; preds = %body__13
  %276 = add i64 %271, 1
  br label %header__13

exit__13:                                         ; preds = %header__13
  call void @__quantum__rt__array_update_alias_count(%Array* %compactOps__1, i32 1)
  br label %header__14

header__14:                                       ; preds = %exiting__14, %exit__13
  %iOp__1 = phi i64 [ 0, %exit__13 ], [ %284, %exiting__14 ]
  %277 = icmp sle i64 %iOp__1, 1
  br i1 %277, label %body__14, label %exit__14

body__14:                                         ; preds = %header__14
  %278 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps__1, i64 %iOp__1)
  %279 = bitcast i8* %278 to %Array**
  %280 = load %Array*, %Array** %279, align 8
  %compactOp__1 = alloca %Array*, align 8
  store %Array* %280, %Array** %compactOp__1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %280, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %280, i32 1)
  %281 = load i2, i2* @PauliI, align 1
  %282 = call %Array* @__quantum__rt__array_create_1d(i32 1, i64 %nQubits)
  %283 = sub i64 %nQubits, 1
  br label %header__15

exiting__14:                                      ; preds = %continue__3
  %284 = add i64 %iOp__1, 1
  br label %header__14

exit__14:                                         ; preds = %header__14
  br label %header__17

header__15:                                       ; preds = %exiting__15, %body__14
  %285 = phi i64 [ 0, %body__14 ], [ %289, %exiting__15 ]
  %286 = icmp sle i64 %285, %283
  br i1 %286, label %body__15, label %exit__15

body__15:                                         ; preds = %header__15
  %287 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %282, i64 %285)
  %288 = bitcast i8* %287 to i2*
  store i2 %281, i2* %288, align 1
  br label %exiting__15

exiting__15:                                      ; preds = %body__15
  %289 = add i64 %285, 1
  br label %header__15

exit__15:                                         ; preds = %header__15
  %op__2 = alloca %Array*, align 8
  store %Array* %282, %Array** %op__2, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %282, i32 1)
  %nIndices = call i64 @__quantum__rt__array_get_size_1d(%Array* %indices)
  call void @__quantum__rt__array_update_alias_count(%Array* %282, i32 -1)
  %290 = call %Array* @__quantum__rt__array_copy(%Array* %282, i1 false)
  %291 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %280, i64 0)
  %292 = bitcast i8* %291 to i2*
  %293 = load i2, i2* %292, align 1
  %294 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 0)
  %295 = bitcast i8* %294 to i64*
  %296 = load i64, i64* %295, align 4
  %297 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %290, i64 %296)
  %298 = bitcast i8* %297 to i2*
  store i2 %293, i2* %298, align 1
  call void @__quantum__rt__array_update_alias_count(%Array* %290, i32 1)
  store %Array* %290, %Array** %op__2, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %290, i32 -1)
  %299 = call %Array* @__quantum__rt__array_copy(%Array* %290, i1 false)
  %300 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %280, i64 1)
  %301 = bitcast i8* %300 to i2*
  %302 = load i2, i2* %301, align 1
  %303 = sub i64 %nIndices, 1
  %304 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 %303)
  %305 = bitcast i8* %304 to i64*
  %306 = load i64, i64* %305, align 4
  %307 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %299, i64 %306)
  %308 = bitcast i8* %307 to i2*
  %309 = load i2, i2* %308, align 1
  store i2 %302, i2* %308, align 1
  call void @__quantum__rt__array_update_alias_count(%Array* %299, i32 1)
  store %Array* %299, %Array** %op__2, align 8
  %310 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 0)
  %311 = bitcast i8* %310 to i64*
  %312 = load i64, i64* %311, align 4
  %313 = add i64 %312, 1
  %314 = sub i64 %nIndices, 1
  %315 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 %314)
  %316 = bitcast i8* %315 to i64*
  %317 = load i64, i64* %316, align 4
  %318 = sub i64 %317, 1
  br label %header__16

header__16:                                       ; preds = %exiting__16, %exit__15
  %i__3 = phi i64 [ %313, %exit__15 ], [ %325, %exiting__16 ]
  %319 = icmp sle i64 %i__3, %318
  br i1 %319, label %body__16, label %exit__16

body__16:                                         ; preds = %header__16
  %320 = load %Array*, %Array** %op__2, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %320, i32 -1)
  %321 = call %Array* @__quantum__rt__array_copy(%Array* %320, i1 false)
  %322 = load i2, i2* @PauliZ, align 1
  %323 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %321, i64 %i__3)
  %324 = bitcast i8* %323 to i2*
  store i2 %322, i2* %324, align 1
  call void @__quantum__rt__array_update_alias_count(%Array* %321, i32 1)
  store %Array* %321, %Array** %op__2, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %320, i32 -1)
  br label %exiting__16

exiting__16:                                      ; preds = %body__16
  %325 = add i64 %i__3, 1
  br label %header__16

exit__16:                                         ; preds = %header__16
  %326 = icmp eq i64 %nIndices, 4
  br i1 %326, label %then0__3, label %continue__3

then0__3:                                         ; preds = %exit__16
  %327 = load %Array*, %Array** %op__2, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %327, i32 -1)
  %328 = call %Array* @__quantum__rt__array_copy(%Array* %327, i1 false)
  %329 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 0)
  %330 = bitcast i8* %329 to i64*
  %331 = load i64, i64* %330, align 4
  %332 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 1)
  %333 = bitcast i8* %332 to i64*
  %334 = load i64, i64* %333, align 4
  %335 = icmp slt i64 %331, %334
  br i1 %335, label %condTrue__2, label %condContinue__2

condTrue__2:                                      ; preds = %then0__3
  %336 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 1)
  %337 = bitcast i8* %336 to i64*
  %338 = load i64, i64* %337, align 4
  %339 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 3)
  %340 = bitcast i8* %339 to i64*
  %341 = load i64, i64* %340, align 4
  %342 = icmp slt i64 %338, %341
  br label %condContinue__2

condContinue__2:                                  ; preds = %condTrue__2, %then0__3
  %343 = phi i1 [ %342, %condTrue__2 ], [ %335, %then0__3 ]
  %344 = load i2, i2* @PauliI, align 1
  %345 = load i2, i2* @PauliZ, align 1
  %346 = select i1 %343, i2 %344, i2 %345
  %347 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %indices, i64 1)
  %348 = bitcast i8* %347 to i64*
  %349 = load i64, i64* %348, align 4
  %350 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %328, i64 %349)
  %351 = bitcast i8* %350 to i2*
  store i2 %346, i2* %351, align 1
  call void @__quantum__rt__array_update_alias_count(%Array* %328, i32 1)
  store %Array* %328, %Array** %op__2, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %327, i32 -1)
  br label %continue__3

continue__3:                                      ; preds = %condContinue__2, %exit__16
  %352 = load %Array*, %Array** %ops, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %352, i32 -1)
  %353 = call %Array* @__quantum__rt__array_copy(%Array* %352, i1 false)
  %354 = load %Array*, %Array** %op__2, align 8
  %355 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %353, i64 %iOp__1)
  %356 = bitcast i8* %355 to %Array**
  call void @__quantum__rt__array_update_alias_count(%Array* %354, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %354, i32 1)
  %357 = load %Array*, %Array** %356, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %357, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %357, i32 -1)
  store %Array* %354, %Array** %356, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %353, i32 1)
  store %Array* %353, %Array** %ops, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %280, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %354, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %282, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %290, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %352, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %280, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %354, i32 -1)
  br label %exiting__14

header__17:                                       ; preds = %exiting__17, %exit__14
  %358 = phi i64 [ 0, %exit__14 ], [ %363, %exiting__17 ]
  %359 = icmp sle i64 %358, 1
  br i1 %359, label %body__17, label %exit__17

body__17:                                         ; preds = %header__17
  %360 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps__1, i64 %358)
  %361 = bitcast i8* %360 to %Array**
  %362 = load %Array*, %Array** %361, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %362, i32 -1)
  br label %exiting__17

exiting__17:                                      ; preds = %body__17
  %363 = add i64 %358, 1
  br label %header__17

exit__17:                                         ; preds = %header__17
  call void @__quantum__rt__array_update_alias_count(%Array* %compactOps__1, i32 -1)
  br label %header__18

header__18:                                       ; preds = %exiting__18, %exit__17
  %364 = phi i64 [ 0, %exit__17 ], [ %369, %exiting__18 ]
  %365 = icmp sle i64 %364, 1
  br i1 %365, label %body__18, label %exit__18

body__18:                                         ; preds = %header__18
  %366 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %compactOps__1, i64 %364)
  %367 = bitcast i8* %366 to %Array**
  %368 = load %Array*, %Array** %367, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %368, i32 -1)
  br label %exiting__18

exiting__18:                                      ; preds = %body__18
  %369 = add i64 %364, 1
  br label %header__18

exit__18:                                         ; preds = %header__18
  call void @__quantum__rt__array_update_reference_count(%Array* %compactOps__1, i32 -1)
  br label %continue__2

header__19:                                       ; preds = %exiting__19, %continue__2
  %370 = phi i64 [ 0, %continue__2 ], [ %375, %exiting__19 ]
  %371 = icmp sle i64 %370, %167
  br i1 %371, label %body__19, label %exit__19

body__19:                                         ; preds = %header__19
  %372 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %165, i64 %370)
  %373 = bitcast i8* %372 to %Array**
  %374 = load %Array*, %Array** %373, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %374, i32 -1)
  br label %exiting__19

exiting__19:                                      ; preds = %body__19
  %375 = add i64 %370, 1
  br label %header__19

exit__19:                                         ; preds = %header__19
  call void @__quantum__rt__array_update_alias_count(%Array* %165, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %2, i32 -1)
  ret %Array* %165
}

define internal %Array* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__Utils__ExpandedCoefficients__body(%Array* %coeff, i64 %termType) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %coeff, i32 1)
  %nCoeffs = alloca i64, align 8
  store i64 0, i64* %nCoeffs, align 4
  %0 = icmp eq i64 %termType, 2
  br i1 %0, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  store i64 2, i64* %nCoeffs, align 4
  br label %continue__1

test1__1:                                         ; preds = %entry
  %1 = icmp eq i64 %termType, 3
  br i1 %1, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  store i64 8, i64* %nCoeffs, align 4
  br label %continue__1

else__1:                                          ; preds = %test1__1
  store i64 1, i64* %nCoeffs, align 4
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  %2 = load i64, i64* %nCoeffs, align 4
  %3 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 %2)
  %4 = sub i64 %2, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %continue__1
  %5 = phi i64 [ 0, %continue__1 ], [ %9, %exiting__1 ]
  %6 = icmp sle i64 %5, %4
  br i1 %6, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %3, i64 %5)
  %8 = bitcast i8* %7 to double*
  store double 0.000000e+00, double* %8, align 8
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %9 = add i64 %5, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %coeffs = alloca %Array*, align 8
  store %Array* %3, %Array** %coeffs, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %3, i32 1)
  %10 = icmp eq i64 %termType, 0
  %11 = xor i1 %10, true
  br i1 %11, label %condTrue__1, label %condContinue__1

condTrue__1:                                      ; preds = %exit__1
  %12 = icmp eq i64 %termType, 1
  br label %condContinue__1

condContinue__1:                                  ; preds = %condTrue__1, %exit__1
  %13 = phi i1 [ %12, %condTrue__1 ], [ %10, %exit__1 ]
  br i1 %13, label %then0__2, label %test1__2

then0__2:                                         ; preds = %condContinue__1
  call void @__quantum__rt__array_update_alias_count(%Array* %3, i32 -1)
  %14 = call %Array* @__quantum__rt__array_copy(%Array* %3, i1 false)
  %15 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %coeff, i64 0)
  %16 = bitcast i8* %15 to double*
  %17 = load double, double* %16, align 8
  %18 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %14, i64 0)
  %19 = bitcast i8* %18 to double*
  store double %17, double* %19, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %14, i32 1)
  store %Array* %14, %Array** %coeffs, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %3, i32 -1)
  br label %continue__2

test1__2:                                         ; preds = %condContinue__1
  %20 = icmp eq i64 %termType, 2
  %21 = xor i1 %20, true
  br i1 %21, label %condTrue__2, label %condContinue__2

condTrue__2:                                      ; preds = %test1__2
  %22 = icmp eq i64 %termType, 3
  br label %condContinue__2

condContinue__2:                                  ; preds = %condTrue__2, %test1__2
  %23 = phi i1 [ %22, %condTrue__2 ], [ %20, %test1__2 ]
  br i1 %23, label %then1__2, label %continue__2

then1__2:                                         ; preds = %condContinue__2
  %24 = sub i64 %2, 1
  br label %header__2

continue__2:                                      ; preds = %exit__2, %condContinue__2, %then0__2
  %25 = load %Array*, %Array** %coeffs, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %coeff, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %25, i32 -1)
  ret %Array* %25

header__2:                                        ; preds = %exiting__2, %then1__2
  %i = phi i64 [ 0, %then1__2 ], [ %35, %exiting__2 ]
  %26 = icmp sle i64 %i, %24
  br i1 %26, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %27 = load %Array*, %Array** %coeffs, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %27, i32 -1)
  %28 = call %Array* @__quantum__rt__array_copy(%Array* %27, i1 false)
  %29 = sdiv i64 %i, 2
  %30 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %coeff, i64 %29)
  %31 = bitcast i8* %30 to double*
  %32 = load double, double* %31, align 8
  %33 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %28, i64 %i)
  %34 = bitcast i8* %33 to double*
  store double %32, double* %34, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %28, i32 1)
  store %Array* %28, %Array** %coeffs, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %27, i32 -1)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %35 = add i64 %i, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  br label %continue__2
}

define internal double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__EstimateEnergy__SumTermExpectation__body({ i64, %Array* }* %inputState, %Array* %ops, %Array* %coeffs, i64 %nQubits, i64 %nSamples) {
entry:
  %0 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %inputState, i32 0, i32 1
  %1 = load %Array*, %Array** %0, align 8
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %1)
  %3 = sub i64 %2, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %4 = phi i64 [ 0, %entry ], [ %15, %exiting__1 ]
  %5 = icmp sle i64 %4, %3
  br i1 %5, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %6 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %1, i64 %4)
  %7 = bitcast i8* %6 to { { double, double }*, %Array* }**
  %8 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %7, align 8
  %9 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %8, i32 0, i32 0
  %10 = load { double, double }*, { double, double }** %9, align 8
  %11 = bitcast { double, double }* %10 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %11, i32 1)
  %12 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %8, i32 0, i32 1
  %13 = load %Array*, %Array** %12, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %13, i32 1)
  %14 = bitcast { { double, double }*, %Array* }* %8 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %14, i32 1)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %15 = add i64 %4, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  call void @__quantum__rt__array_update_alias_count(%Array* %1, i32 1)
  %16 = bitcast { i64, %Array* }* %inputState to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %16, i32 1)
  %17 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ops)
  %18 = sub i64 %17, 1
  br label %header__2

header__2:                                        ; preds = %exiting__2, %exit__1
  %19 = phi i64 [ 0, %exit__1 ], [ %24, %exiting__2 ]
  %20 = icmp sle i64 %19, %18
  br i1 %20, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %21 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ops, i64 %19)
  %22 = bitcast i8* %21 to %Array**
  %23 = load %Array*, %Array** %22, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %23, i32 1)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %24 = add i64 %19, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %ops, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %coeffs, i32 1)
  %jwTermEnergy = alloca double, align 8
  store double 0.000000e+00, double* %jwTermEnergy, align 8
  %25 = call i64 @__quantum__rt__array_get_size_1d(%Array* %coeffs)
  %26 = sub i64 %25, 1
  br label %header__3

header__3:                                        ; preds = %exiting__3, %exit__2
  %i = phi i64 [ 0, %exit__2 ], [ %41, %exiting__3 ]
  %27 = icmp sle i64 %i, %26
  br i1 %27, label %body__3, label %exit__3

body__3:                                          ; preds = %header__3
  %28 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %coeffs, i64 %i)
  %29 = bitcast i8* %28 to double*
  %coeff = load double, double* %29, align 8
  %30 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ops, i64 %i)
  %31 = bitcast i8* %30 to %Array**
  %op = load %Array*, %Array** %31, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %op, i32 1)
  %32 = fcmp oge double %coeff, 1.000000e-10
  %33 = xor i1 %32, true
  br i1 %33, label %condTrue__1, label %condContinue__1

condTrue__1:                                      ; preds = %body__3
  %34 = fcmp ole double %coeff, -1.000000e-10
  br label %condContinue__1

condContinue__1:                                  ; preds = %condTrue__1, %body__3
  %35 = phi i1 [ %34, %condTrue__1 ], [ %32, %body__3 ]
  br i1 %35, label %then0__1, label %continue__1

then0__1:                                         ; preds = %condContinue__1
  %termExpectation = call double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__EstimateEnergy__TermExpectation__body({ i64, %Array* }* %inputState, %Array* %op, i64 %nQubits, i64 %nSamples)
  %36 = load double, double* %jwTermEnergy, align 8
  %37 = fmul double 2.000000e+00, %termExpectation
  %38 = fsub double %37, 1.000000e+00
  %39 = fmul double %38, %coeff
  %40 = fadd double %36, %39
  store double %40, double* %jwTermEnergy, align 8
  br label %continue__1

continue__1:                                      ; preds = %then0__1, %condContinue__1
  call void @__quantum__rt__array_update_alias_count(%Array* %op, i32 -1)
  br label %exiting__3

exiting__3:                                       ; preds = %continue__1
  %41 = add i64 %i, 1
  br label %header__3

exit__3:                                          ; preds = %header__3
  %42 = load double, double* %jwTermEnergy, align 8
  %43 = sub i64 %2, 1
  br label %header__4

header__4:                                        ; preds = %exiting__4, %exit__3
  %44 = phi i64 [ 0, %exit__3 ], [ %55, %exiting__4 ]
  %45 = icmp sle i64 %44, %43
  br i1 %45, label %body__4, label %exit__4

body__4:                                          ; preds = %header__4
  %46 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %1, i64 %44)
  %47 = bitcast i8* %46 to { { double, double }*, %Array* }**
  %48 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %47, align 8
  %49 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %48, i32 0, i32 0
  %50 = load { double, double }*, { double, double }** %49, align 8
  %51 = bitcast { double, double }* %50 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %51, i32 -1)
  %52 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %48, i32 0, i32 1
  %53 = load %Array*, %Array** %52, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %53, i32 -1)
  %54 = bitcast { { double, double }*, %Array* }* %48 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %54, i32 -1)
  br label %exiting__4

exiting__4:                                       ; preds = %body__4
  %55 = add i64 %44, 1
  br label %header__4

exit__4:                                          ; preds = %header__4
  call void @__quantum__rt__array_update_alias_count(%Array* %1, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %16, i32 -1)
  %56 = sub i64 %17, 1
  br label %header__5

header__5:                                        ; preds = %exiting__5, %exit__4
  %57 = phi i64 [ 0, %exit__4 ], [ %62, %exiting__5 ]
  %58 = icmp sle i64 %57, %56
  br i1 %58, label %body__5, label %exit__5

body__5:                                          ; preds = %header__5
  %59 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ops, i64 %57)
  %60 = bitcast i8* %59 to %Array**
  %61 = load %Array*, %Array** %60, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %61, i32 -1)
  br label %exiting__5

exiting__5:                                       ; preds = %body__5
  %62 = add i64 %57, 1
  br label %header__5

exit__5:                                          ; preds = %header__5
  call void @__quantum__rt__array_update_alias_count(%Array* %ops, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %coeffs, i32 -1)
  ret double %42
}

define internal { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JordanWignerEncodingData__body(i64 %__Item1__, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, { i64, %Array* }* %0, double %__Item5__) {
entry:
  %1 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, i32 0, i32 0
  %2 = load %Array*, %Array** %1, align 8
  %3 = call i64 @__quantum__rt__array_get_size_1d(%Array* %2)
  %4 = sub i64 %3, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %5 = phi i64 [ 0, %entry ], [ %15, %exiting__1 ]
  %6 = icmp sle i64 %5, %4
  br i1 %6, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %2, i64 %5)
  %8 = bitcast i8* %7 to { %Array*, %Array* }**
  %9 = load { %Array*, %Array* }*, { %Array*, %Array* }** %8, align 8
  %10 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %9, i32 0, i32 0
  %11 = load %Array*, %Array** %10, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %11, i32 1)
  %12 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %9, i32 0, i32 1
  %13 = load %Array*, %Array** %12, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %13, i32 1)
  %14 = bitcast { %Array*, %Array* }* %9 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %14, i32 1)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %15 = add i64 %5, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  call void @__quantum__rt__array_update_alias_count(%Array* %2, i32 1)
  %16 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, i32 0, i32 1
  %17 = load %Array*, %Array** %16, align 8
  %18 = call i64 @__quantum__rt__array_get_size_1d(%Array* %17)
  %19 = sub i64 %18, 1
  br label %header__2

header__2:                                        ; preds = %exiting__2, %exit__1
  %20 = phi i64 [ 0, %exit__1 ], [ %30, %exiting__2 ]
  %21 = icmp sle i64 %20, %19
  br i1 %21, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %22 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %17, i64 %20)
  %23 = bitcast i8* %22 to { %Array*, %Array* }**
  %24 = load { %Array*, %Array* }*, { %Array*, %Array* }** %23, align 8
  %25 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %24, i32 0, i32 0
  %26 = load %Array*, %Array** %25, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %26, i32 1)
  %27 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %24, i32 0, i32 1
  %28 = load %Array*, %Array** %27, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %28, i32 1)
  %29 = bitcast { %Array*, %Array* }* %24 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %29, i32 1)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %30 = add i64 %20, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %17, i32 1)
  %31 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, i32 0, i32 2
  %32 = load %Array*, %Array** %31, align 8
  %33 = call i64 @__quantum__rt__array_get_size_1d(%Array* %32)
  %34 = sub i64 %33, 1
  br label %header__3

header__3:                                        ; preds = %exiting__3, %exit__2
  %35 = phi i64 [ 0, %exit__2 ], [ %45, %exiting__3 ]
  %36 = icmp sle i64 %35, %34
  br i1 %36, label %body__3, label %exit__3

body__3:                                          ; preds = %header__3
  %37 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %32, i64 %35)
  %38 = bitcast i8* %37 to { %Array*, %Array* }**
  %39 = load { %Array*, %Array* }*, { %Array*, %Array* }** %38, align 8
  %40 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %39, i32 0, i32 0
  %41 = load %Array*, %Array** %40, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %41, i32 1)
  %42 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %39, i32 0, i32 1
  %43 = load %Array*, %Array** %42, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %43, i32 1)
  %44 = bitcast { %Array*, %Array* }* %39 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %44, i32 1)
  br label %exiting__3

exiting__3:                                       ; preds = %body__3
  %45 = add i64 %35, 1
  br label %header__3

exit__3:                                          ; preds = %header__3
  call void @__quantum__rt__array_update_alias_count(%Array* %32, i32 1)
  %46 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, i32 0, i32 3
  %47 = load %Array*, %Array** %46, align 8
  %48 = call i64 @__quantum__rt__array_get_size_1d(%Array* %47)
  %49 = sub i64 %48, 1
  br label %header__4

header__4:                                        ; preds = %exiting__4, %exit__3
  %50 = phi i64 [ 0, %exit__3 ], [ %60, %exiting__4 ]
  %51 = icmp sle i64 %50, %49
  br i1 %51, label %body__4, label %exit__4

body__4:                                          ; preds = %header__4
  %52 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %47, i64 %50)
  %53 = bitcast i8* %52 to { %Array*, %Array* }**
  %54 = load { %Array*, %Array* }*, { %Array*, %Array* }** %53, align 8
  %55 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %54, i32 0, i32 0
  %56 = load %Array*, %Array** %55, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %56, i32 1)
  %57 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %54, i32 0, i32 1
  %58 = load %Array*, %Array** %57, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %58, i32 1)
  %59 = bitcast { %Array*, %Array* }* %54 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %59, i32 1)
  br label %exiting__4

exiting__4:                                       ; preds = %body__4
  %60 = add i64 %50, 1
  br label %header__4

exit__4:                                          ; preds = %header__4
  call void @__quantum__rt__array_update_alias_count(%Array* %47, i32 1)
  %61 = bitcast { %Array*, %Array*, %Array*, %Array* }* %__Item2__ to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %61, i32 1)
  %62 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* getelementptr ({ i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }, { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* null, i32 1) to i64))
  %63 = bitcast %Tuple* %62 to { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }*
  %64 = getelementptr inbounds { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }, { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* %63, i32 0, i32 0
  %65 = getelementptr inbounds { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }, { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* %63, i32 0, i32 1
  %66 = getelementptr inbounds { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }, { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* %63, i32 0, i32 2
  %67 = getelementptr inbounds { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }, { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* %63, i32 0, i32 3
  store i64 %__Item1__, i64* %64, align 4
  store { %Array*, %Array*, %Array*, %Array* }* %__Item2__, { %Array*, %Array*, %Array*, %Array* }** %65, align 8
  store { i64, %Array* }* %0, { i64, %Array* }** %66, align 8
  store double %__Item5__, double* %67, align 8
  %68 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, i32 0, i32 0
  %69 = load %Array*, %Array** %68, align 8
  %70 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, i32 0, i32 1
  %71 = load %Array*, %Array** %70, align 8
  %72 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, i32 0, i32 2
  %73 = load %Array*, %Array** %72, align 8
  %74 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %__Item2__, i32 0, i32 3
  %75 = load %Array*, %Array** %74, align 8
  %76 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %0, i32 0, i32 1
  %77 = load %Array*, %Array** %76, align 8
  %78 = call i64 @__quantum__rt__array_get_size_1d(%Array* %69)
  %79 = sub i64 %78, 1
  br label %header__5

header__5:                                        ; preds = %exiting__5, %exit__4
  %80 = phi i64 [ 0, %exit__4 ], [ %90, %exiting__5 ]
  %81 = icmp sle i64 %80, %79
  br i1 %81, label %body__5, label %exit__5

body__5:                                          ; preds = %header__5
  %82 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %69, i64 %80)
  %83 = bitcast i8* %82 to { %Array*, %Array* }**
  %84 = load { %Array*, %Array* }*, { %Array*, %Array* }** %83, align 8
  %85 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %84, i32 0, i32 0
  %86 = load %Array*, %Array** %85, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %86, i32 1)
  %87 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %84, i32 0, i32 1
  %88 = load %Array*, %Array** %87, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %88, i32 1)
  %89 = bitcast { %Array*, %Array* }* %84 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %89, i32 1)
  br label %exiting__5

exiting__5:                                       ; preds = %body__5
  %90 = add i64 %80, 1
  br label %header__5

exit__5:                                          ; preds = %header__5
  call void @__quantum__rt__array_update_reference_count(%Array* %69, i32 1)
  %91 = call i64 @__quantum__rt__array_get_size_1d(%Array* %71)
  %92 = sub i64 %91, 1
  br label %header__6

header__6:                                        ; preds = %exiting__6, %exit__5
  %93 = phi i64 [ 0, %exit__5 ], [ %103, %exiting__6 ]
  %94 = icmp sle i64 %93, %92
  br i1 %94, label %body__6, label %exit__6

body__6:                                          ; preds = %header__6
  %95 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %71, i64 %93)
  %96 = bitcast i8* %95 to { %Array*, %Array* }**
  %97 = load { %Array*, %Array* }*, { %Array*, %Array* }** %96, align 8
  %98 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %97, i32 0, i32 0
  %99 = load %Array*, %Array** %98, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %99, i32 1)
  %100 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %97, i32 0, i32 1
  %101 = load %Array*, %Array** %100, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %101, i32 1)
  %102 = bitcast { %Array*, %Array* }* %97 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %102, i32 1)
  br label %exiting__6

exiting__6:                                       ; preds = %body__6
  %103 = add i64 %93, 1
  br label %header__6

exit__6:                                          ; preds = %header__6
  call void @__quantum__rt__array_update_reference_count(%Array* %71, i32 1)
  %104 = call i64 @__quantum__rt__array_get_size_1d(%Array* %73)
  %105 = sub i64 %104, 1
  br label %header__7

header__7:                                        ; preds = %exiting__7, %exit__6
  %106 = phi i64 [ 0, %exit__6 ], [ %116, %exiting__7 ]
  %107 = icmp sle i64 %106, %105
  br i1 %107, label %body__7, label %exit__7

body__7:                                          ; preds = %header__7
  %108 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %73, i64 %106)
  %109 = bitcast i8* %108 to { %Array*, %Array* }**
  %110 = load { %Array*, %Array* }*, { %Array*, %Array* }** %109, align 8
  %111 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %110, i32 0, i32 0
  %112 = load %Array*, %Array** %111, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %112, i32 1)
  %113 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %110, i32 0, i32 1
  %114 = load %Array*, %Array** %113, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %114, i32 1)
  %115 = bitcast { %Array*, %Array* }* %110 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %115, i32 1)
  br label %exiting__7

exiting__7:                                       ; preds = %body__7
  %116 = add i64 %106, 1
  br label %header__7

exit__7:                                          ; preds = %header__7
  call void @__quantum__rt__array_update_reference_count(%Array* %73, i32 1)
  %117 = call i64 @__quantum__rt__array_get_size_1d(%Array* %75)
  %118 = sub i64 %117, 1
  br label %header__8

header__8:                                        ; preds = %exiting__8, %exit__7
  %119 = phi i64 [ 0, %exit__7 ], [ %129, %exiting__8 ]
  %120 = icmp sle i64 %119, %118
  br i1 %120, label %body__8, label %exit__8

body__8:                                          ; preds = %header__8
  %121 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %75, i64 %119)
  %122 = bitcast i8* %121 to { %Array*, %Array* }**
  %123 = load { %Array*, %Array* }*, { %Array*, %Array* }** %122, align 8
  %124 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %123, i32 0, i32 0
  %125 = load %Array*, %Array** %124, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %125, i32 1)
  %126 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %123, i32 0, i32 1
  %127 = load %Array*, %Array** %126, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %127, i32 1)
  %128 = bitcast { %Array*, %Array* }* %123 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %128, i32 1)
  br label %exiting__8

exiting__8:                                       ; preds = %body__8
  %129 = add i64 %119, 1
  br label %header__8

exit__8:                                          ; preds = %header__8
  call void @__quantum__rt__array_update_reference_count(%Array* %75, i32 1)
  %130 = bitcast { %Array*, %Array*, %Array*, %Array* }* %__Item2__ to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %130, i32 1)
  %131 = call i64 @__quantum__rt__array_get_size_1d(%Array* %77)
  %132 = sub i64 %131, 1
  br label %header__9

header__9:                                        ; preds = %exiting__9, %exit__8
  %133 = phi i64 [ 0, %exit__8 ], [ %144, %exiting__9 ]
  %134 = icmp sle i64 %133, %132
  br i1 %134, label %body__9, label %exit__9

body__9:                                          ; preds = %header__9
  %135 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %77, i64 %133)
  %136 = bitcast i8* %135 to { { double, double }*, %Array* }**
  %137 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %136, align 8
  %138 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %137, i32 0, i32 0
  %139 = load { double, double }*, { double, double }** %138, align 8
  %140 = bitcast { double, double }* %139 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %140, i32 1)
  %141 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %137, i32 0, i32 1
  %142 = load %Array*, %Array** %141, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %142, i32 1)
  %143 = bitcast { { double, double }*, %Array* }* %137 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %143, i32 1)
  br label %exiting__9

exiting__9:                                       ; preds = %body__9
  %144 = add i64 %133, 1
  br label %header__9

exit__9:                                          ; preds = %header__9
  call void @__quantum__rt__array_update_reference_count(%Array* %77, i32 1)
  %145 = bitcast { i64, %Array* }* %0 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %145, i32 1)
  %146 = sub i64 %3, 1
  br label %header__10

header__10:                                       ; preds = %exiting__10, %exit__9
  %147 = phi i64 [ 0, %exit__9 ], [ %157, %exiting__10 ]
  %148 = icmp sle i64 %147, %146
  br i1 %148, label %body__10, label %exit__10

body__10:                                         ; preds = %header__10
  %149 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %2, i64 %147)
  %150 = bitcast i8* %149 to { %Array*, %Array* }**
  %151 = load { %Array*, %Array* }*, { %Array*, %Array* }** %150, align 8
  %152 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %151, i32 0, i32 0
  %153 = load %Array*, %Array** %152, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %153, i32 -1)
  %154 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %151, i32 0, i32 1
  %155 = load %Array*, %Array** %154, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %155, i32 -1)
  %156 = bitcast { %Array*, %Array* }* %151 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %156, i32 -1)
  br label %exiting__10

exiting__10:                                      ; preds = %body__10
  %157 = add i64 %147, 1
  br label %header__10

exit__10:                                         ; preds = %header__10
  call void @__quantum__rt__array_update_alias_count(%Array* %2, i32 -1)
  %158 = sub i64 %18, 1
  br label %header__11

header__11:                                       ; preds = %exiting__11, %exit__10
  %159 = phi i64 [ 0, %exit__10 ], [ %169, %exiting__11 ]
  %160 = icmp sle i64 %159, %158
  br i1 %160, label %body__11, label %exit__11

body__11:                                         ; preds = %header__11
  %161 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %17, i64 %159)
  %162 = bitcast i8* %161 to { %Array*, %Array* }**
  %163 = load { %Array*, %Array* }*, { %Array*, %Array* }** %162, align 8
  %164 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %163, i32 0, i32 0
  %165 = load %Array*, %Array** %164, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %165, i32 -1)
  %166 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %163, i32 0, i32 1
  %167 = load %Array*, %Array** %166, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %167, i32 -1)
  %168 = bitcast { %Array*, %Array* }* %163 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %168, i32 -1)
  br label %exiting__11

exiting__11:                                      ; preds = %body__11
  %169 = add i64 %159, 1
  br label %header__11

exit__11:                                         ; preds = %header__11
  call void @__quantum__rt__array_update_alias_count(%Array* %17, i32 -1)
  %170 = sub i64 %33, 1
  br label %header__12

header__12:                                       ; preds = %exiting__12, %exit__11
  %171 = phi i64 [ 0, %exit__11 ], [ %181, %exiting__12 ]
  %172 = icmp sle i64 %171, %170
  br i1 %172, label %body__12, label %exit__12

body__12:                                         ; preds = %header__12
  %173 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %32, i64 %171)
  %174 = bitcast i8* %173 to { %Array*, %Array* }**
  %175 = load { %Array*, %Array* }*, { %Array*, %Array* }** %174, align 8
  %176 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %175, i32 0, i32 0
  %177 = load %Array*, %Array** %176, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %177, i32 -1)
  %178 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %175, i32 0, i32 1
  %179 = load %Array*, %Array** %178, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %179, i32 -1)
  %180 = bitcast { %Array*, %Array* }* %175 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %180, i32 -1)
  br label %exiting__12

exiting__12:                                      ; preds = %body__12
  %181 = add i64 %171, 1
  br label %header__12

exit__12:                                         ; preds = %header__12
  call void @__quantum__rt__array_update_alias_count(%Array* %32, i32 -1)
  %182 = sub i64 %48, 1
  br label %header__13

header__13:                                       ; preds = %exiting__13, %exit__12
  %183 = phi i64 [ 0, %exit__12 ], [ %193, %exiting__13 ]
  %184 = icmp sle i64 %183, %182
  br i1 %184, label %body__13, label %exit__13

body__13:                                         ; preds = %header__13
  %185 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %47, i64 %183)
  %186 = bitcast i8* %185 to { %Array*, %Array* }**
  %187 = load { %Array*, %Array* }*, { %Array*, %Array* }** %186, align 8
  %188 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %187, i32 0, i32 0
  %189 = load %Array*, %Array** %188, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %189, i32 -1)
  %190 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %187, i32 0, i32 1
  %191 = load %Array*, %Array** %190, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %191, i32 -1)
  %192 = bitcast { %Array*, %Array* }* %187 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %192, i32 -1)
  br label %exiting__13

exiting__13:                                      ; preds = %body__13
  %193 = add i64 %183, 1
  br label %header__13

exit__13:                                         ; preds = %header__13
  call void @__quantum__rt__array_update_alias_count(%Array* %47, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %61, i32 -1)
  ret { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* %63
}

declare %Tuple* @__quantum__rt__tuple_create(i64)

define internal { { double, double }*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JordanWignerInputState__body({ double, double }* %0, %Array* %__Item3__) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item3__, i32 1)
  %1 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ { double, double }*, %Array* }* getelementptr ({ { double, double }*, %Array* }, { { double, double }*, %Array* }* null, i32 1) to i64))
  %2 = bitcast %Tuple* %1 to { { double, double }*, %Array* }*
  %3 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %2, i32 0, i32 0
  %4 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %2, i32 0, i32 1
  store { double, double }* %0, { double, double }** %3, align 8
  store %Array* %__Item3__, %Array** %4, align 8
  %5 = bitcast { double, double }* %0 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %5, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__Item3__, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item3__, i32 -1)
  ret { { double, double }*, %Array* }* %2
}

define internal { %Array*, %Array*, %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JWOptimizedHTerms__body(%Array* %__Item1__, %Array* %__Item2__, %Array* %__Item3__, %Array* %__Item4__) {
entry:
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %__Item1__)
  %1 = sub i64 %0, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %2 = phi i64 [ 0, %entry ], [ %12, %exiting__1 ]
  %3 = icmp sle i64 %2, %1
  br i1 %3, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %4 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item1__, i64 %2)
  %5 = bitcast i8* %4 to { %Array*, %Array* }**
  %6 = load { %Array*, %Array* }*, { %Array*, %Array* }** %5, align 8
  %7 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %6, i32 0, i32 0
  %8 = load %Array*, %Array** %7, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %8, i32 1)
  %9 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %6, i32 0, i32 1
  %10 = load %Array*, %Array** %9, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %10, i32 1)
  %11 = bitcast { %Array*, %Array* }* %6 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %11, i32 1)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %12 = add i64 %2, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item1__, i32 1)
  %13 = call i64 @__quantum__rt__array_get_size_1d(%Array* %__Item2__)
  %14 = sub i64 %13, 1
  br label %header__2

header__2:                                        ; preds = %exiting__2, %exit__1
  %15 = phi i64 [ 0, %exit__1 ], [ %25, %exiting__2 ]
  %16 = icmp sle i64 %15, %14
  br i1 %16, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %17 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item2__, i64 %15)
  %18 = bitcast i8* %17 to { %Array*, %Array* }**
  %19 = load { %Array*, %Array* }*, { %Array*, %Array* }** %18, align 8
  %20 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %19, i32 0, i32 0
  %21 = load %Array*, %Array** %20, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %21, i32 1)
  %22 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %19, i32 0, i32 1
  %23 = load %Array*, %Array** %22, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %23, i32 1)
  %24 = bitcast { %Array*, %Array* }* %19 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %24, i32 1)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %25 = add i64 %15, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item2__, i32 1)
  %26 = call i64 @__quantum__rt__array_get_size_1d(%Array* %__Item3__)
  %27 = sub i64 %26, 1
  br label %header__3

header__3:                                        ; preds = %exiting__3, %exit__2
  %28 = phi i64 [ 0, %exit__2 ], [ %38, %exiting__3 ]
  %29 = icmp sle i64 %28, %27
  br i1 %29, label %body__3, label %exit__3

body__3:                                          ; preds = %header__3
  %30 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item3__, i64 %28)
  %31 = bitcast i8* %30 to { %Array*, %Array* }**
  %32 = load { %Array*, %Array* }*, { %Array*, %Array* }** %31, align 8
  %33 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %32, i32 0, i32 0
  %34 = load %Array*, %Array** %33, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %34, i32 1)
  %35 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %32, i32 0, i32 1
  %36 = load %Array*, %Array** %35, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %36, i32 1)
  %37 = bitcast { %Array*, %Array* }* %32 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %37, i32 1)
  br label %exiting__3

exiting__3:                                       ; preds = %body__3
  %38 = add i64 %28, 1
  br label %header__3

exit__3:                                          ; preds = %header__3
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item3__, i32 1)
  %39 = call i64 @__quantum__rt__array_get_size_1d(%Array* %__Item4__)
  %40 = sub i64 %39, 1
  br label %header__4

header__4:                                        ; preds = %exiting__4, %exit__3
  %41 = phi i64 [ 0, %exit__3 ], [ %51, %exiting__4 ]
  %42 = icmp sle i64 %41, %40
  br i1 %42, label %body__4, label %exit__4

body__4:                                          ; preds = %header__4
  %43 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item4__, i64 %41)
  %44 = bitcast i8* %43 to { %Array*, %Array* }**
  %45 = load { %Array*, %Array* }*, { %Array*, %Array* }** %44, align 8
  %46 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %45, i32 0, i32 0
  %47 = load %Array*, %Array** %46, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %47, i32 1)
  %48 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %45, i32 0, i32 1
  %49 = load %Array*, %Array** %48, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %49, i32 1)
  %50 = bitcast { %Array*, %Array* }* %45 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %50, i32 1)
  br label %exiting__4

exiting__4:                                       ; preds = %body__4
  %51 = add i64 %41, 1
  br label %header__4

exit__4:                                          ; preds = %header__4
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item4__, i32 1)
  %52 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Array*, %Array*, %Array* }* getelementptr ({ %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* null, i32 1) to i64))
  %53 = bitcast %Tuple* %52 to { %Array*, %Array*, %Array*, %Array* }*
  %54 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %53, i32 0, i32 0
  %55 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %53, i32 0, i32 1
  %56 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %53, i32 0, i32 2
  %57 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %53, i32 0, i32 3
  store %Array* %__Item1__, %Array** %54, align 8
  store %Array* %__Item2__, %Array** %55, align 8
  store %Array* %__Item3__, %Array** %56, align 8
  store %Array* %__Item4__, %Array** %57, align 8
  %58 = call i64 @__quantum__rt__array_get_size_1d(%Array* %__Item1__)
  %59 = sub i64 %58, 1
  br label %header__5

header__5:                                        ; preds = %exiting__5, %exit__4
  %60 = phi i64 [ 0, %exit__4 ], [ %70, %exiting__5 ]
  %61 = icmp sle i64 %60, %59
  br i1 %61, label %body__5, label %exit__5

body__5:                                          ; preds = %header__5
  %62 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item1__, i64 %60)
  %63 = bitcast i8* %62 to { %Array*, %Array* }**
  %64 = load { %Array*, %Array* }*, { %Array*, %Array* }** %63, align 8
  %65 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %64, i32 0, i32 0
  %66 = load %Array*, %Array** %65, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %66, i32 1)
  %67 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %64, i32 0, i32 1
  %68 = load %Array*, %Array** %67, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %68, i32 1)
  %69 = bitcast { %Array*, %Array* }* %64 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %69, i32 1)
  br label %exiting__5

exiting__5:                                       ; preds = %body__5
  %70 = add i64 %60, 1
  br label %header__5

exit__5:                                          ; preds = %header__5
  call void @__quantum__rt__array_update_reference_count(%Array* %__Item1__, i32 1)
  %71 = call i64 @__quantum__rt__array_get_size_1d(%Array* %__Item2__)
  %72 = sub i64 %71, 1
  br label %header__6

header__6:                                        ; preds = %exiting__6, %exit__5
  %73 = phi i64 [ 0, %exit__5 ], [ %83, %exiting__6 ]
  %74 = icmp sle i64 %73, %72
  br i1 %74, label %body__6, label %exit__6

body__6:                                          ; preds = %header__6
  %75 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item2__, i64 %73)
  %76 = bitcast i8* %75 to { %Array*, %Array* }**
  %77 = load { %Array*, %Array* }*, { %Array*, %Array* }** %76, align 8
  %78 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %77, i32 0, i32 0
  %79 = load %Array*, %Array** %78, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %79, i32 1)
  %80 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %77, i32 0, i32 1
  %81 = load %Array*, %Array** %80, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %81, i32 1)
  %82 = bitcast { %Array*, %Array* }* %77 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %82, i32 1)
  br label %exiting__6

exiting__6:                                       ; preds = %body__6
  %83 = add i64 %73, 1
  br label %header__6

exit__6:                                          ; preds = %header__6
  call void @__quantum__rt__array_update_reference_count(%Array* %__Item2__, i32 1)
  %84 = call i64 @__quantum__rt__array_get_size_1d(%Array* %__Item3__)
  %85 = sub i64 %84, 1
  br label %header__7

header__7:                                        ; preds = %exiting__7, %exit__6
  %86 = phi i64 [ 0, %exit__6 ], [ %96, %exiting__7 ]
  %87 = icmp sle i64 %86, %85
  br i1 %87, label %body__7, label %exit__7

body__7:                                          ; preds = %header__7
  %88 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item3__, i64 %86)
  %89 = bitcast i8* %88 to { %Array*, %Array* }**
  %90 = load { %Array*, %Array* }*, { %Array*, %Array* }** %89, align 8
  %91 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %90, i32 0, i32 0
  %92 = load %Array*, %Array** %91, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %92, i32 1)
  %93 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %90, i32 0, i32 1
  %94 = load %Array*, %Array** %93, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %94, i32 1)
  %95 = bitcast { %Array*, %Array* }* %90 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %95, i32 1)
  br label %exiting__7

exiting__7:                                       ; preds = %body__7
  %96 = add i64 %86, 1
  br label %header__7

exit__7:                                          ; preds = %header__7
  call void @__quantum__rt__array_update_reference_count(%Array* %__Item3__, i32 1)
  %97 = call i64 @__quantum__rt__array_get_size_1d(%Array* %__Item4__)
  %98 = sub i64 %97, 1
  br label %header__8

header__8:                                        ; preds = %exiting__8, %exit__7
  %99 = phi i64 [ 0, %exit__7 ], [ %109, %exiting__8 ]
  %100 = icmp sle i64 %99, %98
  br i1 %100, label %body__8, label %exit__8

body__8:                                          ; preds = %header__8
  %101 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item4__, i64 %99)
  %102 = bitcast i8* %101 to { %Array*, %Array* }**
  %103 = load { %Array*, %Array* }*, { %Array*, %Array* }** %102, align 8
  %104 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %103, i32 0, i32 0
  %105 = load %Array*, %Array** %104, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %105, i32 1)
  %106 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %103, i32 0, i32 1
  %107 = load %Array*, %Array** %106, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %107, i32 1)
  %108 = bitcast { %Array*, %Array* }* %103 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %108, i32 1)
  br label %exiting__8

exiting__8:                                       ; preds = %body__8
  %109 = add i64 %99, 1
  br label %header__8

exit__8:                                          ; preds = %header__8
  call void @__quantum__rt__array_update_reference_count(%Array* %__Item4__, i32 1)
  %110 = sub i64 %0, 1
  br label %header__9

header__9:                                        ; preds = %exiting__9, %exit__8
  %111 = phi i64 [ 0, %exit__8 ], [ %121, %exiting__9 ]
  %112 = icmp sle i64 %111, %110
  br i1 %112, label %body__9, label %exit__9

body__9:                                          ; preds = %header__9
  %113 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item1__, i64 %111)
  %114 = bitcast i8* %113 to { %Array*, %Array* }**
  %115 = load { %Array*, %Array* }*, { %Array*, %Array* }** %114, align 8
  %116 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %115, i32 0, i32 0
  %117 = load %Array*, %Array** %116, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %117, i32 -1)
  %118 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %115, i32 0, i32 1
  %119 = load %Array*, %Array** %118, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %119, i32 -1)
  %120 = bitcast { %Array*, %Array* }* %115 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %120, i32 -1)
  br label %exiting__9

exiting__9:                                       ; preds = %body__9
  %121 = add i64 %111, 1
  br label %header__9

exit__9:                                          ; preds = %header__9
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item1__, i32 -1)
  %122 = sub i64 %13, 1
  br label %header__10

header__10:                                       ; preds = %exiting__10, %exit__9
  %123 = phi i64 [ 0, %exit__9 ], [ %133, %exiting__10 ]
  %124 = icmp sle i64 %123, %122
  br i1 %124, label %body__10, label %exit__10

body__10:                                         ; preds = %header__10
  %125 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item2__, i64 %123)
  %126 = bitcast i8* %125 to { %Array*, %Array* }**
  %127 = load { %Array*, %Array* }*, { %Array*, %Array* }** %126, align 8
  %128 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %127, i32 0, i32 0
  %129 = load %Array*, %Array** %128, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %129, i32 -1)
  %130 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %127, i32 0, i32 1
  %131 = load %Array*, %Array** %130, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %131, i32 -1)
  %132 = bitcast { %Array*, %Array* }* %127 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %132, i32 -1)
  br label %exiting__10

exiting__10:                                      ; preds = %body__10
  %133 = add i64 %123, 1
  br label %header__10

exit__10:                                         ; preds = %header__10
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item2__, i32 -1)
  %134 = sub i64 %26, 1
  br label %header__11

header__11:                                       ; preds = %exiting__11, %exit__10
  %135 = phi i64 [ 0, %exit__10 ], [ %145, %exiting__11 ]
  %136 = icmp sle i64 %135, %134
  br i1 %136, label %body__11, label %exit__11

body__11:                                         ; preds = %header__11
  %137 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item3__, i64 %135)
  %138 = bitcast i8* %137 to { %Array*, %Array* }**
  %139 = load { %Array*, %Array* }*, { %Array*, %Array* }** %138, align 8
  %140 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %139, i32 0, i32 0
  %141 = load %Array*, %Array** %140, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %141, i32 -1)
  %142 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %139, i32 0, i32 1
  %143 = load %Array*, %Array** %142, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %143, i32 -1)
  %144 = bitcast { %Array*, %Array* }* %139 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %144, i32 -1)
  br label %exiting__11

exiting__11:                                      ; preds = %body__11
  %145 = add i64 %135, 1
  br label %header__11

exit__11:                                         ; preds = %header__11
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item3__, i32 -1)
  %146 = sub i64 %39, 1
  br label %header__12

header__12:                                       ; preds = %exiting__12, %exit__11
  %147 = phi i64 [ 0, %exit__11 ], [ %157, %exiting__12 ]
  %148 = icmp sle i64 %147, %146
  br i1 %148, label %body__12, label %exit__12

body__12:                                         ; preds = %header__12
  %149 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__Item4__, i64 %147)
  %150 = bitcast i8* %149 to { %Array*, %Array* }**
  %151 = load { %Array*, %Array* }*, { %Array*, %Array* }** %150, align 8
  %152 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %151, i32 0, i32 0
  %153 = load %Array*, %Array** %152, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %153, i32 -1)
  %154 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %151, i32 0, i32 1
  %155 = load %Array*, %Array** %154, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %155, i32 -1)
  %156 = bitcast { %Array*, %Array* }* %151 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %156, i32 -1)
  br label %exiting__12

exiting__12:                                      ; preds = %body__12
  %157 = add i64 %147, 1
  br label %header__12

exit__12:                                         ; preds = %header__12
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item4__, i32 -1)
  ret { %Array*, %Array*, %Array*, %Array* }* %53
}

define internal %Result* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__EstimateEnergy__JointMeasure__body(%Array* %ops, %Array* %qbs) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ops, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %qbs, i32 1)
  %aux = call %Qubit* @__quantum__rt__qubit_allocate()
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %qbs)
  %1 = sub i64 %0, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %i = phi i64 [ 0, %entry ], [ %13, %exiting__1 ]
  %2 = icmp sle i64 %i, %1
  br i1 %2, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %3 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ops, i64 %i)
  %4 = bitcast i8* %3 to i2*
  %op = load i2, i2* %4, align 1
  %5 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %qbs, i64 %i)
  %6 = bitcast i8* %5 to %Qubit**
  %qb = load %Qubit*, %Qubit** %6, align 8
  %7 = load i2, i2* @PauliX, align 1
  %8 = icmp eq i2 %op, %7
  br i1 %8, label %then0__1, label %test1__1

then0__1:                                         ; preds = %body__1
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qb)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %qb, %Qubit* %aux)
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qb)
  br label %continue__1

test1__1:                                         ; preds = %body__1
  %9 = load i2, i2* @PauliY, align 1
  %10 = icmp eq i2 %op, %9
  br i1 %10, label %then1__1, label %test2__1

then1__1:                                         ; preds = %test1__1
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qb)
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qb)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %qb, %Qubit* %aux)
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qb)
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qb)
  call void @Microsoft__Quantum__Intrinsic__Z__body(%Qubit* %qb)
  br label %continue__1

test2__1:                                         ; preds = %test1__1
  %11 = load i2, i2* @PauliZ, align 1
  %12 = icmp eq i2 %op, %11
  br i1 %12, label %then2__1, label %continue__1

then2__1:                                         ; preds = %test2__1
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %qb, %Qubit* %aux)
  br label %continue__1

continue__1:                                      ; preds = %then2__1, %test2__1, %then1__1, %then0__1
  br label %exiting__1

exiting__1:                                       ; preds = %continue__1
  %13 = add i64 %i, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %14 = call %Result* @Microsoft__Quantum__Intrinsic__M__body(%Qubit* %aux)
  call void @__quantum__rt__array_update_alias_count(%Array* %ops, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %qbs, i32 -1)
  call void @__quantum__rt__qubit_release(%Qubit* %aux)
  ret %Result* %14
}

declare %Qubit* @__quantum__rt__qubit_allocate()

declare %Array* @__quantum__rt__qubit_allocate_array(i64)

declare void @__quantum__rt__qubit_release(%Qubit*)

define internal void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %control, %Qubit* %target) {
entry:
  call void @__quantum__qis__cnot__body(%Qubit* %control, %Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Z__body(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledZ____body(%Qubit* %qubit)
  ret void
}

define internal %Result* @Microsoft__Quantum__Intrinsic__M__body(%Qubit* %qubit) {
entry:
  %result = call %Result* @__quantum__qis__m__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PreparePostM____body(%Result* %result, %Qubit* %qubit)
  ret %Result* %result
}

define internal double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__EstimateEnergy__TermExpectation__body({ i64, %Array* }* %inputState, %Array* %measOp, i64 %nQubits, i64 %nSamples) {
entry:
  %0 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %inputState, i32 0, i32 1
  %1 = load %Array*, %Array** %0, align 8
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %1)
  %3 = sub i64 %2, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %4 = phi i64 [ 0, %entry ], [ %15, %exiting__1 ]
  %5 = icmp sle i64 %4, %3
  br i1 %5, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %6 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %1, i64 %4)
  %7 = bitcast i8* %6 to { { double, double }*, %Array* }**
  %8 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %7, align 8
  %9 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %8, i32 0, i32 0
  %10 = load { double, double }*, { double, double }** %9, align 8
  %11 = bitcast { double, double }* %10 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %11, i32 1)
  %12 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %8, i32 0, i32 1
  %13 = load %Array*, %Array** %12, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %13, i32 1)
  %14 = bitcast { { double, double }*, %Array* }* %8 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %14, i32 1)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %15 = add i64 %4, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  call void @__quantum__rt__array_update_alias_count(%Array* %1, i32 1)
  %16 = bitcast { i64, %Array* }* %inputState to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %16, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %measOp, i32 1)
  %nUp = alloca i64, align 8
  store i64 0, i64* %nUp, align 4
  %17 = sub i64 %nSamples, 1
  br label %header__2

header__2:                                        ; preds = %exiting__2, %exit__1
  %idxMeasurement = phi i64 [ 0, %exit__1 ], [ %25, %exiting__2 ]
  %18 = icmp sle i64 %idxMeasurement, %17
  br i1 %18, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %register = call %Array* @__quantum__rt__qubit_allocate_array(i64 %nQubits)
  call void @__quantum__rt__array_update_alias_count(%Array* %register, i32 1)
  call void @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__StatePrep__PrepareTrialState__body({ i64, %Array* }* %inputState, %Array* %register)
  %result = call %Result* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__EstimateEnergy__JointMeasure__body(%Array* %measOp, %Array* %register)
  %19 = call %Result* @__quantum__rt__result_get_zero()
  %20 = call i1 @__quantum__rt__result_equal(%Result* %result, %Result* %19)
  br i1 %20, label %then0__1, label %continue__1

then0__1:                                         ; preds = %body__2
  %21 = load i64, i64* %nUp, align 4
  %22 = add i64 %21, 1
  store i64 %22, i64* %nUp, align 4
  br label %continue__1

continue__1:                                      ; preds = %then0__1, %body__2
  %23 = call i64 @__quantum__rt__array_get_size_1d(%Array* %register)
  %24 = sub i64 %23, 1
  br label %header__3

exiting__2:                                       ; preds = %exit__3
  %25 = add i64 %idxMeasurement, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  %26 = load i64, i64* %nUp, align 4
  %27 = sitofp i64 %26 to double
  %28 = sitofp i64 %nSamples to double
  %29 = fdiv double %27, %28
  %30 = sub i64 %2, 1
  br label %header__4

header__3:                                        ; preds = %exiting__3, %continue__1
  %31 = phi i64 [ 0, %continue__1 ], [ %35, %exiting__3 ]
  %32 = icmp sle i64 %31, %24
  br i1 %32, label %body__3, label %exit__3

body__3:                                          ; preds = %header__3
  %33 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %register, i64 %31)
  %34 = bitcast i8* %33 to %Qubit**
  %q = load %Qubit*, %Qubit** %34, align 8
  %r = call %Result* @Microsoft__Quantum__Intrinsic__M__body(%Qubit* %q)
  call void @__quantum__rt__result_update_reference_count(%Result* %r, i32 -1)
  br label %exiting__3

exiting__3:                                       ; preds = %body__3
  %35 = add i64 %31, 1
  br label %header__3

exit__3:                                          ; preds = %header__3
  call void @__quantum__rt__array_update_alias_count(%Array* %register, i32 -1)
  call void @__quantum__rt__result_update_reference_count(%Result* %result, i32 -1)
  call void @__quantum__rt__qubit_release_array(%Array* %register)
  br label %exiting__2

header__4:                                        ; preds = %exiting__4, %exit__2
  %36 = phi i64 [ 0, %exit__2 ], [ %47, %exiting__4 ]
  %37 = icmp sle i64 %36, %30
  br i1 %37, label %body__4, label %exit__4

body__4:                                          ; preds = %header__4
  %38 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %1, i64 %36)
  %39 = bitcast i8* %38 to { { double, double }*, %Array* }**
  %40 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %39, align 8
  %41 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %40, i32 0, i32 0
  %42 = load { double, double }*, { double, double }** %41, align 8
  %43 = bitcast { double, double }* %42 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %43, i32 -1)
  %44 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %40, i32 0, i32 1
  %45 = load %Array*, %Array** %44, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %45, i32 -1)
  %46 = bitcast { { double, double }*, %Array* }* %40 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %46, i32 -1)
  br label %exiting__4

exiting__4:                                       ; preds = %body__4
  %47 = add i64 %36, 1
  br label %header__4

exit__4:                                          ; preds = %header__4
  call void @__quantum__rt__array_update_alias_count(%Array* %1, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %16, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %measOp, i32 -1)
  ret double %29
}

declare void @__quantum__rt__qubit_release_array(%Array*)

declare %Result* @__quantum__rt__result_get_zero()

declare i1 @__quantum__rt__result_equal(%Result*, %Result*)

declare void @__quantum__rt__result_update_reference_count(%Result*, i32)

declare %Array* @__quantum__rt__array_copy(%Array*, i1)

define internal double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE__body(double %theta1, double %theta2, double %theta3, i64 %nSamples) {
entry:
  %0 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 4)
  %1 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %0, i64 0)
  %2 = bitcast i8* %1 to { %Array*, %Array* }**
  %3 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %0, i64 1)
  %4 = bitcast i8* %3 to { %Array*, %Array* }**
  %5 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %0, i64 2)
  %6 = bitcast i8* %5 to { %Array*, %Array* }**
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %0, i64 3)
  %8 = bitcast i8* %7 to { %Array*, %Array* }**
  %9 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %10 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %9, i64 0)
  %11 = bitcast i8* %10 to i64*
  store i64 0, i64* %11, align 4
  %12 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %13 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %12, i64 0)
  %14 = bitcast i8* %13 to double*
  store double 0x3FC5E9EC780DD8B0, double* %14, align 8
  %15 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %9, %Array* %12)
  call void @__quantum__rt__array_update_reference_count(%Array* %9, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %12, i32 -1)
  %16 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %17 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %16, i64 0)
  %18 = bitcast i8* %17 to i64*
  store i64 1, i64* %18, align 4
  %19 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %20 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %19, i64 0)
  %21 = bitcast i8* %20 to double*
  store double 0x3FC5E9EC780DD8B0, double* %21, align 8
  %22 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %16, %Array* %19)
  call void @__quantum__rt__array_update_reference_count(%Array* %16, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %19, i32 -1)
  %23 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %24 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %23, i64 0)
  %25 = bitcast i8* %24 to i64*
  store i64 2, i64* %25, align 4
  %26 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %27 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %26, i64 0)
  %28 = bitcast i8* %27 to double*
  store double 0xBFCC8498CDE41B6A, double* %28, align 8
  %29 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %23, %Array* %26)
  call void @__quantum__rt__array_update_reference_count(%Array* %23, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %26, i32 -1)
  %30 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %31 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %30, i64 0)
  %32 = bitcast i8* %31 to i64*
  store i64 3, i64* %32, align 4
  %33 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %34 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %33, i64 0)
  %35 = bitcast i8* %34 to double*
  store double 0xBFCC8498CDE41B6A, double* %35, align 8
  %36 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %30, %Array* %33)
  call void @__quantum__rt__array_update_reference_count(%Array* %30, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %33, i32 -1)
  store { %Array*, %Array* }* %15, { %Array*, %Array* }** %2, align 8
  store { %Array*, %Array* }* %22, { %Array*, %Array* }** %4, align 8
  store { %Array*, %Array* }* %29, { %Array*, %Array* }** %6, align 8
  store { %Array*, %Array* }* %36, { %Array*, %Array* }** %8, align 8
  %37 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 6)
  %38 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %37, i64 0)
  %39 = bitcast i8* %38 to { %Array*, %Array* }**
  %40 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %37, i64 1)
  %41 = bitcast i8* %40 to { %Array*, %Array* }**
  %42 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %37, i64 2)
  %43 = bitcast i8* %42 to { %Array*, %Array* }**
  %44 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %37, i64 3)
  %45 = bitcast i8* %44 to { %Array*, %Array* }**
  %46 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %37, i64 4)
  %47 = bitcast i8* %46 to { %Array*, %Array* }**
  %48 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %37, i64 5)
  %49 = bitcast i8* %48 to { %Array*, %Array* }**
  %50 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %51 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %50, i64 0)
  %52 = bitcast i8* %51 to i64*
  %53 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %50, i64 1)
  %54 = bitcast i8* %53 to i64*
  store i64 0, i64* %52, align 4
  store i64 1, i64* %54, align 4
  %55 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %56 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %55, i64 0)
  %57 = bitcast i8* %56 to double*
  store double 0x3FC59572B12B0E54, double* %57, align 8
  %58 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %50, %Array* %55)
  call void @__quantum__rt__array_update_reference_count(%Array* %50, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %55, i32 -1)
  %59 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %60 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %59, i64 0)
  %61 = bitcast i8* %60 to i64*
  %62 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %59, i64 1)
  %63 = bitcast i8* %62 to i64*
  store i64 0, i64* %61, align 4
  store i64 2, i64* %63, align 4
  %64 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %65 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %64, i64 0)
  %66 = bitcast i8* %65 to double*
  store double 0x3FBEDC1CB9A7B498, double* %66, align 8
  %67 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %59, %Array* %64)
  call void @__quantum__rt__array_update_reference_count(%Array* %59, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %64, i32 -1)
  %68 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %69 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %68, i64 0)
  %70 = bitcast i8* %69 to i64*
  %71 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %68, i64 1)
  %72 = bitcast i8* %71 to i64*
  store i64 0, i64* %70, align 4
  store i64 3, i64* %72, align 4
  %73 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %74 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %73, i64 0)
  %75 = bitcast i8* %74 to double*
  store double 0x3FC53B29D7F34F20, double* %75, align 8
  %76 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %68, %Array* %73)
  call void @__quantum__rt__array_update_reference_count(%Array* %68, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %73, i32 -1)
  %77 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %78 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %77, i64 0)
  %79 = bitcast i8* %78 to i64*
  %80 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %77, i64 1)
  %81 = bitcast i8* %80 to i64*
  store i64 1, i64* %79, align 4
  store i64 2, i64* %81, align 4
  %82 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %83 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %82, i64 0)
  %84 = bitcast i8* %83 to double*
  store double 0x3FC53B29D7F34F20, double* %84, align 8
  %85 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %77, %Array* %82)
  call void @__quantum__rt__array_update_reference_count(%Array* %77, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %82, i32 -1)
  %86 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %87 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %86, i64 0)
  %88 = bitcast i8* %87 to i64*
  %89 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %86, i64 1)
  %90 = bitcast i8* %89 to i64*
  store i64 1, i64* %88, align 4
  store i64 3, i64* %90, align 4
  %91 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %92 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %91, i64 0)
  %93 = bitcast i8* %92 to double*
  store double 0x3FBEDC1CB9A7B498, double* %93, align 8
  %94 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %86, %Array* %91)
  call void @__quantum__rt__array_update_reference_count(%Array* %86, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %91, i32 -1)
  %95 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %96 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %95, i64 0)
  %97 = bitcast i8* %96 to i64*
  %98 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %95, i64 1)
  %99 = bitcast i8* %98 to i64*
  store i64 2, i64* %97, align 4
  store i64 3, i64* %99, align 4
  %100 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %101 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %100, i64 0)
  %102 = bitcast i8* %101 to double*
  store double 0x3FC65115A1A7DAFB, double* %102, align 8
  %103 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %95, %Array* %100)
  call void @__quantum__rt__array_update_reference_count(%Array* %95, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %100, i32 -1)
  store { %Array*, %Array* }* %58, { %Array*, %Array* }** %39, align 8
  store { %Array*, %Array* }* %67, { %Array*, %Array* }** %41, align 8
  store { %Array*, %Array* }* %76, { %Array*, %Array* }** %43, align 8
  store { %Array*, %Array* }* %85, { %Array*, %Array* }** %45, align 8
  store { %Array*, %Array* }* %94, { %Array*, %Array* }** %47, align 8
  store { %Array*, %Array* }* %103, { %Array*, %Array* }** %49, align 8
  %104 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 0)
  %105 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 0)
  %106 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Array* }* getelementptr ({ %Array*, %Array* }, { %Array*, %Array* }* null, i32 1) to i64))
  %107 = bitcast %Tuple* %106 to { %Array*, %Array* }*
  %108 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %107, i32 0, i32 0
  %109 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %107, i32 0, i32 1
  store %Array* %104, %Array** %108, align 8
  store %Array* %105, %Array** %109, align 8
  %110 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 0)
  %111 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %112 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %111, i64 0)
  %113 = bitcast i8* %112 to { %Array*, %Array* }**
  %114 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 4)
  %115 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %114, i64 0)
  %116 = bitcast i8* %115 to i64*
  %117 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %114, i64 1)
  %118 = bitcast i8* %117 to i64*
  %119 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %114, i64 2)
  %120 = bitcast i8* %119 to i64*
  %121 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %114, i64 3)
  %122 = bitcast i8* %121 to i64*
  store i64 0, i64* %116, align 4
  store i64 1, i64* %118, align 4
  store i64 2, i64* %120, align 4
  store i64 3, i64* %122, align 4
  %123 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 4)
  %124 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %123, i64 0)
  %125 = bitcast i8* %124 to double*
  %126 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %123, i64 1)
  %127 = bitcast i8* %126 to double*
  %128 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %123, i64 2)
  %129 = bitcast i8* %128 to double*
  %130 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %123, i64 3)
  %131 = bitcast i8* %130 to double*
  store double 0.000000e+00, double* %125, align 8
  store double 0xBFA7346DEC7DD351, double* %127, align 8
  store double 0.000000e+00, double* %129, align 8
  store double 0x3FA7346DEC7DD351, double* %131, align 8
  %132 = call { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %114, %Array* %123)
  call void @__quantum__rt__array_update_reference_count(%Array* %114, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %123, i32 -1)
  store { %Array*, %Array* }* %132, { %Array*, %Array* }** %113, align 8
  %hamiltonian = call { %Array*, %Array*, %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JWOptimizedHTerms__body(%Array* %0, %Array* %37, %Array* %110, %Array* %111)
  %133 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %hamiltonian, i32 0, i32 0
  %134 = load %Array*, %Array** %133, align 8
  %135 = call i64 @__quantum__rt__array_get_size_1d(%Array* %134)
  %136 = sub i64 %135, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %137 = phi i64 [ 0, %entry ], [ %147, %exiting__1 ]
  %138 = icmp sle i64 %137, %136
  br i1 %138, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %139 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %134, i64 %137)
  %140 = bitcast i8* %139 to { %Array*, %Array* }**
  %141 = load { %Array*, %Array* }*, { %Array*, %Array* }** %140, align 8
  %142 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %141, i32 0, i32 0
  %143 = load %Array*, %Array** %142, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %143, i32 1)
  %144 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %141, i32 0, i32 1
  %145 = load %Array*, %Array** %144, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %145, i32 1)
  %146 = bitcast { %Array*, %Array* }* %141 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %146, i32 1)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %147 = add i64 %137, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  call void @__quantum__rt__array_update_alias_count(%Array* %134, i32 1)
  %148 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %hamiltonian, i32 0, i32 1
  %149 = load %Array*, %Array** %148, align 8
  %150 = call i64 @__quantum__rt__array_get_size_1d(%Array* %149)
  %151 = sub i64 %150, 1
  br label %header__2

header__2:                                        ; preds = %exiting__2, %exit__1
  %152 = phi i64 [ 0, %exit__1 ], [ %162, %exiting__2 ]
  %153 = icmp sle i64 %152, %151
  br i1 %153, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %154 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %149, i64 %152)
  %155 = bitcast i8* %154 to { %Array*, %Array* }**
  %156 = load { %Array*, %Array* }*, { %Array*, %Array* }** %155, align 8
  %157 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %156, i32 0, i32 0
  %158 = load %Array*, %Array** %157, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %158, i32 1)
  %159 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %156, i32 0, i32 1
  %160 = load %Array*, %Array** %159, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %160, i32 1)
  %161 = bitcast { %Array*, %Array* }* %156 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %161, i32 1)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %162 = add i64 %152, 1
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %149, i32 1)
  %163 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %hamiltonian, i32 0, i32 2
  %164 = load %Array*, %Array** %163, align 8
  %165 = call i64 @__quantum__rt__array_get_size_1d(%Array* %164)
  %166 = sub i64 %165, 1
  br label %header__3

header__3:                                        ; preds = %exiting__3, %exit__2
  %167 = phi i64 [ 0, %exit__2 ], [ %177, %exiting__3 ]
  %168 = icmp sle i64 %167, %166
  br i1 %168, label %body__3, label %exit__3

body__3:                                          ; preds = %header__3
  %169 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %164, i64 %167)
  %170 = bitcast i8* %169 to { %Array*, %Array* }**
  %171 = load { %Array*, %Array* }*, { %Array*, %Array* }** %170, align 8
  %172 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %171, i32 0, i32 0
  %173 = load %Array*, %Array** %172, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %173, i32 1)
  %174 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %171, i32 0, i32 1
  %175 = load %Array*, %Array** %174, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %175, i32 1)
  %176 = bitcast { %Array*, %Array* }* %171 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %176, i32 1)
  br label %exiting__3

exiting__3:                                       ; preds = %body__3
  %177 = add i64 %167, 1
  br label %header__3

exit__3:                                          ; preds = %header__3
  call void @__quantum__rt__array_update_alias_count(%Array* %164, i32 1)
  %178 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %hamiltonian, i32 0, i32 3
  %179 = load %Array*, %Array** %178, align 8
  %180 = call i64 @__quantum__rt__array_get_size_1d(%Array* %179)
  %181 = sub i64 %180, 1
  br label %header__4

header__4:                                        ; preds = %exiting__4, %exit__3
  %182 = phi i64 [ 0, %exit__3 ], [ %192, %exiting__4 ]
  %183 = icmp sle i64 %182, %181
  br i1 %183, label %body__4, label %exit__4

body__4:                                          ; preds = %header__4
  %184 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %179, i64 %182)
  %185 = bitcast i8* %184 to { %Array*, %Array* }**
  %186 = load { %Array*, %Array* }*, { %Array*, %Array* }** %185, align 8
  %187 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %186, i32 0, i32 0
  %188 = load %Array*, %Array** %187, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %188, i32 1)
  %189 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %186, i32 0, i32 1
  %190 = load %Array*, %Array** %189, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %190, i32 1)
  %191 = bitcast { %Array*, %Array* }* %186 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %191, i32 1)
  br label %exiting__4

exiting__4:                                       ; preds = %body__4
  %192 = add i64 %182, 1
  br label %header__4

exit__4:                                          ; preds = %header__4
  call void @__quantum__rt__array_update_alias_count(%Array* %179, i32 1)
  %193 = bitcast { %Array*, %Array*, %Array*, %Array* }* %hamiltonian to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %193, i32 1)
  %194 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i64, %Array* }* getelementptr ({ i64, %Array* }, { i64, %Array* }* null, i32 1) to i64))
  %inputState = bitcast %Tuple* %194 to { i64, %Array* }*
  %195 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %inputState, i32 0, i32 0
  %196 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %inputState, i32 0, i32 1
  %197 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 4)
  %198 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %197, i64 0)
  %199 = bitcast i8* %198 to { { double, double }*, %Array* }**
  %200 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %197, i64 1)
  %201 = bitcast i8* %200 to { { double, double }*, %Array* }**
  %202 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %197, i64 2)
  %203 = bitcast i8* %202 to { { double, double }*, %Array* }**
  %204 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %197, i64 3)
  %205 = bitcast i8* %204 to { { double, double }*, %Array* }**
  %206 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, double }* getelementptr ({ double, double }, { double, double }* null, i32 1) to i64))
  %207 = bitcast %Tuple* %206 to { double, double }*
  %208 = getelementptr inbounds { double, double }, { double, double }* %207, i32 0, i32 0
  %209 = getelementptr inbounds { double, double }, { double, double }* %207, i32 0, i32 1
  store double %theta1, double* %208, align 8
  store double 0.000000e+00, double* %209, align 8
  %210 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %211 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %210, i64 0)
  %212 = bitcast i8* %211 to i64*
  %213 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %210, i64 1)
  %214 = bitcast i8* %213 to i64*
  store i64 2, i64* %212, align 4
  store i64 0, i64* %214, align 4
  %215 = call { { double, double }*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JordanWignerInputState__body({ double, double }* %207, %Array* %210)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %206, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %210, i32 -1)
  %216 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, double }* getelementptr ({ double, double }, { double, double }* null, i32 1) to i64))
  %217 = bitcast %Tuple* %216 to { double, double }*
  %218 = getelementptr inbounds { double, double }, { double, double }* %217, i32 0, i32 0
  %219 = getelementptr inbounds { double, double }, { double, double }* %217, i32 0, i32 1
  store double %theta2, double* %218, align 8
  store double 0.000000e+00, double* %219, align 8
  %220 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %221 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %220, i64 0)
  %222 = bitcast i8* %221 to i64*
  %223 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %220, i64 1)
  %224 = bitcast i8* %223 to i64*
  store i64 3, i64* %222, align 4
  store i64 1, i64* %224, align 4
  %225 = call { { double, double }*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JordanWignerInputState__body({ double, double }* %217, %Array* %220)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %216, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %220, i32 -1)
  %226 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, double }* getelementptr ({ double, double }, { double, double }* null, i32 1) to i64))
  %227 = bitcast %Tuple* %226 to { double, double }*
  %228 = getelementptr inbounds { double, double }, { double, double }* %227, i32 0, i32 0
  %229 = getelementptr inbounds { double, double }, { double, double }* %227, i32 0, i32 1
  store double %theta3, double* %228, align 8
  store double 0.000000e+00, double* %229, align 8
  %230 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 4)
  %231 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %230, i64 0)
  %232 = bitcast i8* %231 to i64*
  %233 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %230, i64 1)
  %234 = bitcast i8* %233 to i64*
  %235 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %230, i64 2)
  %236 = bitcast i8* %235 to i64*
  %237 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %230, i64 3)
  %238 = bitcast i8* %237 to i64*
  store i64 2, i64* %232, align 4
  store i64 3, i64* %234, align 4
  store i64 1, i64* %236, align 4
  store i64 0, i64* %238, align 4
  %239 = call { { double, double }*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JordanWignerInputState__body({ double, double }* %227, %Array* %230)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %226, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %230, i32 -1)
  %240 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, double }* getelementptr ({ double, double }, { double, double }* null, i32 1) to i64))
  %241 = bitcast %Tuple* %240 to { double, double }*
  %242 = getelementptr inbounds { double, double }, { double, double }* %241, i32 0, i32 0
  %243 = getelementptr inbounds { double, double }, { double, double }* %241, i32 0, i32 1
  store double 1.000000e+00, double* %242, align 8
  store double 0.000000e+00, double* %243, align 8
  %244 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %245 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %244, i64 0)
  %246 = bitcast i8* %245 to i64*
  %247 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %244, i64 1)
  %248 = bitcast i8* %247 to i64*
  store i64 0, i64* %246, align 4
  store i64 1, i64* %248, align 4
  %249 = call { { double, double }*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JordanWignerInputState__body({ double, double }* %241, %Array* %244)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %240, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %244, i32 -1)
  store { { double, double }*, %Array* }* %215, { { double, double }*, %Array* }** %199, align 8
  store { { double, double }*, %Array* }* %225, { { double, double }*, %Array* }** %201, align 8
  store { { double, double }*, %Array* }* %239, { { double, double }*, %Array* }** %203, align 8
  store { { double, double }*, %Array* }* %249, { { double, double }*, %Array* }** %205, align 8
  store i64 3, i64* %195, align 4
  store %Array* %197, %Array** %196, align 8
  br label %header__5

header__5:                                        ; preds = %exiting__5, %exit__4
  %250 = phi i64 [ 0, %exit__4 ], [ %261, %exiting__5 ]
  %251 = icmp sle i64 %250, 3
  br i1 %251, label %body__5, label %exit__5

body__5:                                          ; preds = %header__5
  %252 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %197, i64 %250)
  %253 = bitcast i8* %252 to { { double, double }*, %Array* }**
  %254 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %253, align 8
  %255 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %254, i32 0, i32 0
  %256 = load { double, double }*, { double, double }** %255, align 8
  %257 = bitcast { double, double }* %256 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %257, i32 1)
  %258 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %254, i32 0, i32 1
  %259 = load %Array*, %Array** %258, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %259, i32 1)
  %260 = bitcast { { double, double }*, %Array* }* %254 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %260, i32 1)
  br label %exiting__5

exiting__5:                                       ; preds = %body__5
  %261 = add i64 %250, 1
  br label %header__5

exit__5:                                          ; preds = %header__5
  call void @__quantum__rt__array_update_alias_count(%Array* %197, i32 1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %194, i32 1)
  %JWEncodedData = call { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__JordanWigner__JordanWignerEncodingData__body(i64 4, { %Array*, %Array*, %Array*, %Array* }* %hamiltonian, { i64, %Array* }* %inputState, double 0xBFB94D36D949CC98)
  %262 = getelementptr inbounds { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }, { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* %JWEncodedData, i32 0, i32 1
  %263 = load { %Array*, %Array*, %Array*, %Array* }*, { %Array*, %Array*, %Array*, %Array* }** %262, align 8
  %264 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %263, i32 0, i32 0
  %265 = load %Array*, %Array** %264, align 8
  %266 = call i64 @__quantum__rt__array_get_size_1d(%Array* %265)
  %267 = sub i64 %266, 1
  br label %header__6

header__6:                                        ; preds = %exiting__6, %exit__5
  %268 = phi i64 [ 0, %exit__5 ], [ %278, %exiting__6 ]
  %269 = icmp sle i64 %268, %267
  br i1 %269, label %body__6, label %exit__6

body__6:                                          ; preds = %header__6
  %270 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %265, i64 %268)
  %271 = bitcast i8* %270 to { %Array*, %Array* }**
  %272 = load { %Array*, %Array* }*, { %Array*, %Array* }** %271, align 8
  %273 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %272, i32 0, i32 0
  %274 = load %Array*, %Array** %273, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %274, i32 1)
  %275 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %272, i32 0, i32 1
  %276 = load %Array*, %Array** %275, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %276, i32 1)
  %277 = bitcast { %Array*, %Array* }* %272 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %277, i32 1)
  br label %exiting__6

exiting__6:                                       ; preds = %body__6
  %278 = add i64 %268, 1
  br label %header__6

exit__6:                                          ; preds = %header__6
  call void @__quantum__rt__array_update_alias_count(%Array* %265, i32 1)
  %279 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %263, i32 0, i32 1
  %280 = load %Array*, %Array** %279, align 8
  %281 = call i64 @__quantum__rt__array_get_size_1d(%Array* %280)
  %282 = sub i64 %281, 1
  br label %header__7

header__7:                                        ; preds = %exiting__7, %exit__6
  %283 = phi i64 [ 0, %exit__6 ], [ %293, %exiting__7 ]
  %284 = icmp sle i64 %283, %282
  br i1 %284, label %body__7, label %exit__7

body__7:                                          ; preds = %header__7
  %285 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %280, i64 %283)
  %286 = bitcast i8* %285 to { %Array*, %Array* }**
  %287 = load { %Array*, %Array* }*, { %Array*, %Array* }** %286, align 8
  %288 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %287, i32 0, i32 0
  %289 = load %Array*, %Array** %288, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %289, i32 1)
  %290 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %287, i32 0, i32 1
  %291 = load %Array*, %Array** %290, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %291, i32 1)
  %292 = bitcast { %Array*, %Array* }* %287 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %292, i32 1)
  br label %exiting__7

exiting__7:                                       ; preds = %body__7
  %293 = add i64 %283, 1
  br label %header__7

exit__7:                                          ; preds = %header__7
  call void @__quantum__rt__array_update_alias_count(%Array* %280, i32 1)
  %294 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %263, i32 0, i32 2
  %295 = load %Array*, %Array** %294, align 8
  %296 = call i64 @__quantum__rt__array_get_size_1d(%Array* %295)
  %297 = sub i64 %296, 1
  br label %header__8

header__8:                                        ; preds = %exiting__8, %exit__7
  %298 = phi i64 [ 0, %exit__7 ], [ %308, %exiting__8 ]
  %299 = icmp sle i64 %298, %297
  br i1 %299, label %body__8, label %exit__8

body__8:                                          ; preds = %header__8
  %300 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %295, i64 %298)
  %301 = bitcast i8* %300 to { %Array*, %Array* }**
  %302 = load { %Array*, %Array* }*, { %Array*, %Array* }** %301, align 8
  %303 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %302, i32 0, i32 0
  %304 = load %Array*, %Array** %303, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %304, i32 1)
  %305 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %302, i32 0, i32 1
  %306 = load %Array*, %Array** %305, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %306, i32 1)
  %307 = bitcast { %Array*, %Array* }* %302 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %307, i32 1)
  br label %exiting__8

exiting__8:                                       ; preds = %body__8
  %308 = add i64 %298, 1
  br label %header__8

exit__8:                                          ; preds = %header__8
  call void @__quantum__rt__array_update_alias_count(%Array* %295, i32 1)
  %309 = getelementptr inbounds { %Array*, %Array*, %Array*, %Array* }, { %Array*, %Array*, %Array*, %Array* }* %263, i32 0, i32 3
  %310 = load %Array*, %Array** %309, align 8
  %311 = call i64 @__quantum__rt__array_get_size_1d(%Array* %310)
  %312 = sub i64 %311, 1
  br label %header__9

header__9:                                        ; preds = %exiting__9, %exit__8
  %313 = phi i64 [ 0, %exit__8 ], [ %323, %exiting__9 ]
  %314 = icmp sle i64 %313, %312
  br i1 %314, label %body__9, label %exit__9

body__9:                                          ; preds = %header__9
  %315 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %310, i64 %313)
  %316 = bitcast i8* %315 to { %Array*, %Array* }**
  %317 = load { %Array*, %Array* }*, { %Array*, %Array* }** %316, align 8
  %318 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %317, i32 0, i32 0
  %319 = load %Array*, %Array** %318, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %319, i32 1)
  %320 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %317, i32 0, i32 1
  %321 = load %Array*, %Array** %320, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %321, i32 1)
  %322 = bitcast { %Array*, %Array* }* %317 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %322, i32 1)
  br label %exiting__9

exiting__9:                                       ; preds = %body__9
  %323 = add i64 %313, 1
  br label %header__9

exit__9:                                          ; preds = %header__9
  call void @__quantum__rt__array_update_alias_count(%Array* %310, i32 1)
  %324 = bitcast { %Array*, %Array*, %Array*, %Array* }* %263 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %324, i32 1)
  %325 = getelementptr inbounds { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }, { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* %JWEncodedData, i32 0, i32 2
  %326 = load { i64, %Array* }*, { i64, %Array* }** %325, align 8
  %327 = getelementptr inbounds { i64, %Array* }, { i64, %Array* }* %326, i32 0, i32 1
  %328 = load %Array*, %Array** %327, align 8
  %329 = call i64 @__quantum__rt__array_get_size_1d(%Array* %328)
  %330 = sub i64 %329, 1
  br label %header__10

header__10:                                       ; preds = %exiting__10, %exit__9
  %331 = phi i64 [ 0, %exit__9 ], [ %342, %exiting__10 ]
  %332 = icmp sle i64 %331, %330
  br i1 %332, label %body__10, label %exit__10

body__10:                                         ; preds = %header__10
  %333 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %328, i64 %331)
  %334 = bitcast i8* %333 to { { double, double }*, %Array* }**
  %335 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %334, align 8
  %336 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %335, i32 0, i32 0
  %337 = load { double, double }*, { double, double }** %336, align 8
  %338 = bitcast { double, double }* %337 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %338, i32 1)
  %339 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %335, i32 0, i32 1
  %340 = load %Array*, %Array** %339, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %340, i32 1)
  %341 = bitcast { { double, double }*, %Array* }* %335 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %341, i32 1)
  br label %exiting__10

exiting__10:                                      ; preds = %body__10
  %342 = add i64 %331, 1
  br label %header__10

exit__10:                                         ; preds = %header__10
  call void @__quantum__rt__array_update_alias_count(%Array* %328, i32 1)
  %343 = bitcast { i64, %Array* }* %326 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %343, i32 1)
  %344 = bitcast { i64, { %Array*, %Array*, %Array*, %Array* }*, { i64, %Array* }*, double }* %JWEncodedData to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %344, i32 1)
  %345 = call double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__VariationalQuantumEigensolver__EstimateEnergy__body(i64 5, { %Array*, %Array*, %Array*, %Array* }* %hamiltonian, { i64, %Array* }* %inputState, double 0xBFB94D36D949CC98, i64 %nSamples)
  %346 = sub i64 %135, 1
  br label %header__11

header__11:                                       ; preds = %exiting__11, %exit__10
  %347 = phi i64 [ 0, %exit__10 ], [ %357, %exiting__11 ]
  %348 = icmp sle i64 %347, %346
  br i1 %348, label %body__11, label %exit__11

body__11:                                         ; preds = %header__11
  %349 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %134, i64 %347)
  %350 = bitcast i8* %349 to { %Array*, %Array* }**
  %351 = load { %Array*, %Array* }*, { %Array*, %Array* }** %350, align 8
  %352 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %351, i32 0, i32 0
  %353 = load %Array*, %Array** %352, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %353, i32 -1)
  %354 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %351, i32 0, i32 1
  %355 = load %Array*, %Array** %354, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %355, i32 -1)
  %356 = bitcast { %Array*, %Array* }* %351 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %356, i32 -1)
  br label %exiting__11

exiting__11:                                      ; preds = %body__11
  %357 = add i64 %347, 1
  br label %header__11

exit__11:                                         ; preds = %header__11
  call void @__quantum__rt__array_update_alias_count(%Array* %134, i32 -1)
  %358 = sub i64 %150, 1
  br label %header__12

header__12:                                       ; preds = %exiting__12, %exit__11
  %359 = phi i64 [ 0, %exit__11 ], [ %369, %exiting__12 ]
  %360 = icmp sle i64 %359, %358
  br i1 %360, label %body__12, label %exit__12

body__12:                                         ; preds = %header__12
  %361 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %149, i64 %359)
  %362 = bitcast i8* %361 to { %Array*, %Array* }**
  %363 = load { %Array*, %Array* }*, { %Array*, %Array* }** %362, align 8
  %364 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %363, i32 0, i32 0
  %365 = load %Array*, %Array** %364, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %365, i32 -1)
  %366 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %363, i32 0, i32 1
  %367 = load %Array*, %Array** %366, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %367, i32 -1)
  %368 = bitcast { %Array*, %Array* }* %363 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %368, i32 -1)
  br label %exiting__12

exiting__12:                                      ; preds = %body__12
  %369 = add i64 %359, 1
  br label %header__12

exit__12:                                         ; preds = %header__12
  call void @__quantum__rt__array_update_alias_count(%Array* %149, i32 -1)
  %370 = sub i64 %165, 1
  br label %header__13

header__13:                                       ; preds = %exiting__13, %exit__12
  %371 = phi i64 [ 0, %exit__12 ], [ %381, %exiting__13 ]
  %372 = icmp sle i64 %371, %370
  br i1 %372, label %body__13, label %exit__13

body__13:                                         ; preds = %header__13
  %373 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %164, i64 %371)
  %374 = bitcast i8* %373 to { %Array*, %Array* }**
  %375 = load { %Array*, %Array* }*, { %Array*, %Array* }** %374, align 8
  %376 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %375, i32 0, i32 0
  %377 = load %Array*, %Array** %376, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %377, i32 -1)
  %378 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %375, i32 0, i32 1
  %379 = load %Array*, %Array** %378, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %379, i32 -1)
  %380 = bitcast { %Array*, %Array* }* %375 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %380, i32 -1)
  br label %exiting__13

exiting__13:                                      ; preds = %body__13
  %381 = add i64 %371, 1
  br label %header__13

exit__13:                                         ; preds = %header__13
  call void @__quantum__rt__array_update_alias_count(%Array* %164, i32 -1)
  %382 = sub i64 %180, 1
  br label %header__14

header__14:                                       ; preds = %exiting__14, %exit__13
  %383 = phi i64 [ 0, %exit__13 ], [ %393, %exiting__14 ]
  %384 = icmp sle i64 %383, %382
  br i1 %384, label %body__14, label %exit__14

body__14:                                         ; preds = %header__14
  %385 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %179, i64 %383)
  %386 = bitcast i8* %385 to { %Array*, %Array* }**
  %387 = load { %Array*, %Array* }*, { %Array*, %Array* }** %386, align 8
  %388 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %387, i32 0, i32 0
  %389 = load %Array*, %Array** %388, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %389, i32 -1)
  %390 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %387, i32 0, i32 1
  %391 = load %Array*, %Array** %390, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %391, i32 -1)
  %392 = bitcast { %Array*, %Array* }* %387 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %392, i32 -1)
  br label %exiting__14

exiting__14:                                      ; preds = %body__14
  %393 = add i64 %383, 1
  br label %header__14

exit__14:                                         ; preds = %header__14
  call void @__quantum__rt__array_update_alias_count(%Array* %179, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %193, i32 -1)
  br label %header__15

header__15:                                       ; preds = %exiting__15, %exit__14
  %394 = phi i64 [ 0, %exit__14 ], [ %405, %exiting__15 ]
  %395 = icmp sle i64 %394, 3
  br i1 %395, label %body__15, label %exit__15

body__15:                                         ; preds = %header__15
  %396 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %197, i64 %394)
  %397 = bitcast i8* %396 to { { double, double }*, %Array* }**
  %398 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %397, align 8
  %399 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %398, i32 0, i32 0
  %400 = load { double, double }*, { double, double }** %399, align 8
  %401 = bitcast { double, double }* %400 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %401, i32 -1)
  %402 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %398, i32 0, i32 1
  %403 = load %Array*, %Array** %402, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %403, i32 -1)
  %404 = bitcast { { double, double }*, %Array* }* %398 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %404, i32 -1)
  br label %exiting__15

exiting__15:                                      ; preds = %body__15
  %405 = add i64 %394, 1
  br label %header__15

exit__15:                                         ; preds = %header__15
  call void @__quantum__rt__array_update_alias_count(%Array* %197, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %194, i32 -1)
  %406 = sub i64 %266, 1
  br label %header__16

header__16:                                       ; preds = %exiting__16, %exit__15
  %407 = phi i64 [ 0, %exit__15 ], [ %417, %exiting__16 ]
  %408 = icmp sle i64 %407, %406
  br i1 %408, label %body__16, label %exit__16

body__16:                                         ; preds = %header__16
  %409 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %265, i64 %407)
  %410 = bitcast i8* %409 to { %Array*, %Array* }**
  %411 = load { %Array*, %Array* }*, { %Array*, %Array* }** %410, align 8
  %412 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %411, i32 0, i32 0
  %413 = load %Array*, %Array** %412, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %413, i32 -1)
  %414 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %411, i32 0, i32 1
  %415 = load %Array*, %Array** %414, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %415, i32 -1)
  %416 = bitcast { %Array*, %Array* }* %411 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %416, i32 -1)
  br label %exiting__16

exiting__16:                                      ; preds = %body__16
  %417 = add i64 %407, 1
  br label %header__16

exit__16:                                         ; preds = %header__16
  call void @__quantum__rt__array_update_alias_count(%Array* %265, i32 -1)
  %418 = sub i64 %281, 1
  br label %header__17

header__17:                                       ; preds = %exiting__17, %exit__16
  %419 = phi i64 [ 0, %exit__16 ], [ %429, %exiting__17 ]
  %420 = icmp sle i64 %419, %418
  br i1 %420, label %body__17, label %exit__17

body__17:                                         ; preds = %header__17
  %421 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %280, i64 %419)
  %422 = bitcast i8* %421 to { %Array*, %Array* }**
  %423 = load { %Array*, %Array* }*, { %Array*, %Array* }** %422, align 8
  %424 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %423, i32 0, i32 0
  %425 = load %Array*, %Array** %424, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %425, i32 -1)
  %426 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %423, i32 0, i32 1
  %427 = load %Array*, %Array** %426, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %427, i32 -1)
  %428 = bitcast { %Array*, %Array* }* %423 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %428, i32 -1)
  br label %exiting__17

exiting__17:                                      ; preds = %body__17
  %429 = add i64 %419, 1
  br label %header__17

exit__17:                                         ; preds = %header__17
  call void @__quantum__rt__array_update_alias_count(%Array* %280, i32 -1)
  %430 = sub i64 %296, 1
  br label %header__18

header__18:                                       ; preds = %exiting__18, %exit__17
  %431 = phi i64 [ 0, %exit__17 ], [ %441, %exiting__18 ]
  %432 = icmp sle i64 %431, %430
  br i1 %432, label %body__18, label %exit__18

body__18:                                         ; preds = %header__18
  %433 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %295, i64 %431)
  %434 = bitcast i8* %433 to { %Array*, %Array* }**
  %435 = load { %Array*, %Array* }*, { %Array*, %Array* }** %434, align 8
  %436 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %435, i32 0, i32 0
  %437 = load %Array*, %Array** %436, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %437, i32 -1)
  %438 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %435, i32 0, i32 1
  %439 = load %Array*, %Array** %438, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %439, i32 -1)
  %440 = bitcast { %Array*, %Array* }* %435 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %440, i32 -1)
  br label %exiting__18

exiting__18:                                      ; preds = %body__18
  %441 = add i64 %431, 1
  br label %header__18

exit__18:                                         ; preds = %header__18
  call void @__quantum__rt__array_update_alias_count(%Array* %295, i32 -1)
  %442 = sub i64 %311, 1
  br label %header__19

header__19:                                       ; preds = %exiting__19, %exit__18
  %443 = phi i64 [ 0, %exit__18 ], [ %453, %exiting__19 ]
  %444 = icmp sle i64 %443, %442
  br i1 %444, label %body__19, label %exit__19

body__19:                                         ; preds = %header__19
  %445 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %310, i64 %443)
  %446 = bitcast i8* %445 to { %Array*, %Array* }**
  %447 = load { %Array*, %Array* }*, { %Array*, %Array* }** %446, align 8
  %448 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %447, i32 0, i32 0
  %449 = load %Array*, %Array** %448, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %449, i32 -1)
  %450 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %447, i32 0, i32 1
  %451 = load %Array*, %Array** %450, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %451, i32 -1)
  %452 = bitcast { %Array*, %Array* }* %447 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %452, i32 -1)
  br label %exiting__19

exiting__19:                                      ; preds = %body__19
  %453 = add i64 %443, 1
  br label %header__19

exit__19:                                         ; preds = %header__19
  call void @__quantum__rt__array_update_alias_count(%Array* %310, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %324, i32 -1)
  %454 = sub i64 %329, 1
  br label %header__20

header__20:                                       ; preds = %exiting__20, %exit__19
  %455 = phi i64 [ 0, %exit__19 ], [ %466, %exiting__20 ]
  %456 = icmp sle i64 %455, %454
  br i1 %456, label %body__20, label %exit__20

body__20:                                         ; preds = %header__20
  %457 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %328, i64 %455)
  %458 = bitcast i8* %457 to { { double, double }*, %Array* }**
  %459 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %458, align 8
  %460 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %459, i32 0, i32 0
  %461 = load { double, double }*, { double, double }** %460, align 8
  %462 = bitcast { double, double }* %461 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %462, i32 -1)
  %463 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %459, i32 0, i32 1
  %464 = load %Array*, %Array** %463, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %464, i32 -1)
  %465 = bitcast { { double, double }*, %Array* }* %459 to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %465, i32 -1)
  br label %exiting__20

exiting__20:                                      ; preds = %body__20
  %466 = add i64 %455, 1
  br label %header__20

exit__20:                                         ; preds = %header__20
  call void @__quantum__rt__array_update_alias_count(%Array* %328, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %343, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %344, i32 -1)
  br label %header__21

header__21:                                       ; preds = %exiting__21, %exit__20
  %467 = phi i64 [ 0, %exit__20 ], [ %477, %exiting__21 ]
  %468 = icmp sle i64 %467, 3
  br i1 %468, label %body__21, label %exit__21

body__21:                                         ; preds = %header__21
  %469 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %0, i64 %467)
  %470 = bitcast i8* %469 to { %Array*, %Array* }**
  %471 = load { %Array*, %Array* }*, { %Array*, %Array* }** %470, align 8
  %472 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %471, i32 0, i32 0
  %473 = load %Array*, %Array** %472, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %473, i32 -1)
  %474 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %471, i32 0, i32 1
  %475 = load %Array*, %Array** %474, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %475, i32 -1)
  %476 = bitcast { %Array*, %Array* }* %471 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %476, i32 -1)
  br label %exiting__21

exiting__21:                                      ; preds = %body__21
  %477 = add i64 %467, 1
  br label %header__21

exit__21:                                         ; preds = %header__21
  call void @__quantum__rt__array_update_reference_count(%Array* %0, i32 -1)
  br label %header__22

header__22:                                       ; preds = %exiting__22, %exit__21
  %478 = phi i64 [ 0, %exit__21 ], [ %488, %exiting__22 ]
  %479 = icmp sle i64 %478, 5
  br i1 %479, label %body__22, label %exit__22

body__22:                                         ; preds = %header__22
  %480 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %37, i64 %478)
  %481 = bitcast i8* %480 to { %Array*, %Array* }**
  %482 = load { %Array*, %Array* }*, { %Array*, %Array* }** %481, align 8
  %483 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %482, i32 0, i32 0
  %484 = load %Array*, %Array** %483, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %484, i32 -1)
  %485 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %482, i32 0, i32 1
  %486 = load %Array*, %Array** %485, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %486, i32 -1)
  %487 = bitcast { %Array*, %Array* }* %482 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %487, i32 -1)
  br label %exiting__22

exiting__22:                                      ; preds = %body__22
  %488 = add i64 %478, 1
  br label %header__22

exit__22:                                         ; preds = %header__22
  call void @__quantum__rt__array_update_reference_count(%Array* %37, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %104, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %105, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %106, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %110, i32 -1)
  br label %header__23

header__23:                                       ; preds = %exiting__23, %exit__22
  %489 = phi i64 [ 0, %exit__22 ], [ %499, %exiting__23 ]
  %490 = icmp sle i64 %489, 0
  br i1 %490, label %body__23, label %exit__23

body__23:                                         ; preds = %header__23
  %491 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %111, i64 %489)
  %492 = bitcast i8* %491 to { %Array*, %Array* }**
  %493 = load { %Array*, %Array* }*, { %Array*, %Array* }** %492, align 8
  %494 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %493, i32 0, i32 0
  %495 = load %Array*, %Array** %494, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %495, i32 -1)
  %496 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %493, i32 0, i32 1
  %497 = load %Array*, %Array** %496, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %497, i32 -1)
  %498 = bitcast { %Array*, %Array* }* %493 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %498, i32 -1)
  br label %exiting__23

exiting__23:                                      ; preds = %body__23
  %499 = add i64 %489, 1
  br label %header__23

exit__23:                                         ; preds = %header__23
  call void @__quantum__rt__array_update_reference_count(%Array* %111, i32 -1)
  %500 = sub i64 %135, 1
  br label %header__24

header__24:                                       ; preds = %exiting__24, %exit__23
  %501 = phi i64 [ 0, %exit__23 ], [ %511, %exiting__24 ]
  %502 = icmp sle i64 %501, %500
  br i1 %502, label %body__24, label %exit__24

body__24:                                         ; preds = %header__24
  %503 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %134, i64 %501)
  %504 = bitcast i8* %503 to { %Array*, %Array* }**
  %505 = load { %Array*, %Array* }*, { %Array*, %Array* }** %504, align 8
  %506 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %505, i32 0, i32 0
  %507 = load %Array*, %Array** %506, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %507, i32 -1)
  %508 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %505, i32 0, i32 1
  %509 = load %Array*, %Array** %508, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %509, i32 -1)
  %510 = bitcast { %Array*, %Array* }* %505 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %510, i32 -1)
  br label %exiting__24

exiting__24:                                      ; preds = %body__24
  %511 = add i64 %501, 1
  br label %header__24

exit__24:                                         ; preds = %header__24
  call void @__quantum__rt__array_update_reference_count(%Array* %134, i32 -1)
  %512 = sub i64 %150, 1
  br label %header__25

header__25:                                       ; preds = %exiting__25, %exit__24
  %513 = phi i64 [ 0, %exit__24 ], [ %523, %exiting__25 ]
  %514 = icmp sle i64 %513, %512
  br i1 %514, label %body__25, label %exit__25

body__25:                                         ; preds = %header__25
  %515 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %149, i64 %513)
  %516 = bitcast i8* %515 to { %Array*, %Array* }**
  %517 = load { %Array*, %Array* }*, { %Array*, %Array* }** %516, align 8
  %518 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %517, i32 0, i32 0
  %519 = load %Array*, %Array** %518, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %519, i32 -1)
  %520 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %517, i32 0, i32 1
  %521 = load %Array*, %Array** %520, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %521, i32 -1)
  %522 = bitcast { %Array*, %Array* }* %517 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %522, i32 -1)
  br label %exiting__25

exiting__25:                                      ; preds = %body__25
  %523 = add i64 %513, 1
  br label %header__25

exit__25:                                         ; preds = %header__25
  call void @__quantum__rt__array_update_reference_count(%Array* %149, i32 -1)
  %524 = sub i64 %165, 1
  br label %header__26

header__26:                                       ; preds = %exiting__26, %exit__25
  %525 = phi i64 [ 0, %exit__25 ], [ %535, %exiting__26 ]
  %526 = icmp sle i64 %525, %524
  br i1 %526, label %body__26, label %exit__26

body__26:                                         ; preds = %header__26
  %527 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %164, i64 %525)
  %528 = bitcast i8* %527 to { %Array*, %Array* }**
  %529 = load { %Array*, %Array* }*, { %Array*, %Array* }** %528, align 8
  %530 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %529, i32 0, i32 0
  %531 = load %Array*, %Array** %530, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %531, i32 -1)
  %532 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %529, i32 0, i32 1
  %533 = load %Array*, %Array** %532, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %533, i32 -1)
  %534 = bitcast { %Array*, %Array* }* %529 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %534, i32 -1)
  br label %exiting__26

exiting__26:                                      ; preds = %body__26
  %535 = add i64 %525, 1
  br label %header__26

exit__26:                                         ; preds = %header__26
  call void @__quantum__rt__array_update_reference_count(%Array* %164, i32 -1)
  %536 = sub i64 %180, 1
  br label %header__27

header__27:                                       ; preds = %exiting__27, %exit__26
  %537 = phi i64 [ 0, %exit__26 ], [ %547, %exiting__27 ]
  %538 = icmp sle i64 %537, %536
  br i1 %538, label %body__27, label %exit__27

body__27:                                         ; preds = %header__27
  %539 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %179, i64 %537)
  %540 = bitcast i8* %539 to { %Array*, %Array* }**
  %541 = load { %Array*, %Array* }*, { %Array*, %Array* }** %540, align 8
  %542 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %541, i32 0, i32 0
  %543 = load %Array*, %Array** %542, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %543, i32 -1)
  %544 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %541, i32 0, i32 1
  %545 = load %Array*, %Array** %544, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %545, i32 -1)
  %546 = bitcast { %Array*, %Array* }* %541 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %546, i32 -1)
  br label %exiting__27

exiting__27:                                      ; preds = %body__27
  %547 = add i64 %537, 1
  br label %header__27

exit__27:                                         ; preds = %header__27
  call void @__quantum__rt__array_update_reference_count(%Array* %179, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %193, i32 -1)
  br label %header__28

header__28:                                       ; preds = %exiting__28, %exit__27
  %548 = phi i64 [ 0, %exit__27 ], [ %559, %exiting__28 ]
  %549 = icmp sle i64 %548, 3
  br i1 %549, label %body__28, label %exit__28

body__28:                                         ; preds = %header__28
  %550 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %197, i64 %548)
  %551 = bitcast i8* %550 to { { double, double }*, %Array* }**
  %552 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %551, align 8
  %553 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %552, i32 0, i32 0
  %554 = load { double, double }*, { double, double }** %553, align 8
  %555 = bitcast { double, double }* %554 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %555, i32 -1)
  %556 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %552, i32 0, i32 1
  %557 = load %Array*, %Array** %556, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %557, i32 -1)
  %558 = bitcast { { double, double }*, %Array* }* %552 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %558, i32 -1)
  br label %exiting__28

exiting__28:                                      ; preds = %body__28
  %559 = add i64 %548, 1
  br label %header__28

exit__28:                                         ; preds = %header__28
  call void @__quantum__rt__array_update_reference_count(%Array* %197, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %194, i32 -1)
  %560 = sub i64 %266, 1
  br label %header__29

header__29:                                       ; preds = %exiting__29, %exit__28
  %561 = phi i64 [ 0, %exit__28 ], [ %571, %exiting__29 ]
  %562 = icmp sle i64 %561, %560
  br i1 %562, label %body__29, label %exit__29

body__29:                                         ; preds = %header__29
  %563 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %265, i64 %561)
  %564 = bitcast i8* %563 to { %Array*, %Array* }**
  %565 = load { %Array*, %Array* }*, { %Array*, %Array* }** %564, align 8
  %566 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %565, i32 0, i32 0
  %567 = load %Array*, %Array** %566, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %567, i32 -1)
  %568 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %565, i32 0, i32 1
  %569 = load %Array*, %Array** %568, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %569, i32 -1)
  %570 = bitcast { %Array*, %Array* }* %565 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %570, i32 -1)
  br label %exiting__29

exiting__29:                                      ; preds = %body__29
  %571 = add i64 %561, 1
  br label %header__29

exit__29:                                         ; preds = %header__29
  call void @__quantum__rt__array_update_reference_count(%Array* %265, i32 -1)
  %572 = sub i64 %281, 1
  br label %header__30

header__30:                                       ; preds = %exiting__30, %exit__29
  %573 = phi i64 [ 0, %exit__29 ], [ %583, %exiting__30 ]
  %574 = icmp sle i64 %573, %572
  br i1 %574, label %body__30, label %exit__30

body__30:                                         ; preds = %header__30
  %575 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %280, i64 %573)
  %576 = bitcast i8* %575 to { %Array*, %Array* }**
  %577 = load { %Array*, %Array* }*, { %Array*, %Array* }** %576, align 8
  %578 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %577, i32 0, i32 0
  %579 = load %Array*, %Array** %578, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %579, i32 -1)
  %580 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %577, i32 0, i32 1
  %581 = load %Array*, %Array** %580, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %581, i32 -1)
  %582 = bitcast { %Array*, %Array* }* %577 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %582, i32 -1)
  br label %exiting__30

exiting__30:                                      ; preds = %body__30
  %583 = add i64 %573, 1
  br label %header__30

exit__30:                                         ; preds = %header__30
  call void @__quantum__rt__array_update_reference_count(%Array* %280, i32 -1)
  %584 = sub i64 %296, 1
  br label %header__31

header__31:                                       ; preds = %exiting__31, %exit__30
  %585 = phi i64 [ 0, %exit__30 ], [ %595, %exiting__31 ]
  %586 = icmp sle i64 %585, %584
  br i1 %586, label %body__31, label %exit__31

body__31:                                         ; preds = %header__31
  %587 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %295, i64 %585)
  %588 = bitcast i8* %587 to { %Array*, %Array* }**
  %589 = load { %Array*, %Array* }*, { %Array*, %Array* }** %588, align 8
  %590 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %589, i32 0, i32 0
  %591 = load %Array*, %Array** %590, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %591, i32 -1)
  %592 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %589, i32 0, i32 1
  %593 = load %Array*, %Array** %592, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %593, i32 -1)
  %594 = bitcast { %Array*, %Array* }* %589 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %594, i32 -1)
  br label %exiting__31

exiting__31:                                      ; preds = %body__31
  %595 = add i64 %585, 1
  br label %header__31

exit__31:                                         ; preds = %header__31
  call void @__quantum__rt__array_update_reference_count(%Array* %295, i32 -1)
  %596 = sub i64 %311, 1
  br label %header__32

header__32:                                       ; preds = %exiting__32, %exit__31
  %597 = phi i64 [ 0, %exit__31 ], [ %607, %exiting__32 ]
  %598 = icmp sle i64 %597, %596
  br i1 %598, label %body__32, label %exit__32

body__32:                                         ; preds = %header__32
  %599 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %310, i64 %597)
  %600 = bitcast i8* %599 to { %Array*, %Array* }**
  %601 = load { %Array*, %Array* }*, { %Array*, %Array* }** %600, align 8
  %602 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %601, i32 0, i32 0
  %603 = load %Array*, %Array** %602, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %603, i32 -1)
  %604 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %601, i32 0, i32 1
  %605 = load %Array*, %Array** %604, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %605, i32 -1)
  %606 = bitcast { %Array*, %Array* }* %601 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %606, i32 -1)
  br label %exiting__32

exiting__32:                                      ; preds = %body__32
  %607 = add i64 %597, 1
  br label %header__32

exit__32:                                         ; preds = %header__32
  call void @__quantum__rt__array_update_reference_count(%Array* %310, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %324, i32 -1)
  %608 = sub i64 %329, 1
  br label %header__33

header__33:                                       ; preds = %exiting__33, %exit__32
  %609 = phi i64 [ 0, %exit__32 ], [ %620, %exiting__33 ]
  %610 = icmp sle i64 %609, %608
  br i1 %610, label %body__33, label %exit__33

body__33:                                         ; preds = %header__33
  %611 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %328, i64 %609)
  %612 = bitcast i8* %611 to { { double, double }*, %Array* }**
  %613 = load { { double, double }*, %Array* }*, { { double, double }*, %Array* }** %612, align 8
  %614 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %613, i32 0, i32 0
  %615 = load { double, double }*, { double, double }** %614, align 8
  %616 = bitcast { double, double }* %615 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %616, i32 -1)
  %617 = getelementptr inbounds { { double, double }*, %Array* }, { { double, double }*, %Array* }* %613, i32 0, i32 1
  %618 = load %Array*, %Array** %617, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %618, i32 -1)
  %619 = bitcast { { double, double }*, %Array* }* %613 to %Tuple*
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %619, i32 -1)
  br label %exiting__33

exiting__33:                                      ; preds = %body__33
  %620 = add i64 %609, 1
  br label %header__33

exit__33:                                         ; preds = %header__33
  call void @__quantum__rt__array_update_reference_count(%Array* %328, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %343, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %344, i32 -1)
  ret double %345
}

define internal { %Array*, %Array* }* @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__ChemUtils__HTerm__body(%Array* %__Item1__, %Array* %__Item2__) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item1__, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item2__, i32 1)
  %0 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Array* }* getelementptr ({ %Array*, %Array* }, { %Array*, %Array* }* null, i32 1) to i64))
  %1 = bitcast %Tuple* %0 to { %Array*, %Array* }*
  %2 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %1, i32 0, i32 0
  %3 = getelementptr inbounds { %Array*, %Array* }, { %Array*, %Array* }* %1, i32 0, i32 1
  store %Array* %__Item1__, %Array** %2, align 8
  store %Array* %__Item2__, %Array** %3, align 8
  call void @__quantum__rt__array_update_reference_count(%Array* %__Item1__, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__Item2__, i32 1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item1__, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__Item2__, i32 -1)
  ret { %Array*, %Array* }* %1
}

define internal double @Microsoft__Quantum__Math__PI__body() {
entry:
  ret double 0x400921FB54442D18
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyControlledX____body(%Qubit* %control, %Qubit* %target) {
entry:
  call void @__quantum__qis__cnot__body(%Qubit* %control, %Qubit* %target)
  ret void
}

declare void @__quantum__qis__cnot__body(%Qubit*, %Qubit*)

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyControlledX____adj(%Qubit* %control, %Qubit* %target) {
entry:
  call void @__quantum__qis__cnot__body(%Qubit* %control, %Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyControlledZ____body(%Qubit* %control, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____body(%Qubit* %target)
  call void @__quantum__qis__cnot__body(%Qubit* %control, %Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____body(%Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____body(%Qubit* %qubit) {
entry:
  %0 = call double @Microsoft__Quantum__Math__PI__body()
  %1 = fneg double %0
  %2 = fdiv double %1, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic__Rx__body(double %2, %Qubit* %qubit)
  %3 = call double @Microsoft__Quantum__Math__PI__body()
  %4 = fneg double %3
  %5 = fdiv double %4, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %5, %Qubit* %qubit)
  %6 = call double @Microsoft__Quantum__Math__PI__body()
  %7 = fneg double %6
  %8 = fdiv double %7, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic__Rx__body(double %8, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyControlledZ____adj(%Qubit* %control, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyControlledZ____body(%Qubit* %control, %Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyGlobalPhase____body(double %theta) {
entry:
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyGlobalPhase____adj(double %theta) {
entry:
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyGlobalPhase____ctl(%Array* %controls, double %theta) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %controls)
  %1 = icmp sgt i64 %0, 0
  br i1 %1, label %then0__1, label %continue__1

then0__1:                                         ; preds = %entry
  %2 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 0)
  %3 = bitcast i8* %2 to %Qubit**
  %qubit = load %Qubit*, %Qubit** %3, align 8
  %4 = sub i64 %0, 1
  %5 = load %Range, %Range* @EmptyRange, align 4
  %6 = insertvalue %Range %5, i64 1, 0
  %7 = insertvalue %Range %6, i64 1, 1
  %8 = insertvalue %Range %7, i64 %4, 2
  %rest = call %Array* @__quantum__rt__array_slice_1d(%Array* %controls, %Range %8, i1 true)
  call void @__quantum__rt__array_update_alias_count(%Array* %rest, i32 1)
  %9 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %10 = bitcast %Tuple* %9 to { double, %Qubit* }*
  %11 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %10, i32 0, i32 0
  %12 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %10, i32 0, i32 1
  store double %theta, double* %11, align 8
  store %Qubit* %qubit, %Qubit** %12, align 8
  call void @Microsoft__Quantum__Intrinsic__R1__ctl(%Array* %rest, { double, %Qubit* }* %10)
  call void @__quantum__rt__array_update_alias_count(%Array* %rest, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %rest, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %9, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %then0__1, %entry
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 -1)
  ret void
}

declare %Array* @__quantum__rt__array_slice_1d(%Array*, %Range, i1)

define internal void @Microsoft__Quantum__Intrinsic__R1__ctl(%Array* %__controlQubits__, { double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %theta = load double, double* %1, align 8
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %qubit = load %Qubit*, %Qubit** %2, align 8
  %3 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, double, %Qubit* }* getelementptr ({ i2, double, %Qubit* }, { i2, double, %Qubit* }* null, i32 1) to i64))
  %4 = bitcast %Tuple* %3 to { i2, double, %Qubit* }*
  %5 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %4, i32 0, i32 0
  %6 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %4, i32 0, i32 1
  %7 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %4, i32 0, i32 2
  %8 = load i2, i2* @PauliZ, align 1
  store i2 %8, i2* %5, align 1
  store double %theta, double* %6, align 8
  store %Qubit* %qubit, %Qubit** %7, align 8
  call void @Microsoft__Quantum__Intrinsic__R__ctl(%Array* %__controlQubits__, { i2, double, %Qubit* }* %4)
  %9 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, double, %Qubit* }* getelementptr ({ i2, double, %Qubit* }, { i2, double, %Qubit* }* null, i32 1) to i64))
  %10 = bitcast %Tuple* %9 to { i2, double, %Qubit* }*
  %11 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %10, i32 0, i32 0
  %12 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %10, i32 0, i32 1
  %13 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %10, i32 0, i32 2
  %14 = load i2, i2* @PauliI, align 1
  %15 = fneg double %theta
  store i2 %14, i2* %11, align 1
  store double %15, double* %12, align 8
  store %Qubit* %qubit, %Qubit** %13, align 8
  call void @Microsoft__Quantum__Intrinsic__R__ctl(%Array* %__controlQubits__, { i2, double, %Qubit* }* %10)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %9, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyGlobalPhase____ctladj(%Array* %controls, double %theta) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %controls)
  %1 = icmp sgt i64 %0, 0
  br i1 %1, label %then0__1, label %continue__1

then0__1:                                         ; preds = %entry
  %2 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 0)
  %3 = bitcast i8* %2 to %Qubit**
  %__qsVar0__qubit__ = load %Qubit*, %Qubit** %3, align 8
  %4 = sub i64 %0, 1
  %5 = load %Range, %Range* @EmptyRange, align 4
  %6 = insertvalue %Range %5, i64 1, 0
  %7 = insertvalue %Range %6, i64 1, 1
  %8 = insertvalue %Range %7, i64 %4, 2
  %__qsVar1__rest__ = call %Array* @__quantum__rt__array_slice_1d(%Array* %controls, %Range %8, i1 true)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1__rest__, i32 1)
  %9 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %10 = bitcast %Tuple* %9 to { double, %Qubit* }*
  %11 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %10, i32 0, i32 0
  %12 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %10, i32 0, i32 1
  store double %theta, double* %11, align 8
  store %Qubit* %__qsVar0__qubit__, %Qubit** %12, align 8
  call void @Microsoft__Quantum__Intrinsic__R1__ctladj(%Array* %__qsVar1__rest__, { double, %Qubit* }* %10)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1__rest__, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__rest__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %9, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %then0__1, %entry
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R1__ctladj(%Array* %__controlQubits__, { double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %theta = load double, double* %1, align 8
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %qubit = load %Qubit*, %Qubit** %2, align 8
  %3 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, double, %Qubit* }* getelementptr ({ i2, double, %Qubit* }, { i2, double, %Qubit* }* null, i32 1) to i64))
  %4 = bitcast %Tuple* %3 to { i2, double, %Qubit* }*
  %5 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %4, i32 0, i32 0
  %6 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %4, i32 0, i32 1
  %7 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %4, i32 0, i32 2
  %8 = load i2, i2* @PauliI, align 1
  %9 = fneg double %theta
  store i2 %8, i2* %5, align 1
  store double %9, double* %6, align 8
  store %Qubit* %qubit, %Qubit** %7, align 8
  call void @Microsoft__Quantum__Intrinsic__R__ctladj(%Array* %__controlQubits__, { i2, double, %Qubit* }* %4)
  %10 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, double, %Qubit* }* getelementptr ({ i2, double, %Qubit* }, { i2, double, %Qubit* }* null, i32 1) to i64))
  %11 = bitcast %Tuple* %10 to { i2, double, %Qubit* }*
  %12 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %11, i32 0, i32 0
  %13 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %11, i32 0, i32 1
  %14 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %11, i32 0, i32 2
  %15 = load i2, i2* @PauliZ, align 1
  store i2 %15, i2* %12, align 1
  store double %theta, double* %13, align 8
  store %Qubit* %qubit, %Qubit** %14, align 8
  call void @Microsoft__Quantum__Intrinsic__R__ctladj(%Array* %__controlQubits__, { i2, double, %Qubit* }* %11)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %10, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____adj(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledRx____body(double %theta, %Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledSAdj____body(%Qubit* %qubit)
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %0 = phi i64 [ 1, %entry ], [ %2, %exiting__1 ]
  %1 = icmp sle i64 %0, 3
  br i1 %1, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  call void @__quantum__qis__sqrtx__body(%Qubit* %qubit)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %2 = add i64 %0, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %theta, %Qubit* %qubit)
  call void @__quantum__qis__sqrtx__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledS____body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledSAdj____body(%Qubit* %qubit) {
entry:
  %0 = call double @Microsoft__Quantum__Math__PI__body()
  %1 = fneg double %0
  %2 = fdiv double %1, 2.000000e+00
  call void @__quantum__qis__rz__body(double %2, %Qubit* %qubit)
  ret void
}

declare void @__quantum__qis__sqrtx__body(%Qubit*)

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledS____body(%Qubit* %qubit) {
entry:
  %0 = call double @Microsoft__Quantum__Math__PI__body()
  %1 = fdiv double %0, 2.000000e+00
  call void @__quantum__qis__rz__body(double %1, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledRy____body(double %theta, %Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____body(%Qubit* %qubit)
  call void @__quantum__qis__rz__body(double %theta, %Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____body(%Qubit* %qubit)
  ret void
}

declare void @__quantum__qis__rz__body(double, %Qubit*)

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledT____body(%Qubit* %qubit) {
entry:
  %0 = call double @Microsoft__Quantum__Math__PI__body()
  %1 = fdiv double %0, 4.000000e+00
  call void @__quantum__qis__rz__body(double %1, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledTAdj____body(%Qubit* %qubit) {
entry:
  %0 = call double @Microsoft__Quantum__Math__PI__body()
  %1 = fneg double %0
  %2 = fdiv double %1, 4.000000e+00
  call void @__quantum__qis__rz__body(double %2, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledX____body(%Qubit* %qubit) {
entry:
  call void @__quantum__qis__x__body(%Qubit* %qubit)
  ret void
}

declare void @__quantum__qis__x__body(%Qubit*)

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledX____adj(%Qubit* %qubit) {
entry:
  call void @__quantum__qis__x__body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledZ____body(%Qubit* %qubit) {
entry:
  %0 = call double @Microsoft__Quantum__Math__PI__body()
  call void @__quantum__qis__rz__body(double %0, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledZ____adj(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledZ____body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__CCZ____body(%Qubit* %control1, %Qubit* %control2, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %target, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %control2, %Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %control2, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %control2, %Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %target, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %control2, %Qubit* %control1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledTAdj____body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledT____body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__CCZ____adj(%Qubit* %control1, %Qubit* %control2, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %control2, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %target, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %control2, %Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %control2, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %control2, %Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %target, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %control1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %control, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %control, %Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__MapPauli____body(%Qubit* %qubit, i2 %from, i2 %to) {
entry:
  %0 = icmp eq i2 %from, %to
  br i1 %0, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  br label %continue__1

test1__1:                                         ; preds = %entry
  %1 = load i2, i2* @PauliZ, align 1
  %2 = icmp eq i2 %from, %1
  br i1 %2, label %condTrue__1, label %condContinue__1

condTrue__1:                                      ; preds = %test1__1
  %3 = load i2, i2* @PauliX, align 1
  %4 = icmp eq i2 %to, %3
  br label %condContinue__1

condContinue__1:                                  ; preds = %condTrue__1, %test1__1
  %5 = phi i1 [ %4, %condTrue__1 ], [ %2, %test1__1 ]
  %6 = xor i1 %5, true
  br i1 %6, label %condTrue__2, label %condContinue__2

condTrue__2:                                      ; preds = %condContinue__1
  %7 = load i2, i2* @PauliX, align 1
  %8 = icmp eq i2 %from, %7
  br i1 %8, label %condTrue__3, label %condContinue__3

condTrue__3:                                      ; preds = %condTrue__2
  %9 = load i2, i2* @PauliZ, align 1
  %10 = icmp eq i2 %to, %9
  br label %condContinue__3

condContinue__3:                                  ; preds = %condTrue__3, %condTrue__2
  %11 = phi i1 [ %10, %condTrue__3 ], [ %8, %condTrue__2 ]
  br label %condContinue__2

condContinue__2:                                  ; preds = %condContinue__3, %condContinue__1
  %12 = phi i1 [ %11, %condContinue__3 ], [ %5, %condContinue__1 ]
  br i1 %12, label %then1__1, label %test2__1

then1__1:                                         ; preds = %condContinue__2
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qubit)
  br label %continue__1

test2__1:                                         ; preds = %condContinue__2
  %13 = load i2, i2* @PauliZ, align 1
  %14 = icmp eq i2 %from, %13
  br i1 %14, label %condTrue__4, label %condContinue__4

condTrue__4:                                      ; preds = %test2__1
  %15 = load i2, i2* @PauliY, align 1
  %16 = icmp eq i2 %to, %15
  br label %condContinue__4

condContinue__4:                                  ; preds = %condTrue__4, %test2__1
  %17 = phi i1 [ %16, %condTrue__4 ], [ %14, %test2__1 ]
  br i1 %17, label %then2__1, label %test3__1

then2__1:                                         ; preds = %condContinue__4
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qubit)
  br label %continue__1

test3__1:                                         ; preds = %condContinue__4
  %18 = load i2, i2* @PauliY, align 1
  %19 = icmp eq i2 %from, %18
  br i1 %19, label %condTrue__5, label %condContinue__5

condTrue__5:                                      ; preds = %test3__1
  %20 = load i2, i2* @PauliZ, align 1
  %21 = icmp eq i2 %to, %20
  br label %condContinue__5

condContinue__5:                                  ; preds = %condTrue__5, %test3__1
  %22 = phi i1 [ %21, %condTrue__5 ], [ %19, %test3__1 ]
  br i1 %22, label %then3__1, label %test4__1

then3__1:                                         ; preds = %condContinue__5
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__S__adj(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qubit)
  br label %continue__1

test4__1:                                         ; preds = %condContinue__5
  %23 = load i2, i2* @PauliY, align 1
  %24 = icmp eq i2 %from, %23
  br i1 %24, label %condTrue__6, label %condContinue__6

condTrue__6:                                      ; preds = %test4__1
  %25 = load i2, i2* @PauliX, align 1
  %26 = icmp eq i2 %to, %25
  br label %condContinue__6

condContinue__6:                                  ; preds = %condTrue__6, %test4__1
  %27 = phi i1 [ %26, %condTrue__6 ], [ %24, %test4__1 ]
  br i1 %27, label %then4__1, label %test5__1

then4__1:                                         ; preds = %condContinue__6
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qubit)
  br label %continue__1

test5__1:                                         ; preds = %condContinue__6
  %28 = load i2, i2* @PauliX, align 1
  %29 = icmp eq i2 %from, %28
  br i1 %29, label %condTrue__7, label %condContinue__7

condTrue__7:                                      ; preds = %test5__1
  %30 = load i2, i2* @PauliY, align 1
  %31 = icmp eq i2 %to, %30
  br label %condContinue__7

condContinue__7:                                  ; preds = %condTrue__7, %test5__1
  %32 = phi i1 [ %31, %condTrue__7 ], [ %29, %test5__1 ]
  br i1 %32, label %then5__1, label %else__1

then5__1:                                         ; preds = %condContinue__7
  call void @Microsoft__Quantum__Intrinsic__S__adj(%Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %condContinue__7
  %33 = call %String* @__quantum__rt__string_create(i8* getelementptr inbounds ([18 x i8], [18 x i8]* @0, i32 0, i32 0))
  call void @__quantum__rt__fail(%String* %33)
  unreachable

continue__1:                                      ; preds = %then5__1, %then4__1, %then3__1, %then2__1, %then1__1, %then0__1
  ret void
}

declare %String* @__quantum__rt__string_create(i8*)

declare void @__quantum__rt__fail(%String*)

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__MapPauli____adj(%Qubit* %qubit, i2 %from, i2 %to) {
entry:
  %0 = icmp eq i2 %from, %to
  br i1 %0, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  br label %continue__1

test1__1:                                         ; preds = %entry
  %1 = load i2, i2* @PauliZ, align 1
  %2 = icmp eq i2 %from, %1
  br i1 %2, label %condTrue__1, label %condContinue__1

condTrue__1:                                      ; preds = %test1__1
  %3 = load i2, i2* @PauliX, align 1
  %4 = icmp eq i2 %to, %3
  br label %condContinue__1

condContinue__1:                                  ; preds = %condTrue__1, %test1__1
  %5 = phi i1 [ %4, %condTrue__1 ], [ %2, %test1__1 ]
  %6 = xor i1 %5, true
  br i1 %6, label %condTrue__2, label %condContinue__2

condTrue__2:                                      ; preds = %condContinue__1
  %7 = load i2, i2* @PauliX, align 1
  %8 = icmp eq i2 %from, %7
  br i1 %8, label %condTrue__3, label %condContinue__3

condTrue__3:                                      ; preds = %condTrue__2
  %9 = load i2, i2* @PauliZ, align 1
  %10 = icmp eq i2 %to, %9
  br label %condContinue__3

condContinue__3:                                  ; preds = %condTrue__3, %condTrue__2
  %11 = phi i1 [ %10, %condTrue__3 ], [ %8, %condTrue__2 ]
  br label %condContinue__2

condContinue__2:                                  ; preds = %condContinue__3, %condContinue__1
  %12 = phi i1 [ %11, %condContinue__3 ], [ %5, %condContinue__1 ]
  br i1 %12, label %then1__1, label %test2__1

then1__1:                                         ; preds = %condContinue__2
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %qubit)
  br label %continue__1

test2__1:                                         ; preds = %condContinue__2
  %13 = load i2, i2* @PauliZ, align 1
  %14 = icmp eq i2 %from, %13
  br i1 %14, label %condTrue__4, label %condContinue__4

condTrue__4:                                      ; preds = %test2__1
  %15 = load i2, i2* @PauliY, align 1
  %16 = icmp eq i2 %to, %15
  br label %condContinue__4

condContinue__4:                                  ; preds = %condTrue__4, %test2__1
  %17 = phi i1 [ %16, %condTrue__4 ], [ %14, %test2__1 ]
  br i1 %17, label %then2__1, label %test3__1

then2__1:                                         ; preds = %condContinue__4
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__S__adj(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %qubit)
  br label %continue__1

test3__1:                                         ; preds = %condContinue__4
  %18 = load i2, i2* @PauliY, align 1
  %19 = icmp eq i2 %from, %18
  br i1 %19, label %condTrue__5, label %condContinue__5

condTrue__5:                                      ; preds = %test3__1
  %20 = load i2, i2* @PauliZ, align 1
  %21 = icmp eq i2 %to, %20
  br label %condContinue__5

condContinue__5:                                  ; preds = %condTrue__5, %test3__1
  %22 = phi i1 [ %21, %condTrue__5 ], [ %19, %test3__1 ]
  br i1 %22, label %then3__1, label %test4__1

then3__1:                                         ; preds = %condContinue__5
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %qubit)
  br label %continue__1

test4__1:                                         ; preds = %condContinue__5
  %23 = load i2, i2* @PauliY, align 1
  %24 = icmp eq i2 %from, %23
  br i1 %24, label %condTrue__6, label %condContinue__6

condTrue__6:                                      ; preds = %test4__1
  %25 = load i2, i2* @PauliX, align 1
  %26 = icmp eq i2 %to, %25
  br label %condContinue__6

condContinue__6:                                  ; preds = %condTrue__6, %test4__1
  %27 = phi i1 [ %26, %condTrue__6 ], [ %24, %test4__1 ]
  br i1 %27, label %then4__1, label %test5__1

then4__1:                                         ; preds = %condContinue__6
  call void @Microsoft__Quantum__Intrinsic__S__adj(%Qubit* %qubit)
  br label %continue__1

test5__1:                                         ; preds = %condContinue__6
  %28 = load i2, i2* @PauliX, align 1
  %29 = icmp eq i2 %from, %28
  br i1 %29, label %condTrue__7, label %condContinue__7

condTrue__7:                                      ; preds = %test5__1
  %30 = load i2, i2* @PauliY, align 1
  %31 = icmp eq i2 %to, %30
  br label %condContinue__7

condContinue__7:                                  ; preds = %condTrue__7, %test5__1
  %32 = phi i1 [ %31, %condTrue__7 ], [ %29, %test5__1 ]
  br i1 %32, label %then5__1, label %else__1

then5__1:                                         ; preds = %condContinue__7
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %condContinue__7
  %33 = call %String* @__quantum__rt__string_create(i8* getelementptr inbounds ([18 x i8], [18 x i8]* @0, i32 0, i32 0))
  call void @__quantum__rt__fail(%String* %33)
  unreachable

continue__1:                                      ; preds = %then5__1, %then4__1, %then3__1, %then2__1, %then1__1, %then0__1
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____body(%Qubit* %control1, %Qubit* %control2, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %target, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %control1, %Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %target, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %control1, %Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %target, %Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____adj(%Qubit* %control1, %Qubit* %control2, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %target, %Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %control1, %Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %target, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %control1, %Qubit* %control2)
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %target, %Qubit* %control1)
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic____QsRef36__PreparePostM____body(%Result* %result, %Qubit* %qubit) {
entry:
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CCNOT__body(%Qubit* %control1, %Qubit* %control2, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic____QsRef36__CCZ____body(%Qubit* %control1, %Qubit* %control2, %Qubit* %target)
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CCNOT__adj(%Qubit* %control1, %Qubit* %control2, %Qubit* %target) {
entry:
  call void @Microsoft__Quantum__Intrinsic__CCNOT__body(%Qubit* %control1, %Qubit* %control2, %Qubit* %target)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CCNOT__ctl(%Array* %ctls, { %Qubit*, %Qubit*, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %1 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %0, i32 0, i32 0
  %control1 = load %Qubit*, %Qubit** %1, align 8
  %2 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %0, i32 0, i32 1
  %control2 = load %Qubit*, %Qubit** %2, align 8
  %3 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %0, i32 0, i32 2
  %target = load %Qubit*, %Qubit** %3, align 8
  %4 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 2)
  %5 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %4, i64 0)
  %6 = bitcast i8* %5 to %Qubit**
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %4, i64 1)
  %8 = bitcast i8* %7 to %Qubit**
  store %Qubit* %control1, %Qubit** %6, align 8
  store %Qubit* %control2, %Qubit** %8, align 8
  %9 = call %Array* @__quantum__rt__array_concatenate(%Array* %ctls, %Array* %4)
  call void @__quantum__rt__array_update_reference_count(%Array* %9, i32 1)
  call void @Microsoft__Quantum__Intrinsic__X__ctl(%Array* %9, %Qubit* %target)
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %4, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %9, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %9, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__X__ctl(%Array* %ctls, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %1 = icmp eq i64 %0, 0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @__quantum__qis__x__body(%Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %3 = icmp eq i64 %2, 1
  br i1 %3, label %then1__1, label %test2__1

then1__1:                                         ; preds = %test1__1
  %4 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %5 = bitcast i8* %4 to %Qubit**
  %control = load %Qubit*, %Qubit** %5, align 8
  call void @__quantum__qis__cnot__body(%Qubit* %control, %Qubit* %qubit)
  br label %continue__1

test2__1:                                         ; preds = %test1__1
  %6 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %7 = icmp eq i64 %6, 2
  br i1 %7, label %then2__1, label %else__1

then2__1:                                         ; preds = %test2__1
  %8 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %9 = bitcast i8* %8 to %Qubit**
  %10 = load %Qubit*, %Qubit** %9, align 8
  %11 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 1)
  %12 = bitcast i8* %11 to %Qubit**
  %13 = load %Qubit*, %Qubit** %12, align 8
  call void @Microsoft__Quantum__Intrinsic__CCNOT__body(%Qubit* %10, %Qubit* %13, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test2__1
  %14 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__X__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %14)
  %15 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %16 = bitcast %Tuple* %15 to { %Array*, %Qubit* }*
  %17 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %16, i32 0, i32 0
  %18 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %16, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  store %Array* %ctls, %Array** %17, align 8
  store %Qubit* %qubit, %Qubit** %18, align 8
  call void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____body(%Callable* %14, { %Array*, %Qubit* }* %16)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %14, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %14, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %15, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then2__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

declare %Array* @__quantum__rt__array_concatenate(%Array*, %Array*)

define internal void @Microsoft__Quantum__Intrinsic__CCNOT__ctladj(%Array* %__controlQubits__, { %Qubit*, %Qubit*, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %0, i32 0, i32 0
  %control1 = load %Qubit*, %Qubit** %1, align 8
  %2 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %0, i32 0, i32 1
  %control2 = load %Qubit*, %Qubit** %2, align 8
  %3 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %0, i32 0, i32 2
  %target = load %Qubit*, %Qubit** %3, align 8
  %4 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Qubit*, %Qubit*, %Qubit* }* getelementptr ({ %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* null, i32 1) to i64))
  %5 = bitcast %Tuple* %4 to { %Qubit*, %Qubit*, %Qubit* }*
  %6 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %5, i32 0, i32 0
  %7 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %5, i32 0, i32 1
  %8 = getelementptr inbounds { %Qubit*, %Qubit*, %Qubit* }, { %Qubit*, %Qubit*, %Qubit* }* %5, i32 0, i32 2
  store %Qubit* %control1, %Qubit** %6, align 8
  store %Qubit* %control2, %Qubit** %7, align 8
  store %Qubit* %target, %Qubit** %8, align 8
  call void @Microsoft__Quantum__Intrinsic__CCNOT__ctl(%Array* %__controlQubits__, { %Qubit*, %Qubit*, %Qubit* }* %5)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %4, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CNOT__ctl(%Array* %ctls, { %Qubit*, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %1 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %0, i32 0, i32 0
  %control = load %Qubit*, %Qubit** %1, align 8
  %2 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %0, i32 0, i32 1
  %target = load %Qubit*, %Qubit** %2, align 8
  %3 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %4 = icmp eq i64 %3, 0
  br i1 %4, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @__quantum__qis__cnot__body(%Qubit* %control, %Qubit* %target)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %5 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %6 = icmp eq i64 %5, 1
  br i1 %6, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  call void @Microsoft__Quantum__Intrinsic__CCNOT__body(%Qubit* %9, %Qubit* %control, %Qubit* %target)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %10 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__CNOT__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %10)
  %11 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, { %Qubit*, %Qubit* }* }* getelementptr ({ %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* null, i32 1) to i64))
  %12 = bitcast %Tuple* %11 to { %Array*, { %Qubit*, %Qubit* }* }*
  %13 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %12, i32 0, i32 0
  %14 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %12, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  %15 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Qubit*, %Qubit* }* getelementptr ({ %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* null, i32 1) to i64))
  %16 = bitcast %Tuple* %15 to { %Qubit*, %Qubit* }*
  %17 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %16, i32 0, i32 0
  %18 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %16, i32 0, i32 1
  store %Qubit* %control, %Qubit** %17, align 8
  store %Qubit* %target, %Qubit** %18, align 8
  store %Array* %ctls, %Array** %13, align 8
  store { %Qubit*, %Qubit* }* %16, { %Qubit*, %Qubit* }** %14, align 8
  call void @Microsoft__Quantum__Intrinsic___6bbae0085dbd4f50a756d3774cceb0fe___QsRef36__ApplyWithLessControlsA____body(%Callable* %10, { %Array*, { %Qubit*, %Qubit* }* }* %12)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %10, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %10, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %15, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %11, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic___6bbae0085dbd4f50a756d3774cceb0fe___QsRef36__ApplyWithLessControlsA____body(%Callable* %op, { %Array*, { %Qubit*, %Qubit* }* }* %0) {
entry:
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 1)
  %1 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %0, i32 0, i32 0
  %controls = load %Array*, %Array** %1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 1)
  %2 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %0, i32 0, i32 1
  %arg = load { %Qubit*, %Qubit* }*, { %Qubit*, %Qubit* }** %2, align 8
  %3 = bitcast { %Qubit*, %Qubit* }* %arg to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %3, i32 1)
  %numControls = call i64 @__quantum__rt__array_get_size_1d(%Array* %controls)
  %numControlPairs = sdiv i64 %numControls, 2
  %temps = call %Array* @__quantum__rt__qubit_allocate_array(i64 %numControlPairs)
  call void @__quantum__rt__array_update_alias_count(%Array* %temps, i32 1)
  %4 = sub i64 %numControlPairs, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %__qsVar0__numPair__ = phi i64 [ 0, %entry ], [ %18, %exiting__1 ]
  %5 = icmp sle i64 %__qsVar0__numPair__, %4
  br i1 %5, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %6 = mul i64 2, %__qsVar0__numPair__
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %6)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  %10 = mul i64 2, %__qsVar0__numPair__
  %11 = add i64 %10, 1
  %12 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %11)
  %13 = bitcast i8* %12 to %Qubit**
  %14 = load %Qubit*, %Qubit** %13, align 8
  %15 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %temps, i64 %__qsVar0__numPair__)
  %16 = bitcast i8* %15 to %Qubit**
  %17 = load %Qubit*, %Qubit** %16, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____body(%Qubit* %9, %Qubit* %14, %Qubit* %17)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %18 = add i64 %__qsVar0__numPair__, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %19 = srem i64 %numControls, 2
  %20 = icmp eq i64 %19, 0
  br i1 %20, label %condTrue__1, label %condFalse__1

condTrue__1:                                      ; preds = %exit__1
  call void @__quantum__rt__array_update_reference_count(%Array* %temps, i32 1)
  br label %condContinue__1

condFalse__1:                                     ; preds = %exit__1
  %21 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %22 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %21, i64 0)
  %23 = bitcast i8* %22 to %Qubit**
  %24 = sub i64 %numControls, 1
  %25 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %24)
  %26 = bitcast i8* %25 to %Qubit**
  %27 = load %Qubit*, %Qubit** %26, align 8
  store %Qubit* %27, %Qubit** %23, align 8
  %28 = call %Array* @__quantum__rt__array_concatenate(%Array* %temps, %Array* %21)
  call void @__quantum__rt__array_update_reference_count(%Array* %28, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %21, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %28, i32 -1)
  br label %condContinue__1

condContinue__1:                                  ; preds = %condFalse__1, %condTrue__1
  %__qsVar1__newControls__ = phi %Array* [ %temps, %condTrue__1 ], [ %28, %condFalse__1 ]
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1__newControls__, i32 1)
  %29 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, { %Qubit*, %Qubit* }* }* getelementptr ({ %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* null, i32 1) to i64))
  %30 = bitcast %Tuple* %29 to { %Array*, { %Qubit*, %Qubit* }* }*
  %31 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %30, i32 0, i32 0
  %32 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %30, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 1)
  store %Array* %__qsVar1__newControls__, %Array** %31, align 8
  store { %Qubit*, %Qubit* }* %arg, { %Qubit*, %Qubit* }** %32, align 8
  call void @__quantum__rt__callable_invoke(%Callable* %op, %Tuple* %29, %Tuple* null)
  %33 = sub i64 %numControlPairs, 1
  %34 = sub i64 %33, 0
  %35 = sdiv i64 %34, 1
  %36 = mul i64 1, %35
  %37 = add i64 0, %36
  %38 = load %Range, %Range* @EmptyRange, align 4
  %39 = insertvalue %Range %38, i64 %37, 0
  %40 = insertvalue %Range %39, i64 -1, 1
  %41 = insertvalue %Range %40, i64 0, 2
  %42 = extractvalue %Range %41, 0
  %43 = extractvalue %Range %41, 1
  %44 = extractvalue %Range %41, 2
  br label %preheader__1

preheader__1:                                     ; preds = %condContinue__1
  %45 = icmp sgt i64 %43, 0
  br label %header__2

header__2:                                        ; preds = %exiting__2, %preheader__1
  %__qsVar0____qsVar0__numPair____ = phi i64 [ %42, %preheader__1 ], [ %61, %exiting__2 ]
  %46 = icmp sle i64 %__qsVar0____qsVar0__numPair____, %44
  %47 = icmp sge i64 %__qsVar0____qsVar0__numPair____, %44
  %48 = select i1 %45, i1 %46, i1 %47
  br i1 %48, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %49 = mul i64 2, %__qsVar0____qsVar0__numPair____
  %50 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %49)
  %51 = bitcast i8* %50 to %Qubit**
  %52 = load %Qubit*, %Qubit** %51, align 8
  %53 = mul i64 2, %__qsVar0____qsVar0__numPair____
  %54 = add i64 %53, 1
  %55 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %54)
  %56 = bitcast i8* %55 to %Qubit**
  %57 = load %Qubit*, %Qubit** %56, align 8
  %58 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %temps, i64 %__qsVar0____qsVar0__numPair____)
  %59 = bitcast i8* %58 to %Qubit**
  %60 = load %Qubit*, %Qubit** %59, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____adj(%Qubit* %52, %Qubit* %57, %Qubit* %60)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %61 = add i64 %__qsVar0____qsVar0__numPair____, %43
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %temps, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %29, i32 -1)
  call void @__quantum__rt__qubit_release_array(%Array* %temps)
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %3, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CNOT__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit*, %Qubit* }*
  %1 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Qubit*, %Qubit** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CNOT__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit*, %Qubit* }*
  %1 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Qubit*, %Qubit** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__adj(%Qubit* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CNOT__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, { %Qubit*, %Qubit* }* }*
  %1 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load { %Qubit*, %Qubit* }*, { %Qubit*, %Qubit* }** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__ctl(%Array* %3, { %Qubit*, %Qubit* }* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__CNOT__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, { %Qubit*, %Qubit* }* }*
  %1 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load { %Qubit*, %Qubit* }*, { %Qubit*, %Qubit* }** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__ctladj(%Array* %3, { %Qubit*, %Qubit* }* %4)
  ret void
}

declare %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]*, [2 x void (%Tuple*, i32)*]*, %Tuple*)

declare void @__quantum__rt__callable_make_controlled(%Callable*)

declare void @__quantum__rt__capture_update_reference_count(%Callable*, i32)

declare void @__quantum__rt__callable_update_reference_count(%Callable*, i32)

define internal void @Microsoft__Quantum__Intrinsic__CNOT__ctladj(%Array* %__controlQubits__, { %Qubit*, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %0, i32 0, i32 0
  %control = load %Qubit*, %Qubit** %1, align 8
  %2 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %0, i32 0, i32 1
  %target = load %Qubit*, %Qubit** %2, align 8
  %3 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Qubit*, %Qubit* }* getelementptr ({ %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* null, i32 1) to i64))
  %4 = bitcast %Tuple* %3 to { %Qubit*, %Qubit* }*
  %5 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %4, i32 0, i32 0
  %6 = getelementptr inbounds { %Qubit*, %Qubit* }, { %Qubit*, %Qubit* }* %4, i32 0, i32 1
  store %Qubit* %control, %Qubit** %5, align 8
  store %Qubit* %target, %Qubit** %6, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__ctl(%Array* %__controlQubits__, { %Qubit*, %Qubit* }* %4)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__H__ctl(%Array* %ctls, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %1 = icmp eq i64 %0, 0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledH____body(%Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %3 = icmp eq i64 %2, 1
  br i1 %3, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %qubit)
  %4 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %5 = bitcast i8* %4 to %Qubit**
  %6 = load %Qubit*, %Qubit** %5, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %6, %Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__S__adj(%Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %7 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__H__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %7)
  %8 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %9 = bitcast %Tuple* %8 to { %Array*, %Qubit* }*
  %10 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %9, i32 0, i32 0
  %11 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %9, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  store %Array* %ctls, %Array** %10, align 8
  store %Qubit* %qubit, %Qubit** %11, align 8
  call void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____body(%Callable* %7, { %Array*, %Qubit* }* %9)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %7, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %7, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %8, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____body(%Callable* %op, { %Array*, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 1)
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %controls = load %Array*, %Array** %1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 1)
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %arg = load %Qubit*, %Qubit** %2, align 8
  %numControls = call i64 @__quantum__rt__array_get_size_1d(%Array* %controls)
  %numControlPairs = sdiv i64 %numControls, 2
  %temps = call %Array* @__quantum__rt__qubit_allocate_array(i64 %numControlPairs)
  call void @__quantum__rt__array_update_alias_count(%Array* %temps, i32 1)
  %3 = sub i64 %numControlPairs, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %__qsVar0__numPair__ = phi i64 [ 0, %entry ], [ %17, %exiting__1 ]
  %4 = icmp sle i64 %__qsVar0__numPair__, %3
  br i1 %4, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %5 = mul i64 2, %__qsVar0__numPair__
  %6 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %5)
  %7 = bitcast i8* %6 to %Qubit**
  %8 = load %Qubit*, %Qubit** %7, align 8
  %9 = mul i64 2, %__qsVar0__numPair__
  %10 = add i64 %9, 1
  %11 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %10)
  %12 = bitcast i8* %11 to %Qubit**
  %13 = load %Qubit*, %Qubit** %12, align 8
  %14 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %temps, i64 %__qsVar0__numPair__)
  %15 = bitcast i8* %14 to %Qubit**
  %16 = load %Qubit*, %Qubit** %15, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____body(%Qubit* %8, %Qubit* %13, %Qubit* %16)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %17 = add i64 %__qsVar0__numPair__, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %18 = srem i64 %numControls, 2
  %19 = icmp eq i64 %18, 0
  br i1 %19, label %condTrue__1, label %condFalse__1

condTrue__1:                                      ; preds = %exit__1
  call void @__quantum__rt__array_update_reference_count(%Array* %temps, i32 1)
  br label %condContinue__1

condFalse__1:                                     ; preds = %exit__1
  %20 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %21 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %20, i64 0)
  %22 = bitcast i8* %21 to %Qubit**
  %23 = sub i64 %numControls, 1
  %24 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %23)
  %25 = bitcast i8* %24 to %Qubit**
  %26 = load %Qubit*, %Qubit** %25, align 8
  store %Qubit* %26, %Qubit** %22, align 8
  %27 = call %Array* @__quantum__rt__array_concatenate(%Array* %temps, %Array* %20)
  call void @__quantum__rt__array_update_reference_count(%Array* %27, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %20, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %27, i32 -1)
  br label %condContinue__1

condContinue__1:                                  ; preds = %condFalse__1, %condTrue__1
  %__qsVar1__newControls__ = phi %Array* [ %temps, %condTrue__1 ], [ %27, %condFalse__1 ]
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1__newControls__, i32 1)
  %28 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %29 = bitcast %Tuple* %28 to { %Array*, %Qubit* }*
  %30 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %29, i32 0, i32 0
  %31 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %29, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 1)
  store %Array* %__qsVar1__newControls__, %Array** %30, align 8
  store %Qubit* %arg, %Qubit** %31, align 8
  call void @__quantum__rt__callable_invoke(%Callable* %op, %Tuple* %28, %Tuple* null)
  %32 = sub i64 %numControlPairs, 1
  %33 = sub i64 %32, 0
  %34 = sdiv i64 %33, 1
  %35 = mul i64 1, %34
  %36 = add i64 0, %35
  %37 = load %Range, %Range* @EmptyRange, align 4
  %38 = insertvalue %Range %37, i64 %36, 0
  %39 = insertvalue %Range %38, i64 -1, 1
  %40 = insertvalue %Range %39, i64 0, 2
  %41 = extractvalue %Range %40, 0
  %42 = extractvalue %Range %40, 1
  %43 = extractvalue %Range %40, 2
  br label %preheader__1

preheader__1:                                     ; preds = %condContinue__1
  %44 = icmp sgt i64 %42, 0
  br label %header__2

header__2:                                        ; preds = %exiting__2, %preheader__1
  %__qsVar0____qsVar0__numPair____ = phi i64 [ %41, %preheader__1 ], [ %60, %exiting__2 ]
  %45 = icmp sle i64 %__qsVar0____qsVar0__numPair____, %43
  %46 = icmp sge i64 %__qsVar0____qsVar0__numPair____, %43
  %47 = select i1 %44, i1 %45, i1 %46
  br i1 %47, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %48 = mul i64 2, %__qsVar0____qsVar0__numPair____
  %49 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %48)
  %50 = bitcast i8* %49 to %Qubit**
  %51 = load %Qubit*, %Qubit** %50, align 8
  %52 = mul i64 2, %__qsVar0____qsVar0__numPair____
  %53 = add i64 %52, 1
  %54 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %53)
  %55 = bitcast i8* %54 to %Qubit**
  %56 = load %Qubit*, %Qubit** %55, align 8
  %57 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %temps, i64 %__qsVar0____qsVar0__numPair____)
  %58 = bitcast i8* %57 to %Qubit**
  %59 = load %Qubit*, %Qubit** %58, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____adj(%Qubit* %51, %Qubit* %56, %Qubit* %59)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %60 = add i64 %__qsVar0____qsVar0__numPair____, %42
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %temps, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %28, i32 -1)
  call void @__quantum__rt__qubit_release_array(%Array* %temps)
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__H__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__H__body(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__H__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__H__adj(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__H__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__H__ctl(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__H__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__H__ctladj(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__H__ctladj(%Array* %__controlQubits__, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  call void @Microsoft__Quantum__Intrinsic__H__ctl(%Array* %__controlQubits__, %Qubit* %qubit)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  ret void
}

declare %Result* @__quantum__qis__m__body(%Qubit*)

define internal void @Microsoft__Quantum__Intrinsic__R__body(i2 %pauli, double %theta, %Qubit* %qubit) {
entry:
  %0 = load i2, i2* @PauliX, align 1
  %1 = icmp eq i2 %pauli, %0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic__Rx__body(double %theta, %Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = load i2, i2* @PauliY, align 1
  %3 = icmp eq i2 %pauli, %2
  br i1 %3, label %then1__1, label %test2__1

then1__1:                                         ; preds = %test1__1
  call void @Microsoft__Quantum__Intrinsic__Ry__body(double %theta, %Qubit* %qubit)
  br label %continue__1

test2__1:                                         ; preds = %test1__1
  %4 = load i2, i2* @PauliZ, align 1
  %5 = icmp eq i2 %pauli, %4
  br i1 %5, label %then2__1, label %else__1

then2__1:                                         ; preds = %test2__1
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %theta, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test2__1
  %6 = fneg double %theta
  %7 = fdiv double %6, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyGlobalPhase____body(double %7)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then2__1, %then1__1, %then0__1
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Ry__body(double %theta, %Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledRy____body(double %theta, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R__adj(i2 %pauli, double %theta, %Qubit* %qubit) {
entry:
  %0 = load i2, i2* @PauliX, align 1
  %1 = icmp eq i2 %pauli, %0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic__Rx__adj(double %theta, %Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = load i2, i2* @PauliY, align 1
  %3 = icmp eq i2 %pauli, %2
  br i1 %3, label %then1__1, label %test2__1

then1__1:                                         ; preds = %test1__1
  call void @Microsoft__Quantum__Intrinsic__Ry__adj(double %theta, %Qubit* %qubit)
  br label %continue__1

test2__1:                                         ; preds = %test1__1
  %4 = load i2, i2* @PauliZ, align 1
  %5 = icmp eq i2 %pauli, %4
  br i1 %5, label %then2__1, label %else__1

then2__1:                                         ; preds = %test2__1
  call void @Microsoft__Quantum__Intrinsic__Rz__adj(double %theta, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test2__1
  %6 = fneg double %theta
  %7 = fdiv double %6, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyGlobalPhase____adj(double %7)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then2__1, %then1__1, %then0__1
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rx__adj(double %theta, %Qubit* %qubit) {
entry:
  %0 = fneg double %theta
  call void @Microsoft__Quantum__Intrinsic__Rx__body(double %0, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Ry__adj(double %theta, %Qubit* %qubit) {
entry:
  %0 = fneg double %theta
  call void @Microsoft__Quantum__Intrinsic__Ry__body(double %0, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rz__adj(double %theta, %Qubit* %qubit) {
entry:
  %0 = fneg double %theta
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %0, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R__ctl(%Array* %__controlQubits__, { i2, double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %0, i32 0, i32 0
  %pauli = load i2, i2* %1, align 1
  %2 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %0, i32 0, i32 1
  %theta = load double, double* %2, align 8
  %3 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %0, i32 0, i32 2
  %qubit = load %Qubit*, %Qubit** %3, align 8
  %4 = load i2, i2* @PauliX, align 1
  %5 = icmp eq i2 %pauli, %4
  br i1 %5, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  %6 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %7 = bitcast %Tuple* %6 to { double, %Qubit* }*
  %8 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %7, i32 0, i32 0
  %9 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %7, i32 0, i32 1
  store double %theta, double* %8, align 8
  store %Qubit* %qubit, %Qubit** %9, align 8
  call void @Microsoft__Quantum__Intrinsic__Rx__ctl(%Array* %__controlQubits__, { double, %Qubit* }* %7)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %6, i32 -1)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %10 = load i2, i2* @PauliY, align 1
  %11 = icmp eq i2 %pauli, %10
  br i1 %11, label %then1__1, label %test2__1

then1__1:                                         ; preds = %test1__1
  %12 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %13 = bitcast %Tuple* %12 to { double, %Qubit* }*
  %14 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %13, i32 0, i32 0
  %15 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %13, i32 0, i32 1
  store double %theta, double* %14, align 8
  store %Qubit* %qubit, %Qubit** %15, align 8
  call void @Microsoft__Quantum__Intrinsic__Ry__ctl(%Array* %__controlQubits__, { double, %Qubit* }* %13)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %12, i32 -1)
  br label %continue__1

test2__1:                                         ; preds = %test1__1
  %16 = load i2, i2* @PauliZ, align 1
  %17 = icmp eq i2 %pauli, %16
  br i1 %17, label %then2__1, label %else__1

then2__1:                                         ; preds = %test2__1
  %18 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %19 = bitcast %Tuple* %18 to { double, %Qubit* }*
  %20 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %19, i32 0, i32 0
  %21 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %19, i32 0, i32 1
  store double %theta, double* %20, align 8
  store %Qubit* %qubit, %Qubit** %21, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__ctl(%Array* %__controlQubits__, { double, %Qubit* }* %19)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %18, i32 -1)
  br label %continue__1

else__1:                                          ; preds = %test2__1
  %22 = fneg double %theta
  %23 = fdiv double %22, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyGlobalPhase____ctl(%Array* %__controlQubits__, double %23)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then2__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rx__ctl(%Array* %ctls, { double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %theta = load double, double* %1, align 8
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %qubit = load %Qubit*, %Qubit** %2, align 8
  %3 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %4 = icmp eq i64 %3, 0
  br i1 %4, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledRx____body(double %theta, %Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %5 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %6 = icmp eq i64 %5, 1
  br i1 %6, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  %7 = load i2, i2* @PauliZ, align 1
  %8 = load i2, i2* @PauliX, align 1
  call void @Microsoft__Quantum__Intrinsic____QsRef36__MapPauli____body(%Qubit* %qubit, i2 %7, i2 %8)
  %9 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %10 = bitcast %Tuple* %9 to { double, %Qubit* }*
  %11 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %10, i32 0, i32 0
  %12 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %10, i32 0, i32 1
  store double %theta, double* %11, align 8
  store %Qubit* %qubit, %Qubit** %12, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__ctl(%Array* %ctls, { double, %Qubit* }* %10)
  %13 = load i2, i2* @PauliZ, align 1
  %14 = load i2, i2* @PauliX, align 1
  call void @Microsoft__Quantum__Intrinsic____QsRef36__MapPauli____adj(%Qubit* %qubit, i2 %13, i2 %14)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %9, i32 -1)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %15 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__Rx__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %15)
  %16 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, { double, %Qubit* }* }* getelementptr ({ %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* null, i32 1) to i64))
  %17 = bitcast %Tuple* %16 to { %Array*, { double, %Qubit* }* }*
  %18 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %17, i32 0, i32 0
  %19 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %17, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  %20 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %21 = bitcast %Tuple* %20 to { double, %Qubit* }*
  %22 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %21, i32 0, i32 0
  %23 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %21, i32 0, i32 1
  store double %theta, double* %22, align 8
  store %Qubit* %qubit, %Qubit** %23, align 8
  store %Array* %ctls, %Array** %18, align 8
  store { double, %Qubit* }* %21, { double, %Qubit* }** %19, align 8
  call void @Microsoft__Quantum__Intrinsic___b748ebe27fd44b9880ed4a2ba7b23b25___QsRef36__ApplyWithLessControlsA____body(%Callable* %15, { %Array*, { double, %Qubit* }* }* %17)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %15, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %15, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %20, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %16, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Ry__ctl(%Array* %ctls, { double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %theta = load double, double* %1, align 8
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %qubit = load %Qubit*, %Qubit** %2, align 8
  %3 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %4 = icmp eq i64 %3, 0
  br i1 %4, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledRy____body(double %theta, %Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %5 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %6 = icmp eq i64 %5, 1
  br i1 %6, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  %7 = load i2, i2* @PauliZ, align 1
  %8 = load i2, i2* @PauliY, align 1
  call void @Microsoft__Quantum__Intrinsic____QsRef36__MapPauli____body(%Qubit* %qubit, i2 %7, i2 %8)
  %9 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %10 = bitcast %Tuple* %9 to { double, %Qubit* }*
  %11 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %10, i32 0, i32 0
  %12 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %10, i32 0, i32 1
  store double %theta, double* %11, align 8
  store %Qubit* %qubit, %Qubit** %12, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__ctl(%Array* %ctls, { double, %Qubit* }* %10)
  %13 = load i2, i2* @PauliZ, align 1
  %14 = load i2, i2* @PauliY, align 1
  call void @Microsoft__Quantum__Intrinsic____QsRef36__MapPauli____adj(%Qubit* %qubit, i2 %13, i2 %14)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %9, i32 -1)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %15 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__Ry__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %15)
  %16 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, { double, %Qubit* }* }* getelementptr ({ %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* null, i32 1) to i64))
  %17 = bitcast %Tuple* %16 to { %Array*, { double, %Qubit* }* }*
  %18 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %17, i32 0, i32 0
  %19 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %17, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  %20 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %21 = bitcast %Tuple* %20 to { double, %Qubit* }*
  %22 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %21, i32 0, i32 0
  %23 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %21, i32 0, i32 1
  store double %theta, double* %22, align 8
  store %Qubit* %qubit, %Qubit** %23, align 8
  store %Array* %ctls, %Array** %18, align 8
  store { double, %Qubit* }* %21, { double, %Qubit* }** %19, align 8
  call void @Microsoft__Quantum__Intrinsic___b748ebe27fd44b9880ed4a2ba7b23b25___QsRef36__ApplyWithLessControlsA____body(%Callable* %15, { %Array*, { double, %Qubit* }* }* %17)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %15, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %15, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %20, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %16, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rz__ctl(%Array* %ctls, { double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %theta = load double, double* %1, align 8
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %qubit = load %Qubit*, %Qubit** %2, align 8
  %3 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %4 = icmp eq i64 %3, 0
  br i1 %4, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %theta, %Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %5 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %6 = icmp eq i64 %5, 1
  br i1 %6, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  %7 = fdiv double %theta, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %7, %Qubit* %qubit)
  %8 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %9 = bitcast i8* %8 to %Qubit**
  %10 = load %Qubit*, %Qubit** %9, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %10, %Qubit* %qubit)
  %11 = fneg double %theta
  %12 = fdiv double %11, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %12, %Qubit* %qubit)
  %13 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %14 = bitcast i8* %13 to %Qubit**
  %15 = load %Qubit*, %Qubit** %14, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %15, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %16 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__Rz__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %16)
  %17 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, { double, %Qubit* }* }* getelementptr ({ %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* null, i32 1) to i64))
  %18 = bitcast %Tuple* %17 to { %Array*, { double, %Qubit* }* }*
  %19 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %18, i32 0, i32 0
  %20 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %18, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  %21 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %22 = bitcast %Tuple* %21 to { double, %Qubit* }*
  %23 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %22, i32 0, i32 0
  %24 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %22, i32 0, i32 1
  store double %theta, double* %23, align 8
  store %Qubit* %qubit, %Qubit** %24, align 8
  store %Array* %ctls, %Array** %19, align 8
  store { double, %Qubit* }* %22, { double, %Qubit* }** %20, align 8
  call void @Microsoft__Quantum__Intrinsic___b748ebe27fd44b9880ed4a2ba7b23b25___QsRef36__ApplyWithLessControlsA____body(%Callable* %16, { %Array*, { double, %Qubit* }* }* %18)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %16, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %16, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %21, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %17, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R__ctladj(%Array* %__controlQubits__, { i2, double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %0, i32 0, i32 0
  %pauli = load i2, i2* %1, align 1
  %2 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %0, i32 0, i32 1
  %theta = load double, double* %2, align 8
  %3 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %0, i32 0, i32 2
  %qubit = load %Qubit*, %Qubit** %3, align 8
  %4 = load i2, i2* @PauliX, align 1
  %5 = icmp eq i2 %pauli, %4
  br i1 %5, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  %6 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %7 = bitcast %Tuple* %6 to { double, %Qubit* }*
  %8 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %7, i32 0, i32 0
  %9 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %7, i32 0, i32 1
  store double %theta, double* %8, align 8
  store %Qubit* %qubit, %Qubit** %9, align 8
  call void @Microsoft__Quantum__Intrinsic__Rx__ctladj(%Array* %__controlQubits__, { double, %Qubit* }* %7)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %6, i32 -1)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %10 = load i2, i2* @PauliY, align 1
  %11 = icmp eq i2 %pauli, %10
  br i1 %11, label %then1__1, label %test2__1

then1__1:                                         ; preds = %test1__1
  %12 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %13 = bitcast %Tuple* %12 to { double, %Qubit* }*
  %14 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %13, i32 0, i32 0
  %15 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %13, i32 0, i32 1
  store double %theta, double* %14, align 8
  store %Qubit* %qubit, %Qubit** %15, align 8
  call void @Microsoft__Quantum__Intrinsic__Ry__ctladj(%Array* %__controlQubits__, { double, %Qubit* }* %13)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %12, i32 -1)
  br label %continue__1

test2__1:                                         ; preds = %test1__1
  %16 = load i2, i2* @PauliZ, align 1
  %17 = icmp eq i2 %pauli, %16
  br i1 %17, label %then2__1, label %else__1

then2__1:                                         ; preds = %test2__1
  %18 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %19 = bitcast %Tuple* %18 to { double, %Qubit* }*
  %20 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %19, i32 0, i32 0
  %21 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %19, i32 0, i32 1
  store double %theta, double* %20, align 8
  store %Qubit* %qubit, %Qubit** %21, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__ctladj(%Array* %__controlQubits__, { double, %Qubit* }* %19)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %18, i32 -1)
  br label %continue__1

else__1:                                          ; preds = %test2__1
  %22 = fneg double %theta
  %23 = fdiv double %22, 2.000000e+00
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyGlobalPhase____ctladj(%Array* %__controlQubits__, double %23)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then2__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rx__ctladj(%Array* %__controlQubits__, { double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %theta = load double, double* %1, align 8
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %qubit = load %Qubit*, %Qubit** %2, align 8
  %3 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %4 = bitcast %Tuple* %3 to { double, %Qubit* }*
  %5 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %4, i32 0, i32 0
  %6 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %4, i32 0, i32 1
  %7 = fneg double %theta
  store double %7, double* %5, align 8
  store %Qubit* %qubit, %Qubit** %6, align 8
  call void @Microsoft__Quantum__Intrinsic__Rx__ctl(%Array* %__controlQubits__, { double, %Qubit* }* %4)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Ry__ctladj(%Array* %__controlQubits__, { double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %theta = load double, double* %1, align 8
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %qubit = load %Qubit*, %Qubit** %2, align 8
  %3 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %4 = bitcast %Tuple* %3 to { double, %Qubit* }*
  %5 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %4, i32 0, i32 0
  %6 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %4, i32 0, i32 1
  %7 = fneg double %theta
  store double %7, double* %5, align 8
  store %Qubit* %qubit, %Qubit** %6, align 8
  call void @Microsoft__Quantum__Intrinsic__Ry__ctl(%Array* %__controlQubits__, { double, %Qubit* }* %4)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rz__ctladj(%Array* %__controlQubits__, { double, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %theta = load double, double* %1, align 8
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %qubit = load %Qubit*, %Qubit** %2, align 8
  %3 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ double, %Qubit* }* getelementptr ({ double, %Qubit* }, { double, %Qubit* }* null, i32 1) to i64))
  %4 = bitcast %Tuple* %3 to { double, %Qubit* }*
  %5 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %4, i32 0, i32 0
  %6 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %4, i32 0, i32 1
  %7 = fneg double %theta
  store double %7, double* %5, align 8
  store %Qubit* %qubit, %Qubit** %6, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__ctl(%Array* %__controlQubits__, { double, %Qubit* }* %4)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R1__body(double %theta, %Qubit* %qubit) {
entry:
  %0 = load i2, i2* @PauliZ, align 1
  call void @Microsoft__Quantum__Intrinsic__R__body(i2 %0, double %theta, %Qubit* %qubit)
  %1 = load i2, i2* @PauliI, align 1
  %2 = fneg double %theta
  call void @Microsoft__Quantum__Intrinsic__R__body(i2 %1, double %2, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R1__adj(double %theta, %Qubit* %qubit) {
entry:
  %0 = load i2, i2* @PauliI, align 1
  %1 = fneg double %theta
  call void @Microsoft__Quantum__Intrinsic__R__adj(i2 %0, double %1, %Qubit* %qubit)
  %2 = load i2, i2* @PauliZ, align 1
  call void @Microsoft__Quantum__Intrinsic__R__adj(i2 %2, double %theta, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R1Frac__body(i64 %numerator, i64 %power, %Qubit* %qubit) {
entry:
  %0 = load i2, i2* @PauliZ, align 1
  %1 = sub i64 0, %numerator
  %2 = add i64 %power, 1
  call void @Microsoft__Quantum__Intrinsic__RFrac__body(i2 %0, i64 %1, i64 %2, %Qubit* %qubit)
  %3 = load i2, i2* @PauliI, align 1
  %4 = add i64 %power, 1
  call void @Microsoft__Quantum__Intrinsic__RFrac__body(i2 %3, i64 %numerator, i64 %4, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__RFrac__body(i2 %pauli, i64 %numerator, i64 %power, %Qubit* %qubit) {
entry:
  %0 = call double @Microsoft__Quantum__Math__PI__body()
  %1 = fmul double -2.000000e+00, %0
  %2 = sitofp i64 %numerator to double
  %3 = fmul double %1, %2
  %4 = sitofp i64 %power to double
  %5 = call double @llvm.pow.f64(double 2.000000e+00, double %4)
  %angle = fdiv double %3, %5
  call void @Microsoft__Quantum__Intrinsic__R__body(i2 %pauli, double %angle, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R1Frac__adj(i64 %numerator, i64 %power, %Qubit* %qubit) {
entry:
  %0 = load i2, i2* @PauliI, align 1
  %1 = add i64 %power, 1
  call void @Microsoft__Quantum__Intrinsic__RFrac__adj(i2 %0, i64 %numerator, i64 %1, %Qubit* %qubit)
  %2 = load i2, i2* @PauliZ, align 1
  %3 = sub i64 0, %numerator
  %4 = add i64 %power, 1
  call void @Microsoft__Quantum__Intrinsic__RFrac__adj(i2 %2, i64 %3, i64 %4, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__RFrac__adj(i2 %pauli, i64 %numerator, i64 %power, %Qubit* %qubit) {
entry:
  %0 = call double @Microsoft__Quantum__Math__PI__body()
  %1 = fmul double -2.000000e+00, %0
  %2 = sitofp i64 %numerator to double
  %3 = fmul double %1, %2
  %4 = sitofp i64 %power to double
  %5 = call double @llvm.pow.f64(double 2.000000e+00, double %4)
  %__qsVar0__angle__ = fdiv double %3, %5
  call void @Microsoft__Quantum__Intrinsic__R__adj(i2 %pauli, double %__qsVar0__angle__, %Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R1Frac__ctl(%Array* %__controlQubits__, { i64, i64, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { i64, i64, %Qubit* }, { i64, i64, %Qubit* }* %0, i32 0, i32 0
  %numerator = load i64, i64* %1, align 4
  %2 = getelementptr inbounds { i64, i64, %Qubit* }, { i64, i64, %Qubit* }* %0, i32 0, i32 1
  %power = load i64, i64* %2, align 4
  %3 = getelementptr inbounds { i64, i64, %Qubit* }, { i64, i64, %Qubit* }* %0, i32 0, i32 2
  %qubit = load %Qubit*, %Qubit** %3, align 8
  %4 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, i64, i64, %Qubit* }* getelementptr ({ i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* null, i32 1) to i64))
  %5 = bitcast %Tuple* %4 to { i2, i64, i64, %Qubit* }*
  %6 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %5, i32 0, i32 0
  %7 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %5, i32 0, i32 1
  %8 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %5, i32 0, i32 2
  %9 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %5, i32 0, i32 3
  %10 = load i2, i2* @PauliZ, align 1
  %11 = sub i64 0, %numerator
  %12 = add i64 %power, 1
  store i2 %10, i2* %6, align 1
  store i64 %11, i64* %7, align 4
  store i64 %12, i64* %8, align 4
  store %Qubit* %qubit, %Qubit** %9, align 8
  call void @Microsoft__Quantum__Intrinsic__RFrac__ctl(%Array* %__controlQubits__, { i2, i64, i64, %Qubit* }* %5)
  %13 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, i64, i64, %Qubit* }* getelementptr ({ i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* null, i32 1) to i64))
  %14 = bitcast %Tuple* %13 to { i2, i64, i64, %Qubit* }*
  %15 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %14, i32 0, i32 0
  %16 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %14, i32 0, i32 1
  %17 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %14, i32 0, i32 2
  %18 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %14, i32 0, i32 3
  %19 = load i2, i2* @PauliI, align 1
  %20 = add i64 %power, 1
  store i2 %19, i2* %15, align 1
  store i64 %numerator, i64* %16, align 4
  store i64 %20, i64* %17, align 4
  store %Qubit* %qubit, %Qubit** %18, align 8
  call void @Microsoft__Quantum__Intrinsic__RFrac__ctl(%Array* %__controlQubits__, { i2, i64, i64, %Qubit* }* %14)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %4, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %13, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__RFrac__ctl(%Array* %__controlQubits__, { i2, i64, i64, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %0, i32 0, i32 0
  %pauli = load i2, i2* %1, align 1
  %2 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %0, i32 0, i32 1
  %numerator = load i64, i64* %2, align 4
  %3 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %0, i32 0, i32 2
  %power = load i64, i64* %3, align 4
  %4 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %0, i32 0, i32 3
  %qubit = load %Qubit*, %Qubit** %4, align 8
  %5 = call double @Microsoft__Quantum__Math__PI__body()
  %6 = fmul double -2.000000e+00, %5
  %7 = sitofp i64 %numerator to double
  %8 = fmul double %6, %7
  %9 = sitofp i64 %power to double
  %10 = call double @llvm.pow.f64(double 2.000000e+00, double %9)
  %angle = fdiv double %8, %10
  %11 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, double, %Qubit* }* getelementptr ({ i2, double, %Qubit* }, { i2, double, %Qubit* }* null, i32 1) to i64))
  %12 = bitcast %Tuple* %11 to { i2, double, %Qubit* }*
  %13 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %12, i32 0, i32 0
  %14 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %12, i32 0, i32 1
  %15 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %12, i32 0, i32 2
  store i2 %pauli, i2* %13, align 1
  store double %angle, double* %14, align 8
  store %Qubit* %qubit, %Qubit** %15, align 8
  call void @Microsoft__Quantum__Intrinsic__R__ctl(%Array* %__controlQubits__, { i2, double, %Qubit* }* %12)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %11, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__R1Frac__ctladj(%Array* %__controlQubits__, { i64, i64, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { i64, i64, %Qubit* }, { i64, i64, %Qubit* }* %0, i32 0, i32 0
  %numerator = load i64, i64* %1, align 4
  %2 = getelementptr inbounds { i64, i64, %Qubit* }, { i64, i64, %Qubit* }* %0, i32 0, i32 1
  %power = load i64, i64* %2, align 4
  %3 = getelementptr inbounds { i64, i64, %Qubit* }, { i64, i64, %Qubit* }* %0, i32 0, i32 2
  %qubit = load %Qubit*, %Qubit** %3, align 8
  %4 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, i64, i64, %Qubit* }* getelementptr ({ i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* null, i32 1) to i64))
  %5 = bitcast %Tuple* %4 to { i2, i64, i64, %Qubit* }*
  %6 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %5, i32 0, i32 0
  %7 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %5, i32 0, i32 1
  %8 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %5, i32 0, i32 2
  %9 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %5, i32 0, i32 3
  %10 = load i2, i2* @PauliI, align 1
  %11 = add i64 %power, 1
  store i2 %10, i2* %6, align 1
  store i64 %numerator, i64* %7, align 4
  store i64 %11, i64* %8, align 4
  store %Qubit* %qubit, %Qubit** %9, align 8
  call void @Microsoft__Quantum__Intrinsic__RFrac__ctladj(%Array* %__controlQubits__, { i2, i64, i64, %Qubit* }* %5)
  %12 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, i64, i64, %Qubit* }* getelementptr ({ i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* null, i32 1) to i64))
  %13 = bitcast %Tuple* %12 to { i2, i64, i64, %Qubit* }*
  %14 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %13, i32 0, i32 0
  %15 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %13, i32 0, i32 1
  %16 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %13, i32 0, i32 2
  %17 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %13, i32 0, i32 3
  %18 = load i2, i2* @PauliZ, align 1
  %19 = sub i64 0, %numerator
  %20 = add i64 %power, 1
  store i2 %18, i2* %14, align 1
  store i64 %19, i64* %15, align 4
  store i64 %20, i64* %16, align 4
  store %Qubit* %qubit, %Qubit** %17, align 8
  call void @Microsoft__Quantum__Intrinsic__RFrac__ctladj(%Array* %__controlQubits__, { i2, i64, i64, %Qubit* }* %13)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %4, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %12, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__RFrac__ctladj(%Array* %__controlQubits__, { i2, i64, i64, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  %1 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %0, i32 0, i32 0
  %pauli = load i2, i2* %1, align 1
  %2 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %0, i32 0, i32 1
  %numerator = load i64, i64* %2, align 4
  %3 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %0, i32 0, i32 2
  %power = load i64, i64* %3, align 4
  %4 = getelementptr inbounds { i2, i64, i64, %Qubit* }, { i2, i64, i64, %Qubit* }* %0, i32 0, i32 3
  %qubit = load %Qubit*, %Qubit** %4, align 8
  %5 = call double @Microsoft__Quantum__Math__PI__body()
  %6 = fmul double -2.000000e+00, %5
  %7 = sitofp i64 %numerator to double
  %8 = fmul double %6, %7
  %9 = sitofp i64 %power to double
  %10 = call double @llvm.pow.f64(double 2.000000e+00, double %9)
  %__qsVar0__angle__ = fdiv double %8, %10
  %11 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ i2, double, %Qubit* }* getelementptr ({ i2, double, %Qubit* }, { i2, double, %Qubit* }* null, i32 1) to i64))
  %12 = bitcast %Tuple* %11 to { i2, double, %Qubit* }*
  %13 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %12, i32 0, i32 0
  %14 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %12, i32 0, i32 1
  %15 = getelementptr inbounds { i2, double, %Qubit* }, { i2, double, %Qubit* }* %12, i32 0, i32 2
  store i2 %pauli, i2* %13, align 1
  store double %__qsVar0__angle__, double* %14, align 8
  store %Qubit* %qubit, %Qubit** %15, align 8
  call void @Microsoft__Quantum__Intrinsic__R__ctladj(%Array* %__controlQubits__, { i2, double, %Qubit* }* %12)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %11, i32 -1)
  ret void
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.pow.f64(double, double) #0

define internal void @Microsoft__Quantum__Intrinsic___b748ebe27fd44b9880ed4a2ba7b23b25___QsRef36__ApplyWithLessControlsA____body(%Callable* %op, { %Array*, { double, %Qubit* }* }* %0) {
entry:
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 1)
  %1 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 0
  %controls = load %Array*, %Array** %1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 1)
  %2 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 1
  %arg = load { double, %Qubit* }*, { double, %Qubit* }** %2, align 8
  %3 = bitcast { double, %Qubit* }* %arg to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %3, i32 1)
  %numControls = call i64 @__quantum__rt__array_get_size_1d(%Array* %controls)
  %numControlPairs = sdiv i64 %numControls, 2
  %temps = call %Array* @__quantum__rt__qubit_allocate_array(i64 %numControlPairs)
  call void @__quantum__rt__array_update_alias_count(%Array* %temps, i32 1)
  %4 = sub i64 %numControlPairs, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %__qsVar0__numPair__ = phi i64 [ 0, %entry ], [ %18, %exiting__1 ]
  %5 = icmp sle i64 %__qsVar0__numPair__, %4
  br i1 %5, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %6 = mul i64 2, %__qsVar0__numPair__
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %6)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  %10 = mul i64 2, %__qsVar0__numPair__
  %11 = add i64 %10, 1
  %12 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %11)
  %13 = bitcast i8* %12 to %Qubit**
  %14 = load %Qubit*, %Qubit** %13, align 8
  %15 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %temps, i64 %__qsVar0__numPair__)
  %16 = bitcast i8* %15 to %Qubit**
  %17 = load %Qubit*, %Qubit** %16, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____body(%Qubit* %9, %Qubit* %14, %Qubit* %17)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %18 = add i64 %__qsVar0__numPair__, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %19 = srem i64 %numControls, 2
  %20 = icmp eq i64 %19, 0
  br i1 %20, label %condTrue__1, label %condFalse__1

condTrue__1:                                      ; preds = %exit__1
  call void @__quantum__rt__array_update_reference_count(%Array* %temps, i32 1)
  br label %condContinue__1

condFalse__1:                                     ; preds = %exit__1
  %21 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %22 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %21, i64 0)
  %23 = bitcast i8* %22 to %Qubit**
  %24 = sub i64 %numControls, 1
  %25 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %24)
  %26 = bitcast i8* %25 to %Qubit**
  %27 = load %Qubit*, %Qubit** %26, align 8
  store %Qubit* %27, %Qubit** %23, align 8
  %28 = call %Array* @__quantum__rt__array_concatenate(%Array* %temps, %Array* %21)
  call void @__quantum__rt__array_update_reference_count(%Array* %28, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %21, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %28, i32 -1)
  br label %condContinue__1

condContinue__1:                                  ; preds = %condFalse__1, %condTrue__1
  %__qsVar1__newControls__ = phi %Array* [ %temps, %condTrue__1 ], [ %28, %condFalse__1 ]
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1__newControls__, i32 1)
  %29 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, { double, %Qubit* }* }* getelementptr ({ %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* null, i32 1) to i64))
  %30 = bitcast %Tuple* %29 to { %Array*, { double, %Qubit* }* }*
  %31 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %30, i32 0, i32 0
  %32 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %30, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 1)
  store %Array* %__qsVar1__newControls__, %Array** %31, align 8
  store { double, %Qubit* }* %arg, { double, %Qubit* }** %32, align 8
  call void @__quantum__rt__callable_invoke(%Callable* %op, %Tuple* %29, %Tuple* null)
  %33 = sub i64 %numControlPairs, 1
  %34 = sub i64 %33, 0
  %35 = sdiv i64 %34, 1
  %36 = mul i64 1, %35
  %37 = add i64 0, %36
  %38 = load %Range, %Range* @EmptyRange, align 4
  %39 = insertvalue %Range %38, i64 %37, 0
  %40 = insertvalue %Range %39, i64 -1, 1
  %41 = insertvalue %Range %40, i64 0, 2
  %42 = extractvalue %Range %41, 0
  %43 = extractvalue %Range %41, 1
  %44 = extractvalue %Range %41, 2
  br label %preheader__1

preheader__1:                                     ; preds = %condContinue__1
  %45 = icmp sgt i64 %43, 0
  br label %header__2

header__2:                                        ; preds = %exiting__2, %preheader__1
  %__qsVar0____qsVar0__numPair____ = phi i64 [ %42, %preheader__1 ], [ %61, %exiting__2 ]
  %46 = icmp sle i64 %__qsVar0____qsVar0__numPair____, %44
  %47 = icmp sge i64 %__qsVar0____qsVar0__numPair____, %44
  %48 = select i1 %45, i1 %46, i1 %47
  br i1 %48, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %49 = mul i64 2, %__qsVar0____qsVar0__numPair____
  %50 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %49)
  %51 = bitcast i8* %50 to %Qubit**
  %52 = load %Qubit*, %Qubit** %51, align 8
  %53 = mul i64 2, %__qsVar0____qsVar0__numPair____
  %54 = add i64 %53, 1
  %55 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %54)
  %56 = bitcast i8* %55 to %Qubit**
  %57 = load %Qubit*, %Qubit** %56, align 8
  %58 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %temps, i64 %__qsVar0____qsVar0__numPair____)
  %59 = bitcast i8* %58 to %Qubit**
  %60 = load %Qubit*, %Qubit** %59, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____adj(%Qubit* %52, %Qubit* %57, %Qubit* %60)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %61 = add i64 %__qsVar0____qsVar0__numPair____, %43
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %temps, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1__newControls__, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %29, i32 -1)
  call void @__quantum__rt__qubit_release_array(%Array* %temps)
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %3, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rx__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { double, %Qubit* }*
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %3 = load double, double* %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Rx__body(double %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rx__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { double, %Qubit* }*
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %3 = load double, double* %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Rx__adj(double %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rx__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, { double, %Qubit* }* }*
  %1 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load { double, %Qubit* }*, { double, %Qubit* }** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Rx__ctl(%Array* %3, { double, %Qubit* }* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rx__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, { double, %Qubit* }* }*
  %1 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load { double, %Qubit* }*, { double, %Qubit* }** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Rx__ctladj(%Array* %3, { double, %Qubit* }* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Ry__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { double, %Qubit* }*
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %3 = load double, double* %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Ry__body(double %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Ry__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { double, %Qubit* }*
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %3 = load double, double* %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Ry__adj(double %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Ry__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, { double, %Qubit* }* }*
  %1 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load { double, %Qubit* }*, { double, %Qubit* }** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Ry__ctl(%Array* %3, { double, %Qubit* }* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Ry__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, { double, %Qubit* }* }*
  %1 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load { double, %Qubit* }*, { double, %Qubit* }** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Ry__ctladj(%Array* %3, { double, %Qubit* }* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rz__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { double, %Qubit* }*
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %3 = load double, double* %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__body(double %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rz__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { double, %Qubit* }*
  %1 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { double, %Qubit* }, { double, %Qubit* }* %0, i32 0, i32 1
  %3 = load double, double* %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__adj(double %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rz__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, { double, %Qubit* }* }*
  %1 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load { double, %Qubit* }*, { double, %Qubit* }** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__ctl(%Array* %3, { double, %Qubit* }* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Rz__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, { double, %Qubit* }* }*
  %1 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load { double, %Qubit* }*, { double, %Qubit* }** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Rz__ctladj(%Array* %3, { double, %Qubit* }* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__S__ctl(%Array* %ctls, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %1 = icmp eq i64 %0, 0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledS____body(%Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %3 = icmp eq i64 %2, 1
  br i1 %3, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  %4 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %5 = bitcast i8* %4 to %Qubit**
  %6 = load %Qubit*, %Qubit** %5, align 8
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %6)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %qubit)
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %9, %Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %qubit)
  %10 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %11 = bitcast i8* %10 to %Qubit**
  %12 = load %Qubit*, %Qubit** %11, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %12, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %13 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__S__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %13)
  %14 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %15 = bitcast %Tuple* %14 to { %Array*, %Qubit* }*
  %16 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %15, i32 0, i32 0
  %17 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %15, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  store %Array* %ctls, %Array** %16, align 8
  store %Qubit* %qubit, %Qubit** %17, align 8
  call void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____body(%Callable* %13, { %Array*, %Qubit* }* %15)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %13, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %13, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %14, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__S__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__S__body(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__S__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__S__adj(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__S__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__S__ctl(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__S__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__S__ctladj(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__S__ctladj(%Array* %ctls, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %1 = icmp eq i64 %0, 0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledSAdj____body(%Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %3 = icmp eq i64 %2, 1
  br i1 %3, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  %4 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %5 = bitcast i8* %4 to %Qubit**
  %6 = load %Qubit*, %Qubit** %5, align 8
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %6)
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %qubit)
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %9, %Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %qubit)
  %10 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %11 = bitcast i8* %10 to %Qubit**
  %12 = load %Qubit*, %Qubit** %11, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %12, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %13 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__S__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_adjoint(%Callable* %13)
  call void @__quantum__rt__callable_make_controlled(%Callable* %13)
  %14 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %15 = bitcast %Tuple* %14 to { %Array*, %Qubit* }*
  %16 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %15, i32 0, i32 0
  %17 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %15, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  store %Array* %ctls, %Array** %16, align 8
  store %Qubit* %qubit, %Qubit** %17, align 8
  call void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____body(%Callable* %13, { %Array*, %Qubit* }* %15)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %13, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %13, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %14, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

declare void @__quantum__rt__callable_make_adjoint(%Callable*)

define internal void @Microsoft__Quantum__Intrinsic__T__ctl(%Array* %ctls, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %1 = icmp eq i64 %0, 0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledT____body(%Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %3 = icmp eq i64 %2, 1
  br i1 %3, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  %4 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %5 = bitcast i8* %4 to %Qubit**
  %6 = load %Qubit*, %Qubit** %5, align 8
  call void @Microsoft__Quantum__Intrinsic__R1Frac__body(i64 1, i64 3, %Qubit* %6)
  call void @Microsoft__Quantum__Intrinsic__R1Frac__body(i64 1, i64 3, %Qubit* %qubit)
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %9, %Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__R1Frac__adj(i64 1, i64 3, %Qubit* %qubit)
  %10 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %11 = bitcast i8* %10 to %Qubit**
  %12 = load %Qubit*, %Qubit** %11, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %12, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %13 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__T__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %13)
  %14 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %15 = bitcast %Tuple* %14 to { %Array*, %Qubit* }*
  %16 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %15, i32 0, i32 0
  %17 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %15, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  store %Array* %ctls, %Array** %16, align 8
  store %Qubit* %qubit, %Qubit** %17, align 8
  call void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____body(%Callable* %13, { %Array*, %Qubit* }* %15)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %13, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %13, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %14, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__T__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__T__body(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__T__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__T__adj(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__T__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__T__ctl(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__T__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__T__ctladj(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__T__ctladj(%Array* %ctls, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %1 = icmp eq i64 %0, 0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledTAdj____body(%Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %3 = icmp eq i64 %2, 1
  br i1 %3, label %then1__1, label %else__1

then1__1:                                         ; preds = %test1__1
  %4 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %5 = bitcast i8* %4 to %Qubit**
  %6 = load %Qubit*, %Qubit** %5, align 8
  call void @Microsoft__Quantum__Intrinsic__R1Frac__adj(i64 1, i64 3, %Qubit* %6)
  call void @Microsoft__Quantum__Intrinsic__R1Frac__adj(i64 1, i64 3, %Qubit* %qubit)
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %9, %Qubit* %qubit)
  call void @Microsoft__Quantum__Intrinsic__R1Frac__body(i64 1, i64 3, %Qubit* %qubit)
  %10 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %11 = bitcast i8* %10 to %Qubit**
  %12 = load %Qubit*, %Qubit** %11, align 8
  call void @Microsoft__Quantum__Intrinsic__CNOT__body(%Qubit* %12, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test1__1
  %13 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__T__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_adjoint(%Callable* %13)
  call void @__quantum__rt__callable_make_controlled(%Callable* %13)
  %14 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %15 = bitcast %Tuple* %14 to { %Array*, %Qubit* }*
  %16 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %15, i32 0, i32 0
  %17 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %15, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  store %Array* %ctls, %Array** %16, align 8
  store %Qubit* %qubit, %Qubit** %17, align 8
  call void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____body(%Callable* %13, { %Array*, %Qubit* }* %15)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %13, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %13, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %14, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__X__adj(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic__X__body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__X__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__X__body(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__X__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__X__adj(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__X__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__X__ctl(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__X__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__X__ctladj(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__X__ctladj(%Array* %__controlQubits__, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  call void @Microsoft__Quantum__Intrinsic__X__ctl(%Array* %__controlQubits__, %Qubit* %qubit)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Z__adj(%Qubit* %qubit) {
entry:
  call void @Microsoft__Quantum__Intrinsic__Z__body(%Qubit* %qubit)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Z__ctl(%Array* %ctls, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 1)
  %0 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %1 = icmp eq i64 %0, 0
  br i1 %1, label %then0__1, label %test1__1

then0__1:                                         ; preds = %entry
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyUncontrolledZ____body(%Qubit* %qubit)
  br label %continue__1

test1__1:                                         ; preds = %entry
  %2 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %3 = icmp eq i64 %2, 1
  br i1 %3, label %then1__1, label %test2__1

then1__1:                                         ; preds = %test1__1
  %4 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %5 = bitcast i8* %4 to %Qubit**
  %6 = load %Qubit*, %Qubit** %5, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__ApplyControlledZ____body(%Qubit* %6, %Qubit* %qubit)
  br label %continue__1

test2__1:                                         ; preds = %test1__1
  %7 = call i64 @__quantum__rt__array_get_size_1d(%Array* %ctls)
  %8 = icmp eq i64 %7, 2
  br i1 %8, label %then2__1, label %else__1

then2__1:                                         ; preds = %test2__1
  %9 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 0)
  %10 = bitcast i8* %9 to %Qubit**
  %11 = load %Qubit*, %Qubit** %10, align 8
  %12 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %ctls, i64 1)
  %13 = bitcast i8* %12 to %Qubit**
  %14 = load %Qubit*, %Qubit** %13, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__CCZ____body(%Qubit* %11, %Qubit* %14, %Qubit* %qubit)
  br label %continue__1

else__1:                                          ; preds = %test2__1
  %15 = call %Callable* @__quantum__rt__callable_create([4 x void (%Tuple*, %Tuple*, %Tuple*)*]* @Microsoft__Quantum__Intrinsic__Z__FunctionTable, [2 x void (%Tuple*, i32)*]* null, %Tuple* null)
  call void @__quantum__rt__callable_make_controlled(%Callable* %15)
  %16 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %17 = bitcast %Tuple* %16 to { %Array*, %Qubit* }*
  %18 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %17, i32 0, i32 0
  %19 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %17, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 1)
  store %Array* %ctls, %Array** %18, align 8
  store %Qubit* %qubit, %Qubit** %19, align 8
  call void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____body(%Callable* %15, { %Array*, %Qubit* }* %17)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %15, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %15, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %ctls, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %16, i32 -1)
  br label %continue__1

continue__1:                                      ; preds = %else__1, %then2__1, %then1__1, %then0__1
  call void @__quantum__rt__array_update_alias_count(%Array* %ctls, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Z__body__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__Z__body(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Z__adj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Qubit* }*
  %1 = getelementptr inbounds { %Qubit* }, { %Qubit* }* %0, i32 0, i32 0
  %2 = load %Qubit*, %Qubit** %1, align 8
  call void @Microsoft__Quantum__Intrinsic__Z__adj(%Qubit* %2)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Z__ctl__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Z__ctl(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Z__ctladj__wrapper(%Tuple* %capture-tuple, %Tuple* %arg-tuple, %Tuple* %result-tuple) {
entry:
  %0 = bitcast %Tuple* %arg-tuple to { %Array*, %Qubit* }*
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %3 = load %Array*, %Array** %1, align 8
  %4 = load %Qubit*, %Qubit** %2, align 8
  call void @Microsoft__Quantum__Intrinsic__Z__ctladj(%Array* %3, %Qubit* %4)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic__Z__ctladj(%Array* %__controlQubits__, %Qubit* %qubit) {
entry:
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 1)
  call void @Microsoft__Quantum__Intrinsic__Z__ctl(%Array* %__controlQubits__, %Qubit* %qubit)
  call void @__quantum__rt__array_update_alias_count(%Array* %__controlQubits__, i32 -1)
  ret void
}

declare void @__quantum__rt__capture_update_alias_count(%Callable*, i32)

declare void @__quantum__rt__callable_update_alias_count(%Callable*, i32)

declare void @__quantum__rt__callable_invoke(%Callable*, %Tuple*, %Tuple*)

define internal void @Microsoft__Quantum__Intrinsic___4d8f155091f54a0bb1d9365f6790ec15___QsRef36__ApplyWithLessControlsA____adj(%Callable* %op, { %Array*, %Qubit* }* %0) {
entry:
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 1)
  %1 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 0
  %controls = load %Array*, %Array** %1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 1)
  %2 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %0, i32 0, i32 1
  %arg = load %Qubit*, %Qubit** %2, align 8
  %__qsVar0__numControls__ = call i64 @__quantum__rt__array_get_size_1d(%Array* %controls)
  %__qsVar1__numControlPairs__ = sdiv i64 %__qsVar0__numControls__, 2
  %__qsVar2__temps__ = call %Array* @__quantum__rt__qubit_allocate_array(i64 %__qsVar1__numControlPairs__)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar2__temps__, i32 1)
  %3 = sub i64 %__qsVar1__numControlPairs__, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %__qsVar0____qsVar3__numPair____ = phi i64 [ 0, %entry ], [ %17, %exiting__1 ]
  %4 = icmp sle i64 %__qsVar0____qsVar3__numPair____, %3
  br i1 %4, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %5 = mul i64 2, %__qsVar0____qsVar3__numPair____
  %6 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %5)
  %7 = bitcast i8* %6 to %Qubit**
  %8 = load %Qubit*, %Qubit** %7, align 8
  %9 = mul i64 2, %__qsVar0____qsVar3__numPair____
  %10 = add i64 %9, 1
  %11 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %10)
  %12 = bitcast i8* %11 to %Qubit**
  %13 = load %Qubit*, %Qubit** %12, align 8
  %14 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__qsVar2__temps__, i64 %__qsVar0____qsVar3__numPair____)
  %15 = bitcast i8* %14 to %Qubit**
  %16 = load %Qubit*, %Qubit** %15, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____body(%Qubit* %8, %Qubit* %13, %Qubit* %16)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %17 = add i64 %__qsVar0____qsVar3__numPair____, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %18 = srem i64 %__qsVar0__numControls__, 2
  %19 = icmp eq i64 %18, 0
  br i1 %19, label %condTrue__1, label %condFalse__1

condTrue__1:                                      ; preds = %exit__1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar2__temps__, i32 1)
  br label %condContinue__1

condFalse__1:                                     ; preds = %exit__1
  %20 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %21 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %20, i64 0)
  %22 = bitcast i8* %21 to %Qubit**
  %23 = sub i64 %__qsVar0__numControls__, 1
  %24 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %23)
  %25 = bitcast i8* %24 to %Qubit**
  %26 = load %Qubit*, %Qubit** %25, align 8
  store %Qubit* %26, %Qubit** %22, align 8
  %27 = call %Array* @__quantum__rt__array_concatenate(%Array* %__qsVar2__temps__, %Array* %20)
  call void @__quantum__rt__array_update_reference_count(%Array* %27, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %20, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %27, i32 -1)
  br label %condContinue__1

condContinue__1:                                  ; preds = %condFalse__1, %condTrue__1
  %__qsVar1____qsVar4__newControls____ = phi %Array* [ %__qsVar2__temps__, %condTrue__1 ], [ %27, %condFalse__1 ]
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1____qsVar4__newControls____, i32 1)
  %28 = call %Callable* @__quantum__rt__callable_copy(%Callable* %op, i1 false)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %28, i32 1)
  call void @__quantum__rt__callable_make_adjoint(%Callable* %28)
  %29 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, %Qubit* }* getelementptr ({ %Array*, %Qubit* }, { %Array*, %Qubit* }* null, i32 1) to i64))
  %30 = bitcast %Tuple* %29 to { %Array*, %Qubit* }*
  %31 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %30, i32 0, i32 0
  %32 = getelementptr inbounds { %Array*, %Qubit* }, { %Array*, %Qubit* }* %30, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 1)
  store %Array* %__qsVar1____qsVar4__newControls____, %Array** %31, align 8
  store %Qubit* %arg, %Qubit** %32, align 8
  call void @__quantum__rt__callable_invoke(%Callable* %28, %Tuple* %29, %Tuple* null)
  %33 = sub i64 %__qsVar1__numControlPairs__, 1
  %34 = sub i64 %33, 0
  %35 = sdiv i64 %34, 1
  %36 = mul i64 1, %35
  %37 = add i64 0, %36
  %38 = load %Range, %Range* @EmptyRange, align 4
  %39 = insertvalue %Range %38, i64 %37, 0
  %40 = insertvalue %Range %39, i64 -1, 1
  %41 = insertvalue %Range %40, i64 0, 2
  %42 = extractvalue %Range %41, 0
  %43 = extractvalue %Range %41, 1
  %44 = extractvalue %Range %41, 2
  br label %preheader__1

preheader__1:                                     ; preds = %condContinue__1
  %45 = icmp sgt i64 %43, 0
  br label %header__2

header__2:                                        ; preds = %exiting__2, %preheader__1
  %__qsVar0____qsVar0____qsVar3__numPair______ = phi i64 [ %42, %preheader__1 ], [ %61, %exiting__2 ]
  %46 = icmp sle i64 %__qsVar0____qsVar0____qsVar3__numPair______, %44
  %47 = icmp sge i64 %__qsVar0____qsVar0____qsVar3__numPair______, %44
  %48 = select i1 %45, i1 %46, i1 %47
  br i1 %48, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %49 = mul i64 2, %__qsVar0____qsVar0____qsVar3__numPair______
  %50 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %49)
  %51 = bitcast i8* %50 to %Qubit**
  %52 = load %Qubit*, %Qubit** %51, align 8
  %53 = mul i64 2, %__qsVar0____qsVar0____qsVar3__numPair______
  %54 = add i64 %53, 1
  %55 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %54)
  %56 = bitcast i8* %55 to %Qubit**
  %57 = load %Qubit*, %Qubit** %56, align 8
  %58 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__qsVar2__temps__, i64 %__qsVar0____qsVar0____qsVar3__numPair______)
  %59 = bitcast i8* %58 to %Qubit**
  %60 = load %Qubit*, %Qubit** %59, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____adj(%Qubit* %52, %Qubit* %57, %Qubit* %60)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %61 = add i64 %__qsVar0____qsVar0____qsVar3__numPair______, %43
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar2__temps__, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %28, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %28, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %29, i32 -1)
  call void @__quantum__rt__qubit_release_array(%Array* %__qsVar2__temps__)
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 -1)
  ret void
}

declare %Callable* @__quantum__rt__callable_copy(%Callable*, i1)

define internal void @Microsoft__Quantum__Intrinsic___b748ebe27fd44b9880ed4a2ba7b23b25___QsRef36__ApplyWithLessControlsA____adj(%Callable* %op, { %Array*, { double, %Qubit* }* }* %0) {
entry:
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 1)
  %1 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 0
  %controls = load %Array*, %Array** %1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 1)
  %2 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %0, i32 0, i32 1
  %arg = load { double, %Qubit* }*, { double, %Qubit* }** %2, align 8
  %3 = bitcast { double, %Qubit* }* %arg to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %3, i32 1)
  %__qsVar0__numControls__ = call i64 @__quantum__rt__array_get_size_1d(%Array* %controls)
  %__qsVar1__numControlPairs__ = sdiv i64 %__qsVar0__numControls__, 2
  %__qsVar2__temps__ = call %Array* @__quantum__rt__qubit_allocate_array(i64 %__qsVar1__numControlPairs__)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar2__temps__, i32 1)
  %4 = sub i64 %__qsVar1__numControlPairs__, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %__qsVar0____qsVar3__numPair____ = phi i64 [ 0, %entry ], [ %18, %exiting__1 ]
  %5 = icmp sle i64 %__qsVar0____qsVar3__numPair____, %4
  br i1 %5, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %6 = mul i64 2, %__qsVar0____qsVar3__numPair____
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %6)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  %10 = mul i64 2, %__qsVar0____qsVar3__numPair____
  %11 = add i64 %10, 1
  %12 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %11)
  %13 = bitcast i8* %12 to %Qubit**
  %14 = load %Qubit*, %Qubit** %13, align 8
  %15 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__qsVar2__temps__, i64 %__qsVar0____qsVar3__numPair____)
  %16 = bitcast i8* %15 to %Qubit**
  %17 = load %Qubit*, %Qubit** %16, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____body(%Qubit* %9, %Qubit* %14, %Qubit* %17)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %18 = add i64 %__qsVar0____qsVar3__numPair____, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %19 = srem i64 %__qsVar0__numControls__, 2
  %20 = icmp eq i64 %19, 0
  br i1 %20, label %condTrue__1, label %condFalse__1

condTrue__1:                                      ; preds = %exit__1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar2__temps__, i32 1)
  br label %condContinue__1

condFalse__1:                                     ; preds = %exit__1
  %21 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %22 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %21, i64 0)
  %23 = bitcast i8* %22 to %Qubit**
  %24 = sub i64 %__qsVar0__numControls__, 1
  %25 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %24)
  %26 = bitcast i8* %25 to %Qubit**
  %27 = load %Qubit*, %Qubit** %26, align 8
  store %Qubit* %27, %Qubit** %23, align 8
  %28 = call %Array* @__quantum__rt__array_concatenate(%Array* %__qsVar2__temps__, %Array* %21)
  call void @__quantum__rt__array_update_reference_count(%Array* %28, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %21, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %28, i32 -1)
  br label %condContinue__1

condContinue__1:                                  ; preds = %condFalse__1, %condTrue__1
  %__qsVar1____qsVar4__newControls____ = phi %Array* [ %__qsVar2__temps__, %condTrue__1 ], [ %28, %condFalse__1 ]
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1____qsVar4__newControls____, i32 1)
  %29 = call %Callable* @__quantum__rt__callable_copy(%Callable* %op, i1 false)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %29, i32 1)
  call void @__quantum__rt__callable_make_adjoint(%Callable* %29)
  %30 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, { double, %Qubit* }* }* getelementptr ({ %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* null, i32 1) to i64))
  %31 = bitcast %Tuple* %30 to { %Array*, { double, %Qubit* }* }*
  %32 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %31, i32 0, i32 0
  %33 = getelementptr inbounds { %Array*, { double, %Qubit* }* }, { %Array*, { double, %Qubit* }* }* %31, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 1)
  store %Array* %__qsVar1____qsVar4__newControls____, %Array** %32, align 8
  store { double, %Qubit* }* %arg, { double, %Qubit* }** %33, align 8
  call void @__quantum__rt__callable_invoke(%Callable* %29, %Tuple* %30, %Tuple* null)
  %34 = sub i64 %__qsVar1__numControlPairs__, 1
  %35 = sub i64 %34, 0
  %36 = sdiv i64 %35, 1
  %37 = mul i64 1, %36
  %38 = add i64 0, %37
  %39 = load %Range, %Range* @EmptyRange, align 4
  %40 = insertvalue %Range %39, i64 %38, 0
  %41 = insertvalue %Range %40, i64 -1, 1
  %42 = insertvalue %Range %41, i64 0, 2
  %43 = extractvalue %Range %42, 0
  %44 = extractvalue %Range %42, 1
  %45 = extractvalue %Range %42, 2
  br label %preheader__1

preheader__1:                                     ; preds = %condContinue__1
  %46 = icmp sgt i64 %44, 0
  br label %header__2

header__2:                                        ; preds = %exiting__2, %preheader__1
  %__qsVar0____qsVar0____qsVar3__numPair______ = phi i64 [ %43, %preheader__1 ], [ %62, %exiting__2 ]
  %47 = icmp sle i64 %__qsVar0____qsVar0____qsVar3__numPair______, %45
  %48 = icmp sge i64 %__qsVar0____qsVar0____qsVar3__numPair______, %45
  %49 = select i1 %46, i1 %47, i1 %48
  br i1 %49, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %50 = mul i64 2, %__qsVar0____qsVar0____qsVar3__numPair______
  %51 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %50)
  %52 = bitcast i8* %51 to %Qubit**
  %53 = load %Qubit*, %Qubit** %52, align 8
  %54 = mul i64 2, %__qsVar0____qsVar0____qsVar3__numPair______
  %55 = add i64 %54, 1
  %56 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %55)
  %57 = bitcast i8* %56 to %Qubit**
  %58 = load %Qubit*, %Qubit** %57, align 8
  %59 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__qsVar2__temps__, i64 %__qsVar0____qsVar0____qsVar3__numPair______)
  %60 = bitcast i8* %59 to %Qubit**
  %61 = load %Qubit*, %Qubit** %60, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____adj(%Qubit* %53, %Qubit* %58, %Qubit* %61)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %62 = add i64 %__qsVar0____qsVar0____qsVar3__numPair______, %44
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar2__temps__, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %29, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %29, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %30, i32 -1)
  call void @__quantum__rt__qubit_release_array(%Array* %__qsVar2__temps__)
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %3, i32 -1)
  ret void
}

define internal void @Microsoft__Quantum__Intrinsic___6bbae0085dbd4f50a756d3774cceb0fe___QsRef36__ApplyWithLessControlsA____adj(%Callable* %op, { %Array*, { %Qubit*, %Qubit* }* }* %0) {
entry:
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 1)
  %1 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %0, i32 0, i32 0
  %controls = load %Array*, %Array** %1, align 8
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 1)
  %2 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %0, i32 0, i32 1
  %arg = load { %Qubit*, %Qubit* }*, { %Qubit*, %Qubit* }** %2, align 8
  %3 = bitcast { %Qubit*, %Qubit* }* %arg to %Tuple*
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %3, i32 1)
  %__qsVar0__numControls__ = call i64 @__quantum__rt__array_get_size_1d(%Array* %controls)
  %__qsVar1__numControlPairs__ = sdiv i64 %__qsVar0__numControls__, 2
  %__qsVar2__temps__ = call %Array* @__quantum__rt__qubit_allocate_array(i64 %__qsVar1__numControlPairs__)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar2__temps__, i32 1)
  %4 = sub i64 %__qsVar1__numControlPairs__, 1
  br label %header__1

header__1:                                        ; preds = %exiting__1, %entry
  %__qsVar0____qsVar3__numPair____ = phi i64 [ 0, %entry ], [ %18, %exiting__1 ]
  %5 = icmp sle i64 %__qsVar0____qsVar3__numPair____, %4
  br i1 %5, label %body__1, label %exit__1

body__1:                                          ; preds = %header__1
  %6 = mul i64 2, %__qsVar0____qsVar3__numPair____
  %7 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %6)
  %8 = bitcast i8* %7 to %Qubit**
  %9 = load %Qubit*, %Qubit** %8, align 8
  %10 = mul i64 2, %__qsVar0____qsVar3__numPair____
  %11 = add i64 %10, 1
  %12 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %11)
  %13 = bitcast i8* %12 to %Qubit**
  %14 = load %Qubit*, %Qubit** %13, align 8
  %15 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__qsVar2__temps__, i64 %__qsVar0____qsVar3__numPair____)
  %16 = bitcast i8* %15 to %Qubit**
  %17 = load %Qubit*, %Qubit** %16, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____body(%Qubit* %9, %Qubit* %14, %Qubit* %17)
  br label %exiting__1

exiting__1:                                       ; preds = %body__1
  %18 = add i64 %__qsVar0____qsVar3__numPair____, 1
  br label %header__1

exit__1:                                          ; preds = %header__1
  %19 = srem i64 %__qsVar0__numControls__, 2
  %20 = icmp eq i64 %19, 0
  br i1 %20, label %condTrue__1, label %condFalse__1

condTrue__1:                                      ; preds = %exit__1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar2__temps__, i32 1)
  br label %condContinue__1

condFalse__1:                                     ; preds = %exit__1
  %21 = call %Array* @__quantum__rt__array_create_1d(i32 8, i64 1)
  %22 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %21, i64 0)
  %23 = bitcast i8* %22 to %Qubit**
  %24 = sub i64 %__qsVar0__numControls__, 1
  %25 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %24)
  %26 = bitcast i8* %25 to %Qubit**
  %27 = load %Qubit*, %Qubit** %26, align 8
  store %Qubit* %27, %Qubit** %23, align 8
  %28 = call %Array* @__quantum__rt__array_concatenate(%Array* %__qsVar2__temps__, %Array* %21)
  call void @__quantum__rt__array_update_reference_count(%Array* %28, i32 1)
  call void @__quantum__rt__array_update_reference_count(%Array* %21, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %28, i32 -1)
  br label %condContinue__1

condContinue__1:                                  ; preds = %condFalse__1, %condTrue__1
  %__qsVar1____qsVar4__newControls____ = phi %Array* [ %__qsVar2__temps__, %condTrue__1 ], [ %28, %condFalse__1 ]
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1____qsVar4__newControls____, i32 1)
  %29 = call %Callable* @__quantum__rt__callable_copy(%Callable* %op, i1 false)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %29, i32 1)
  call void @__quantum__rt__callable_make_adjoint(%Callable* %29)
  %30 = call %Tuple* @__quantum__rt__tuple_create(i64 ptrtoint ({ %Array*, { %Qubit*, %Qubit* }* }* getelementptr ({ %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* null, i32 1) to i64))
  %31 = bitcast %Tuple* %30 to { %Array*, { %Qubit*, %Qubit* }* }*
  %32 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %31, i32 0, i32 0
  %33 = getelementptr inbounds { %Array*, { %Qubit*, %Qubit* }* }, { %Array*, { %Qubit*, %Qubit* }* }* %31, i32 0, i32 1
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 1)
  store %Array* %__qsVar1____qsVar4__newControls____, %Array** %32, align 8
  store { %Qubit*, %Qubit* }* %arg, { %Qubit*, %Qubit* }** %33, align 8
  call void @__quantum__rt__callable_invoke(%Callable* %29, %Tuple* %30, %Tuple* null)
  %34 = sub i64 %__qsVar1__numControlPairs__, 1
  %35 = sub i64 %34, 0
  %36 = sdiv i64 %35, 1
  %37 = mul i64 1, %36
  %38 = add i64 0, %37
  %39 = load %Range, %Range* @EmptyRange, align 4
  %40 = insertvalue %Range %39, i64 %38, 0
  %41 = insertvalue %Range %40, i64 -1, 1
  %42 = insertvalue %Range %41, i64 0, 2
  %43 = extractvalue %Range %42, 0
  %44 = extractvalue %Range %42, 1
  %45 = extractvalue %Range %42, 2
  br label %preheader__1

preheader__1:                                     ; preds = %condContinue__1
  %46 = icmp sgt i64 %44, 0
  br label %header__2

header__2:                                        ; preds = %exiting__2, %preheader__1
  %__qsVar0____qsVar0____qsVar3__numPair______ = phi i64 [ %43, %preheader__1 ], [ %62, %exiting__2 ]
  %47 = icmp sle i64 %__qsVar0____qsVar0____qsVar3__numPair______, %45
  %48 = icmp sge i64 %__qsVar0____qsVar0____qsVar3__numPair______, %45
  %49 = select i1 %46, i1 %47, i1 %48
  br i1 %49, label %body__2, label %exit__2

body__2:                                          ; preds = %header__2
  %50 = mul i64 2, %__qsVar0____qsVar0____qsVar3__numPair______
  %51 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %50)
  %52 = bitcast i8* %51 to %Qubit**
  %53 = load %Qubit*, %Qubit** %52, align 8
  %54 = mul i64 2, %__qsVar0____qsVar0____qsVar3__numPair______
  %55 = add i64 %54, 1
  %56 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %controls, i64 %55)
  %57 = bitcast i8* %56 to %Qubit**
  %58 = load %Qubit*, %Qubit** %57, align 8
  %59 = call i8* @__quantum__rt__array_get_element_ptr_1d(%Array* %__qsVar2__temps__, i64 %__qsVar0____qsVar0____qsVar3__numPair______)
  %60 = bitcast i8* %59 to %Qubit**
  %61 = load %Qubit*, %Qubit** %60, align 8
  call void @Microsoft__Quantum__Intrinsic____QsRef36__PhaseCCX____adj(%Qubit* %53, %Qubit* %58, %Qubit* %61)
  br label %exiting__2

exiting__2:                                       ; preds = %body__2
  %62 = add i64 %__qsVar0____qsVar0____qsVar3__numPair______, %44
  br label %header__2

exit__2:                                          ; preds = %header__2
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar2__temps__, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__capture_update_reference_count(%Callable* %29, i32 -1)
  call void @__quantum__rt__callable_update_reference_count(%Callable* %29, i32 -1)
  call void @__quantum__rt__array_update_reference_count(%Array* %__qsVar1____qsVar4__newControls____, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %3, i32 -1)
  call void @__quantum__rt__tuple_update_reference_count(%Tuple* %30, i32 -1)
  call void @__quantum__rt__qubit_release_array(%Array* %__qsVar2__temps__)
  call void @__quantum__rt__capture_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__callable_update_alias_count(%Callable* %op, i32 -1)
  call void @__quantum__rt__array_update_alias_count(%Array* %controls, i32 -1)
  call void @__quantum__rt__tuple_update_alias_count(%Tuple* %3, i32 -1)
  ret void
}

define double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE__Interop(double %theta1, double %theta2, double %theta3, i64 %nSamples) #1 {
entry:
  %0 = call double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE__body(double %theta1, double %theta2, double %theta3, i64 %nSamples)
  ret double %0
}

define void @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE(double %theta1, double %theta2, double %theta3, i64 %nSamples) #2 {
entry:
  %0 = call double @Microsoft__Quantum__Samples__Chemistry__SimpleVQE__GetEnergyHydrogenVQE__body(double %theta1, double %theta2, double %theta3, i64 %nSamples)
  %1 = call %String* @__quantum__rt__double_to_string(double %0)
  call void @__quantum__rt__message(%String* %1)
  call void @__quantum__rt__string_update_reference_count(%String* %1, i32 -1)
  ret void
}

declare void @__quantum__rt__message(%String*)

declare %String* @__quantum__rt__double_to_string(double)

declare void @__quantum__rt__string_update_reference_count(%String*, i32)

attributes #0 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #1 = { "InteropFriendly" }
attributes #2 = { "EntryPoint" }