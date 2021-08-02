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
    using CN = dd::ComplexNumbers;

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
        noiseProbability   = cGateNoiseProbability;
        noiseProbFromTable = dd->cn.lookup(noiseProbability, 0);
        CN::incRef(noiseProbFromTable);
        noiseProbFromTableAmp = dd->cn.lookup(noiseProbability * 2, 0);
        CN::incRef(noiseProbFromTableAmp);
        oneMinusNoiseProbFromTableAmp = dd->cn.lookup(1 - noiseProbability * 2, 0);
        CN::incRef(oneMinusNoiseProbFromTableAmp);
        sqrtOneMinusNoiseProbFromTableAmp = dd->cn.lookup(std::sqrt(1 - noiseProbability * 2), 0);
        CN::incRef(sqrtOneMinusNoiseProbFromTableAmp);
        oneMinusNoiseTwoProbFromTable = dd->cn.lookup(1 - noiseProbability * 2, 0);
        CN::incRef(oneMinusNoiseTwoProbFromTable);
        twoMinusNoiseProbFromTable = dd->cn.lookup(2 - noiseProbability, 0);
        CN::incRef(twoMinusNoiseProbFromTable);
        oneMinusNoiseProbFromTable = dd->cn.lookup(1 - noiseProbability, 0);
        CN::incRef(oneMinusNoiseProbFromTable);
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

    void applyDetNoiseSequential(const qc::Targets& targets);

    [[nodiscard]] std::map<std::string, double> AnalyseState(dd::QubitCount nr_qubits, bool full_state) const;

    void setNoiseEffects(const std::string& cGateNoise) { gateNoiseTypes = cGateNoise; }

    [[nodiscard]] dd::QubitCount getNumberOfQubits() const override { return qc->getNqubits(); };

    [[nodiscard]] std::size_t getNumberOfOps() const override { return qc->getNops(); };

    [[nodiscard]] std::string getName() const override { return qc->getName(); };

    const std::map<char, int> noiseEffects = {
            {'B', 2}, //Bit-flip
            {'P', 2}, //Phase-flip
            {'A', 2}, //Amplitude Damping
            {'D', 4}, //Depolarisation
    };

    double      noiseProbability                  = 0.0;
    dd::Complex noiseProbFromTable                = {};
    dd::Complex noiseProbFromTableAmp             = {};
    dd::Complex oneMinusNoiseProbFromTableAmp     = {};
    dd::Complex sqrtOneMinusNoiseProbFromTableAmp = {};
    dd::Complex oneMinusNoiseTwoProbFromTable     = {};
    dd::Complex twoMinusNoiseProbFromTable     = {};
    dd::Complex oneMinusNoiseProbFromTable     = {};

    //todo implement a new structure for density matrices
    qc::MatrixDD density_root_edge{};

    bool sequentialApplyNoise = false;
    char MeasureOneCollapsing(dd::Qubit index);

private:
    // TODO integrate the NoiseCacheTable into the ddPackage
    static constexpr unsigned short NoiseSLOTS = 16384;
    static constexpr unsigned short NoiseMASK  = NoiseSLOTS - 1;
    struct NoiseEntry {
        dd::Package::mNode *   a, *r; // a is the argument, r is the result
        dd::ComplexValue       aw, rw;
        dd::NoiseOperationKind which; // type of operation
        signed char            usedQubits[std::numeric_limits<dd::Qubit>::max() + 1];
        // TODO change to a more compact data structure
    };

    std::unique_ptr<qc::QuantumComputation>& qc;

    std::string gateNoiseTypes;

    std::array<NoiseEntry, NoiseSLOTS> NoiseTable{};

    //    const unsigned int step_number;Q
    //    const double       step_fidelity;
    //    unsigned long long approximation_runs{0};
    //    long double        final_fidelity{1.0L};

    void ApplyAmplitudeDampingToNode(std::array<qc::MatrixDD, 4>& e);

    void ApplyPhaseFlipToNode(std::array<qc::MatrixDD, 4>& e);

    void ApplyDepolaritationToNode(std::array<qc::MatrixDD, 4>& e);

    void generateGate(qc::MatrixDD* pointer_for_matrices, char noise_type, dd::Qubit target);

    qc::MatrixDD makeZeroDensityOperator(dd::QubitCount n);

    qc::MatrixDD         ApplyNoiseEffects(qc::MatrixDD density_op, const std::unique_ptr<qc::Operation>& op, unsigned char maxDepth);
    static unsigned long noiseHash(dd::Package::mNode* a, const dd::ComplexValue& aw, const std::vector<signed char>& usedQubits);
    qc::MatrixDD         noiseLookup(const qc::MatrixDD& a, const std::vector<signed char>& usedQubits);
    void                 noiseInsert(const qc::MatrixDD& a, const std::vector<signed char>& usedQubits, const qc::MatrixDD& r);
    dd::fp               probForIndexToBeZero(qc::MatrixDD e, dd::Qubit index, dd::fp pathProb, dd::fp global_prob);
};

#endif //DDSIM_DETERMINISTICNOISESIMULATOR_HPP
