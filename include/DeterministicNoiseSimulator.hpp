#pragma once

#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "StochasticNoiseSimulator.hpp"
#include "dd/NoiseFunctionality.hpp"

template<class DDPackage = DensityMatrixPackage>
class DeterministicNoiseSimulator: public Simulator<DDPackage> {
    using dEdge = dd::dEdge;
    using dNode = dd::dNode;

public:
    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc,
                                const std::string&                       noiseEffects,
                                double                                   noiseProbability,
                                std::optional<double>                    ampDampingProbability,
                                double                                   multiQubitGateFactor,
                                bool                                     unoptimizedSim = false,
                                unsigned long long                       seed           = 0):
        Simulator<DDPackage>(seed),
        qc(qc),
        noiseEffects(StochasticNoiseSimulator<StochasticNoiseSimulatorDDPackageConfig>::initializeNoiseEffects(noiseEffects)),
        noiseProbSingleQubit(noiseProbability),
        ampDampingProbSingleQubit(ampDampingProbability ? ampDampingProbability.value() : noiseProbability * 2),
        noiseProbMultiQubit(noiseProbability * multiQubitGateFactor),
        ampDampingProbMultiQubit(ampDampingProbSingleQubit * multiQubitGateFactor),
        sequentiallyApplyNoise(unoptimizedSim),
        useDensityMatrixType(!unoptimizedSim) {
        StochasticNoiseSimulator<StochasticNoiseSimulatorDDPackageConfig>::sanityCheckOfNoiseProbabilities(noiseProbability, ampDampingProbSingleQubit, multiQubitGateFactor);
        Simulator<DDPackage>::dd->resize(qc->getNqubits());
    }

    explicit DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, unsigned long long seed = 0):
        DeterministicNoiseSimulator(qc, std::string("APD"), 0.001, std::optional<double>{}, 2, false, seed) {}

    std::map<std::string, std::size_t> Simulate(unsigned int shots) override {
        return sampleFromProbabilityMap(this->DeterministicSimulate(), shots);
    };

    std::map<std::string, dd::fp> DeterministicSimulate();

    std::map<std::string, std::size_t> sampleFromProbabilityMap(const std::map<std::string, dd::fp>& resultProbabilityMap, unsigned int shots);

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    [[nodiscard]] std::size_t getActiveNodeCount() const override { return Simulator<DDPackage>::dd->dUniqueTable.getActiveNodeCount(); }
    [[nodiscard]] std::size_t getMaxNodeCount() const override { return Simulator<DDPackage>::dd->dUniqueTable.getMaxActiveNodes(); }

    [[nodiscard]] std::size_t countNodesFromRoot() {
        size_t tmp;
        if (useDensityMatrixType) {
            dEdge::alignDensityEdge(rootEdge);
            tmp = Simulator<DDPackage>::dd->size(rootEdge);
            dEdge::setDensityMatrixTrue(rootEdge);
        } else {
            tmp = Simulator<DDPackage>::dd->size(rootEdge);
        }
        return tmp;
    }

    dEdge rootEdge{};

private:
    const std::unique_ptr<qc::QuantumComputation>& qc;
    const std::vector<dd::NoiseOperations>         noiseEffects;

    const double noiseProbSingleQubit{};
    const double ampDampingProbSingleQubit{};
    const double noiseProbMultiQubit{};
    const double ampDampingProbMultiQubit{};

    const double measurementThreshold = 0.01;

    const bool sequentiallyApplyNoise{};
    const bool useDensityMatrixType{};
};
