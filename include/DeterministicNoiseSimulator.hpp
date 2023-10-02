#pragma once

#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "StochasticNoiseSimulator.hpp"
#include "dd/NoiseFunctionality.hpp"
#include "dd/Package.hpp"

template<class Config = DensityMatrixSimulatorDDPackageConfig>
class DeterministicNoiseSimulator: public Simulator<Config> {
public:
    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                                const std::string&                        noiseEffects_,
                                double                                    noiseProbability,
                                std::optional<double>                     ampDampingProbability,
                                double                                    multiQubitGateFactor,
                                bool                                      unoptimizedSim = false,
                                std::uint64_t                             seed_          = 0):
        Simulator<Config>(seed_),
        qc(std::move(qc_)),
        noiseEffects(StochasticNoiseSimulator<StochasticNoiseSimulatorDDPackageConfig>::initializeNoiseEffects(noiseEffects_)),
        noiseProbSingleQubit(noiseProbability),
        ampDampingProbSingleQubit(ampDampingProbability ? ampDampingProbability.value() : noiseProbability * 2),
        noiseProbMultiQubit(noiseProbability * multiQubitGateFactor),
        ampDampingProbMultiQubit(ampDampingProbSingleQubit * multiQubitGateFactor),
        sequentiallyApplyNoise(unoptimizedSim),
        useDensityMatrixType(!unoptimizedSim) {
        StochasticNoiseSimulator<StochasticNoiseSimulatorDDPackageConfig>::sanityCheckOfNoiseProbabilities(noiseProbability, ampDampingProbSingleQubit, multiQubitGateFactor);
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    explicit DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, std::uint64_t seed_ = 0):
        DeterministicNoiseSimulator(std::move(qc_), std::string("APD"), 0.001, std::optional<double>{}, 2, false, seed_) {}

    std::map<std::string, std::size_t> simulate(size_t shots) override {
        return sampleFromProbabilityMap(deterministicSimulate(), shots);
    };

    dd::SparsePVecStrKeys deterministicSimulate();

    std::map<std::string, std::size_t> sampleFromProbabilityMap(const dd::SparsePVecStrKeys& resultProbabilityMap, std::size_t shots);

    [[nodiscard]] std::size_t getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    [[nodiscard]] std::size_t getActiveNodeCount() const override { return Simulator<Config>::dd->template getUniqueTable<dd::dNode>().getNumActiveEntries(); }
    [[nodiscard]] std::size_t getMaxNodeCount() const override { return Simulator<Config>::dd->template getUniqueTable<dd::dNode>().getPeakNumActiveEntries(); }

    [[nodiscard]] std::size_t countNodesFromRoot() override {
        if (useDensityMatrixType) {
            qc::DensityMatrixDD::alignDensityEdge(rootEdge);
            const std::size_t tmp = rootEdge.size();
            qc::DensityMatrixDD::setDensityMatrixTrue(rootEdge);
            return tmp;
        }
        return rootEdge.size();
    }

    qc::DensityMatrixDD rootEdge{};

private:
    std::unique_ptr<qc::QuantumComputation> qc;
    std::vector<dd::NoiseOperations>        noiseEffects;

    double noiseProbSingleQubit{};
    double ampDampingProbSingleQubit{};
    double noiseProbMultiQubit{};
    double ampDampingProbMultiQubit{};

    double measurementThreshold = 0.01;

    bool sequentiallyApplyNoise{};
    bool useDensityMatrixType{};
};
