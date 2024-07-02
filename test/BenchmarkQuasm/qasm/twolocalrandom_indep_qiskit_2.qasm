// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg meas[2];
ry(4.846350532897925) q[0];
ry(0.13038834331032634) q[1];
cx q[0],q[1];
ry(3.9813292796090525) q[0];
ry(4.704873552725635) q[1];
cx q[0],q[1];
ry(3.1322119352256292) q[0];
ry(1.4124389803026796) q[1];
cx q[0],q[1];
ry(1.2444656817555668) q[0];
ry(4.778555396547324) q[1];
barrier q[0],q[1];
measure q[0] -> meas[0];
measure q[1] -> meas[1];