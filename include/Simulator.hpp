#ifndef DDSIMULATOR_H
#define DDSIMULATOR_H

#include <memory>
#include <string>
#include <array>
#include <map>
#include <random>
#include <utility>
#include <unordered_map>
#include <thread>

#include "DDpackage.h"

class Simulator {
public:
    explicit Simulator(unsigned long long seed) : seed(seed), has_fixed_seed(true) {
        mt.seed(seed);
    };

    explicit Simulator() : seed(0), has_fixed_seed(false) {
        // this is probably overkill but better safe than sorry
        std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> random_data{};
        std::random_device rd;
        std::generate(std::begin(random_data), std::end(random_data), [&rd]() { return rd(); });
        std::seed_seq seeds(std::begin(random_data), std::end(random_data));
        mt.seed(seeds);
    };

    virtual ~Simulator() = default;

    virtual std::map<std::string, unsigned int> Simulate(unsigned int shots) = 0;
    virtual std::map<std::string, double> StochSimulate() = 0;

    virtual std::map<std::string, std::string> AdditionalStatistics() { return {}; };

    std::string MeasureAll(bool collapse = false);
    std::map<std::string, unsigned int> MeasureAllNonCollapsing(unsigned int shots);
    char MeasureOneCollapsing(unsigned short index, bool assume_probability_normalization = true);

    dd::Edge MeasureOneCollapsingConcurrent(unsigned short index, const std::unique_ptr<dd::Package> &localDD,
                                            dd::Edge local_root_edge,
                                            std::default_random_engine &generator,
                                            char *result,
                                            bool assume_probability_normalization = true);

    // TODO: void ResetOne(unsigned short index);
    // TODO: void ResetAll();
    std::vector<dd::ComplexValue> getVector() const;

    unsigned long getNodeCount() const { return dd->activeNodeCount; }
    unsigned long getMaxNodeCount() const { return dd->maxActive; }
    std::pair<dd::ComplexValue, std::string> getPathOfLeastResistance();

    std::string getSeed() const { return has_fixed_seed ? std::to_string(seed) : "-1"; }

    virtual unsigned short getNumberOfQubits() const = 0;
    virtual unsigned long getNumberOfOps() const = 0;
    virtual std::string getName() const = 0;

    double ApproximateByFidelity(double targetFidelity, bool removeNodes);
    double ApproximateBySampling(int nSamples, int threshold, bool removeNodes);
    dd::Edge RemoveNodes(dd::Edge edge, std::map<dd::NodePtr, dd::Edge> &dag_edges);

    std::unique_ptr<dd::Package> dd = std::make_unique<dd::Package>();
    dd::Edge root_edge{};

    // 
    void setNoiseEffects(char *cGateNoise) { strncpy(gate_noise, cGateNoise, 4); }

    double noise_probability = 0.0;
    dd::ComplexValue sqrt_amplitude_damping_probability;
    dd::ComplexValue one_minus_sqrt_amplitude_damping_probability;


    void setAmplitudeDampingProbability(double cGateNoiseProbability) {
        //The probability of amplitude damping (t1) often is double the probability , of phase flip, which is why I double it here
        noise_probability = cGateNoiseProbability;
        sqrt_amplitude_damping_probability = {sqrt(noise_probability * 2), 0};
        one_minus_sqrt_amplitude_damping_probability = {sqrt(1 - noise_probability * 2), 0};
    }

    double stoch_error_margin = 0.01;
    double stoch_confidence = 0.05;
    long stochastic_runs = 0;

    void setRecordedProperties(const std::string &input);

    std::vector<std::tuple<long, std::string>> recorded_properties;
    std::vector<std::vector<double> > recorded_properties_per_instance;

    char gate_noise[5] = {0};

    unsigned short max_instances = std::thread::hardware_concurrency();


protected:
    std::mt19937_64 mt;

    const unsigned long long seed;
    const bool has_fixed_seed;
    const fp epsilon = 0.001L;

    static void NextPath(std::string &s);

    double assign_probs(dd::Edge edge, std::unordered_map<dd::NodePtr, double> &probs);

    dd::CTkind ReturnNoiseOperation(char i, double d) const;

    std::string intToString(long target_number) const;
};


#endif //DDSIMULATOR_H
