#include <queue>
#include <map>
#include "SimpleSimulator.hpp"



void SimpleSimulator::Simulate() {
    const unsigned short n_qubits = qc->getNqubits();

    std::array<short, qc::MAX_QUBITS> line{};
    line.fill(qc::LINE_DEFAULT);

    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);

    //unsigned long op_num = 0;

    for (auto& op : *qc) {
        if (!op->isUnitary()) {
        	if(auto* nu_op = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
        	    if (nu_op->getName()[0] == 'M') {
        	        std::cout << "[WARN] Measurements are not saved to classical registers.\n";
                    for(const auto& qubit : nu_op->getControls()) {
                        MeasureOneCollapsing(qubit.qubit);
                    }
                } else {
                    std::cerr << "[ERROR] Unsupported non-unitary functionality." << std::endl;
                    std::exit(1);
        	    }
        	} else {
                std::cerr << "[ERROR] Dynamic cast to NonUnitaryOperation failed." << std::endl;
                std::exit(1);
        	}
        } else {
            //std::cout << "[INFO] op " << op_num++ << " is " << op->getName() << "\n";
            auto dd_op = op->getDD(dd, line);
            auto tmp = dd->multiply(dd_op, root_edge);
            dd->incRef(tmp);
            dd->decRef(root_edge);
            root_edge = tmp;
        }

        dd->garbageCollect();
    }
}

std::string SimpleSimulator::MeasureAll(const bool collapse) {
    if(std::abs(dd::ComplexNumbers::mag2(root_edge.w) -1.0L) > CN::TOLERANCE) {
        if(CN::equalsZero(root_edge.w)) {
            std::cerr << "ERROR: numerical instabilities led to a 0-vector! Abort simulation!\n";
            std::exit(1);
        }
        std::cerr << "WARNING in measurement: numerical instability occurred during simulation: |alpha|^2 + |beta|^2 = " << dd::ComplexNumbers::mag2(root_edge.w) << ", but should be 1!\n";
    }

    dd::Edge cur = root_edge;

    std::string result(qc->getNqubits(), '_');

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
            result[cur.p->v] = '0';
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

        for(int p=0; p < qc->getNqubits(); p++) {
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

    return result;
}

std::map<std::string, unsigned int> SimpleSimulator::MeasureAllNonCollapsing(unsigned int shots) {
    std::map<std::string, unsigned int> results;
    for(unsigned int i = 0; i < shots; i++) {
        const auto m = MeasureAll(false);
        results[m]++;
    }
    return results;
}

std::vector<dd::ComplexValue> SimpleSimulator::getVector() const {
    std::string path(getNumberOfQubits(), '0');
    std::vector<dd::ComplexValue> results(1ull << getNumberOfQubits(), {0,0});

    for(unsigned long long i = 0; i < 1ull << getNumberOfQubits(); ++i) {
        results[i] = dd->getValueByPath(root_edge, path);
        NextPath(path);
    }
    return results;
}

void SimpleSimulator::NextPath(std::string& s) {
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

std::string SimpleSimulator::MeasureOneCollapsing(unsigned short index) {
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
    std::string result;

    if(n < pzero/sum) {
        measure_m[0][0] = {1,0};
        norm_factor = pzero;
        result = "0";
    } else {
        measure_m[1][1] = {1, 0};
        norm_factor = pone;
        result = "1";
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
