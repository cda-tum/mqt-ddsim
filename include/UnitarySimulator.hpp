#ifndef DDSIM_UNITARYSIMULATOR_HPP
#define DDSIM_UNITARYSIMULATOR_HPP

#include "CircuitOptimizer.hpp"
#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "dd/Export.hpp"
#include "dd/Package.hpp"

#include <cmath>
#include <complex>
#include <memory>

class UnitarySimulator: public CircuitSimulator {
public:
    enum class Mode {
        Sequential,
        Recursive
    };

    explicit UnitarySimulator(std::unique_ptr<qc::QuantumComputation>&& qc, Mode mode = Mode::Recursive):
        CircuitSimulator(std::move(qc)), mode(mode) {
        // remove final measurements
        qc::CircuitOptimizer::removeFinalMeasurements(*(this->qc));
    }

    UnitarySimulator(std::unique_ptr<qc::QuantumComputation>&& qc, const ApproximationInfo approx_info, const unsigned long long seed, Mode mode = Mode::Recursive):
        CircuitSimulator(std::move(qc), approx_info, seed), mode(mode) {
        // remove final measurements
        qc::CircuitOptimizer::removeFinalMeasurements(*(this->qc));
    }

    void Construct();

    [[nodiscard]] Mode         getMode() const { return mode; }
    [[nodiscard]] qc::MatrixDD getConstructedDD() const { return e; }
    [[nodiscard]] double       getConstructionTime() const { return constructionTime; }
    [[nodiscard]] std::size_t  getFinalNodeCount() const { return dd->size(e); }
    [[nodiscard]] std::size_t  getMaxNodeCount() const override { return dd->mUniqueTable.getPeakNodeCount(); }

private:
    qc::MatrixDD e{};

    Mode mode = Mode::Recursive;

    double constructionTime = 0.;
};

#endif //DDSIM_UNITARYSIMULATOR_HPP
