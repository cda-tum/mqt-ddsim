// Benchmark was created by MQT Bench on 2024-03-17
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[4];
creg meas[4];
ry(-5.347518124948548) q[0];
ry(-0.2775667806412738) q[1];
cz q[0],q[1];
ry(4.388931218439147) q[2];
cz q[0],q[2];
cz q[1],q[2];
ry(0.4998199810650877) q[3];
cz q[0],q[3];
ry(-3.1597284583330474) q[0];
cz q[1],q[3];
ry(-3.6049733174107406) q[1];
cz q[0],q[1];
cz q[2],q[3];
ry(3.144241615997172) q[2];
cz q[0],q[2];
cz q[1],q[2];
ry(-1.2437766801193173) q[3];
cz q[0],q[3];
ry(2.9746984747507126) q[0];
cz q[1],q[3];
ry(-2.916403245420324) q[1];
cz q[0],q[1];
cz q[2],q[3];
ry(-1.0916657255867384) q[2];
cz q[0],q[2];
cz q[1],q[2];
ry(5.9922638613377) q[3];
cz q[0],q[3];
ry(-0.15522972874406707) q[0];
cz q[1],q[3];
ry(-0.6989501141709386) q[1];
cz q[2],q[3];
ry(2.532771468834448) q[2];
ry(5.1911851792027655) q[3];
barrier q[0],q[1],q[2],q[3];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];