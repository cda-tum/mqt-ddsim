// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
qreg psi[1];
creg c[2];
h q[0];
h q[1];
x psi[0];
cp(3*pi/4) psi[0],q[0];
cp(-pi/2) psi[0],q[1];
swap q[0],q[1];
h q[0];
cp(-pi/2) q[1],q[0];
h q[1];
barrier q[0],q[1],psi[0];
measure q[0] -> c[0];
measure q[1] -> c[1];