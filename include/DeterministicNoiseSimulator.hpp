#ifndef DDSIM_DETERMINISTICNOISESIMULATOR_HPP
#define DDSIM_DETERMINISTICNOISESIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "dd/NoiseFunctionality.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>

template<class DDPackage = DensityMatrixPackage>
class DeterministicNoiseSimulator: public Simulator<DDPackage> {
    using dEdge = dd::dEdge;
    using dNode = dd::dNode;

public:
    explicit DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, unsigned long long seed = 0):
        DeterministicNoiseSimulator(qc, std::string("APD"), 0.001, std::optional<double>{}, 2, false, seed) {}

    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc,
                                const std::string&                       cNoiseEffects,
                                double                                   cNoiseProbability,
                                std::optional<double>                    cAmpDampingProbability,
                                double                                   cMultiQubitGateFactor,
                                bool                                     unoptimizedSim = false,
                                unsigned long long                       seed           = 0):
        Simulator<DDPackage>(seed),
        qc(qc),
        noiseProbSingleQubit(cNoiseProbability),
        noiseProbMultiQubit(cNoiseProbability * cMultiQubitGateFactor),
        sequentiallyApplyNoise(unoptimizedSim),
        useDensityMatrixType(!unoptimizedSim) {
        // setNoiseEffects
        for (auto noise: cNoiseEffects) {
            switch (noise) {
                case 'A':
                    noiseEffects.push_back(dd::amplitudeDamping);
                    break;
                case 'P':
                    noiseEffects.push_back(dd::phaseFlip);
                    break;
                case 'D':
                    noiseEffects.push_back(dd::depolarization);
                    break;
                case 'I':
                    noiseEffects.push_back(dd::identity);
                    break;
                default:
                    throw std::runtime_error("Unknown noise operation '" + cNoiseEffects + "'\n");
            }
        }

        // initializeNoiseProbabilities
        noiseProbSingleQubit = cNoiseProbability;
        noiseProbMultiQubit  = cNoiseProbability * cMultiQubitGateFactor;

        if (!cAmpDampingProbability) {
            // Default value for amplitude damping prob is double the general error probability
            ampDampingProbSingleQubit = cNoiseProbability * 2;
            ampDampingProbMultiQubit  = cNoiseProbability * 2 * cMultiQubitGateFactor;
        } else {
            ampDampingProbSingleQubit = cAmpDampingProbability.value();
            ampDampingProbMultiQubit  = cAmpDampingProbability.value() * cMultiQubitGateFactor;
        }

        if (noiseProbSingleQubit < 0 || ampDampingProbSingleQubit < 0 || ampDampingProbMultiQubit > 1 || noiseProbMultiQubit > 1) {
            throw std::runtime_error("Error probabilities are faulty!"
                                     "\n single qubit error probability: " +
                                     std::to_string(noiseProbSingleQubit) +
                                     " multi qubit error probability: " + std::to_string(noiseProbMultiQubit) +
                                     "\n single qubit amplitude damping  probability: " + std::to_string(ampDampingProbSingleQubit) +
                                     " multi qubit amplitude damping  probability: " + std::to_string(ampDampingProbMultiQubit));
        }
        Simulator<DDPackage>::dd->resize(qc->getNqubits());
    }

    std::map<std::string, std::size_t> Simulate([[maybe_unused]] unsigned int shots) override { return {}; };

    std::map<std::string, double> DeterministicSimulate();

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    [[nodiscard]] std::size_t getActiveNodeCount() const override { return Simulator<DDPackage>::dd->dUniqueTable.getActiveNodeCount(); }
    [[nodiscard]] std::size_t getMaxNodeCount() const override { return Simulator<DDPackage>::dd->dUniqueTable.getMaxActiveNodes(); }
    [[nodiscard]] std::size_t getMaxMatrixNodeCount() const override { return Simulator<DDPackage>::dd->mUniqueTable.getMaxActiveNodes(); }
    [[nodiscard]] std::size_t getMatrixActiveNodeCount() const override { return Simulator<DDPackage>::dd->mUniqueTable.getActiveNodeCount(); }
    [[nodiscard]] std::size_t countNodesFromRoot() const override { return Simulator<DDPackage>::dd->size(rootEdge); }

    dEdge rootEdge{};

private:
    std::unique_ptr<qc::QuantumComputation>& qc;
    std::vector<dd::NoiseOperations>         noiseEffects;

    double noiseProbSingleQubit      = 0.0;
    double ampDampingProbSingleQubit = 0.0;
    double noiseProbMultiQubit       = 0.0;
    double ampDampingProbMultiQubit  = 0.0;

    double measurementThreshold = 0.01;

    bool sequentiallyApplyNoise;
    bool useDensityMatrixType;
};

#endif //DDSIM_DETERMINISTICNOISESIMULATOR_HPP
