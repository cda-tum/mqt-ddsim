// Benchmark was created by MQT Bench on 2024-03-17
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
creg meas[3];
ry(4.702531904090672) q[0];
ry(0.27912916317897923) q[1];
cz q[0],q[1];
ry(3.1324512545711407) q[2];
cz q[0],q[2];
ry(-5.910838761425785) q[0];
cz q[1],q[2];
ry(-2.8063676966033793) q[1];
cz q[0],q[1];
ry(-3.6622582190162847) q[2];
cz q[0],q[2];
ry(-0.29837891906294317) q[0];
cz q[1],q[2];
ry(-4.130720669019984) q[1];
cz q[0],q[1];
ry(-1.023492757978003) q[2];
cz q[0],q[2];
ry(-2.8095712872076577) q[0];
cz q[1],q[2];
ry(1.2348318981948359) q[1];
ry(0.1520689023222807) q[2];
barrier q[0],q[1],q[2];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];