// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[4];
creg meas[4];
ry(2.2550838042508814) q[0];
ry(-0.00024211540731557916) q[1];
ry(1.6710237090859774) q[2];
ry(-2.4683764674701445) q[3];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(0.05242096891171133) q[0];
ry(0.06725228443923906) q[1];
ry(0.15986425080312602) q[2];
ry(-0.12427572772829466) q[3];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(-2.253454955203389) q[0];
ry(-3.099043494910433) q[1];
ry(1.5706098165320757) q[2];
ry(2.462214751559368) q[3];
barrier q[0],q[1],q[2],q[3];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];