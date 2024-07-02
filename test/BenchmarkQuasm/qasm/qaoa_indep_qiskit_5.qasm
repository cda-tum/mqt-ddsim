// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[5];
creg meas[5];
h q[0];
h q[1];
rzz(-1.930004982171882) q[0],q[1];
h q[2];
rzz(-1.930004982171882) q[0],q[2];
rx(-2.6436411784439646) q[0];
h q[3];
rzz(-1.930004982171882) q[1],q[3];
rx(-2.6436411784439646) q[1];
rzz(-3.639726195721819) q[0],q[1];
h q[4];
rzz(-1.930004982171882) q[2],q[4];
rx(-2.6436411784439646) q[2];
rzz(-3.639726195721819) q[0],q[2];
rx(1.9303385045328598) q[0];
rzz(-1.930004982171882) q[3],q[4];
rx(-2.6436411784439646) q[3];
rzz(-3.639726195721819) q[1],q[3];
rx(1.9303385045328598) q[1];
rx(-2.6436411784439646) q[4];
rzz(-3.639726195721819) q[2],q[4];
rx(1.9303385045328598) q[2];
rzz(-3.639726195721819) q[3],q[4];
rx(1.9303385045328598) q[3];
rx(1.9303385045328598) q[4];
barrier q[0],q[1],q[2],q[3],q[4];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];
measure q[4] -> meas[4];