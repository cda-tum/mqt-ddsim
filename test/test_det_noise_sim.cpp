#include "DeterministicNoiseSimulator.hpp"

#include <gtest/gtest.h>
#include <memory>

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPDApplySequential) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3, dd::Control::Type::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1, dd::Control::Type::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3, dd::Control::Type::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    DeterministicNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.01, -2, 1, 1, "-1-1000");

    auto m = ddsim.DeterministicSimulate();

    EXPECT_LT(abs(m.find("0000")->second - 0.0616548), 0.00001);
    EXPECT_LT(abs(m.find("0001")->second - 0.0570879), 0.00001);
    EXPECT_LT(abs(m.find("0100")->second - 0.0155602), 0.00001);
    EXPECT_LT(abs(m.find("0101")->second - 0.0157508), 0.00001);
    EXPECT_LT(abs(m.find("0110")->second - 0.0301652), 0.00001);
    EXPECT_LT(abs(m.find("1000")->second - 0.1487735), 0.00001);
    EXPECT_LT(abs(m.find("1001")->second - 0.551925), 0.00001);
    EXPECT_LT(abs(m.find("1011")->second - 0.0132641), 0.00001);
    EXPECT_LT(abs(m.find("1100")->second - 0.0166178), 0.00001);
    EXPECT_LT(abs(m.find("1101")->second - 0.0187341), 0.00001);
    EXPECT_LT(abs(m.find("1110")->second - 0.0301853), 0.00001);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4Track_A) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3, dd::Control::Type::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1, dd::Control::Type::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3, dd::Control::Type::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    DeterministicNoiseSimulator ddsim(quantumComputation, std::string("A"), 0.01, -1, 1, 1, "-1-1000");

    auto m = ddsim.DeterministicSimulate();

    EXPECT_LT(abs(m.find("0000")->second - 0.0496435), 0.00001);
    EXPECT_LT(abs(m.find("0001")->second - 0.0454906), 0.00001);
    EXPECT_LT(abs(m.find("0100")->second - 0.0107246), 0.00001);
    EXPECT_LT(abs(m.find("0101")->second - 0.0108673), 0.00001);
    EXPECT_LT(abs(m.find("0110")->second - 0.031149), 0.00001);
    EXPECT_LT(abs(m.find("1000")->second - 0.0355728), 0.00001);
    EXPECT_LT(abs(m.find("1001")->second - 0.7588863), 0.00001);
    EXPECT_LT(abs(m.find("1101")->second - 0.0101381), 0.00001);
    EXPECT_LT(abs(m.find("1110")->second - 0.0287471), 0.00001);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4TrackAPD) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3, dd::Control::Type::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1, dd::Control::Type::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3, dd::Control::Type::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    DeterministicNoiseSimulator ddsim(quantumComputation, std::string("APD"), 0.01, -1, 1, 1, "-1-1000");

    auto m = ddsim.DeterministicSimulate();

    EXPECT_LT(abs(m.find("0000")->second - 0.0616548), 0.00001);
    EXPECT_LT(abs(m.find("0001")->second - 0.0570879), 0.00001);
    EXPECT_LT(abs(m.find("0100")->second - 0.0155602), 0.00001);
    EXPECT_LT(abs(m.find("0101")->second - 0.0157508), 0.00001);
    EXPECT_LT(abs(m.find("0110")->second - 0.0301652), 0.00001);
    EXPECT_LT(abs(m.find("1000")->second - 0.1487735), 0.00001);
    EXPECT_LT(abs(m.find("1001")->second - 0.551925 ), 0.00001);
    EXPECT_LT(abs(m.find("1011")->second - 0.0132641), 0.00001);
    EXPECT_LT(abs(m.find("1100")->second - 0.0166178), 0.00001);
    EXPECT_LT(abs(m.find("1101")->second - 0.0187341), 0.00001);
    EXPECT_LT(abs(m.find("1110")->second - 0.0301853), 0.00001);
}

TEST(DeterministicNoiseSimTest, SimulateAdder4Track_D) {
    auto quantumComputation = std::make_unique<qc::QuantumComputation>(4);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3, dd::Control::Type::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{1, dd::Control::Type::pos}}, 2, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 0, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 1, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 2, qc::Tdag);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::T);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{0, dd::Control::Type::pos}}, 1, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{2, dd::Control::Type::pos}}, 3, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::S);
    quantumComputation->emplace_back<qc::StandardOperation>(4, dd::Controls{dd::Control{3, dd::Control::Type::pos}}, 0, qc::X);
    quantumComputation->emplace_back<qc::StandardOperation>(4, 3, qc::H);
    DeterministicNoiseSimulator ddsim(quantumComputation, std::string("D"), 0.01, -1, 1, 1, "-1-1000");

    auto m = ddsim.DeterministicSimulate();

    EXPECT_LT(abs(m.find("0000")->second - 0.020155),  0.00001);
    EXPECT_LT(abs(m.find("0001")->second - 0.0199196), 0.00001);
    EXPECT_LT(abs(m.find("1000")->second - 0.0482997), 0.00001);
    EXPECT_LT(abs(m.find("1001")->second - 0.8264367), 0.00001);
    EXPECT_LT(abs(m.find("1011")->second - 0.0172063), 0.00001);
    EXPECT_LT(abs(m.find("1101")->second - 0.011233), 0.00001);
    EXPECT_LT(abs(m.find("1110")->second - 0.0129619), 0.00001);

}