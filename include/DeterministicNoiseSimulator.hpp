#pragma once

#include "CircuitSimulator.hpp"
#include "QuantumComputation.hpp"
#include "StochasticNoiseSimulator.hpp"
#include "dd/NoiseFunctionality.hpp"
#include "dd/Package.hpp"

template<class Config = dd::DensityMatrixSimulatorDDPackageConfig>
class DeterministicNoiseSimulator: public CircuitSimulator<Config> {
public:
    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_,
                                const std::string&                        noiseEffects_,
                                double                                    noiseProbability,
                                std::optional<double>                     ampDampingProbability,
                                double                                    multiQubitGateFactor,
                                std::uint64_t                             seed_ = 0):
        CircuitSimulator<Config>(std::move(qc_), seed_),
        noiseEffects(StochasticNoiseSimulator<dd::StochasticNoiseSimulatorDDPackageConfig>::initializeNoiseEffects(noiseEffects_)),
        noiseProbSingleQubit(noiseProbability),
        ampDampingProbSingleQubit(ampDampingProbability ? ampDampingProbability.value() : noiseProbability * 2),
        noiseProbMultiQubit(noiseProbability * multiQubitGateFactor),
        ampDampingProbMultiQubit(ampDampingProbSingleQubit * multiQubitGateFactor),
        deterministicNoiseFunctionalityObject(Simulator<Config>::dd,
                                              static_cast<dd::Qubit>(CircuitSimulator<Config>::qc->getNqubits()),
                                              noiseProbSingleQubit,
                                              noiseProbMultiQubit,
                                              ampDampingProbSingleQubit,
                                              ampDampingProbMultiQubit,
                                              noiseEffects) {
        StochasticNoiseSimulator<dd::StochasticNoiseSimulatorDDPackageConfig>::sanityCheckOfNoiseProbabilities(noiseProbability, ampDampingProbSingleQubit, multiQubitGateFactor);
    }

    explicit DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>&& qc_, std::uint64_t seed_ = 0):
        DeterministicNoiseSimulator(std::move(qc_), std::string("APD"), 0.001, std::optional<double>{}, 2, seed_) {}

    std::map<std::string, std::size_t> measureAllNonCollapsing(std::size_t shots) override{
        return sampleFromProbabilityMap(rootEdge.getSparseProbabilityVectorStrKeys(measurementThreshold), shots);
    }

    void initializeSimulation(std::size_t nQubits) override;
    char measure(unsigned int i) override;
    void reset(qc::NonUnitaryOperation* nonUnitaryOp) override;
    void applyOperationToState(std::unique_ptr<qc::Operation>& op) override;

    std::map<std::string, std::size_t> sampleFromProbabilityMap(const dd::SparsePVecStrKeys& resultProbabilityMap, std::size_t shots);

    [[nodiscard]] std::size_t getNumberOfQubits() const override { return CircuitSimulator<Config>::qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return CircuitSimulator<Config>::qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return CircuitSimulator<Config>::qc->getName(); };

    [[nodiscard]] std::size_t getActiveNodeCount() const override { return Simulator<Config>::dd->template getUniqueTable<dd::dNode>().getNumActiveEntries(); }
    [[nodiscard]] std::size_t getMaxNodeCount() const override { return Simulator<Config>::dd->template getUniqueTable<dd::dNode>().getPeakNumActiveEntries(); }

    [[nodiscard]] std::size_t countNodesFromRoot() override {
        qc::DensityMatrixDD::alignDensityEdge(rootEdge);
        const std::size_t tmp = rootEdge.size();
        qc::DensityMatrixDD::setDensityMatrixTrue(rootEdge);
        return tmp;
    }

    qc::DensityMatrixDD rootEdge{};

private:
    std::vector<dd::NoiseOperations> noiseEffects;

    double noiseProbSingleQubit{};
    double ampDampingProbSingleQubit{};
    double noiseProbMultiQubit{};
    double ampDampingProbMultiQubit{};

    double                                      measurementThreshold = 0.01;
    dd::DeterministicNoiseFunctionality<Config> deterministicNoiseFunctionalityObject;
};
