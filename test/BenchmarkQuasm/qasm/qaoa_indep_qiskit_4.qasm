// Benchmark was created by MQT Bench on 2024-03-18
// For more information about MQT Bench, please visit https://www.cda.cit.tum.de/mqtbench/
// MQT Bench version: 1.1.0
// Qiskit version: 1.0.2

OPENQASM 2.0;
include "qelib1.inc";
qreg q[4];
creg meas[4];
h q[0];
h q[1];
rzz(-4.712387194486129) q[0],q[1];
h q[2];
rzz(-4.712387194486129) q[0],q[2];
rx(8.63937561261258) q[0];
h q[3];
rzz(-4.712387194486129) q[1],q[3];
rx(8.63937561261258) q[1];
rzz(0.785393361472697) q[0],q[1];
rzz(-4.712387194486129) q[2],q[3];
rx(8.63937561261258) q[2];
rzz(0.785393361472697) q[0],q[2];
rx(4.712387043046583) q[0];
rx(8.63937561261258) q[3];
rzz(0.785393361472697) q[1],q[3];
rx(4.712387043046583) q[1];
rzz(0.785393361472697) q[2],q[3];
rx(4.712387043046583) q[2];
rx(4.712387043046583) q[3];
barrier q[0],q[1],q[2],q[3];
measure q[0] -> meas[0];
measure q[1] -> meas[1];
measure q[2] -> meas[2];
measure q[3] -> meas[3];