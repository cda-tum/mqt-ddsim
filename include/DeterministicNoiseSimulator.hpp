#ifndef DDSIM_DETERMINISTICNOISESIMULATOR_HPP
#define DDSIM_DETERMINISTICNOISESIMULATOR_HPP

#include "QuantumComputation.hpp"
#include "Simulator.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

class DeterministicNoiseSimulator: public Simulator<dd::DensityMatrix> {
    using CN    = dd::ComplexNumbers;
    using dEdge = dd::dEdge;

public:
    explicit DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, unsigned long long seed = 0):
        DeterministicNoiseSimulator(qc, std::string("APD"), 0.001, -1, 2, false, seed) {}

    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc,
                                const std::string&                       cGateNoise,
                                double                                   cGateNoiseProbability,
                                double                                   cAmplitudeDampingProb,
                                double                                   cMultiQubitGateFactor,
                                bool                                     unoptimizedSim = false,
                                unsigned long long                       seed           = 0):
        Simulator(seed),
        qc(qc) {
        useDensityMatrixType = !unoptimizedSim;
        sequentialApplyNoise = unoptimizedSim;

        // setNoiseEffects
        if (cGateNoise.find_first_not_of("APD") != std::string::npos) {
            throw std::runtime_error("Unknown noise operation in '" + cGateNoise + "'\n");
        }
        gateNoiseTypes = cGateNoise;

        // initializeNoiseProbabilities
        noiseProb            = cGateNoiseProbability;
        noiseProbSingleQubit = cGateNoiseProbability;
        noiseProbMultiQubit  = cGateNoiseProbability * cMultiQubitGateFactor;

        if (cAmplitudeDampingProb < 0) {
            // Default value for amplitude damping prob is double the general error probability
            ampDampingProb            = cGateNoiseProbability * 2;
            ampDampingProbSingleQubit = cGateNoiseProbability * 2;
            ampDampingProbMultiQubit  = cGateNoiseProbability * 2 * cMultiQubitGateFactor;
        } else {
            ampDampingProb            = cAmplitudeDampingProb;
            ampDampingProbSingleQubit = cAmplitudeDampingProb;
            ampDampingProbMultiQubit  = cAmplitudeDampingProb * cMultiQubitGateFactor;
        }

        if (noiseProb < 0 || ampDampingProb < 0 || ampDampingProbMultiQubit > 1 || noiseProbMultiQubit > 1) {
            throw std::runtime_error("Error probabilities are faulty!"
                                     "\n single qubit error probability: " +
                                     std::to_string(noiseProbSingleQubit) +
                                     " multi qubit error probability: " + std::to_string(noiseProbMultiQubit) +
                                     "\n single qubit amplitude damping  probability: " + std::to_string(ampDampingProbSingleQubit) +
                                     " multi qubit amplitude damping  probability: " + std::to_string(ampDampingProbMultiQubit));
        }
    }

    double noiseProb                 = 0.0;
    double ampDampingProb            = 0.0;
    double noiseProbSingleQubit      = 0.0;
    double ampDampingProbSingleQubit = 0.0;
    double noiseProbMultiQubit       = 0.0;
    double ampDampingProbMultiQubit  = 0.0;

    std::map<std::string, std::size_t> Simulate([[maybe_unused]] unsigned int shots) override { return {}; };

    std::map<std::string, double> DeterministicSimulate();

    [[nodiscard]] std::string intToString(long target_number, char value) const;

    void applyDetNoiseSequential(const qc::Targets& targets);

    [[nodiscard]] std::map<std::string, double> AnalyseState(dd::QubitCount nr_qubits, bool full_state);

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    const std::map<char, int> gateNoiseEffects = {
            {'B', 2}, //Bit-flip
            {'P', 2}, //Phase-flip
            {'A', 2}, //Amplitude Damping
            {'D', 4}, //Depolarisation
    };

    dEdge densityRootEdge{};

    bool sequentialApplyNoise = false;
    bool useDensityMatrixType = true;
    //    char MeasureOneCollapsing(dd::Qubit index);

private:
    std::unique_ptr<qc::QuantumComputation>& qc;

    std::string gateNoiseTypes;

    void ApplyAmplitudeDampingToNode(std::array<dEdge, 4>& e, double probability);
    void ApplyPhaseFlipToNode(std::array<dEdge, 4>& e, double probability);
    void ApplyDepolarisationToNode(std::array<dEdge, 4>& e, double probability);

    void generateGate(qc::MatrixDD* pointer_for_matrices, char noise_type, dd::Qubit target, double probability);

    dEdge makeZeroDensityOperator(dd::QubitCount n);

    dEdge applyNoiseEffects(dEdge& originalEdge, const std::vector<dd::Qubit>& used_qubits, bool firstPathEdge);

    //    dd::fp probForIndexToBeZero(dEdge e, dd::Qubit index, dd::fp pathProb, dd::fp global_prob);
};

#endif //DDSIM_DETERMINISTICNOISESIMULATOR_HPP
