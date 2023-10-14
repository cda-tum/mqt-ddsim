from __future__ import annotations

import pathlib

from qiskit import BasicAer
from qiskit.algorithms import AmplificationProblem, Grover
from qiskit.circuit.library import PhaseOracle

from mqt.ddsim import DDSIMProvider

input_3sat_instance = """
c example DIMACS-CNF 3-SAT
p cnf 3 5
-1 -2 -3 0
1 -2 3 0
1 2 -3 0
1 -2 -3 0
-1 2 3 0
"""
with pathlib.Path("dimacs_file.txt").open("w") as fd:
    fd.write(input_3sat_instance)

print("pyddsim")
backend = DDSIMProvider().get_backend("qasm_simulator")
backend.set_options(shots=1024)

oracle = PhaseOracle.from_dimacs_file("dimacs_file.txt")
grover = Grover(quantum_instance=backend)

problem = AmplificationProblem(oracle, is_good_state=oracle.evaluate_bitstring)
result = grover.amplify(problem)

print(result.assignment)

print("BasicAer")
backend = BasicAer.get_backend("qasm_simulator")

oracle = PhaseOracle.from_dimacs_file("dimacs_file.txt")
grover = Grover(quantum_instance=backend)

problem = AmplificationProblem(oracle, is_good_state=oracle.evaluate_bitstring)
result = grover.amplify(problem)

print(result.assignment)
