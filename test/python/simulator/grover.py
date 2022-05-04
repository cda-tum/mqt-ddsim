from qiskit.algorithms import Grover
from qiskit.algorithms import AmplificationProblem
from qiskit.circuit.library import PhaseOracle
from qiskit import BasicAer
from mqt.ddsim import DDSIMProvider
input_3sat_instance = '''
c example DIMACS-CNF 3-SAT
p cnf 3 5
-1 -2 -3 0
1 -2 3 0
1 2 -3 0
1 -2 -3 0
-1 2 3 0
'''
with open('dimacs_file.txt', 'w') as fd:
    fd.write(input_3sat_instance)

print('pyddsim')
backend = DDSIMProvider().get_backend('qasm_simulator')
backend.set_options(shots=1024)

oracle = PhaseOracle.from_dimacs_file('dimacs_file.txt')
grover = Grover(quantum_instance=backend)

problem = AmplificationProblem(oracle, is_good_state=oracle.evaluate_bitstring)
result = grover.amplify(problem)

print(result.assignment)

print('BasicAer')
backend = BasicAer.get_backend('qasm_simulator')

oracle = PhaseOracle.from_dimacs_file('dimacs_file.txt')
grover = Grover(quantum_instance=backend)

problem = AmplificationProblem(oracle, is_good_state=oracle.evaluate_bitstring)
result = grover.amplify(problem)

print(result.assignment)

