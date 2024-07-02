// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[6];
creg meas[6];
h q[0];
h q[1];
h q[2];
rzz(-4.993767341069986) q[0],q[2];
h q[3];
rzz(-4.993767341069986) q[0],q[3];
rx(-2.4967770622792953) q[0];
h q[4];
rzz(-4.993767341069986) q[1],q[4];
rzz(-4.993767341069986) q[2],q[4];
rx(-2.4967770622792953) q[2];
rzz(-0.6447018505867251) q[0],q[2];
rx(-2.4967770622792953) q[4];
h q[5];
rzz(-4.993767341069986) q[1],q[5];
rx(-2.4967770622792953) q[1];
rzz(-0.6447018505867251) q[1],q[4];
rzz(-0.6447018505867251) q[2],q[4];
rx(8.135245684982658) q[2];
rzz(-4.993767341069986) q[3],q[5];
rx(-2.4967770622792953) q[3];
rzz(-0.6447018505867251) q[0],q[3];
rx(8.135245684982658) q[0];
rx(8.135245684982658) q[4];
rx(-2.4967770622792953) q[5];
rzz(-0.6447018505867251) q[1],q[5];
rx(8.135245684982658) q[1];
rzz(-0.6447018505867251) q[3],q[5];
rx(8.135245684982658) q[3];
rx(8.135245684982658) q[5];
barrier q[0],q[1],q[2],q[3],q[4],q[5];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];
measure q[4] -> meas[4];
measure q[5] -> meas[5];