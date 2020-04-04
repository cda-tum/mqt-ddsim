#include "Simulator.hpp"

#include <queue>
#include <set>



std::string Simulator::MeasureAll(const bool collapse) {
    if(std::abs(dd::ComplexNumbers::mag2(root_edge.w) -1.0L) > epsilon) {
        if(CN::equalsZero(root_edge.w)) {
            std::cerr << "ERROR: numerical instabilities led to a 0-vector! Abort simulation!\n";
            std::exit(1);
        }
        std::cerr << "WARNING in measurement: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 - 1 = "
                  << 1.0L - dd::ComplexNumbers::mag2(root_edge.w) << ", but should be 1!\n";
    }

    dd::Edge cur = root_edge;

    std::string result(getNumberOfQubits(), '0');

    std::uniform_real_distribution<fp> dist(0.0, 1.0L);

    for(int i = dd->invVarOrder[root_edge.p->v]; i >= 0;--i) {
        fp p0 = dd::ComplexNumbers::mag2(cur.p->e[0].w);
        fp p1 = dd::ComplexNumbers::mag2(cur.p->e[2].w);
        fp tmp = p0 + p1;

        if(std::abs(tmp-1.0L) > epsilon) {
            std::cerr << "[ERROR] In MeasureAll: Added probabilities differ from 1 by "
                      << std::scientific << std::abs(tmp-1.0L) << std::defaultfloat << std::endl;
            std::exit(1);
        }
        p0 /= tmp;

        const fp n = dist(mt);
        if(n < p0) {
            //result[cur.p->v] = '0';
            cur = cur.p->e[0];
        } else {
            result[cur.p->v] = '1';
            cur = cur.p->e[2];
        }
    }

    if(collapse) {
        dd->decRef(root_edge);

        dd::Edge e = dd->DDone;
        dd::Edge edges[4];
        edges[1]=edges[3]=dd->DDzero;

        for(int p=0; p < getNumberOfQubits(); p++) {
            if(result[p] == '0') {
                edges[0] = e;
                edges[2] = dd->DDzero;
            } else {
                edges[0] = dd->DDzero;
                edges[2] = e;
            }
            e = dd->makeNonterminal(p, edges, false);
        }
        dd->incRef(e);
        root_edge = e;
        dd->garbageCollect();
    }

    return std::string{result.rbegin(), result.rend()};
}

std::map<std::string, unsigned int> Simulator::MeasureAllNonCollapsing(unsigned int shots) {
    std::map<std::string, unsigned int> results;
    for(unsigned int i = 0; i < shots; i++) {
        const auto m = MeasureAll(false);
        results[m]++;
    }
    return results;
}

std::vector<dd::ComplexValue> Simulator::getVector() const {
    assert(getNumberOfQubits() < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
    std::string path(getNumberOfQubits(), '0');
    std::vector<dd::ComplexValue> results(1ull << getNumberOfQubits(), {0,0});
    for(unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        const std::string corrected_path{path.rbegin(), path.rend()};
        results[i] = dd->getValueByPath(root_edge, corrected_path);
        NextPath(path);
    }
    return results;
}

void Simulator::NextPath(std::string& s) {
    std::string::reverse_iterator iter = s.rbegin(), end = s.rend();
    int carry = 1;
    while (carry && iter != end)
    {
        int value = (*iter - '0')/2 + carry;
        carry = (value / 2);
        *iter = '0' + (value % 2)*2;
        ++iter;
    }
    if (carry)
        s.insert(0, "1");
}

char Simulator::MeasureOneCollapsing(unsigned short index) {
    std::map<dd::NodePtr, fp> probsMone;
    std::set<dd::NodePtr> visited_nodes2;
    std::queue<dd::NodePtr> q;

    probsMone[root_edge.p] = CN::mag2(root_edge.w);
    visited_nodes2.insert(root_edge.p);
    q.push(root_edge.p);

    while(q.front()->v != index) {
        dd::NodePtr ptr = q.front();
        q.pop();
        double prob = probsMone[ptr];

        if(!CN::equalsZero(ptr->e[0].w)) {
            const fp tmp1 = prob * CN::mag2(ptr->e[0].w);

            if(visited_nodes2.find(ptr->e[0].p) != visited_nodes2.end()) {
                probsMone[ptr->e[0].p] = probsMone[ptr->e[0].p] + tmp1;
            } else {
                probsMone[ptr->e[0].p] = tmp1;
                visited_nodes2.insert(ptr->e[0].p);
                q.push(ptr->e[0].p);
            }
        }

        if(!CN::equalsZero(ptr->e[2].w)) {
            const fp tmp1 = prob * CN::mag2(ptr->e[2].w);

            if(visited_nodes2.find(ptr->e[2].p) != visited_nodes2.end()) {
                probsMone[ptr->e[2].p] = probsMone[ptr->e[2].p] + tmp1;
            } else {
                probsMone[ptr->e[2].p] = tmp1;
                visited_nodes2.insert(ptr->e[2].p);
                q.push(ptr->e[2].p);
            }
        }
    }

    fp pzero{0}, pone{0};
    while(!q.empty()) {
        dd::NodePtr ptr = q.front();
        q.pop();

        if(!CN::equalsZero(ptr->e[0].w)) {
            pzero += probsMone[ptr] * CN::mag2(ptr->e[0].w);
        }

        if(!CN::equalsZero(ptr->e[2].w)) {
            pone += probsMone[ptr] * CN::mag2(ptr->e[2].w);
        }
    }

    if(std::abs(pzero + pone - 1) > epsilon) {
        std::cerr << "WARNING in measurement: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 = " << (pzero + pone) << ", but should be 1!\n";
        std::exit(1);
    }

    const fp sum = pzero + pone;

    //std::pair<fp, fp> probs = std::make_pair(pzero, pone);
    dd::Matrix2x2 measure_m{
            {{0,0}, {0,0}},
            {{0,0}, {0,0}}
    };

    std::uniform_real_distribution<fp> dist(0.0, 1.0L);
    fp n = dist(mt);
    fp norm_factor;
    char result;

    if(n < pzero/sum) {
        measure_m[0][0] = {1,0};
        norm_factor = pzero;
        result = '0';
    } else {
        measure_m[1][1] = {1, 0};
        norm_factor = pone;
        result = '1';
    }
    std::array<short, dd::MAXN> line{};
    line.fill(-1);
    line[index] = 2;
    dd::Edge m_gate = dd->makeGateDD(measure_m, getNumberOfQubits(), line.data());

    dd::Edge e = dd->multiply(m_gate, root_edge);
    dd->decRef(root_edge);

    dd::Complex c = dd->cn.getCachedComplex(std::sqrt(1.0L/norm_factor), 0);
    CN::mul(c, e.w, c);
    e.w = dd->cn.lookup(c);
    dd->incRef(e);
    root_edge = e;

    return result;
}

double Simulator::ApproximateByFidelity(double targetFidelity, bool removeNodes) {
    std::queue<dd::NodePtr> q;
    std::map<dd::NodePtr, fp> probsMone;

    probsMone[root_edge.p] = CN::mag2(root_edge.w);

    q.push(root_edge.p);

    while(!q.empty()) {
        dd::NodePtr ptr = q.front();
        q.pop();
        const fp parent_prob = probsMone[ptr];

        if(ptr->e[0].w != dd::ComplexNumbers::ZERO) {
            if (probsMone.find(ptr->e[0].p) == probsMone.end()) {
                q.push(ptr->e[0].p);
                probsMone[ptr->e[0].p] = 0;
            }
            probsMone[ptr->e[0].p] = probsMone.at(ptr->e[0].p) + parent_prob * CN::mag2(ptr->e[0].w);
        }

        if(ptr->e[2].w != dd::ComplexNumbers::ZERO) {
            if (probsMone.find(ptr->e[2].p) == probsMone.end()) {
                q.push(ptr->e[2].p);
                probsMone[ptr->e[2].p] = 0;
            }
            probsMone[ptr->e[2].p] = probsMone.at(ptr->e[2].p) + parent_prob * CN::mag2(ptr->e[2].w);
        }
    }


    std::array<int, dd::MAXN> nodes{};
    std::array<std::priority_queue<std::pair<double, dd::NodePtr>, std::vector<std::pair<double, dd::NodePtr>>>, dd::MAXN> qq{};


    for(auto & it : probsMone) {
        if (it.first->v < 0) {
            continue; // ignore the terminal node which has v == -1
        }
        nodes.at(it.first->v)++;
        qq.at(it.first->v).emplace(1-it.second, it.first);
    }

    probsMone.clear();
    std::vector<dd::NodePtr> nodes_to_remove;

    int max_remove = 0;
    for(int i = 0; i < getNumberOfQubits(); i++) {
        double sum = 0.0;
        int remove = 0;
        std::vector<dd::NodePtr> tmp;

        while(!qq[i].empty()) {
            auto p = qq[i].top();
            qq[i].pop();
            sum += 1-p.first;
            if(sum < 1-targetFidelity) {
                remove++;
                tmp.push_back(p.second);
            } else {
                break;
            }
        }
        if(remove * i > max_remove) {
            max_remove = remove * i;
            nodes_to_remove = tmp;
        }
    }

    std::map<dd::NodePtr, dd::Edge> dag_edges;

    for(auto & it : nodes_to_remove) {
        dag_edges[it] = dd::Package::DDzero;
    }

    dd::Edge newEdge = RemoveNodes(root_edge, dag_edges);
    assert(!std::isnan(CN::val(root_edge.w.r)));
    assert(!std::isnan(CN::val(root_edge.w.i)));
    dd::Complex c = dd->cn.getTempCachedComplex(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = dd->cn.lookup(c);

    double fidelity = -1.0;
    if(dd->size(newEdge) >= getNumberOfQubits()) {
        fidelity = dd->fidelity(root_edge, newEdge);
    }

    if(removeNodes) {
        dd->decRef(root_edge);
        dd->incRef(newEdge);
        root_edge = newEdge;
    }
    return fidelity;
}

double Simulator::ApproximateBySampling(int nSamples, int threshold, bool removeNodes) {
    std::map<dd::NodePtr, int> visited_nodes;
    std::uniform_real_distribution<fp> dist(0.0, 1.0L);

    for(int j = 0; j < nSamples; j++) {
        dd::Edge cur = root_edge;

        for(int i = dd->invVarOrder[root_edge.p->v]; i >= 0;--i) {
            visited_nodes[cur.p]++;

            fp p0 = CN::mag2(cur.p->e[0].w);
            fp n = dist(mt);

            if(n < p0) {
                cur = cur.p->e[0];
            } else {
                cur = cur.p->e[2];
            }
        }
    }

    std::set<dd::NodePtr> visited_nodes2;
    visited_nodes2.insert(root_edge.p);
    std::queue<dd::NodePtr> q;
    q.push(root_edge.p);

    while(!q.empty()) {
        dd::NodePtr ptr = q.front();
        q.pop();

        if(!CN::equalsZero(ptr->e[0].w) && visited_nodes2.find(ptr->e[0].p) == visited_nodes2.end()) {
            visited_nodes2.insert(ptr->e[0].p);
            q.push(ptr->e[0].p);
        }

        if(!CN::equalsZero(ptr->e[2].w) && visited_nodes2.find(ptr->e[2].p) == visited_nodes2.end()) {
            visited_nodes2.insert(ptr->e[2].p);
            q.push(ptr->e[2].p);
        }
    }

    for(auto & visited_node : visited_nodes) {
        if(visited_node.second > threshold) {
            visited_nodes2.erase(visited_node.first);
        }
    }

    std::map<dd::NodePtr, dd::Edge> dag_edges;

    for(auto it : visited_nodes2) {
        dag_edges[it] = dd::Package::DDzero;
    }

    dd::Edge newEdge = RemoveNodes(root_edge, dag_edges);
    dd::Complex c = dd->cn.getTempCachedComplex(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = dd->cn.lookup(c);

    fp fidelity = dd->fidelity(root_edge, newEdge);

    if(removeNodes) {
        dd->decRef(root_edge);
        dd->incRef(newEdge);
        root_edge = newEdge;
        dd->garbageCollect();
    }

    return fidelity;
}

dd::Edge Simulator::RemoveNodes(dd::Edge e, std::map<dd::NodePtr, dd::Edge>& dag_edges) {
    if(dd->isTerminal(e)) {
        return e;
    }

    const auto it = dag_edges.find(e.p);
    if(it != dag_edges.end()) {
        dd::Edge r = it->second;
        if(CN::equalsZero(r.w)) {
            return dd::Package::DDzero;
        }
        dd::Complex c = dd->cn.getTempCachedComplex();
        CN::mul(c, e.w, r.w);
        r.w = dd->cn.lookup(c);
        return r;
    }

    dd::Edge edges[4] {
            RemoveNodes(e.p->e[0], dag_edges),
            RemoveNodes(e.p->e[1], dag_edges),
            RemoveNodes(e.p->e[2], dag_edges),
            RemoveNodes(e.p->e[3], dag_edges)
    };


    dd::Edge r = dd->makeNonterminal(e.p->v, edges, false);
    dag_edges[e.p] = r;
    dd::Complex c = dd->cn.getTempCachedComplex();
    CN::mul(c, e.w, r.w);
    r.w = dd->cn.lookup(c);
    return r;
}

std::pair<dd::ComplexValue, std::string> Simulator::getPathOfLeastResistance() {
    if(std::abs(dd::ComplexNumbers::mag2(root_edge.w) -1.0L) > epsilon) {
        if(CN::equalsZero(root_edge.w)) {
            std::cerr << "ERROR: numerical instabilities led to a 0-vector! Abort simulation!\n";
            std::exit(1);
        }
        std::cerr << "WARNING in measurement: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 - 1 = "
                  << 1.0L - dd::ComplexNumbers::mag2(root_edge.w) << ", but should be 1!\n";
    }

    std::string result(getNumberOfQubits(), '0');
    dd::Complex path_value = dd->cn.getTempCachedComplex(CN::val(root_edge.w.r), CN::val(root_edge.w.i));
    dd::Edge cur = root_edge;
    for(int i = dd->invVarOrder[root_edge.p->v]; i >= 0; --i) {
        fp p0 = dd::ComplexNumbers::mag2(cur.p->e[0].w);
        fp p1 = dd::ComplexNumbers::mag2(cur.p->e[2].w);
        fp tmp = p0 + p1;

        if(std::abs(tmp-1.0L) > epsilon) {
            std::cerr << "[ERROR] In MeasureAll: Added probabilities differ from 1 by "
                      << std::scientific << std::abs(tmp-1.0L) << std::defaultfloat << std::endl;
            std::exit(1);
        }
        p0 /= tmp;

        if(p0 >= p1) {
            CN::mul(path_value, path_value, cur.w);
            cur = cur.p->e[0];
        } else {
            result[cur.p->v] = '1';
            CN::mul(path_value, path_value, cur.w);
            cur = cur.p->e[2];
        }
    }

    return {{CN::val(path_value.r), CN::val(path_value.i)}, std::string{result.rbegin(), result.rend()}};
}
