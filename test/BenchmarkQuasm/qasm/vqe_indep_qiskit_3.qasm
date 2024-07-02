// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg meas[3];
ry(-0.9413251507535245) q[0];
ry(-0.12641277016975258) q[1];
ry(-pi) q[2];
cx q[1],q[2];
cx q[0],q[1];
ry(-2.2065597028049257) q[0];
ry(-pi) q[1];
ry(1.749025192431949) q[2];
cx q[1],q[2];
cx q[0],q[1];
ry(0.6901007506009792) q[0];
ry(-0.40245553923277066) q[1];
ry(2.7174409942016213) q[2];
barrier q[0],q[1],q[2];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];