#ifndef DDSIM_SLICINGSIMULATOR_HPP
#define DDSIM_SLICINGSIMULATOR_HPP

#include "CircuitSimulator.hpp"

class SlicingSimulator: public CircuitSimulator {
public:
    explicit SlicingSimulator(std::unique_ptr<qc::QuantumComputation> &qc)
            : CircuitSimulator(qc) {}

    SlicingSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const ApproximationInfo approx_info)
            : CircuitSimulator(qc, approx_info) {}

    SlicingSimulator(std::unique_ptr<qc::QuantumComputation> &qc, const ApproximationInfo approx_info, const unsigned long long seed)
            : CircuitSimulator(qc, approx_info, seed) {}

    std::tuple<qc::VectorDD, qc::VectorDD, qc::VectorDD> SimulateSlicing(std::unique_ptr<dd::Package>& dd, dd::Qubit split_qubit, std::size_t controls);

    //  Get # of decisions for given split_qubit, so that lower slice: q0 < i < qubit; upper slice: qubit <= i < nqubits
    std::size_t getNDecisions(dd::Qubit split_qubit);
private:
    class Slice {
    protected:
        dd::Qubit next_control_idx = 0;

        std::size_t getNextControl() {
            std::size_t idx = 1UL << next_control_idx;
            next_control_idx++;
            return controls & idx;
        }
    public:
        const dd::Qubit start;
        const dd::Qubit end;
        const std::size_t controls;
        const dd::QubitCount nqubits;
        std::size_t nDecisionsExecuted = 0;
        qc::VectorDD edge{};

        explicit Slice(std::unique_ptr<dd::Package>& dd, dd::Qubit start, dd::Qubit end, const std::size_t controls)
                : start(start), end(end), controls(controls), nqubits(end - start + 1) {
            edge = dd->makeZeroState(nqubits, start);
            dd->incRef(edge);
        }

        explicit Slice(std::unique_ptr<dd::Package>& dd, qc::VectorDD edge, dd::Qubit start, dd::Qubit end, const std::size_t controls)
                : start(start), end(end), controls(controls), nqubits(end - start + 1), edge(edge) {
            dd->incRef(edge);
        }

        // returns true if this operation was a split operation
        bool apply(std::unique_ptr<dd::Package>& dd, const std::unique_ptr<qc::Operation>& op);
    };
};

#endif //DDSIM_SLICINGSIMULATOR_HPP
