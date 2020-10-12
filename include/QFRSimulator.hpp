#ifndef DDSIM_QFRSIMULATOR_HPP
#define DDSIM_QFRSIMULATOR_HPP

#include "Simulator.hpp"
#include "QuantumComputation.hpp"

class QFRSimulator : public Simulator {
public:
    explicit QFRSimulator(std::unique_ptr<qc::QuantumComputation>& qc) : qc(qc) {

    }
    explicit QFRSimulator(std::unique_ptr<qc::QuantumComputation>& qc, unsigned long long int seed) : Simulator(seed), qc(qc) {

    }

    void Simulate();


    unsigned short getNumberOfQubits() const override {return qc->getNqubits();};
    unsigned long getNumberOfOps() const override {return qc->getNops();};
    std::string getName() const override {return qc->getName();};

private:
    std::unique_ptr<qc::QuantumComputation>& qc;
};


#endif //DDSIM_QFRSIMULATOR_HPP
