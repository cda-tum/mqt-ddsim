#include "Simulator.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>

using CN = dd::ComplexNumbers;

template<class DDPackage>
std::map<std::string, std::size_t> Simulator<DDPackage>::SampleFromAmplitudeVectorInPlace(std::vector<std::complex<dd::fp>>& amplitudes, unsigned int shots) {
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
        auto basisState = toBinaryString(m, getNumberOfQubits());
        results[basisState]++;
    }
    return results;
}

template<class DDPackage>
std::vector<dd::ComplexValue> Simulator<DDPackage>::getVector() const {
    assert(getNumberOfQubits() < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
    std::string                   path(getNumberOfQubits(), '0');
    std::vector<dd::ComplexValue> results(1ull << getNumberOfQubits(), dd::complex_zero);
    for (unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        const std::string corrected_path{path.rbegin(), path.rend()};
        results[i] = dd->getValueByPath(rootEdge, corrected_path);
        NextPath(path);
    }
    return results;
}

template<class Config>
std::vector<std::pair<dd::fp, dd::fp>> Simulator<Config>::getVectorPair() const {
    assert(getNumberOfQubits() < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
    std::string                            path(getNumberOfQubits(), '0');
    std::vector<std::pair<dd::fp, dd::fp>> results{1ull << getNumberOfQubits()};

    for (unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        const std::string      corrected_path{path.rbegin(), path.rend()};
        const dd::ComplexValue cv = dd->getValueByPath(rootEdge, corrected_path);
        results[i]                = std::make_pair(cv.r, cv.i);
        NextPath(path);
    }
    return results;
}

template<class Config>
std::vector<std::complex<dd::fp>> Simulator<Config>::getVectorComplex() const {
    assert(getNumberOfQubits() < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
    std::string                       path(getNumberOfQubits(), '0');
    std::vector<std::complex<dd::fp>> results(1ull << getNumberOfQubits());

    for (unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        const std::string      corrected_path{path.rbegin(), path.rend()};
        const dd::ComplexValue cv = dd->getValueByPath(rootEdge, corrected_path);
        results[i]                = std::complex<dd::fp>(cv.r, cv.i);
        NextPath(path);
    }
    return results;
}

template<class DDPackage>
void Simulator<DDPackage>::NextPath(std::string& s) {
    std::string::reverse_iterator iter = s.rbegin(), end = s.rend();
    int                           carry = 1;
    while (carry && iter != end) {
        int value = (*iter - '0') + carry;
        carry     = (value / 2);
        *iter     = '0' + (value % 2);
        ++iter;
    }
    if (carry)
        s.insert(0, "1");
}

template<class Config>
double Simulator<Config>::ApproximateByFidelity(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge& edge, double targetFidelity, bool allLevels, bool removeNodes, bool verbose) {
    std::queue<dd::vNode*>       q;
    std::map<dd::vNode*, dd::fp> probsMone;

    probsMone[edge.p] = CN::mag2(edge.w);

    q.push(edge.p);

    while (!q.empty()) {
        dd::vNode* ptr = q.front();
        q.pop();
        const dd::fp parent_prob = probsMone[ptr];

        if (ptr->e.at(0).w != dd::Complex::zero) {
            if (probsMone.find(ptr->e.at(0).p) == probsMone.end()) {
                q.push(ptr->e.at(0).p);
                probsMone[ptr->e.at(0).p] = 0;
            }
            probsMone[ptr->e.at(0).p] = probsMone.at(ptr->e.at(0).p) + parent_prob * CN::mag2(ptr->e.at(0).w);
        }

        if (ptr->e.at(1).w != dd::Complex::zero) {
            if (probsMone.find(ptr->e.at(1).p) == probsMone.end()) {
                q.push(ptr->e.at(1).p);
                probsMone[ptr->e.at(1).p] = 0;
            }
            probsMone[ptr->e.at(1).p] = probsMone.at(ptr->e.at(1).p) + parent_prob * CN::mag2(ptr->e.at(1).w);
        }
    }

    std::vector<int> nodes(getNumberOfQubits(), 0);

    std::vector<std::priority_queue<std::pair<double, dd::vNode*>, std::vector<std::pair<double, dd::vNode*>>>> qq(getNumberOfQubits());

    for (auto& it: probsMone) {
        if (it.first->v < 0) {
            continue; // ignore the terminal node which has v == -1
        }
        nodes.at(it.first->v)++;
        qq.at(it.first->v).emplace(1 - it.second, it.first);
    }

    probsMone.clear();
    std::vector<dd::vNode*> nodes_to_remove;

    int max_remove = 0;
    for (dd::QubitCount i = 0; i < getNumberOfQubits(); i++) {
        double                  sum    = 0.0;
        int                     remove = 0;
        std::vector<dd::vNode*> tmp;

        while (!qq.at(i).empty()) {
            auto p = qq.at(i).top();
            qq.at(i).pop();
            sum += 1 - p.first;
            if (sum < 1 - targetFidelity) {
                remove++;
                if (allLevels) {
                    nodes_to_remove.push_back(p.second);
                } else {
                    tmp.push_back(p.second);
                }
            } else {
                break;
            }
        }
        if (!allLevels && remove * i > max_remove) {
            max_remove      = remove * i;
            nodes_to_remove = tmp;
        }
    }

    std::map<dd::vNode*, dd::vEdge> dag_edges;
    for (auto& it: nodes_to_remove) {
        dag_edges[it] = dd::vEdge::zero;
    }

    dd::vEdge newEdge = RemoveNodes(localDD, edge, dag_edges);
    assert(!std::isnan(dd::CTEntry::val(edge.w.r)));
    assert(!std::isnan(dd::CTEntry::val(edge.w.i)));
    dd::Complex c = localDD->cn.getCached(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = localDD->cn.lookup(c);

    dd::fp fidelity = 0;
    if (edge.p->v == newEdge.p->v) {
        fidelity = localDD->fidelity(edge, newEdge);
    }

    if (verbose) {
        const unsigned size_before = localDD->size(edge);
        const unsigned size_after  = localDD->size(newEdge);
        std::cout
                << getName() << ","
                << +getNumberOfQubits() << "," // unary plus for int promotion
                << size_before << ","
                << "fixed_fidelity"
                << ","
                << allLevels << ","
                << targetFidelity << ","
                << size_after << ","
                << static_cast<double>(size_after) / static_cast<double>(size_before) << ","
                << fidelity
                << "\n";
    }

    if (removeNodes) {
        localDD->decRef(edge);
        localDD->incRef(newEdge);
        edge = newEdge;
    }
    return fidelity;
}

template<class Config>
double Simulator<Config>::ApproximateBySampling(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge& edge, std::size_t nSamples, std::size_t threshold, bool removeNodes, bool verbose) {
    assert(nSamples > threshold);
    std::map<dd::vNode*, unsigned int>     visited_nodes;
    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);

    for (unsigned int j = 0; j < nSamples; j++) {
        dd::Edge cur = edge;

        for (dd::Qubit i = edge.p->v; i >= 0; --i) {
            visited_nodes[cur.p]++;

            dd::fp p0 = CN::mag2(cur.p->e.at(0).w);
            dd::fp n  = dist(mt);

            if (n < p0) {
                cur = cur.p->e.at(0);
            } else {
                cur = cur.p->e.at(1);
            }
        }
    }

    std::set<dd::vNode*> visited_nodes2;
    visited_nodes2.insert(edge.p);
    std::queue<dd::vNode*> q;
    q.push(edge.p);

    while (!q.empty()) {
        dd::vNode* ptr = q.front();
        q.pop();

        if (!ptr->e.at(0).w.approximatelyZero() && visited_nodes2.find(ptr->e.at(0).p) == visited_nodes2.end()) {
            visited_nodes2.insert(ptr->e.at(0).p);
            q.push(ptr->e.at(0).p);
        }

        if (!ptr->e.at(1).w.approximatelyZero() && visited_nodes2.find(ptr->e.at(1).p) == visited_nodes2.end()) {
            visited_nodes2.insert(ptr->e.at(1).p);
            q.push(ptr->e.at(1).p);
        }
    }

    for (auto& visited_node: visited_nodes) {
        if (visited_node.second > threshold) {
            visited_nodes2.erase(visited_node.first);
        }
    }

    std::map<dd::vNode*, dd::vEdge> dag_edges;
    for (auto it: visited_nodes2) {
        dag_edges[it] = dd::vEdge::zero;
    }

    dd::vEdge   newEdge = RemoveNodes(localDD, edge, dag_edges);
    dd::Complex c       = localDD->cn.getCached(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = localDD->cn.lookup(c);

    dd::fp fidelity = 0;
    if (edge.p->v == newEdge.p->v) {
        fidelity = localDD->fidelity(edge, newEdge);
    }

    if (verbose) {
        const unsigned size_after  = localDD->size(newEdge);
        const unsigned size_before = localDD->size(edge);
        std::cout
                << getName() << ","
                << +getNumberOfQubits() << "," // unary plus for int promotion
                << size_before << ","
                << "sampling"
                << ","
                << nSamples << ","
                << threshold << ","
                << size_after << ","
                << static_cast<double>(size_after) / static_cast<double>(size_before) << ","
                << fidelity
                << "\n";
    }

    if (removeNodes) {
        localDD->decRef(edge);
        localDD->incRef(newEdge);
        edge = newEdge;
        localDD->garbageCollect();
    }

    return fidelity;
}

template<class Config>
dd::vEdge Simulator<Config>::RemoveNodes(std::unique_ptr<dd::Package<Config>>& localDD, dd::vEdge e, std::map<dd::vNode*, dd::vEdge>& dag_edges) {
    if (e.isTerminal()) {
        return e;
    }

    const auto it = dag_edges.find(e.p);
    if (it != dag_edges.end()) {
        dd::vEdge r = it->second;
        if (r.w.approximatelyZero()) {
            return dd::vEdge::zero;
        }
        dd::Complex c = localDD->cn.getTemporary();
        dd::ComplexNumbers::mul(c, e.w, r.w);
        r.w = localDD->cn.lookup(c);
        return r;
    }

    std::array<dd::vEdge, dd::RADIX> edges{
            RemoveNodes(localDD, e.p->e.at(0), dag_edges),
            RemoveNodes(localDD, e.p->e.at(1), dag_edges)};

    dd::vEdge r    = localDD->makeDDNode(e.p->v, edges, false);
    dag_edges[e.p] = r;
    dd::Complex c  = localDD->cn.getCached();
    CN::mul(c, e.w, r.w);
    r.w = localDD->cn.lookup(c);
    return r;
}

template<class DDPackage>
std::pair<dd::ComplexValue, std::string> Simulator<DDPackage>::getPathOfLeastResistance() const {
    if (std::abs(dd::ComplexNumbers::mag2(rootEdge.w) - 1.0L) > epsilon) {
        if (rootEdge.w.approximatelyZero()) {
            throw std::runtime_error("Numerical instabilities led to a 0-vector! Abort simulation!");
        }
        std::cerr << "WARNING in PoLR: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 - 1 = "
                  << 1.0L - dd::ComplexNumbers::mag2(rootEdge.w) << ", but should be 1!\n";
    }

    std::string result(getNumberOfQubits(), '0');
    dd::Complex path_value = dd->cn.getCached(dd::CTEntry::val(rootEdge.w.r), dd::CTEntry::val(rootEdge.w.i));
    dd::vEdge   cur        = rootEdge;
    for (dd::Qubit i = rootEdge.p->v; i >= 0; --i) {
        dd::fp p0  = dd::ComplexNumbers::mag2(cur.p->e.at(0).w);
        dd::fp p1  = dd::ComplexNumbers::mag2(cur.p->e.at(1).w);
        dd::fp tmp = p0 + p1;

        if (std::abs(tmp - 1.0L) > epsilon) {
            throw std::runtime_error("Added probabilities differ from 1 by " + std::to_string(std::abs(tmp - 1.0L)));
        }
        p0 /= tmp;

        if (p0 >= p1) {
            CN::mul(path_value, path_value, cur.w);
            cur = cur.p->e.at(0);
        } else {
            result[cur.p->v] = '1';
            CN::mul(path_value, path_value, cur.w);
            cur = cur.p->e.at(1);
        }
    }

    return {{dd::CTEntry::val(path_value.r), dd::CTEntry::val(path_value.i)},
            std::string{result.rbegin(), result.rend()}};
}

template class Simulator<dd::DDPackageConfig>;
template class Simulator<StochasticNoiseSimulatorDDPackageConfig>;
