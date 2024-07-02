// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[6];
creg meas[6];
ry(3.061990373758841) q[0];
ry(3.1415302799473563) q[1];
ry(1.5712531165795274) q[2];
ry(-0.0001452392683317902) q[3];
ry(-2.31967727354622) q[4];
ry(pi) q[5];
cx q[4],q[5];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(0.150095507770468) q[0];
ry(-1.0868997256051287) q[1];
ry(0.737464585284184) q[2];
ry(-0.7375123187150454) q[3];
ry(-1.570808525773773) q[4];
ry(1.5709092500923758) q[5];
cx q[4],q[5];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(-2.971912725037559) q[0];
ry(-1.0809012660904078) q[1];
ry(1.570803491785533) q[2];
ry(0.000701975940692948) q[3];
ry(-1.570496169225627) q[4];
ry(-0.7490476703896829) q[5];
barrier q[0],q[1],q[2],q[3],q[4],q[5];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];
measure q[4] -> meas[4];
measure q[5] -> meas[5];