#ifndef DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP
#define DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "dd/Export.hpp"
#include "dd/Operations.hpp"
#include "dd/Package.hpp"

#include <complex>
#include <memory>

template<class Config = dd::DDPackageConfig>
class HybridSchrodingerFeynmanSimulator: public CircuitSimulator<Config> {
public:
    enum class Mode {
        DD,
        Amplitude
    };

    HybridSchrodingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                                      const ApproximationInfo&                  approxInfo_,
                                      Mode                                      mode_     = Mode::Amplitude,
                                      const std::size_t                         nthreads_ = 2):
        CircuitSimulator<Config>(std::move(qc_), approxInfo_),
        mode(mode_), nthreads(nthreads_) {
        // remove final measurements
        qc::CircuitOptimizer::removeFinalMeasurements(*(CircuitSimulator<Config>::qc));
    }

    explicit HybridSchrodingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                                               Mode                                      mode_     = Mode::Amplitude,
                                               const std::size_t                         nthreads_ = 2):
        HybridSchrodingerFeynmanSimulator(std::move(qc_), {}, mode_, nthreads_) {}

    HybridSchrodingerFeynmanSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                                      const ApproximationInfo&                  approxInfo_,
                                      const std::uint64_t                       seed_,
                                      Mode                                      mode_     = Mode::Amplitude,
                                      const std::size_t                         nthreads_ = 2):
        CircuitSimulator<Config>(std::move(qc_), approxInfo_, seed_),
        mode(mode_), nthreads(nthreads_) {
        // remove final measurements
        qc::CircuitOptimizer::removeFinalMeasurements(*(CircuitSimulator<Config>::qc));
    }

    std::map<std::string, std::size_t> simulate(std::size_t shots) override;

    Mode mode = Mode::Amplitude;

    [[nodiscard]] dd::CVec getVectorFromHybridSimulation() const {
        if (CircuitSimulator<Config>::getNumberOfQubits() >= 60) {
            // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
            throw std::range_error("getVector only supports less than 60 qubits.");
        }
        if (getMode() == Mode::Amplitude) {
            return finalAmplitudes;
        }
        return CircuitSimulator<Config>::getVector();
    }

    //  Get # of decisions for given split_qubit, so that lower slice: q0 < i < qubit; upper slice: qubit <= i < nqubits
    std::size_t getNDecisions(qc::Qubit splitQubit);

    [[nodiscard]] Mode getMode() const { return mode; }

protected:
    /// See Simulator<Config>::exportDDtoGraphviz
    void exportDDtoGraphviz(std::ostream& os, bool colored, bool edgeLabels, bool classic, bool memory, bool formatAsPolar) override;

private:
    std::size_t nthreads = 2;
    dd::CVec    finalAmplitudes{};

    void simulateHybridTaskflow(qc::Qubit splitQubit);
    void simulateHybridAmplitudes(qc::Qubit splitQubit);

    qc::VectorDD simulateSlicing(std::unique_ptr<dd::Package<Config>>& sliceDD, qc::Qubit splitQubit, std::size_t controls);

    class Slice {
    protected:
        qc::Qubit nextControlIdx = 0;

        std::size_t getNextControl() {
            std::size_t idx = 1UL << nextControlIdx;
            nextControlIdx++;
            return controls & idx;
        }

    public:
        qc::Qubit    start;
        qc::Qubit    end;
        std::size_t  controls;
        qc::Qubit    nqubits;
        std::size_t  nDecisionsExecuted = 0;
        qc::VectorDD edge{};

        explicit Slice(std::unique_ptr<dd::Package<Config>>& dd, const qc::Qubit start_, const qc::Qubit end_, const std::size_t controls_):
            start(start_), end(end_), controls(controls_), nqubits(end - start + 1), edge(dd->makeZeroState(static_cast<dd::Qubit>(nqubits), start_)) {
            dd->incRef(edge);
        }

        explicit Slice(std::unique_ptr<dd::Package<Config>>& dd, qc::VectorDD edge_, const qc::Qubit start_, const qc::Qubit end_, const std::size_t controls_):
            start(start_), end(end_), controls(controls_), nqubits(end - start + 1), edge(edge_) {
            dd->incRef(edge);
        }

        // returns true if this operation was a split operation
        bool apply(std::unique_ptr<dd::Package<Config>>& sliceDD, const std::unique_ptr<qc::Operation>& op);
    };
};

#endif //DDSIM_HYBRIDSCHRODINGERFEYNMANSIMULATOR_HPP
