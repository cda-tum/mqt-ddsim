#ifndef DDSIM_HYBRIDSCHROEDINGERFEYNMANSIMULATOR_HPP
#define DDSIM_HYBRIDSCHROEDINGERFEYNMANSIMULATOR_HPP

#include "SlicingSimulator.hpp"
#include "dd/Export.hpp"

#include <cmath>
#include <omp.h>

class HybridSchroedingerFeynmanSimulator: public SlicingSimulator {
public:
    explicit HybridSchroedingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>& qc):
        SlicingSimulator(qc) {}

    HybridSchroedingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const ApproximationInfo approx_info):
        SlicingSimulator(qc, approx_info) {}

    HybridSchroedingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const ApproximationInfo approx_info, const unsigned long long seed):
        SlicingSimulator(qc, approx_info, seed) {}

    HybridSchroedingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const ApproximationInfo approx_info, const unsigned long long seed, const std::size_t nthreads):
        SlicingSimulator(qc, approx_info, seed), nthreads(nthreads) {}

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

private:
    std::size_t nthreads = 2;

    void        SimulateParallel(dd::Qubit split_qubit);
    void        SimulateParallelAmplitudes(dd::Qubit split_qubit, const std::string& filename);
    static void addAmplitudes(std::unique_ptr<dd::Package>& dd, const std::string& filename1, const std::string& filename2, const std::string& resultfile, bool binary = false);
    static void addAmplitudes(std::unique_ptr<dd::Package>& dd, std::istream& ifs1, std::istream& ifs2, std::ostream& oss, bool binary = false);
    static void addAmplitudes(std::vector<dd::ComplexValue>& amp1, std::vector<dd::ComplexValue>& amp2);
};

#endif //DDSIM_HYBRIDSCHROEDINGERFEYNMANSIMULATOR_HPP
