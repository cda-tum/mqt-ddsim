#include "Simulator.hpp"

#include "dd/Export.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>

using CN = dd::ComplexNumbers;

template<class Config>
std::map<std::string, std::size_t> Simulator<Config>::sampleFromAmplitudeVectorInPlace(std::vector<std::complex<dd::fp>>& amplitudes, size_t shots) {
    // in-place prefix-sum calculation of probabilities
    std::inclusive_scan(
            amplitudes.begin(), amplitudes.end(), amplitudes.begin(),
            [](const std::complex<dd::fp>& prefix, const std::complex<dd::fp>& value) {
                return std::complex<dd::fp>{std::fma(value.real(), value.real(), std::fma(value.imag(), value.imag(), prefix.real())), value.imag()};
            },
            std::complex<dd::fp>{0., 0.});

    std::map<std::string, std::size_t>     results;
    std::uniform_real_distribution<dd::fp> dist(0.0L, 1.0L);
    for (unsigned int i = 0; i < shots; ++i) {
        auto p = dist(mt);
        // use binary search to find the first entry >= p
        auto mit = std::upper_bound(amplitudes.begin(), amplitudes.end(), p, [](const dd::fp val, const std::complex<dd::fp>& c) { return val < c.real(); });
        auto m   = std::distance(amplitudes.begin(), mit);

        // construct basis state string
        auto basisState = dd::intToBinaryString(static_cast<std::size_t>(m), getNumberOfQubits());
        results[basisState]++;
    }
    return results;
}

/**
 * Calculate the contributions of each node and return as vector of priority queues (each queue corresponds to a level in the decision diagram)
 * @tparam Config Configuration for the underlying DD package
 * @param edge root edge to the decision diagram
 * @return vector of priority queues with each queue corresponding to a level of the decision diagram
 */
template<class Config>
std::vector<std::priority_queue<std::pair<double, dd::vNode*>, std::vector<std::pair<double, dd::vNode*>>>> Simulator<Config>::getNodeContributions(const dd::vEdge& edge) const {
    std::queue<dd::vNode*>       q;
    std::map<dd::vNode*, dd::fp> probsMone;

    probsMone[edge.p] = CN::mag2(edge.w);

    q.push(edge.p);

    while (!q.empty()) {
        dd::vNode* ptr = q.front();
        q.pop();
        const dd::fp parentProb = probsMone[ptr];

        if (ptr->e.at(0).p != nullptr && !ptr->e.at(0).w.exactlyZero()) {
            if (probsMone.find(ptr->e.at(0).p) == probsMone.end()) {
                q.push(ptr->e.at(0).p);
                probsMone[ptr->e.at(0).p] = 0;
            }
            probsMone[ptr->e.at(0).p] = probsMone.at(ptr->e.at(0).p) + parentProb * CN::mag2(ptr->e.at(0).w);
        }

        if (ptr->e.at(1).p != nullptr && !ptr->e.at(1).w.exactlyZero()) {
            if (probsMone.find(ptr->e.at(1).p) == probsMone.end()) {
                q.push(ptr->e.at(1).p);
                probsMone[ptr->e.at(1).p] = 0;
            }
            probsMone[ptr->e.at(1).p] = probsMone.at(ptr->e.at(1).p) + parentProb * CN::mag2(ptr->e.at(1).w);
        }
    }

    std::vector<std::priority_queue<std::pair<double, dd::vNode*>, std::vector<std::pair<double, dd::vNode*>>>> qq(getNumberOfQubits());

    for (auto& [node, probability]: probsMone) {
        if (dd::vNode::isTerminal(node)) {
            continue;
        }
        qq.at(static_cast<std::size_t>(node->v)).emplace(1 - probability, node);
    }

    return qq;
}

/**
 * Approximate a quantum state to a given fidelity.
 * @tparam Config Configuration for the underlying DD package
 * @param localDD pointer to the DD package where the quantum state lives
 * @param edge reference to the root node of the quantum state, will point to the new state afterwards if removeNodes is true
 * @param targetFidelity the fidelity that should be achieved
 * @param allLevels if true, apply approximation to targetFidely to each level, if false, only apply to the most suitable level
 * @param actuallyRemoveNodes if true, actually remove the nodes that are identified as unnecessary for the targetFidelity, if false, don't remove anything
 * @param verbose output information about the process and result
 * @return fidelity of the resulting quantum state
 */
template<class Config>
double Simulator<Config>::approximateByFidelity(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge& edge, double targetFidelity, bool allLevels, bool actuallyRemoveNodes, bool verbose) {
    auto                    qq = getNodeContributions(edge);
    std::vector<dd::vNode*> nodesToRemove;

    std::size_t maxRemove = 0;
    for (std::size_t i = 0; i < getNumberOfQubits(); i++) {
        double                  sum    = 0.0;
        std::size_t             remove = 0;
        std::vector<dd::vNode*> tmp;

        while (!qq.at(i).empty()) {
            const auto [fidelity, node] = qq.at(i).top();
            qq.at(i).pop();
            sum += 1 - fidelity;
            if (sum < 1 - targetFidelity) {
                remove++;
                if (allLevels) {
                    nodesToRemove.push_back(node);
                } else {
                    tmp.push_back(node);
                }
            } else {
                break;
            }
        }
        if (!allLevels) {
            if (i == 0) {
                if (remove > maxRemove) {
                    maxRemove     = remove;
                    nodesToRemove = tmp;
                }
            } else if (remove * i > maxRemove) {
                maxRemove     = remove * i;
                nodesToRemove = tmp;
            }
        }
    }

    std::map<dd::vNode*, dd::vEdge> dagEdges;
    for (auto& it: nodesToRemove) {
        dagEdges[it] = dd::vEdge::zero();
    }

    dd::vEdge newEdge = removeNodes(localDD, edge, dagEdges);
    assert(!std::isnan(dd::RealNumber::val(edge.w.r)));
    assert(!std::isnan(dd::RealNumber::val(edge.w.i)));
    dd::Complex c = localDD->cn.getCached(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = localDD->cn.lookup(c);

    dd::fp fidelity = 0;
    if (edge.p->v == newEdge.p->v) {
        fidelity = localDD->fidelity(edge, newEdge);
    }

    if (verbose) {
        const auto sizeBefore = edge.size();
        const auto sizeAfter  = newEdge.size();
        std::cout
                << getName() << ","
                << +getNumberOfQubits() << "," // unary plus for int promotion
                << sizeBefore << ","
                << "fixed_fidelity"
                << ","
                << allLevels << ","
                << targetFidelity << ","
                << sizeAfter << ","
                << static_cast<double>(sizeAfter) / static_cast<double>(sizeBefore) << ","
                << fidelity
                << "\n";
    }

    if (actuallyRemoveNodes) {
        localDD->decRef(edge);
        localDD->incRef(newEdge);
        edge = newEdge;
    }
    return fidelity;
}

template<class Config>
double Simulator<Config>::approximateBySampling(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge& edge, std::size_t nSamples, std::size_t threshold, bool actuallyRemoveNodes, bool verbose) {
    assert(nSamples > threshold);
    std::map<dd::vNode*, unsigned int>     visitedNodes;
    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);

    for (unsigned int j = 0; j < nSamples; j++) {
        dd::Edge cur = edge;

        for (dd::Qubit i = edge.p->v + 1; i-- > 0;) {
            visitedNodes[cur.p]++;

            const dd::fp p0 = CN::mag2(cur.p->e.at(0).w);
            const dd::fp n  = dist(mt);

            if (n < p0) {
                cur = cur.p->e.at(0);
            } else {
                cur = cur.p->e.at(1);
            }
        }
    }

    std::set<dd::vNode*> visitedNodes2;
    visitedNodes2.insert(edge.p);
    std::queue<dd::vNode*> q;
    q.push(edge.p);

    while (!q.empty()) {
        dd::vNode* ptr = q.front();
        q.pop();

        if (ptr->e.at(0).p != nullptr && !ptr->e.at(0).w.approximatelyZero() && visitedNodes2.find(ptr->e.at(0).p) == visitedNodes2.end()) {
            visitedNodes2.insert(ptr->e.at(0).p);
            q.push(ptr->e.at(0).p);
        }

        if (ptr->e.at(1).p != nullptr && !ptr->e.at(1).w.approximatelyZero() && visitedNodes2.find(ptr->e.at(1).p) == visitedNodes2.end()) {
            visitedNodes2.insert(ptr->e.at(1).p);
            q.push(ptr->e.at(1).p);
        }
    }

    for (auto& visitedNode: visitedNodes) {
        if (visitedNode.second > threshold) {
            visitedNodes2.erase(visitedNode.first);
        }
    }

    std::map<dd::vNode*, dd::vEdge> dagEdges;
    for (auto* it: visitedNodes2) {
        dagEdges[it] = dd::vEdge::zero();
    }

    dd::vEdge   newEdge = removeNodes(localDD, edge, dagEdges);
    dd::Complex c       = localDD->cn.getCached(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = localDD->cn.lookup(c);

    dd::fp fidelity = 0;
    if (edge.p->v == newEdge.p->v) {
        fidelity = localDD->fidelity(edge, newEdge);
    }

    if (verbose) {
        const auto sizeAfter  = newEdge.size();
        const auto sizeBefore = edge.size();
        std::cout
                << getName() << ","
                << +getNumberOfQubits() << "," // unary plus for int promotion
                << sizeBefore << ","
                << "sampling"
                << ","
                << nSamples << ","
                << threshold << ","
                << sizeAfter << ","
                << static_cast<double>(sizeAfter) / static_cast<double>(sizeBefore) << ","
                << fidelity
                << "\n";
    }

    if (actuallyRemoveNodes) {
        localDD->decRef(edge);
        localDD->incRef(newEdge);
        edge = newEdge;
        localDD->garbageCollect();
    }

    return fidelity;
}

template<class Config>
dd::vEdge Simulator<Config>::removeNodes(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge e, std::map<dd::vNode*, dd::vEdge>& dagEdges) {
    if (e.isTerminal()) {
        return e;
    }

    const auto it = dagEdges.find(e.p);
    if (it != dagEdges.end()) {
        dd::vEdge r = it->second;
        if (r.w.approximatelyZero()) {
            return dd::vEdge::zero();
        }
        r.w = localDD->cn.lookup(r.w * e.w);
        return r;
    }

    const std::array<dd::vEdge, dd::RADIX> edges{
            removeNodes(localDD, e.p->e.at(0), dagEdges),
            removeNodes(localDD, e.p->e.at(1), dagEdges)};

    dd::vEdge r   = localDD->makeDDNode(e.p->v, edges, false);
    dagEdges[e.p] = r;
    dd::Complex c = localDD->cn.getCached();
    CN::mul(c, e.w, r.w);
    r.w = localDD->cn.lookup(c);
    return r;
}

template<class Config>
std::pair<dd::ComplexValue, std::string> Simulator<Config>::getPathOfLeastResistance() const {
    if (std::abs(dd::ComplexNumbers::mag2(rootEdge.w) - 1.0) > epsilon) {
        if (rootEdge.w.approximatelyZero()) {
            throw std::runtime_error("Numerical instabilities led to a 0-vector! Abort simulation!");
        }
        std::cerr << "WARNING in PoLR: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 - 1 = "
                  << 1.0 - dd::ComplexNumbers::mag2(rootEdge.w) << ", but should be 1!\n";
    }

    std::string result(getNumberOfQubits(), '0');
    dd::Complex pathValue = dd->cn.getCached(dd::RealNumber::val(rootEdge.w.r), dd::RealNumber::val(rootEdge.w.i));
    dd::vEdge   cur       = rootEdge;
    for (dd::Qubit i = rootEdge.p->v + 1; i-- > 0;) {
        dd::fp       p0  = dd::ComplexNumbers::mag2(cur.p->e.at(0).w);
        const dd::fp p1  = dd::ComplexNumbers::mag2(cur.p->e.at(1).w);
        const dd::fp tmp = p0 + p1;

        if (std::abs(tmp - 1.0) > epsilon) {
            throw std::runtime_error("Added probabilities differ from 1 by " + std::to_string(std::abs(tmp - 1.0)));
        }
        p0 /= tmp; // TODO: should p1 be normalized as well?

        if (p0 >= p1) {
            CN::mul(pathValue, pathValue, cur.w);
            cur = cur.p->e.at(0);
        } else {
            result[static_cast<std::size_t>(cur.p->v)] = '1';
            CN::mul(pathValue, pathValue, cur.w);
            cur = cur.p->e.at(1);
        }
    }

    return {{dd::RealNumber::val(pathValue.r), dd::RealNumber::val(pathValue.i)},
            std::string{result.rbegin(), result.rend()}};
}

template<class Config>
void Simulator<Config>::exportDDtoGraphviz(std::ostream& os, const bool colored, const bool edgeLabels, const bool classic, const bool memory, const bool formatAsPolar) {
    assert(os.good());
    dd::toDot(rootEdge, os, colored, edgeLabels, classic, memory, formatAsPolar);
}

template<class Config>
std::string Simulator<Config>::exportDDtoGraphvizString(const bool colored, const bool edgeLabels, const bool classic, const bool memory, const bool formatAsPolar) {
    std::ostringstream oss{};
    exportDDtoGraphviz(oss, colored, edgeLabels, classic, memory, formatAsPolar);
    return oss.str();
}

template<class Config>
void Simulator<Config>::exportDDtoGraphvizFile(const std::string& filename, const bool colored, const bool edgeLabels, const bool classic, const bool memory, const bool formatAsPolar) {
    std::ofstream ofs(filename);
    exportDDtoGraphviz(ofs, colored, edgeLabels, classic, memory, formatAsPolar);
}

template class Simulator<dd::DDPackageConfig>;
template class Simulator<StochasticNoiseSimulatorDDPackageConfig>;
template class Simulator<DensityMatrixSimulatorDDPackageConfig>;
