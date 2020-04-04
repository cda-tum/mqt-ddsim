#ifndef DDSIMULATOR_H
#define DDSIMULATOR_H

#include <memory>
#include <string>
#include <array>
#include <map>
#include <random>
#include <utility>

#include "DDpackage.h"

class Simulator {
public:
    explicit Simulator(unsigned long long seed) : seed(seed), has_fixed_seed(true) {
        mt.seed(seed);
    };

    explicit Simulator() : seed(0), has_fixed_seed(false) {
        // this is probably overkill but better safe than sorry
        std::array<std::mt19937_64::result_type , std::mt19937_64::state_size> random_data{};
        std::random_device rd;
        std::generate(begin(random_data), end(random_data), [&](){return rd();});
        std::seed_seq seeds(begin(random_data), end(random_data));
        mt.seed(seeds);
    };

    virtual void Simulate() = 0;
    virtual std::map<std::string, std::string> AdditionalStatistics() {return {};};

    std::string MeasureAll(bool collapse=false);
    std::map<std::string, unsigned int> MeasureAllNonCollapsing(unsigned int shots);
    char MeasureOneCollapsing(unsigned short index);
    // TODO: void ResetOne(unsigned short index);
    // TODO: void ResetAll();
    std::vector<dd::ComplexValue> getVector() const;

    unsigned long getNodeCount() const {return dd->activeNodeCount;}
    unsigned long getMaxNodeCount() const {return dd->maxActive;}
    std::pair<dd::ComplexValue, std::string> getPathOfLeastResistance();

    std::string getSeed() const {return has_fixed_seed ? std::to_string(seed) : "-1";}

    virtual unsigned short getNumberOfQubits() const = 0;
    virtual unsigned long getNumberOfOps() const = 0;
    virtual std::string getName() const = 0;

    double ApproximateByFidelity(double targetFidelity, bool removeNodes);
    double ApproximateBySampling(int nSamples, int threshold, bool removeNodes);
    dd::Edge RemoveNodes(dd::Edge edge, std::map<dd::NodePtr, dd::Edge>& dag_edges);

    std::unique_ptr<dd::Package> dd = std::make_unique<dd::Package>();
    dd::Edge root_edge{};
protected:
    std::mt19937_64 mt;

    const unsigned long long seed;
    const bool has_fixed_seed;
    const fp epsilon = 0.001L;

    static void NextPath(std::string &s);
};


#endif //DDSIMULATOR_H
