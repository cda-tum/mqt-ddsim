#ifndef DDSIM_DETERMINISTICNOISESIMULATOR_HPP
#define DDSIM_DETERMINISTICNOISESIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"
#include "dd/NoiseFunctionality.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

class DeterministicNoiseSimulator: public Simulator<DensityMatrixPackage> {
    using CN    = dd::ComplexNumbers;
    using dEdge = dd::dEdge;
    using dNode = dd::dNode;

public:
    explicit DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, unsigned long long seed = 0):
        DeterministicNoiseSimulator(qc, std::string("APD"), 0.001, -1, 2, false, seed) {}

    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc,
                                const std::string&                       cNoiseEffects,
                                double                                   cNoiseProbability,
                                double                                   cAmpDampingProbability,
                                double                                   cMultiQubitGateFactor,
                                bool                                     unoptimizedSim = false,
                                unsigned long long                       seed           = 0):
        Simulator(seed),
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
                default:
                    throw std::runtime_error("Unknown noise operation '" + cNoiseEffects + "'\n");
            }
        }

        // initializeNoiseProbabilities
        noiseProbSingleQubit = cNoiseProbability;
        noiseProbMultiQubit  = cNoiseProbability * cMultiQubitGateFactor;

        if (cAmpDampingProbability < 0) {
            // Default value for amplitude damping prob is double the general error probability
            ampDampingProbSingleQubit = cNoiseProbability * 2;
            ampDampingProbMultiQubit  = cNoiseProbability * 2 * cMultiQubitGateFactor;
        } else {
            ampDampingProbSingleQubit = cAmpDampingProbability;
            ampDampingProbMultiQubit  = cAmpDampingProbability * cMultiQubitGateFactor;
        }

        if (noiseProbSingleQubit < 0 || ampDampingProbSingleQubit < 0 || ampDampingProbMultiQubit > 1 || noiseProbMultiQubit > 1) {
            throw std::runtime_error("Error probabilities are faulty!"
                                     "\n single qubit error probability: " +
                                     std::to_string(noiseProbSingleQubit) +
                                     " multi qubit error probability: " + std::to_string(noiseProbMultiQubit) +
                                     "\n single qubit amplitude damping  probability: " + std::to_string(ampDampingProbSingleQubit) +
                                     " multi qubit amplitude damping  probability: " + std::to_string(ampDampingProbMultiQubit));
        }
    }

    std::map<std::string, std::size_t> Simulate([[maybe_unused]] unsigned int shots) override { return {}; };

    std::map<std::string, double> DeterministicSimulate();

    [[nodiscard]] std::string intToString(long targetNumber, char value) const;

    [[nodiscard]] std::map<std::string, double> analyseState(dd::QubitCount nQubits, bool fullState);

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    [[nodiscard]] std::size_t getActiveNodeCount() const override { return dd->dUniqueTable.getActiveNodeCount(); }
    [[nodiscard]] std::size_t getMaxNodeCount() const override { return dd->dUniqueTable.getMaxActiveNodes(); }
    [[nodiscard]] std::size_t getMaxMatrixNodeCount() const override { return dd->mUniqueTable.getMaxActiveNodes(); }
    [[nodiscard]] std::size_t getMatrixActiveNodeCount() const override { return dd->mUniqueTable.getActiveNodeCount(); }
    [[nodiscard]] std::size_t countNodesFromRoot() const override { return dd->size(root_edge); }

    dEdge densityRootEdge{};

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

    const std::map<dd::NoiseOperations, int> sequentialNoiseMap = {
            {dd::phaseFlip, 2},        //Phase-flip
            {dd::amplitudeDamping, 2}, //Amplitude Damping
            {dd::depolarization, 4},   //Depolarisation
    };

    void   applyAmplitudeDampingToEdges(std::array<dEdge, std::tuple_size_v<decltype(dd::dNode::e)>>& e, double probability);
    void   applyPhaseFlipToEdges(std::array<dEdge, std::tuple_size_v<decltype(dd::dNode::e)>>& e, double probability);
    void   applyDepolarisationToEdges(std::array<dEdge, std::tuple_size_v<decltype(dd::dNode::e)>>& e, double probability);
    dEdge  makeZeroDensityOperator(dd::QubitCount n);
    dEdge  applyNoiseEffects(dEdge& originalEdge, const std::vector<dd::Qubit>& usedQubits, bool firstPathEdge);
    void   applyDetNoiseSequential(const qc::Targets& targets);
    void   generateGate(qc::MatrixDD* pointerForMatrices, dd::NoiseOperations noiseType, dd::Qubit target, double probability);
    double getNoiseProbability(dd::NoiseOperations type, const qc::Targets& targets);
};

#endif //DDSIM_DETERMINISTICNOISESIMULATOR_HPP
