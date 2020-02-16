#ifndef QCEC_DDSIMULATOR_H
#define QCEC_DDSIMULATOR_H

#include <memory>
#include <string>
#include <array>
#include <map>
#include <random>

#include <QuantumComputation.hpp>
#include "DDpackage.h"

class SimpleSimulator {
public:
    /**
     * Construct simulator instance
     * @param qc reference to quantum computation instance
     * @param seed if 0 the PRNG will be seeded from a random source, otherwise the seed will be used directly
     */
    explicit SimpleSimulator(std::unique_ptr<qc::QuantumComputation>& qc, unsigned long seed) : qc(qc), seed(seed) {
        if(seed == 0) {
            // this is probably overkill but better safe than sorry
            std::array<std::mt19937_64::result_type , std::mt19937_64::state_size> random_data{};
            std::random_device rd;
            std::generate(begin(random_data), end(random_data), [&](){return rd();});
            std::seed_seq seeds(begin(random_data), end(random_data));
            mt.seed(seeds);
        } else {
            mt.seed(seed);
        }
    };

    void Simulate();

    std::string MeasureAll(bool collapse=false);
    std::map<std::string, unsigned int> MeasureAllNonCollapsing(unsigned int shots);
    std::string MeasureOneCollapsing(unsigned short index);
    // TODO: void ResetOne(unsigned short index);
    // TODO: void ResetAll();
    std::vector<dd::ComplexValue> getVector() const;

    unsigned long getNodeCount() const {return dd->activeNodeCount;}
    unsigned long getMaxNodeCount() const {return dd->maxActive;}
    unsigned short getNumberOfQubits() const {return qc->getNqubits();}
    unsigned long getNumberOfOps() const {return qc->getNops();}
    unsigned long getSeed() const {return seed;}
    std::string getName() const {return qc->getName();}

    // TODO: dd::Edge ApproximateByFidelity(double targetFidelity, bool removeNodes)
    // TODO: dd::Edge ApproximateBySampling(unsigned int nSamples, unsigned int threshold, bool removeNodes)

    std::unique_ptr<dd::Package> dd = std::make_unique<dd::Package>();
    dd::Edge root_edge{};
private:
    std::mt19937_64 mt;
    std::unique_ptr<qc::QuantumComputation>& qc;
    const unsigned long seed;
    const fp epsilon = 0.001L;

    static void NextPath(std::string &s);


};


#endif //QCEC_DDSIMULATOR_H
