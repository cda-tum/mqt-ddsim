#pragma once

#include "Definitions.hpp"
#include "MBQC.hpp"
#include "dd/Definitions.hpp"
#include "dd/Node.hpp"
#include "dd/Package.hpp"

#include <map>
#include <memory>

template<class DDPackage = dd::Package<>>
class MBQCSimulator {
public:
    MBQCSimulator(std::unique_ptr<zx::MBQCPattern>&& pattern):
        pattern(std::move(pattern)) {}

    std::map<std::string, std::size_t> Simulate(unsigned int shots);

private:
    std::unique_ptr<zx::MBQCPattern> pattern;
    std::unique_ptr<DDPackage>       dd = std::make_unique<DDPackage>();

    std::vector<zx::Vertex>              measurementOrder(const zx::flow& flow) const;
    std::vector<std::vector<zx::Vertex>> entanglementOrder(const zx::flow& flow) const;

    dd::vEdge preparePlusStates(std::size_t nQubits);
    dd::vEdge applyCZ(dd::vEdge& root, const dd::Qubit ctrl, const dd::Qubit tar);
    dd::vEdge entangleNextLayer(dd::vEdge& root, const std::vector<zx::Vertex>& toEntangle, std::vector<bool>& entangled, const std::vector<zx::Vertex>& vertexToQubitMap);
};
