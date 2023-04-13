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
                                const std::string&                        noiseEffects,
                                double                                    noiseProbability,
                                std::optional<double>                     ampDampingProbability,
                                double                                    multiQubitGateFactor,
                                bool                                      unoptimizedSim = false,
                                bool                                      unoptimizedDm  = false,
                                std::uint64_t                             seed           = 0):
        Simulator<Config>(seed),
        qc(std::move(qc_)),
        noiseEffects(StochasticNoiseSimulator<StochasticNoiseSimulatorDDPackageConfig>::initializeNoiseEffects(noiseEffects)),
        noiseProbSingleQubit(noiseProbability),
        ampDampingProbSingleQubit(ampDampingProbability ? ampDampingProbability.value() : noiseProbability * 2),
        noiseProbMultiQubit(noiseProbability * multiQubitGateFactor),
        ampDampingProbMultiQubit(ampDampingProbSingleQubit * multiQubitGateFactor),
        sequentiallyApplyNoise(unoptimizedSim),
        useDensityMatrixType(!unoptimizedDm) {
        StochasticNoiseSimulator<StochasticNoiseSimulatorDDPackageConfig>::sanityCheckOfNoiseProbabilities(noiseProbability, ampDampingProbSingleQubit, multiQubitGateFactor);
        Simulator<Config>::dd->resize(qc->getNqubits());
    }

    std::mt19937_64 mt;

    std::uint64_t seed = 0;
    bool          hasFixedSeed;
    dd::fp        epsilon = 0.001;


    explicit DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc, std::uint64_t seed = 0):
        DeterministicNoiseSimulator(std::move(qc), std::string("APD"), 0.001, std::optional<double>{}, 2, false, seed) {}

    std::map<std::string, std::size_t> simulate(size_t shots) override {
        return deterministicSimulate(shots);
    };

    std::string measureAll2() {
        return Simulator<Config>::dd->measureAll(rootEdge, mt, epsilon);
    }

    std::map<std::string, std::size_t> measureAllNonCollapsing2(std::size_t shots) {
        std::map<std::string, std::size_t> results;
        dd::dEdge::alignDensityEdge(rootEdge);
        for (std::size_t i = 0; i < shots; i++) {
            const auto m = measureAll2();
            results[m]++;
        }
        dd::dEdge::setDensityMatrixTrue(rootEdge);
        return results;
    }

    std::map<std::string, std::size_t> deterministicSimulate(std::size_t shots);

    [[nodiscard]] std::size_t getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    [[nodiscard]] std::size_t getActiveNodeCount() const override { return Simulator<Config>::dd->dUniqueTable.getActiveNodeCount(); }
    [[nodiscard]] std::size_t getMaxNodeCount() const override { return Simulator<Config>::dd->dUniqueTable.getMaxActiveNodes(); }

    [[nodiscard]] std::size_t countNodesFromRoot() override {
        if (useDensityMatrixType) {
            qc::DensityMatrixDD::alignDensityEdge(rootEdge);
            const std::size_t tmp = Simulator<Config>::dd->size(rootEdge);
            qc::DensityMatrixDD::setDensityMatrixTrue(rootEdge);
            return tmp;
        }
        return Simulator<Config>::dd->size(rootEdge);
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
