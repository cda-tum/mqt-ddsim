// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[4];
qreg psi[1];
creg c[4];
h q[0];
h q[1];
h q[2];
h q[3];
x psi[0];
cp(-7*pi/8) psi[0],q[0];
cp(pi/4) psi[0],q[1];
cp(pi/2) psi[0],q[2];
swap q[1],q[2];
cp(pi) psi[0],q[3];
swap q[0],q[3];
h q[0];
cp(-pi/2) q[1],q[0];
h q[1];
cp(-pi/4) q[2],q[0];
cp(-pi/2) q[2],q[1];
h q[2];
cp(-pi/8) q[3],q[0];
cp(-pi/4) q[3],q[1];
cp(-pi/2) q[3],q[2];
h q[3];
barrier q[0],q[1],q[2],q[3],psi[0];
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3];