// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg meas[2];
u2(2.0,-pi) q[0];
u2(2.0,-pi) q[1];
cx q[0],q[1];
p(9.172838187819544) q[1];
cx q[0],q[1];
u2(2.0,-pi) q[0];
u2(2.0,-pi) q[1];
cx q[0],q[1];
p(9.172838187819544) q[1];
cx q[0],q[1];
ry(0.7271911597342195) q[0];
ry(0.26244291289966604) q[1];
cx q[0],q[1];
ry(0.9484429661729774) q[0];
ry(0.9866257564223032) q[1];
barrier q[0],q[1];
measure q[0] -> meas[0];
measure q[1] -> meas[1];