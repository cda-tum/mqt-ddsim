// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[7];
creg meas[7];
ry(-3.4217682360647656e-05) q[0];
ry(-2.710669110771585) q[1];
ry(2.993533072089068) q[2];
ry(2.5592037634130937) q[3];
ry(1.393432511538407) q[4];
ry(-0.006200492891662302) q[5];
ry(-2.350477107018963) q[6];
cx q[5],q[6];
cx q[4],q[5];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(-3.1412478778569413) q[0];
ry(-1.6663051858403142) q[1];
ry(-1.509788849384436) q[2];
ry(1.577513000801852) q[3];
ry(-1.5710074213795457) q[4];
ry(-1.5713972060528005) q[5];
ry(0.7794577454906294) q[6];
cx q[5],q[6];
cx q[4],q[5];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(pi) q[0];
ry(1.479497169015327) q[1];
ry(0.9690107788739298) q[2];
ry(1.4225223699649225) q[3];
ry(0.9882288003719667) q[4];
ry(2.964373564968955) q[5];
ry(1.5664327326785497) q[6];
barrier q[0],q[1],q[2],q[3],q[4],q[5],q[6];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];
measure q[4] -> meas[4];
measure q[5] -> meas[5];
measure q[6] -> meas[6];