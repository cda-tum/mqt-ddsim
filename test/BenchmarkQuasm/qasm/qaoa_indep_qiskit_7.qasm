// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[7];
creg meas[7];
h q[0];
h q[1];
h q[2];
rzz(1.6527519223558647) q[1],q[2];
h q[3];
rzz(1.6527519223558647) q[0],q[3];
h q[4];
rzz(1.6527519223558647) q[1],q[4];
rx(11.777642967412769) q[1];
rzz(1.6527519223558647) q[2],q[4];
rx(11.777642967412769) q[2];
rzz(3.9303882758114326) q[1],q[2];
rx(11.777642967412769) q[4];
rzz(3.9303882758114326) q[1],q[4];
rx(-1.652897253728491) q[1];
rzz(3.9303882758114326) q[2],q[4];
rx(-1.652897253728491) q[2];
rx(-1.652897253728491) q[4];
h q[5];
rzz(1.6527519223558647) q[0],q[5];
rx(11.777642967412769) q[0];
h q[6];
rzz(1.6527519223558647) q[3],q[6];
rx(11.777642967412769) q[3];
rzz(3.9303882758114326) q[0],q[3];
rzz(1.6527519223558647) q[5],q[6];
rx(11.777642967412769) q[5];
rzz(3.9303882758114326) q[0],q[5];
rx(-1.652897253728491) q[0];
rx(11.777642967412769) q[6];
rzz(3.9303882758114326) q[3],q[6];
rx(-1.652897253728491) q[3];
rzz(3.9303882758114326) q[5],q[6];
rx(-1.652897253728491) q[5];
rx(-1.652897253728491) q[6];
barrier q[0],q[1],q[2],q[3],q[4],q[5],q[6];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];
measure q[4] -> meas[4];
measure q[5] -> meas[5];
measure q[6] -> meas[6];