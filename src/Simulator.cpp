#include "Simulator.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>

using CN = dd::ComplexNumbers;

std::string Simulator::MeasureAll(const bool collapse) {
    if (std::abs(dd::ComplexNumbers::mag2(root_edge.w) - 1.0L) > epsilon) {
        if (root_edge.w.approximatelyZero()) {
            throw std::runtime_error("Numerical instabilities led to a 0-vector! Abort simulation!");
        }
        std::cerr << "WARNING in MAll: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 - 1 = "
                  << 1.0L - dd::ComplexNumbers::mag2(root_edge.w) << ", but should be 1!\n";
    }

    dd::Edge cur = root_edge;

    std::string result(getNumberOfQubits(), '0');

    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);

    for (dd::Qubit i = root_edge.p->v; i >= 0; --i) {
        dd::fp p0  = dd::ComplexNumbers::mag2(cur.p->e.at(0).w);
        dd::fp p1  = dd::ComplexNumbers::mag2(cur.p->e.at(1).w);
        dd::fp tmp = p0 + p1;

        if (std::abs(tmp - 1.0L) > epsilon) {
            throw std::runtime_error("Added probabilities differ from 1 by " + std::to_string(std::abs(tmp - 1.0L)));
        }
        p0 /= tmp;

        const dd::fp n = dist(mt);
        if (n < p0) {
            //result[cur.p->v] = '0';
            cur = cur.p->e.at(0);
        } else {
            result[cur.p->v] = '1';
            cur              = cur.p->e.at(1);
        }
    }

    if (collapse) {
        dd->decRef(root_edge);

        dd::Package::vEdge                e = dd::Package::vEdge::one;
        std::array<dd::Package::vEdge, 2> edges{};

        for (dd::Qubit p = 0; p < getNumberOfQubits(); p++) {
            if (result[p] == '0') {
                edges[0] = e;
                edges[1] = dd::Package::vEdge::zero;
            } else {
                edges[0] = dd::Package::vEdge::zero;
                edges[1] = e;
            }
            e = dd->makeDDNode(p, edges, false);
        }
        dd->incRef(e);
        root_edge = e;
        dd->garbageCollect();
    }

    return std::string{result.rbegin(), result.rend()};
}

std::map<std::string, std::size_t> Simulator::MeasureAllNonCollapsing(unsigned int shots) {
    std::map<std::string, std::size_t> results;
    for (unsigned int i = 0; i < shots; i++) {
        const auto m = MeasureAll(false);
        results[m]++;
    }
    return results;
}

std::vector<dd::ComplexValue> Simulator::getVector() const {
    assert(getNumberOfQubits() < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
    std::string                   path(getNumberOfQubits(), '0');
    std::vector<dd::ComplexValue> results(1ull << getNumberOfQubits(), dd::complex_zero);
    for (unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        const std::string corrected_path{path.rbegin(), path.rend()};
        results[i] = dd->getValueByPath(root_edge, corrected_path);
        NextPath(path);
    }
    return results;
}

std::vector<std::pair<dd::fp, dd::fp>> Simulator::getVectorPair() const {
    assert(getNumberOfQubits() < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
    std::string                            path(getNumberOfQubits(), '0');
    std::vector<std::pair<dd::fp, dd::fp>> results{1ull << getNumberOfQubits()};

    for (unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        const std::string      corrected_path{path.rbegin(), path.rend()};
        const dd::ComplexValue cv = dd->getValueByPath(root_edge, corrected_path);
        results[i]                = std::make_pair(cv.r, cv.i);
        NextPath(path);
    }
    return results;
}

std::vector<std::complex<dd::fp>> Simulator::getVectorComplex() const {
    assert(getNumberOfQubits() < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
    std::string                       path(getNumberOfQubits(), '0');
    std::vector<std::complex<dd::fp>> results(1ull << getNumberOfQubits());

    for (unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        const std::string      corrected_path{path.rbegin(), path.rend()};
        const dd::ComplexValue cv = dd->getValueByPath(root_edge, corrected_path);
        results[i]                = std::complex<dd::fp>(cv.r, cv.i);
        NextPath(path);
    }
    return results;
}

void Simulator::NextPath(std::string& s) {
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

/***
 * Destructively measures a single qubit and returns the result
 * @param index Index of the qubit to be measured
 * @param assume_probability_normalization Probability normalization makes measurement faster but isn't always given
 * @return '1' or '0' depending on the measured value
 */
char Simulator::MeasureOneCollapsing(const dd::Qubit index, const bool assume_probability_normalization) {
    std::map<dd::Package::vNode*, dd::fp> probsMone;
    std::set<dd::Package::vNode*>         visited_nodes2;
    std::queue<dd::Package::vNode*>       q;

    probsMone[root_edge.p] = CN::mag2(root_edge.w);
    visited_nodes2.insert(root_edge.p);
    q.push(root_edge.p);

    while (q.front()->v != index) {
        dd::Package::vNode* ptr = q.front();
        q.pop();
        double prob = probsMone[ptr];

        if (!ptr->e.at(0).w.approximatelyZero()) {
            const dd::fp tmp1 = prob * CN::mag2(ptr->e.at(0).w);

            if (visited_nodes2.find(ptr->e.at(0).p) != visited_nodes2.end()) {
                probsMone[ptr->e.at(0).p] = probsMone[ptr->e.at(0).p] + tmp1;
            } else {
                probsMone[ptr->e.at(0).p] = tmp1;
                visited_nodes2.insert(ptr->e.at(0).p);
                q.push(ptr->e.at(0).p);
            }
        }

        if (!ptr->e.at(1).w.approximatelyZero()) {
            const dd::fp tmp1 = prob * CN::mag2(ptr->e.at(1).w);

            if (visited_nodes2.find(ptr->e.at(1).p) != visited_nodes2.end()) {
                probsMone[ptr->e.at(1).p] = probsMone[ptr->e.at(1).p] + tmp1;
            } else {
                probsMone[ptr->e.at(1).p] = tmp1;
                visited_nodes2.insert(ptr->e.at(1).p);
                q.push(ptr->e.at(1).p);
            }
        }
    }

    dd::fp pzero{0}, pone{0};

    if (assume_probability_normalization) {
        while (!q.empty()) {
            dd::Package::vNode* ptr = q.front();
            q.pop();

            if (!ptr->e.at(0).w.approximatelyZero()) {
                pzero += probsMone[ptr] * CN::mag2(ptr->e.at(0).w);
            }

            if (!ptr->e.at(1).w.approximatelyZero()) {
                pone += probsMone[ptr] * CN::mag2(ptr->e.at(1).w);
            }
        }
    } else {
        std::unordered_map<dd::Package::vNode*, double> probs;
        assign_probs(root_edge, probs);

        while (!q.empty()) {
            dd::Package::vNode* ptr = q.front();
            q.pop();

            if (!ptr->e.at(0).w.approximatelyZero()) {
                pzero += probsMone[ptr] * probs[ptr->e.at(0).p] * CN::mag2(ptr->e.at(0).w);
            }

            if (!ptr->e.at(1).w.approximatelyZero()) {
                pone += probsMone[ptr] * probs[ptr->e.at(1).p] * CN::mag2(ptr->e.at(1).w);
            }
        }
    }

    if (std::abs(pzero + pone - 1) > epsilon) {
        throw std::runtime_error("Numerical instability occurred during measurement: |alpha|^2 + |beta|^2 = " + std::to_string(pzero) + " + " + std::to_string(pone) + " = " +
                                 std::to_string(pzero + pone) + ", but should be 1!");
    }

    const dd::fp sum = pzero + pone;

    //std::pair<fp, fp> probs = std::make_pair(pzero, pone);
    dd::GateMatrix measure_m{
            dd::complex_zero, dd::complex_zero,
            dd::complex_zero, dd::complex_zero};

    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);
    dd::fp                                 n = dist(mt);
    dd::fp                                 norm_factor;
    char                                   result;

    if (n < pzero / sum) {
        measure_m[0] = dd::complex_one;
        norm_factor  = pzero;
        result       = '0';
    } else {
        measure_m[3] = dd::complex_one;
        norm_factor  = pone;
        result       = '1';
    }

    dd::Package::mEdge m_gate = dd->makeGateDD(measure_m, getNumberOfQubits(), index);

    dd::Package::vEdge e = dd->multiply(m_gate, root_edge);

    dd::Complex c = dd->cn.getTemporary(std::sqrt(1.0 / norm_factor), 0);
    CN::mul(c, e.w, c);
    e.w = dd->cn.lookup(c);
    dd->incRef(e);
    dd->decRef(root_edge);
    root_edge = e;

    return result;
}

double Simulator::ApproximateByFidelity(double targetFidelity, bool allLevels, bool removeNodes, bool verbose) {
    std::queue<dd::Package::vNode*>       q;
    std::map<dd::Package::vNode*, dd::fp> probsMone;

    probsMone[root_edge.p] = CN::mag2(root_edge.w);

    q.push(root_edge.p);

    while (!q.empty()) {
        dd::Package::vNode* ptr = q.front();
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

    std::vector<int>                                                                                                              nodes(getNumberOfQubits(), 0);
    std::vector<std::priority_queue<std::pair<double, dd::Package::vNode*>, std::vector<std::pair<double, dd::Package::vNode*>>>> qq(
            getNumberOfQubits());

    for (auto& it: probsMone) {
        if (it.first->v < 0) {
            continue; // ignore the terminal node which has v == -1
        }
        nodes.at(it.first->v)++;
        qq.at(it.first->v).emplace(1 - it.second, it.first);
    }

    probsMone.clear();
    std::vector<dd::Package::vNode*> nodes_to_remove;

    int max_remove = 0;
    for (int i = 0; i < getNumberOfQubits(); i++) {
        double                           sum    = 0.0;
        int                              remove = 0;
        std::vector<dd::Package::vNode*> tmp;

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

    std::map<dd::Package::vNode*, dd::Package::vEdge> dag_edges;
    for (auto& it: nodes_to_remove) {
        dag_edges[it] = dd::Package::vEdge::zero;
    }

    dd::Package::vEdge newEdge = RemoveNodes(root_edge, dag_edges);
    assert(!std::isnan(dd::CTEntry::val(root_edge.w.r)));
    assert(!std::isnan(dd::CTEntry::val(root_edge.w.i)));
    dd::Complex c = dd->cn.getCached(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = dd->cn.lookup(c);

    dd::fp fidelity = 0;
    if (root_edge.p->v == newEdge.p->v) {
        fidelity = dd->fidelity(root_edge, newEdge);
    }

    if (verbose) {
        const unsigned size_before = dd->size(root_edge);
        const unsigned size_after  = dd->size(newEdge);
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
        dd->decRef(root_edge);
        dd->incRef(newEdge);
        root_edge = newEdge;
    }
    return fidelity;
}

double Simulator::ApproximateBySampling(unsigned int nSamples, unsigned int threshold, bool removeNodes, bool verbose) {
    assert(nSamples > threshold);
    std::map<dd::Package::vNode*, unsigned int> visited_nodes;
    std::uniform_real_distribution<dd::fp>      dist(0.0, 1.0L);

    for (unsigned int j = 0; j < nSamples; j++) {
        dd::Edge cur = root_edge;

        for (dd::Qubit i = root_edge.p->v; i >= 0; --i) {
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

    std::set<dd::Package::vNode*> visited_nodes2;
    visited_nodes2.insert(root_edge.p);
    std::queue<dd::Package::vNode*> q;
    q.push(root_edge.p);

    while (!q.empty()) {
        dd::Package::vNode* ptr = q.front();
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

    std::map<dd::Package::vNode*, dd::Package::vEdge> dag_edges;
    for (auto it: visited_nodes2) {
        dag_edges[it] = dd::Package::vEdge::zero;
    }

    dd::Package::vEdge newEdge = RemoveNodes(root_edge, dag_edges);
    dd::Complex        c       = dd->cn.getCached(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = dd->cn.lookup(c);

    dd::fp fidelity = 0;
    if (root_edge.p->v == newEdge.p->v) {
        fidelity = dd->fidelity(root_edge, newEdge);
    }

    if (verbose) {
        const unsigned size_after  = dd->size(newEdge);
        const unsigned size_before = dd->size(root_edge);
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
        dd->decRef(root_edge);
        dd->incRef(newEdge);
        root_edge = newEdge;
        dd->garbageCollect();
    }

    return fidelity;
}

dd::Package::vEdge
Simulator::RemoveNodes(dd::Package::vEdge e, std::map<dd::Package::vNode*, dd::Package::vEdge>& dag_edges) {
    if (e.isTerminal()) {
        return e;
    }

    const auto it = dag_edges.find(e.p);
    if (it != dag_edges.end()) {
        dd::Package::vEdge r = it->second;
        if (r.w.approximatelyZero()) {
            return dd::Package::vEdge::zero;
        }
        dd::Complex c = dd->cn.getTemporary();
        dd::ComplexNumbers::mul(c, e.w, r.w);
        r.w = dd->cn.lookup(c);
        return r;
    }

    std::array<dd::Package::vEdge, dd::RADIX> edges{
            RemoveNodes(e.p->e.at(0), dag_edges),
            RemoveNodes(e.p->e.at(1), dag_edges)};

    dd::Package::vEdge r = dd->makeDDNode(e.p->v, edges, false);
    dag_edges[e.p]       = r;
    dd::Complex c        = dd->cn.getCached();
    CN::mul(c, e.w, r.w);
    r.w = dd->cn.lookup(c);
    return r;
}

std::pair<dd::ComplexValue, std::string> Simulator::getPathOfLeastResistance() const {
    if (std::abs(dd::ComplexNumbers::mag2(root_edge.w) - 1.0L) > epsilon) {
        if (root_edge.w.approximatelyZero()) {
            throw std::runtime_error("Numerical instabilities led to a 0-vector! Abort simulation!");
        }
        std::cerr << "WARNING in PoLR: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 - 1 = "
                  << 1.0L - dd::ComplexNumbers::mag2(root_edge.w) << ", but should be 1!\n";
    }

    std::string        result(getNumberOfQubits(), '0');
    dd::Complex        path_value = dd->cn.getCached(dd::CTEntry::val(root_edge.w.r), dd::CTEntry::val(root_edge.w.i));
    dd::Package::vEdge cur        = root_edge;
    for (dd::Qubit i = root_edge.p->v; i >= 0; --i) {
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

double Simulator::assign_probs(dd::Package::vEdge edge, std::unordered_map<dd::Package::vNode*, double>& probs) {
    auto it = probs.find(edge.p);
    if (it != probs.end()) {
        return CN::mag2(edge.w) * it->second;
    }
    double sum;
    if (edge.isTerminal()) {
        sum = 1.0;
    } else {
        sum = assign_probs(edge.p->e.at(0), probs) + assign_probs(edge.p->e.at(1), probs);
    }

    probs.insert(std::pair<dd::Package::vNode*, double>(edge.p, sum));

    return CN::mag2(edge.w) * sum;
}