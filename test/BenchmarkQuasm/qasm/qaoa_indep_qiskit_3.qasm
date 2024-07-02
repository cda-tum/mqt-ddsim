// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg meas[3];
h q[0];
h q[1];
rzz(5.094008139994057) q[0],q[1];
h q[2];
rzz(5.094008139994057) q[0],q[2];
rx(-6.860310317223841) q[0];
rzz(5.094008139994057) q[1],q[2];
rx(-6.860310317223841) q[1];
rzz(-5.141522309483603) q[0],q[1];
rx(-6.860310317223841) q[2];
rzz(-5.141522309483603) q[0],q[2];
rx(8.165927224507852) q[0];
rzz(-5.141522309483603) q[1],q[2];
rx(8.165927224507852) q[1];
rx(8.165927224507852) q[2];
barrier q[0],q[1],q[2];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];