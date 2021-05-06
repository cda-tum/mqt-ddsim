#ifndef DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP
#define DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP

#include "SlicingSimulator.hpp"
#include "dd/Export.hpp"

#include <cmath>
#include <omp.h>

class HybridSchrodingerFeynmanSimulator: public SlicingSimulator {
public:
    enum class Mode {
        DD,
        Amplitude
    };

    explicit HybridSchrodingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>& qc, Mode mode = Mode::Amplitude, const std::size_t nthreads = 2):
        SlicingSimulator(qc), mode(mode), nthreads(nthreads) {}

    HybridSchrodingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const ApproximationInfo approx_info, const unsigned long long seed, Mode mode = Mode::Amplitude, const std::size_t nthreads = 2):
        SlicingSimulator(qc, approx_info, seed), mode(mode), nthreads(nthreads) {}

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    Mode                                               mode = Mode::Amplitude;
    [[nodiscard]] const std::vector<dd::ComplexValue>& getFinalAmplitudes() const { return finalAmplitudes; }

private:
    std::size_t                   nthreads = 2;
    std::vector<dd::ComplexValue> finalAmplitudes{};

    void SimulateParallel(dd::Qubit split_qubit);
    void SimulateParallelAmplitudes(dd::Qubit split_qubit);
};

#endif //DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP
