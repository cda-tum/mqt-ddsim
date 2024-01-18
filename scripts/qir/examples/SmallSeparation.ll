; ModuleID = 'qat-link'
source_filename = "qat-link"

%Qubit = type opaque
%Result = type opaque
%String = type opaque

define double @SeparationTest__Main__Interop(double %angle) #0 {
entry:
  %x.i = fmul double %angle, 2.000000e+00
  call void @__quantum__qis__rx__body(double %x.i, %Qubit* null)
  call void @__quantum__qis__mz__body(%Qubit* null, %Result* null)
  call void @__quantum__qis__reset__body(%Qubit* null)
  %0 = call %Result* @__quantum__rt__result_get_one()
  %classical_result.i = call i1 @__quantum__rt__result_equal(%Result* null, %Result* %0)
  %retval.i = select i1 %classical_result.i, double 0x4022666666666666, double 3.400000e+00
  ret double %retval.i
}

declare %Qubit* @__quantum__rt__qubit_allocate()

declare %Result* @__quantum__rt__result_get_one()

declare i1 @__quantum__rt__result_equal(%Result*, %Result*)

declare void @__quantum__rt__result_update_reference_count(%Result*, i32)

declare void @__quantum__rt__qubit_release(%Qubit*)

declare %Result* @__quantum__qis__m__body(%Qubit*)

declare void @__quantum__qis__reset__body(%Qubit*)

declare void @__quantum__qis__rx__body(double, %Qubit*)

declare %String* @__quantum__rt__double_to_string(double)

declare void @__quantum__rt__message(%String*)

declare void @__quantum__rt__string_update_reference_count(%String*, i32)

declare void @__quantum__qis__mz__body(%Qubit*, %Result*)

attributes #0 = { "EntryPoint" }
