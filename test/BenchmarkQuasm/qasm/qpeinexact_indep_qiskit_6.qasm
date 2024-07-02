// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[5];
qreg psi[1];
creg c[5];
h q[0];
h q[1];
h q[2];
h q[3];
h q[4];
x psi[0];
cp(2.650718801466388) psi[0],q[0];
cp(-5*pi/16) psi[0],q[1];
cp(-5*pi/8) psi[0],q[2];
cp(3*pi/4) psi[0],q[3];
swap q[1],q[3];
cp(-pi/2) psi[0],q[4];
swap q[0],q[4];
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
cp(-pi/16) q[4],q[0];
cp(-pi/8) q[4],q[1];
cp(-pi/4) q[4],q[2];
cp(-pi/2) q[4],q[3];
h q[4];
barrier q[0],q[1],q[2],q[3],q[4],psi[0];
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3];
measure q[4] -> c[4];