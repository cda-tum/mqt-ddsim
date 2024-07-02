// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
qreg flag[1];
creg meas[3];
h q[0];
h q[1];
x flag[0];
cp(pi/2) q[1],flag[0];
cx q[1],q[0];
cp(-pi/2) q[0],flag[0];
cx q[1],q[0];
cp(pi/2) q[0],flag[0];
u2(0,0) q[0];
u1(-pi) q[1];
cx q[0],q[1];
u2(-pi,-pi) q[0];
u1(-pi) q[1];
barrier q[0],q[1],flag[0];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure flag[0] -> meas[2];