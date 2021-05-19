#ifndef DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP
#define DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP

#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "dd/Export.hpp"
#include "dd/Package.hpp"

#include <cmath>
#include <memory>
#include <omp.h>

class HybridSchrodingerFeynmanSimulator: public CircuitSimulator {
public:
    enum class Mode {
        DD,
        Amplitude
    };

    explicit HybridSchrodingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, Mode mode = Mode::Amplitude, const std::size_t nthreads = 2):
        CircuitSimulator(std::move(qc)), mode(mode), nthreads(nthreads) {}

    HybridSchrodingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, const ApproximationInfo approx_info, const unsigned long long seed, Mode mode = Mode::Amplitude, const std::size_t nthreads = 2):
        CircuitSimulator(std::move(qc), approx_info, seed), mode(mode), nthreads(nthreads) {}

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override;

    Mode                                               mode = Mode::Amplitude;
    [[nodiscard]] const std::vector<dd::ComplexValue>& getFinalAmplitudes() const { return finalAmplitudes; }

    //  Get # of decisions for given split_qubit, so that lower slice: q0 < i < qubit; upper slice: qubit <= i < nqubits
    std::size_t getNDecisions(dd::Qubit split_qubit);

private:
    std::size_t                   nthreads = 2;
    std::vector<dd::ComplexValue> finalAmplitudes{};

    void SimulateHybrid(dd::Qubit split_qubit);
    void SimulateHybridAmplitudes(dd::Qubit split_qubit);

    qc::VectorDD SimulateSlicing(std::unique_ptr<dd::Package>& dd, dd::Qubit split_qubit, std::size_t controls);

    class Slice {
    protected:
        dd::Qubit next_control_idx = 0;

        std::size_t getNextControl() {
            std::size_t idx = 1UL << next_control_idx;
            next_control_idx++;
            return controls & idx;
        }

    public:
        const dd::Qubit      start;
        const dd::Qubit      end;
        const std::size_t    controls;
        const dd::QubitCount nqubits;
        std::size_t          nDecisionsExecuted = 0;
        qc::VectorDD         edge{};

        explicit Slice(std::unique_ptr<dd::Package>& dd, dd::Qubit start, dd::Qubit end, const std::size_t controls):
            start(start), end(end), controls(controls), nqubits(end - start + 1) {
            edge = dd->makeZeroState(nqubits, start);
            dd->incRef(edge);
        }

        explicit Slice(std::unique_ptr<dd::Package>& dd, qc::VectorDD edge, dd::Qubit start, dd::Qubit end, const std::size_t controls):
            start(start), end(end), controls(controls), nqubits(end - start + 1), edge(edge) {
            dd->incRef(edge);
        }

        // returns true if this operation was a split operation
        bool apply(std::unique_ptr<dd::Package>& dd, const std::unique_ptr<qc::Operation>& op);
    };
};

#endif //DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP
