// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[8];
creg meas[8];
ry(-0.3494622741668449) q[0];
ry(pi) q[1];
ry(-1.5710722648883404) q[2];
ry(4.644220373125837e-05) q[3];
ry(-1.5707372732338276) q[4];
ry(-3.1406488201084652) q[5];
ry(-0.2911615975772578) q[6];
ry(0.4617767927498017) q[7];
cx q[6],q[7];
cx q[5],q[6];
cx q[4],q[5];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(1.5361791596451624) q[0];
ry(1.5588683256802904) q[1];
ry(2.3899489574846746) q[2];
ry(2.389935641875764) q[3];
ry(-0.000976780675201439) q[4];
ry(-pi) q[5];
ry(-1.7034581750314917) q[6];
ry(-2.0499397412319795) q[7];
cx q[6],q[7];
cx q[5],q[6];
cx q[4],q[5];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(1.5383130392005682) q[0];
ry(1.9204593767189913) q[1];
ry(1.5707719493954009) q[2];
ry(-3.1412909152953428) q[3];
ry(1.5709641203271556) q[4];
ry(-pi) q[5];
ry(pi) q[6];
ry(-1.3107330688860097) q[7];
barrier q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];
measure q[4] -> meas[4];
measure q[5] -> meas[5];
measure q[6] -> meas[6];
measure q[7] -> meas[7];