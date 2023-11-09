#include "CircuitSimulator.hpp"
#include "HybridSchrodingerFeynmanSimulator.hpp"
#include "UnitarySimulator.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <memory>

using namespace qc::literals;

class DDVis: public testing::Test {
protected:
    void SetUp() override {
        qc = std::make_unique<qc::QuantumComputation>(2U);
        qc->h(0U);
        qc->cx(0U, 1U);
    }

    std::unique_ptr<qc::QuantumComputation> qc;
    std::size_t                             numShots = 1024U;

    static void fileExistsAndIsNonEmpty(const std::string& filename) {
        std::ifstream file(filename);
        ASSERT_TRUE(file.good());
        ASSERT_NE(file.peek(), std::ifstream::traits_type::eof());
        file.close();
    }
};

TEST_F(DDVis, CircuitSimulator) {
    CircuitSimulator sim(std::move(qc));
    sim.simulate(numShots);
    const auto dot = sim.exportDDtoGraphvizString();
    EXPECT_FALSE(dot.empty());
    std::cout << dot << "\n";

    const std::string filename = "test_circuit_simulator.gv";
    sim.exportDDtoGraphvizFile(filename);

    fileExistsAndIsNonEmpty(filename);
    std::filesystem::remove(filename);
}

TEST_F(DDVis, UnitarySimulator) {
    UnitarySimulator sim(std::move(qc));
    sim.construct();

    const auto dot = sim.exportDDtoGraphvizString(true, false, false, false, true);
    EXPECT_FALSE(dot.empty());
    std::cout << dot << "\n";

    const std::string filename = "test_unitary_simulator.gv";
    sim.exportDDtoGraphvizFile(filename);

    fileExistsAndIsNonEmpty(filename);
    std::filesystem::remove(filename);
}

TEST_F(DDVis, HSFSimulatorDD) {
    HybridSchrodingerFeynmanSimulator sim(std::move(qc), HybridSchrodingerFeynmanSimulator<>::Mode::DD);
    sim.simulate(numShots);

    const auto dot = sim.exportDDtoGraphvizString(true, false, false, false, true);
    EXPECT_FALSE(dot.empty());
    std::cout << dot << "\n";

    const std::string filename = "test_hsf_simulator_dd.gv";
    sim.exportDDtoGraphvizFile(filename);

    fileExistsAndIsNonEmpty(filename);
    std::filesystem::remove(filename);
}

TEST_F(DDVis, HSFSimulatorAmplitude) {
    HybridSchrodingerFeynmanSimulator sim(std::move(qc), HybridSchrodingerFeynmanSimulator<>::Mode::Amplitude);
    sim.simulate(numShots);

    const auto dot = sim.exportDDtoGraphvizString(true, false, false, false, true);
    EXPECT_FALSE(dot.empty());
    std::cout << dot << "\n";

    const std::string filename = "test_hsf_simulator_amplitude.gv";
    sim.exportDDtoGraphvizFile(filename);

    fileExistsAndIsNonEmpty(filename);
    std::filesystem::remove(filename);
}
