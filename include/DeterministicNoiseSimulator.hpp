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

class DeterministicNoiseSimulator: public Simulator {
public:
    //    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const unsigned int step_number, const double step_fidelity):
    //        qc(qc), step_number(step_number), step_fidelity(step_fidelity) {
    //        if (step_number == 0) {
    //            throw std::invalid_argument("step_number has to be greater than zero");
    //        }
    //    }

    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, unsigned long long seed):
        Simulator(seed), qc(qc) {
    }

    DeterministicNoiseSimulator(std::unique_ptr<qc::QuantumComputation>& qc, const std::string& noise_effects, double noise_prob):
        qc(qc) {
        setNoiseEffects(noise_effects);
        setAmplitudeDampingProbability(noise_prob);
    }

    void setAmplitudeDampingProbability(double cGateNoiseProbability) {
        //The probability of amplitude damping (t1) often is double the probability , of phase flip, which is why I double it here
        noise_probability                            = cGateNoiseProbability;
        sqrt_amplitude_damping_probability           = {sqrt(noise_probability * 2), 0};
        one_minus_sqrt_amplitude_damping_probability = {sqrt(1 - noise_probability * 2), 0};
    }

    std::map<std::string, std::size_t> Simulate([[maybe_unused]] unsigned int shots) override{};

    std::map<std::string, double> DeterministicSimulate();

    //    std::map<std::string, std::string> AdditionalStatistics() override {
    //        return {
    //                {"step_fidelity", std::to_string(step_fidelity)},
    //                {"approximation_runs", std::to_string(approximation_runs)},
    //                {"final_fidelity", std::to_string(final_fidelity)},
    //        };
    //    };

    [[nodiscard]] std::string intToString(long target_number, char value) const;

    void apply_det_noise_sequential(const qc::Targets& targets);

    std::map<std::string, double> AnalyseState(int nr_qubits, bool full_state);

    void setNoiseEffects(const std::string& cGateNoise) { gate_noise_types = cGateNoise; }

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    const std::map<char, int> noise_effects = {
            {'B', 2}, //Bit-flip
            {'P', 2}, //Phase-flip
            {'A', 2}, //Amplitude Damping
            {'D', 4}, //Depolarisation
    };

    double           noise_probability = 0.0;
    dd::ComplexValue sqrt_amplitude_damping_probability{};
    dd::ComplexValue one_minus_sqrt_amplitude_damping_probability{};

    dd::Package::mEdge density_root_edge;

    bool weird_value_i_dont_understand_but_something_with_sequential = false;
    char MeasureOneCollapsing(dd::Qubit index);

private:
    static constexpr unsigned short NoiseSLOTS = 16384;
    static constexpr unsigned short NoiseMASK  = NoiseSLOTS - 1;
    struct NoiseEntry {
        dd::Package::mNode *   a, *r; // a is the argument, r is the result
        dd::ComplexValue       aw, rw;
        dd::NoiseOperationKind which; // type of operation
        signed char            usedQubits[std::numeric_limits<dd::Qubit>::max() + 1];
    };

    std::unique_ptr<qc::QuantumComputation>& qc;

    std::string gate_noise_types;

    std::array<NoiseEntry, NoiseSLOTS> NoiseTable{};

    //    const unsigned int step_number;
    //    const double       step_fidelity;
    //    unsigned long long approximation_runs{0};
    //    long double        final_fidelity{1.0L};

    void ApplyAmplitudeDampingToNode(std::array<dd::Package::mEdge, 4>& e);

    void ApplyPhaseFlipToNode(std::array<dd::Package::mEdge, 4>& e);

    void ApplyDepolaritationToNode(std::array<dd::Package::mEdge, 4>& e);

    void generate_gate(dd::Package::mEdge* pointer_for_matrices, char noise_type, dd::Qubit target);

    dd::Package::mEdge makeZeroDensityOperator(dd::QubitCount n);

    dd::Package::mEdge   ApplyNoiseEffects(dd::Package::mEdge density_op, const std::unique_ptr<qc::Operation>& op, unsigned char maxDepth);
    static unsigned long noiseHash(dd::Package::mNode* a, const dd::ComplexValue& aw, const std::vector<signed char>& usedQubits);
    dd::Package::mEdge   noiseLookup(const dd::Package::mEdge& a, const std::vector<signed char>& usedQubits);
    void                 noiseInsert(const dd::Package::mEdge& a, const std::vector<signed char>& usedQubits, const dd::Package::mEdge& r);
    dd::fp               probForIndexToBeZero(dd::Package::mEdge e, dd::Qubit index, dd::fp pathProb, dd::fp global_prob);
};

#endif //DDSIM_DETERMINISTICNOISESIMULATOR_HPP
