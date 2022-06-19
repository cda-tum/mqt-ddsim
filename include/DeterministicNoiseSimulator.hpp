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

class DeterministicNoiseSimulator: public Simulator<dd::DensityMatrixSimulatorDDPackage> {
    using CN    = dd::ComplexNumbers;
    using dEdge = dd::dEdge;

public:
    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, unsigned long long seed):
        Simulator(seed), qc(qc) {
    }

    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const std::string& noise_effects, double noise_prob):
        qc(qc) {
        setNoiseEffects(noise_effects);
        initializeNoiseProbabilities(noise_prob);
    }

    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const std::string& noise_effects, double noise_prob, double amp_noise_prob, double multiQubitGateFactor):
        qc(qc) {
        setNoiseEffects(noise_effects);
        initializeNoiseProbabilities(noise_prob, amp_noise_prob, multiQubitGateFactor);
    }

    double noiseProb                 = 0.0;
    double ampDampingProb            = 0.0;
    double noiseProbSingleQubit      = 0.0;
    double ampDampingProbSingleQubit = 0.0;
    double noiseProbMultiQubit       = 0.0;
    double ampDampingProbMultiQubit  = 0.0;

    void initializeNoiseProbabilities(double cGateNoiseProbability, double cAmplitudeDampingProb = -1, double cMultiQubitGateFactor = 2) {
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

    std::map<std::string, std::size_t> Simulate([[maybe_unused]] unsigned int shots) override {
        return {};
    };

    std::map<std::string, double> DeterministicSimulate();

    [[nodiscard]] std::string intToString(long target_number, char value) const;

    void applyDetNoiseSequential(const qc::Targets& targets);

    [[nodiscard]] std::map<std::string, double> AnalyseState(dd::QubitCount nr_qubits, bool full_state);

    void setNoiseEffects(const std::string& cGateNoise) {
        for (const auto& effect: cGateNoise) {
            if (effect != 'A' && effect != 'P' && effect != 'D') {
                throw std::runtime_error("Unknown noise operation '" + std::to_string(effect) + "'\n");
            }
        }
        gateNoiseTypes = cGateNoise;
    }

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    const std::map<char, int> gateNoiseEffects = {
            {'B', 2}, //Bit-flip
            {'P', 2}, //Phase-flip
            {'A', 2}, //Amplitude Damping
            {'D', 4}, //Depolarisation
    };

    dEdge density_root_edge{};

    bool sequentialApplyNoise    = false;
    bool use_density_matrix_type = true;
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
