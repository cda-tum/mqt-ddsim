#include "Simulator.hpp"

#include <queue>
#include <set>
#include <unordered_map>


std::string Simulator::MeasureAll(const bool collapse) {
    if (std::abs(dd::ComplexNumbers::mag2(root_edge.w) - 1.0L) > epsilon) {
        if (CN::equalsZero(root_edge.w)) {
            throw std::runtime_error("Numerical instabilities led to a 0-vector! Abort simulation!");
        }
        std::cerr << "WARNING in MAll: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 - 1 = "
                  << 1.0L - dd::ComplexNumbers::mag2(root_edge.w) << ", but should be 1!\n";
    }

    dd::Edge cur = root_edge;

    std::string result(getNumberOfQubits(), '0');

    std::uniform_real_distribution<fp> dist(0.0, 1.0L);

    for (int i = root_edge.p->v; i >= 0; --i) {
        fp p0 = dd::ComplexNumbers::mag2(cur.p->e[0].w);
        fp p1 = dd::ComplexNumbers::mag2(cur.p->e[2].w);
        fp tmp = p0 + p1;

        if (std::abs(tmp - 1.0L) > epsilon) {
            throw std::runtime_error("Added probabilities differ from 1 by " + std::to_string(std::abs(tmp - 1.0L)));
        }
        p0 /= tmp;

        const fp n = dist(mt);
        if (n < p0) {
            //result[cur.p->v] = '0';
            cur = cur.p->e[0];
        } else {
            result[cur.p->v] = '1';
            cur = cur.p->e[2];
        }
    }

    if (collapse) {
        dd->decRef(root_edge);

        dd::Edge e = dd->DDone;
        dd::Edge edges[4];
        edges[1] = edges[3] = dd->DDzero;

        for (int p = 0; p < getNumberOfQubits(); p++) {
            if (result[p] == '0') {
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
    for (unsigned int i = 0; i < shots; i++) {
        const auto m = MeasureAll(false);
        results[m]++;
    }
    return results;
}

std::vector<dd::ComplexValue> Simulator::getVector() const {
    assert(getNumberOfQubits() < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
    std::string path(getNumberOfQubits(), '0');
    std::vector<dd::ComplexValue> results(1ull << getNumberOfQubits(), {0, 0});
    for (unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        const std::string corrected_path{path.rbegin(), path.rend()};
        results[i] = dd->getValueByPath(root_edge, corrected_path);
        NextPath(path);
    }
    return results;
}

void Simulator::NextPath(std::string &s) {
    std::string::reverse_iterator iter = s.rbegin(), end = s.rend();
    int carry = 1;
    while (carry && iter != end) {
        int value = (*iter - '0') / 2 + carry;
        carry = (value / 2);
        *iter = '0' + (value % 2) * 2;
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
char Simulator::MeasureOneCollapsing(unsigned short index, const bool assume_probability_normalization) {
    std::map<dd::NodePtr, fp> probsMone;
    std::set<dd::NodePtr> visited_nodes2;
    std::queue<dd::NodePtr> q;

    probsMone[root_edge.p] = CN::mag2(root_edge.w);
    visited_nodes2.insert(root_edge.p);
    q.push(root_edge.p);

    while (q.front()->v != index) {
        dd::NodePtr ptr = q.front();
        q.pop();
        double prob = probsMone[ptr];

        if (!CN::equalsZero(ptr->e[0].w)) {
            const fp tmp1 = prob * CN::mag2(ptr->e[0].w);

            if (visited_nodes2.find(ptr->e[0].p) != visited_nodes2.end()) {
                probsMone[ptr->e[0].p] = probsMone[ptr->e[0].p] + tmp1;
            } else {
                probsMone[ptr->e[0].p] = tmp1;
                visited_nodes2.insert(ptr->e[0].p);
                q.push(ptr->e[0].p);
            }
        }

        if (!CN::equalsZero(ptr->e[2].w)) {
            const fp tmp1 = prob * CN::mag2(ptr->e[2].w);

            if (visited_nodes2.find(ptr->e[2].p) != visited_nodes2.end()) {
                probsMone[ptr->e[2].p] = probsMone[ptr->e[2].p] + tmp1;
            } else {
                probsMone[ptr->e[2].p] = tmp1;
                visited_nodes2.insert(ptr->e[2].p);
                q.push(ptr->e[2].p);
            }
        }
    }

    fp pzero{0}, pone{0};

    if (assume_probability_normalization) {
        while (!q.empty()) {
            dd::NodePtr ptr = q.front();
            q.pop();

            if (!CN::equalsZero(ptr->e[0].w)) {
                pzero += probsMone[ptr] * CN::mag2(ptr->e[0].w);
            }

            if (!CN::equalsZero(ptr->e[2].w)) {
                pone += probsMone[ptr] * CN::mag2(ptr->e[2].w);
            }
        }
    } else {
        std::unordered_map<dd::NodePtr, double> probs;
        assign_probs(root_edge, probs);

        while (!q.empty()) {
            dd::NodePtr ptr = q.front();
            q.pop();

            if (!CN::equalsZero(ptr->e[0].w)) {
                pzero += probsMone[ptr] * probs[ptr->e[0].p] * CN::mag2(ptr->e[0].w);
            }

            if (!CN::equalsZero(ptr->e[2].w)) {
                pone += probsMone[ptr] * probs[ptr->e[2].p] * CN::mag2(ptr->e[2].w);
            }
        }
    }

    if (std::abs(pzero + pone - 1) > epsilon) {
        throw std::runtime_error("Numerical instability occurred during measurement: |alpha|^2 + |beta|^2 = "
                                 + std::to_string(pzero) + " + " + std::to_string(pone) + " = " + std::to_string(pzero + pone) + ", but should be 1!");
    }

    const fp sum = pzero + pone;

    //std::pair<fp, fp> probs = std::make_pair(pzero, pone);
    dd::Matrix2x2 measure_m{
            {{0, 0}, {0, 0}},
            {{0, 0}, {0, 0}}
    };

    std::uniform_real_distribution<fp> dist(0.0, 1.0L);
    fp n = dist(mt);
    fp norm_factor;
    char result;

    if (n < pzero / sum) {
        measure_m[0][0] = {1, 0};
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

    dd::Complex c = dd->cn.getTempCachedComplex(std::sqrt(1.0L / norm_factor), 0);
    CN::mul(c, e.w, c);
    e.w = dd->cn.lookup(c);
    dd->incRef(e);
    root_edge = e;

    return result;
}


dd::Edge Simulator::MeasureOneCollapsingConcurrent(unsigned short index, const std::unique_ptr<dd::Package> &localDD,
                                                   dd::Edge local_root_edge, std::default_random_engine &generator,
                                                   char *result, const bool assume_probability_normalization) {
    //todo merge the function with MeasureOneCollapsing
    std::map<dd::NodePtr, fp> probsMone;
    std::set<dd::NodePtr> visited_nodes2;
    std::queue<dd::NodePtr> q;

    probsMone[local_root_edge.p] = CN::mag2(local_root_edge.w);
    visited_nodes2.insert(local_root_edge.p);
    q.push(local_root_edge.p);

    while (q.front()->v != index) {
        dd::NodePtr ptr = q.front();
        q.pop();
        double prob = probsMone[ptr];

        if (!CN::equalsZero(ptr->e[0].w)) {
            const fp tmp1 = prob * CN::mag2(ptr->e[0].w);

            if (visited_nodes2.find(ptr->e[0].p) != visited_nodes2.end()) {
                probsMone[ptr->e[0].p] = probsMone[ptr->e[0].p] + tmp1;
            } else {
                probsMone[ptr->e[0].p] = tmp1;
                visited_nodes2.insert(ptr->e[0].p);
                q.push(ptr->e[0].p);
            }
        }

        if (!CN::equalsZero(ptr->e[2].w)) {
            const fp tmp1 = prob * CN::mag2(ptr->e[2].w);

            if (visited_nodes2.find(ptr->e[2].p) != visited_nodes2.end()) {
                probsMone[ptr->e[2].p] = probsMone[ptr->e[2].p] + tmp1;
            } else {
                probsMone[ptr->e[2].p] = tmp1;
                visited_nodes2.insert(ptr->e[2].p);
                q.push(ptr->e[2].p);
            }
        }
    }

    fp pzero{0}, pone{0};

    if (assume_probability_normalization) {
        while (!q.empty()) {
            dd::NodePtr ptr = q.front();
            q.pop();

            if (!CN::equalsZero(ptr->e[0].w)) {
                pzero += probsMone[ptr] * CN::mag2(ptr->e[0].w);
            }

            if (!CN::equalsZero(ptr->e[2].w)) {
                pone += probsMone[ptr] * CN::mag2(ptr->e[2].w);
            }
        }
    } else {
        std::unordered_map<dd::NodePtr, double> probs;
        assign_probs(local_root_edge, probs);

        while (!q.empty()) {
            dd::NodePtr ptr = q.front();
            q.pop();

            if (!CN::equalsZero(ptr->e[0].w)) {
                pzero += probsMone[ptr] * probs[ptr->e[0].p] * CN::mag2(ptr->e[0].w);
            }

            if (!CN::equalsZero(ptr->e[2].w)) {
                pone += probsMone[ptr] * probs[ptr->e[2].p] * CN::mag2(ptr->e[2].w);
            }
        }
    }

    if (std::abs(pzero + pone - 1) > epsilon) {
        throw std::runtime_error("Numerical instability occurred during measurement: |alpha|^2 + |beta|^2 = "
                                 + std::to_string(pzero) + " + " + std::to_string(pone) + " = " + std::to_string(pzero + pone) + ", but should be 1!");
    }

    const fp sum = pzero + pone;

    //std::pair<fp, fp> probs = std::make_pair(pzero, pone);
    dd::Matrix2x2 measure_m{
            {{0, 0}, {0, 0}},
            {{0, 0}, {0, 0}}
    };

    std::uniform_real_distribution<fp> dist(0.0, 1.0L);
    fp n = dist(generator);
    fp norm_factor;

    if (n < pzero / sum) {
        measure_m[0][0] = {1, 0};
        norm_factor = pzero;
        *result = '0';
    } else {
        measure_m[1][1] = {1, 0};
        norm_factor = pone;
        *result = '1';
    }
    std::array<short, dd::MAXN> line{};
    line.fill(-1);
    line[index] = 2;
    dd::Edge m_gate = localDD->makeGateDD(measure_m, getNumberOfQubits(), line.data());

    dd::Edge e = localDD->multiply(m_gate, local_root_edge);
    localDD->decRef(local_root_edge);

    dd::Complex c = localDD->cn.getTempCachedComplex(std::sqrt(1.0L / norm_factor), 0);
    CN::mul(c, e.w, c);
    e.w = localDD->cn.lookup(c);
    localDD->incRef(e);
    local_root_edge = e;

    return local_root_edge;
}

double Simulator::ApproximateByFidelity(double targetFidelity, bool allLevels, bool removeNodes, bool verbose) {
    std::queue<dd::NodePtr> q;
    std::map<dd::NodePtr, fp> probsMone;

    probsMone[root_edge.p] = CN::mag2(root_edge.w);

    q.push(root_edge.p);

    while (!q.empty()) {
        dd::NodePtr ptr = q.front();
        q.pop();
        const fp parent_prob = probsMone[ptr];

        if (ptr->e[0].w != dd::ComplexNumbers::ZERO) {
            if (probsMone.find(ptr->e[0].p) == probsMone.end()) {
                q.push(ptr->e[0].p);
                probsMone[ptr->e[0].p] = 0;
            }
            probsMone[ptr->e[0].p] = probsMone.at(ptr->e[0].p) + parent_prob * CN::mag2(ptr->e[0].w);
        }

        if (ptr->e[2].w != dd::ComplexNumbers::ZERO) {
            if (probsMone.find(ptr->e[2].p) == probsMone.end()) {
                q.push(ptr->e[2].p);
                probsMone[ptr->e[2].p] = 0;
            }
            probsMone[ptr->e[2].p] = probsMone.at(ptr->e[2].p) + parent_prob * CN::mag2(ptr->e[2].w);
        }
    }


    std::array<int, dd::MAXN> nodes{};
    std::array<std::priority_queue<std::pair<double, dd::NodePtr>, std::vector<std::pair<double, dd::NodePtr>>>, dd::MAXN> qq{};


    for (auto &it : probsMone) {
        if (it.first->v < 0) {
            continue; // ignore the terminal node which has v == -1
        }
        nodes.at(it.first->v)++;
        qq.at(it.first->v).emplace(1 - it.second, it.first);
    }

    probsMone.clear();
    std::vector<dd::NodePtr> nodes_to_remove;

    int max_remove = 0;
    for (int i = 0; i < getNumberOfQubits(); i++) {
        double sum = 0.0;
        int remove = 0;
        std::vector<dd::NodePtr> tmp;

        while (!qq[i].empty()) {
            auto p = qq[i].top();
            qq[i].pop();
            sum += 1 - p.first;
            if (sum < 1 - targetFidelity) {
                remove++;
                if(allLevels) {
                    nodes_to_remove.push_back(p.second);
                } else {
                    tmp.push_back(p.second);
                }
            } else {
                break;
            }
        }
        if (!allLevels && remove * i > max_remove) {
            max_remove = remove * i;
            nodes_to_remove = tmp;
        }
    }

    std::map<dd::NodePtr, dd::Edge> dag_edges;
    for (auto &it : nodes_to_remove) {
        dag_edges[it] = dd::Package::DDzero;
    }

    dd::Edge newEdge = RemoveNodes(root_edge, dag_edges);
    assert(!std::isnan(CN::val(root_edge.w.r)));
    assert(!std::isnan(CN::val(root_edge.w.i)));
    dd::Complex c = dd->cn.getTempCachedComplex(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = dd->cn.lookup(c);

    fp fidelity = 0;
    if (root_edge.p->v == newEdge.p->v) {
        fidelity = dd->fidelity(root_edge, newEdge);
    }

    if (verbose) {
        const unsigned size_before = dd->size(root_edge);
        const unsigned size_after = dd->size(newEdge);
        std::cout
                << getName() << ","
                << getNumberOfQubits() << ","
                << size_before << ","
                << "fixed_fidelity" << ","
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
    std::map<dd::NodePtr, unsigned int> visited_nodes;
    std::uniform_real_distribution<fp> dist(0.0, 1.0L);

    for (unsigned int j = 0; j < nSamples; j++) {
        dd::Edge cur = root_edge;

        for (int i = root_edge.p->v; i >= 0; --i) {
            visited_nodes[cur.p]++;

            fp p0 = CN::mag2(cur.p->e[0].w);
            fp n = dist(mt);

            if (n < p0) {
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

    while (!q.empty()) {
        dd::NodePtr ptr = q.front();
        q.pop();

        if (!CN::equalsZero(ptr->e[0].w) && visited_nodes2.find(ptr->e[0].p) == visited_nodes2.end()) {
            visited_nodes2.insert(ptr->e[0].p);
            q.push(ptr->e[0].p);
        }

        if (!CN::equalsZero(ptr->e[2].w) && visited_nodes2.find(ptr->e[2].p) == visited_nodes2.end()) {
            visited_nodes2.insert(ptr->e[2].p);
            q.push(ptr->e[2].p);
        }
    }

    for (auto &visited_node : visited_nodes) {
        if (visited_node.second > threshold) {
            visited_nodes2.erase(visited_node.first);
        }
    }

    std::map<dd::NodePtr, dd::Edge> dag_edges;
    for (auto it : visited_nodes2) {
        dag_edges[it] = dd::Package::DDzero;
    }

    dd::Edge newEdge = RemoveNodes(root_edge, dag_edges);
    dd::Complex c = dd->cn.getTempCachedComplex(std::sqrt(CN::mag2(newEdge.w)), 0);
    CN::div(c, newEdge.w, c);
    newEdge.w = dd->cn.lookup(c);


    fp fidelity = 0;
    if (root_edge.p->v == newEdge.p->v) {
        fidelity = dd->fidelity(root_edge, newEdge);
    }

    if (verbose) {
        const unsigned size_after = dd->size(newEdge);
        const unsigned size_before = dd->size(root_edge);
        std::cout
                << getName() << ","
                << getNumberOfQubits() << ","
                << size_before << ","
                << "sampling" << ","
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

dd::Edge Simulator::RemoveNodes(dd::Edge e, std::map<dd::NodePtr, dd::Edge> &dag_edges) {
    if (dd->isTerminal(e)) {
        return e;
    }

    const auto it = dag_edges.find(e.p);
    if (it != dag_edges.end()) {
        dd::Edge r = it->second;
        if (CN::equalsZero(r.w)) {
            return dd::Package::DDzero;
        }
        dd::Complex c = dd->cn.getTempCachedComplex();
        CN::mul(c, e.w, r.w);
        r.w = dd->cn.lookup(c);
        return r;
    }

    dd::Edge edges[4]{
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

std::pair<dd::ComplexValue, std::string> Simulator::getPathOfLeastResistance() const {
    if (std::abs(dd::ComplexNumbers::mag2(root_edge.w) - 1.0L) > epsilon) {
        if (CN::equalsZero(root_edge.w)) {
            throw std::runtime_error("Numerical instabilities led to a 0-vector! Abort simulation!");
        }
        std::cerr << "WARNING in PoLR: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 - 1 = "
                  << 1.0L - dd::ComplexNumbers::mag2(root_edge.w) << ", but should be 1!\n";
    }

    std::string result(getNumberOfQubits(), '0');
    dd::Complex path_value = dd->cn.getTempCachedComplex(CN::val(root_edge.w.r), CN::val(root_edge.w.i));
    dd::Edge cur = root_edge;
    for (int i = root_edge.p->v; i >= 0; --i) {
        fp p0 = dd::ComplexNumbers::mag2(cur.p->e[0].w);
        fp p1 = dd::ComplexNumbers::mag2(cur.p->e[2].w);
        fp tmp = p0 + p1;

        if (std::abs(tmp - 1.0L) > epsilon) {
            throw std::runtime_error("Added probabilities differ from 1 by " + std::to_string(std::abs(tmp - 1.0L)));
        }
        p0 /= tmp;

        if (p0 >= p1) {
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

double Simulator::assign_probs(dd::Edge edge, std::unordered_map<dd::NodePtr, double> &probs) {
    auto it = probs.find(edge.p);
    if (it != probs.end()) {
        return CN::mag2(edge.w) * it->second;
    }
    double sum;
    if (dd->isTerminal(edge)) {
        sum = 1.0;
    } else {
        sum = assign_probs(edge.p->e[0], probs) + assign_probs(edge.p->e[2], probs);
    }

    probs.insert(std::pair<dd::NodePtr, double>(edge.p, sum));

    return CN::mag2(edge.w) * sum;
}

dd::CTkind Simulator::ReturnNoiseOperation(char i, double prob) const {

//    if (forcedResult.size() > currentResult) {
//        auto tmp = forcedResult[currentResult];
//        currentResult += 1;
//        return tmp;
//    }
    switch (i) {
        case 'D': {
            if (prob >= 3 * noise_probability / 4) {
//                printf("T");
                return dd::I;
            } else if (prob < noise_probability / 4) {
//                printf("X");
                return dd::X;
            } else if (prob < noise_probability / 2) {
//                printf("Y");
                return dd::Y;
            } else if (prob < 3 * noise_probability / 4) {
//                printf("Z");
                return dd::Z;
            }
        }
        case 'A': {
            return dd::AFalse;
        }
        case 'P': {
            if (prob > noise_probability) {
//                printf("B");
                return dd::I;
            } else {
//                printf("P");
                return dd::Z; // A phase flip is represented by a Z operation
            }
        }
        default:
            std::cerr << "ERROR: Unknown noise effect\n";
            assert(false);
            return dd::none;
    }
}

void Simulator::setRecordedProperties(const std::string &input) {
    std::string subString;
    int list_begin = -2;
    int list_end = -2;
    for (char i : input) {
        if (i == ' ') {
            continue;
        }
        if (i == ',') {
            if (list_begin > -2) {
                list_end = std::stoi(subString);
                if (list_end > pow(2, getNumberOfQubits())) {
                    list_end = (int) pow(2, getNumberOfQubits());
                }
                assert(list_begin < list_end);
                for (int m = list_begin; m <= list_end; m++) {
                    recorded_properties.emplace_back(std::make_tuple(m, intToString(m)));
                }
            } else {
                recorded_properties.emplace_back(std::make_tuple(std::stoi(subString), intToString(std::stoi(subString))));
            }
            subString = "";
            list_begin = -2;
            list_end = -2;
        }
        if (i == '-' && subString.length() > 0) {
            list_begin = std::stoi(subString);
            subString = "";
            continue;
        }
        subString += i;
    }
    if (list_begin > -2) {
        list_end = std::stoi(subString);
        if (list_end > pow(2, getNumberOfQubits())) {
            list_end = (int) pow(2, getNumberOfQubits()) - 1;
        }
        assert(list_begin < list_end);
        for (int m = list_begin; m <= list_end; m++) {
            recorded_properties.emplace_back(std::make_tuple(m, intToString(m)));
        }
    } else {
        recorded_properties.emplace_back(std::make_tuple(std::stoi(subString), intToString(std::stoi(subString))));
    }
}

std::string Simulator::intToString(long target_number) const {
    if (target_number < 0) {
        assert(target_number == -1);
        return (std::string("F"));
    }
    auto qubits = getNumberOfQubits();
    std::string path(qubits, '0');
    auto number = (unsigned long) target_number;
    for (int i = 1; i <= qubits; i++) {
        if (number % 2) {
            path[qubits - i] = '2';
        }
        number = number >> 1u;
    }
    return path;
}