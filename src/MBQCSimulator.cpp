#include "MBQCSimulator.hpp"

#include "Definitions.hpp"
#include "dd/Control.hpp"
#include "dd/Definitions.hpp"
#include "dd/Node.hpp"
#include "dd/Operations.hpp"
#include "operations/OpType.hpp"
#include "operations/StandardOperation.hpp"

#include <algorithm>

template<class DDPackage>
std::map<std::string, std::size_t> MBQCSimulator<DDPackage>::Simulate(unsigned int shots) {
    const auto&             flow    = pattern->computeGFlow();
    const auto&             measOrd = measurementOrder(flow);
    std::vector<zx::Vertex> verticesToQubitMap(pattern->getNVertices()); //TODO: qubitmap changes
    for (std::size_t i = 0; i < measOrd.size(); ++i) {
        verticesToQubitMap[measOrd[i]] = i;
    }
    const auto&       czOrd = entanglementOrder(flow);
    std::vector<bool> entangled(pattern->getNVertices(), false);

    // entangle initial state with inputs of mbqc pattern
    dd::vEdge rootEdge = dd->makeZeroState(pattern->getInputs().size()); //todo can be arbitrary state
    MBQCSimulator<DDPackage>::dd->incRef(rootEdge);

    const auto& secondLayer = pattern->getConnectedSet(pattern->getInputs());
    // std::sort(secondLayer.begin(), secondLayer.end(), [&](const auto& v, const auto& w) { return measOrd[v] < measOrd[w]; });

    dd::vEdge secondLayerQubits = preparePlusStates(secondLayer.size());
    MBQCSimulator<DDPackage>::dd->kronecker(rootEdge, secondLayerQubits);
    MBQCSimulator<DDPackage>::dd->decRef(secondLayerQubits); // necessary?

    for (const auto& v: secondLayer) {
        for (const auto& n: czOrd[v]) {
            auto tmp = applyCZ(rootEdge, pattern->getNVertices() + secondLayer.size(), verticesToQubitMap[v], verticesToQubitMap[n]);
            dd->incRef(tmp);
            dd->decRef(rootEdge);
            rootEdge     = tmp;
            entangled[v] = true;
            entangled[n] = true;
        }
    }

    for (const auto& vertex: measOrd) { //TODO don't measure outputs
        rootEdge = entangleNextLayer(rootEdge, czOrd[vertex], entangled, verticesToQubitMap);
        rootEdge = measure(rootEdge, pattern->getMeasurement(vertex)); //TODO
    }
}

template<class DDPackage>
std::vector<zx::Vertex> MBQCSimulator<DDPackage>::measurementOrder(const zx::flow& flow) const {
    std::vector<zx::Vertex> ord(pattern->getNVertices());
    for (const auto& layer: flow.first) {
        ord.insert(ord.end(), layer.begin(), layer.end()); //TODO: Heuristic for choosing order within one layer
    }
}
template<class DDPackage>
std::vector<std::vector<zx::Vertex>> MBQCSimulator<DDPackage>::entanglementOrder(const zx::flow& flow) const {
    const auto&                          measOrd = measurementOrder(flow);
    std::vector<std::vector<zx::Vertex>> entOrd(pattern->getNVertices());

    for (const auto& v: measOrd) {
        for (const auto& n: pattern->getNeighbourhood(v)) {
            if (measOrd[v] > measOrd[n])
                continue;
            for (const auto& nn: pattern->getNeighbourhood(v)) {
                if (measOrd[v] > measOrd[nn])
                    continue;

                entOrd.emplace_back(nn);
            }
        }
    }

    // ensure entanglement order conforms with measurement order
    for (const auto& layer: entOrd) {
        std::sort(layer.begin(), layer.end(), [&](const auto& v, const auto& w) { measOrd[v] < measOrd[w]; });
    }
    return entOrd;
}

template<class DDPackage>
dd::vEdge MBQCSimulator<DDPackage>::preparePlusStates(std::size_t nQubits) {
    const auto& state = dd->makeBasisState(nQubits, std::vector<dd::BasisStates>(nQubits, dd::BasisStates::plus));
    dd->incRef(state);
    return state;
}

template<class DDPackage>
dd::vEdge MBQCSimulator<DDPackage>::applyCZ(dd::vEdge& root, const dd::Qubit ctrl, const dd::Qubit tar) {
    qc::StandardOperation cz(root.p->v, dd::Control{ctrl, dd::Control::Type::pos}, tar, qc::OpType::Z);
    return dd->multiply(root, dd::getDD(cz, dd));
}

template<class DDPackage>
dd::vEdge MBQCSimulator<DDPackage>::entangleNextLayer(dd::vEdge& root, const std::vector<zx::Vertex>& toEntangle, std::vector<bool>& entangled, const std::vector<zx::Vertex>& vertexToQubitMap) {
    for (const auto& n: toEntangle) {
        if (entangled[n])
            continue;
        for (const auto& nn: pattern->getNeighbourhood(n)) {
            if (!entangled[nn])
                continue;

            if (!entangled[n]) {
                auto tmp = applyCZ(root, vertexToQubitMap[n], vertexToQubitMap[nn]);
                dd->incRef(tmp);
                dd->decRef(root);
                root = tmp;
            }
        }
    }
    return root;
}
