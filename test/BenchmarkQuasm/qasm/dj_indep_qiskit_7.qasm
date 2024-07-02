// Benchmark was created by MQT Bench on 2024-03-17
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[7];
creg c[6];
u2(0,0) q[0];
u2(0,0) q[1];
h q[2];
u2(0,0) q[3];
h q[4];
u2(0,0) q[5];
u2(-pi,-pi) q[6];
cx q[0],q[6];
u2(-pi,-pi) q[0];
cx q[1],q[6];
u2(-pi,-pi) q[1];
cx q[2],q[6];
h q[2];
cx q[3],q[6];
u2(-pi,-pi) q[3];
cx q[4],q[6];
h q[4];
cx q[5],q[6];
u2(-pi,-pi) q[5];
barrier q[0],q[1],q[2],q[3],q[4],q[5],q[6];
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3];
measure q[4] -> c[4];
measure q[5] -> c[5];