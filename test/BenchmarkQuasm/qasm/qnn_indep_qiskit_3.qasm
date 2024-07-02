// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg meas[3];
u2(2.0,-pi) q[0];
u2(2.0,-pi) q[1];
cx q[0],q[1];
p(9.172838187819544) q[1];
cx q[0],q[1];
u2(2.0,-pi) q[2];
cx q[0],q[2];
p(9.172838187819544) q[2];
cx q[0],q[2];
u2(2.0,-pi) q[0];
cx q[1],q[2];
p(9.172838187819544) q[2];
cx q[1],q[2];
u2(2.0,-pi) q[1];
cx q[0],q[1];
p(9.172838187819544) q[1];
cx q[0],q[1];
u2(2.0,-pi) q[2];
cx q[0],q[2];
p(9.172838187819544) q[2];
cx q[0],q[2];
ry(0.11037467731759432) q[0];
cx q[1],q[2];
p(9.172838187819544) q[2];
cx q[1],q[2];
ry(0.5166448702169663) q[1];
ry(0.004821943921094252) q[2];
cx q[1],q[2];
cx q[0],q[1];
ry(0.6538077152553827) q[0];
ry(0.9327551484272038) q[1];
ry(0.8704893014547077) q[2];
barrier q[0],q[1],q[2];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];