// Benchmark was created by MQT Bench on 2024-03-17
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg c[2];
u2(0,0) q[0];
u2(0,0) q[1];
u2(-pi,-pi) q[2];
cx q[0],q[2];
u2(-pi,-pi) q[0];
cx q[1],q[2];
u2(-pi,-pi) q[1];
barrier q[0],q[1],q[2];
measure q[0] -> c[0];
measure q[1] -> c[1];