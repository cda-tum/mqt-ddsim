// Benchmark was created by MQT Bench on 2024-03-19
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[5];
creg meas[5];
ry(-2.399967605019755) q[0];
ry(-0.003596948684433063) q[1];
ry(2.314018886718823) q[2];
ry(-0.10853420674224268) q[3];
ry(2.345026373859055) q[4];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(0.9240833542270186) q[0];
ry(-1.0994127158439237) q[1];
ry(-1.655777277088476) q[2];
ry(-1.6856866830091894) q[3];
ry(-0.7725696035849275) q[4];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
ry(2.03112411886031) q[0];
ry(2.4246203197554133) q[1];
ry(1.5673757927308016) q[2];
ry(-2.8076832584951785) q[3];
ry(-1.6463357401948455) q[4];
barrier q[0],q[1],q[2],q[3],q[4];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];
measure q[4] -> meas[4];