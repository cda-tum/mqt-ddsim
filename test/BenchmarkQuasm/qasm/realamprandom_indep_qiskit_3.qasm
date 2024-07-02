// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg meas[3];
ry(4.846350532897925) q[0];
ry(0.13038834331032634) q[1];
cx q[0],q[1];
ry(3.9813292796090525) q[2];
cx q[0],q[2];
ry(4.704873552725635) q[0];
cx q[1],q[2];
ry(3.1322119352256292) q[1];
cx q[0],q[1];
ry(1.4124389803026796) q[2];
cx q[0],q[2];
ry(1.2444656817555668) q[0];
cx q[1],q[2];
ry(4.778555396547324) q[1];
cx q[0],q[1];
ry(1.0625547235745711) q[2];
cx q[0],q[2];
ry(0.5550554224571163) q[0];
cx q[1],q[2];
ry(4.306242740899814) q[1];
ry(5.990347064774806) q[2];
barrier q[0],q[1],q[2];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];