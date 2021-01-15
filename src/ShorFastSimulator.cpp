#include "ShorFastSimulator.hpp"

#include <iostream>
#include <random>
#include <cmath>
#include <chrono>
#include <limits>

std::map<std::string, unsigned int> ShorFastSimulator::Simulate(unsigned int shots) {
    if (verbose) {
        std::clog << "Simulate Shor's algorithm for n=" << n;
    }
    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);
    //Initialize qubits
    //TODO: other init method where the initial value can be chosen
    line[0] = 2;
    ApplyGate(qc::Xmat);
    line[0] = -1;

    if (verbose) {
        std::clog << " (requires " << n_qubits << " qubits):\n";
    }

    if(coprime_a != 0 && gcd(coprime_a, n) != 1) {
        std::clog << "Warning: gcd(a=" << coprime_a << ", n=" << n << ") != 1 --> choosing a new value for a\n";
        coprime_a = 0;
    }

    if(coprime_a == 0) {
        std::uniform_int_distribution<unsigned int> distribution(1, n-1); // range is inclusive
        do {
            coprime_a = distribution(mt);
        } while (gcd(coprime_a, n) != 1 || coprime_a == 1);
    }

    if (verbose) {
        std::clog << "Find a coprime to N=" << n << ": " << coprime_a << "\n";
    }

    auto* as = new unsigned long long[2*required_bits];
    as[2*required_bits-1] = coprime_a;
    unsigned long long new_a = coprime_a;
    for(int i = 2*required_bits-2; i >= 0; i--) {
        new_a = new_a * new_a;
        new_a = new_a % n;
        as[i] = new_a;
    }


    auto t1 = std::chrono::steady_clock::now();
    std::string measurements(2*required_bits, '0');

    for (int i = 0; i < 2 * required_bits; i++) {
        line[(n_qubits - 1)] = 2;
        ApplyGate(qc::Hmat);
        line[(n_qubits - 1)] = -1;

        if (verbose) {
            std::clog << "[ " << (i + 1) << "/" << 2 * required_bits << " ] u_a_emulate2(" << as[i] << ") "
                      << std::chrono::duration<float>(std::chrono::steady_clock::now() - t1).count() << "\n"
                      << std::flush;
        }
        u_a_emulate2(as[i]);

        if (verbose) {
            std::clog << "[ " << i + 1 << "/" << 2 * required_bits << " ] QFT Pass. dd size=" << dd->size(root_edge)
                      << "\n";
        }

        line[(n_qubits - 1)] = 2;

        double q = 2;
        for (int j = i - 1; j >= 0; j--) {
            if (measurements[j] == '1') {
                double q_r = QMDDcos(1, -q);
                double q_i = QMDDsin(1, -q);
                qc::GateMatrix Qm{qc::complex_one, qc::complex_zero, qc::complex_zero, {q_r, q_i}};
                ApplyGate(Qm);
            }
            q *= 2;
        }

        ApplyGate(qc::Hmat);

        line[(n_qubits - 1)] = -1;

        measurements[i] = MeasureOneCollapsing(n_qubits - 1, false);
        dd->garbageCollect();

        if (measurements[i] == '1') {
            line[n_qubits - 1] = 2;
            ApplyGate(qc::Xmat);
            line[n_qubits - 1] = -1;
        }
    }

    delete[] as;

    // Non-Quantum Post Processing
    sim_factors = post_processing(measurements);
    if (sim_factors.first != 0 && sim_factors.second != 0) {
        sim_result = "SUCCESS(" + std::to_string(sim_factors.first) + "*" + std::to_string(sim_factors.second) + ")";
    } else {
        sim_result = "FAILURE";
    }

    return {};
}

std::pair<unsigned int, unsigned int> ShorFastSimulator::post_processing(const std::string& sample) const {
    unsigned long long res = 0;
    if (verbose) {
        std::clog << "measurement: ";
    }
    for(int i=0; i < 2*required_bits; i++) {
        if (verbose) {
            std::clog << sample.at(2*required_bits -1- i);
        }
        res = (res << 1u) + (sample.at(2*required_bits -1- i) == '1');
    }

    if (verbose) {
        std::clog << " = " << res << "\n";
    }
    unsigned long long denom = 1ull << (2*required_bits);

    bool success = false;
    unsigned long long f1{0}, f2{0};
    if(res == 0) {
        if (verbose) {
            std::clog << "Factorization failed (measured 0)!\n";
        }
    } else {
        if (verbose) {
            std::clog << "Continued fraction expansion of " << res << "/" << denom << " = " << std::flush;
        }
        std::vector<unsigned long long> cf;

        unsigned long long old_res = res;
        unsigned long long old_denom = denom;
        while(res != 0) {
            cf.push_back(denom/res);
            unsigned long long tmp = denom % res;
            denom = res;
            res = tmp;
        }

        if (verbose) {
            for(const auto i : cf) {
                std::clog << i << " ";
            } std::clog << "\n";
        }


        for(unsigned int i=0; i < cf.size(); i++) {
            //determine candidate
            unsigned long long denominator = cf[i];
            unsigned long long numerator = 1;

            for (int j = i - 1; j >= 0; j--) {
                unsigned long long tmp = numerator + cf[j] * denominator;
                numerator = denominator;
                denominator = tmp;
            }
            if (verbose) {
                std::clog << "  Candidate " << numerator << "/" << denominator << ": ";
            }
            if (denominator > n) {
                if (verbose) {
                    std::clog << " denominator too large (greater than " << n << ")!\n";
                }
                success = false;
                if (verbose) {
                    std::clog << "Factorization failed!\n";
                }
                break;
            } else {
                double delta = (double) old_res / (double) old_denom - (double) numerator / (double) denominator;
                if (std::abs(delta) < 1.0 / (2.0 * old_denom)) {
                    unsigned long long fact = 1;
                    while (denominator * fact < n && modpow(coprime_a, denominator * fact, n) != 1) {
                        fact++;
                    }
                    if (modpow(coprime_a, denominator * fact, n) == 1) {
                        if (verbose) {
                            std::clog << "found period: " << denominator << " * " << fact << " = "
                                      << (denominator * fact) << "\n";
                        }
                        if ((denominator * fact) & 1u) {
                            if (verbose) {
                                std::clog << "Factorization failed (period is odd)!\n";
                            }
                        } else {

                            f1 = modpow(coprime_a, (denominator * fact)/2, n);
                            f2 = (f1 + 1) % n;
                            f1 = (f1 == 0) ? n - 1 : f1 - 1;
                            f1 = gcd(f1, n);
                            f2 = gcd(f2, n);

                            if (f1 == 1ull || f2 == 1ull) {
                                if (verbose) {
                                    std::clog << "Factorization failed: found trivial factors " << f1 << " and " << f2 << "\n";
                                }
                            } else {
                                if (verbose) {
                                    std::clog << "Factorization succeeded! Non-trivial factors are: \n"
                                              << "  -- gcd(" << n << "^(" << (denominator * fact) << "/2)-1" << "," << n << ") = " << f1 << "\n"
                                              << "  -- gcd(" << n << "^(" << (denominator * fact) << "/2)+1" << "," << n << ") = " << f2 << "\n";
                                }
                                success = true;
                            }
                        }

                        break;
                    } else {
                        if (verbose) {
                            std::clog << "failed\n";
                        }
                    }
                } else {
                    if (verbose) {
                        std::clog << "delta is too big (" << delta << ")\n";
                    }
                }
            }
        }
    }
    if (success) {
        return {f1, f2};
    } else {
        return {0,0};
    }
}

dd::Edge ShorFastSimulator::limitTo(unsigned long long a) {
    dd::Edge edges[4];

    edges[0]=edges[1]=edges[2]=edges[3]=dd->DDzero;
    if(a & 1u) {
        edges[0] = edges[3] = dd->DDone;
    } else {
        edges[0]=dd->DDone;
    }
    dd::Edge f = dd->makeNonterminal(0, edges, false);

    edges[0]=edges[1]=edges[2]=edges[3]=dd->DDzero;
    for(int p=1; p < required_bits+1;p++) {
        if((a>>p) & 1u) {
            edges[0]=dd->makeIdent(0,p-1); 
            edges[3]=f;
        } else {
            edges[0]=f;
        }
        f = dd->makeNonterminal(p, edges, false);
        edges[3] = dd->DDzero;
    }

    return f;
}

dd::Edge ShorFastSimulator::addConst(unsigned long long a) {

    dd::Edge f = dd->DDone;
    dd::Edge edges[4];
    edges[0]=edges[1]=edges[2]=edges[3]=dd->DDzero;

    int p = 0;
    while(!((a >> p)  & 1u)) {
        edges[0] = f;
        edges[3] = f;
        f = dd->makeNonterminal(p, edges, false);
        p++;
    }

    dd::Edge right, left;

    edges[0]=edges[1]=edges[2]=edges[3]=dd->DDzero;
    edges[2]=f;
    left = dd->makeNonterminal(p, edges, false);
    edges[2]=dd->DDzero;
    edges[1]=f;
    right = dd->makeNonterminal(p, edges, false);
    p++;

    dd::Edge new_left, new_right;
    for(;p < required_bits; p++) {
        edges[0]=edges[1]=edges[2]=edges[3]=dd->DDzero;
        if((a>>p) & 1u) {
            edges[2] = left;
            new_left = dd->makeNonterminal(p, edges, false);
            edges[2] = dd->DDzero;
            edges[0] = right;
            edges[1] = left;
            edges[3] = right;
            new_right = dd->makeNonterminal(p, edges, false);
        } else {
            edges[1] = right;
            new_right = dd->makeNonterminal(p, edges, false);
            edges[1] = dd->DDzero;
            edges[0] = left;
            edges[2] = right;
            edges[3] = left;
            new_left = dd->makeNonterminal(p, edges, false);
        }
        left = new_left;
        right = new_right;
    }

    edges[0] = left;
    edges[1] = right;
    edges[2] = right;
    edges[3] = left;

    return dd->makeNonterminal(p, edges, false);

}

dd::Edge ShorFastSimulator::addConstMod(unsigned long long a) {
    dd::Edge f = addConst(a);

    dd::Edge f2 = addConst(n);

    dd::Edge f3 = limitTo(n-1);

    dd::Edge f4 = limitTo(n-1-a);

    f4.w = CN::neg(f4.w);
    dd::Edge diff2 = dd->add(f3, f4);
    f4.w = CN::neg(f4.w);
    dd::Edge tmp = dd->add(dd->multiply(f, f4), dd->multiply(dd->multiply(dd->transpose(f2), f), diff2));

    return tmp.p->e[0];
}

void ShorFastSimulator::ApplyGate(qc::GateMatrix matrix) {
    number_of_operations++;
    dd::Edge gate = dd->makeGateDD(matrix, n_qubits, line);
    dd::Edge tmp = dd->multiply(gate, root_edge);
    dd->incRef(tmp);
    dd->decRef(root_edge);
    root_edge = tmp;
    
    dd->garbageCollect();
}

void ShorFastSimulator::u_a_emulate2(unsigned long long int a) {
    const long cache_count_before = dd->cn.cacheCount;
    dag_edges.clear();

    dd::Edge f=dd->DDone;
    dd::Edge edges[4];

    edges[0]=edges[1]=edges[2]=edges[3]=dd->DDzero;

    unsigned long long t = a;
    for(int p=0; p < n_qubits-1; ++p) {
        edges[0] = f;
        f = dd->makeNonterminal(p, edges);
        ts[p] = t;
        t = (2*t) % n;
    }

    dd->incRef(f);

    for(int i=0;i<dd::MAXN; i++) {
        nodesOnLevel[i] = std::map<dd::NodePtr, dd::Edge>();
    }
    u_a_emulate2_rec(root_edge.p->e[0]);

    dd->setMode(dd::Matrix);

    for(auto it = nodesOnLevel[0].begin(); it != nodesOnLevel[0].end(); it++) {
        dd::Edge left = f;
        if(it->first->e[0].w == CN::ZERO) {
            left = dd->DDzero;
        } else {
            left.w = dd->cn.mulCached( left.w, it->first->e[0].w);
        }

        dd::Edge right = dd->multiply(addConstMod(ts[it->first->v]), f);

        if(it->first->e[2].w == CN::ZERO) {
            right = dd->DDzero;
        } else {
            right.w = dd->cn.mulCached(right.w, it->first->e[2].w);
        }

        dd::Edge result = dd->add(left, right);

        if(left.w != CN::ZERO) {
            dd->cn.releaseCached(left.w);
        }
        if(right.w != CN::ZERO) {
            dd->cn.releaseCached(right.w);
        }

        dd->incRef(result);

        nodesOnLevel[0][it->first] = result;
    }


    for(int i=1; i < n_qubits-1; i++) {
        std::vector<dd::Edge> saveEdges;
        for(auto it = nodesOnLevel[i].begin(); it != nodesOnLevel[i].end(); it++) {
            dd::Edge left = dd->DDzero;
            if(it->first->e[0].w != CN::ZERO) {
                left = nodesOnLevel[i-1][it->first->e[0].p];
                left.w = dd->cn.mulCached(left.w, it->first->e[0].w);
            }

            dd::Edge right = dd->DDzero;
            if(it->first->e[2].w != CN::ZERO) {
                right = dd->multiply(addConstMod(ts[it->first->v]), nodesOnLevel[i-1][it->first->e[2].p]);
                right.w = dd->cn.mulCached(right.w, it->first->e[2].w);
            }

            dd::Edge result = dd->add(left, right);

            if(left.w != CN::ZERO) {
                dd->cn.releaseCached(left.w);
            }
            if(right.w != CN::ZERO) {
                dd->cn.releaseCached(right.w);
            }

            dd->incRef(result);
            nodesOnLevel[i][it->first] = result;
            saveEdges.push_back(result);
        }
        for(auto & it : nodesOnLevel[i-1]) {
            dd->decRef(it.second);
        }
        dd->garbageCollect();
        saveEdges.push_back(root_edge);
        nodesOnLevel[i-1].clear();
    }

    if(nodesOnLevel[n_qubits-2].size() != 1) {
        throw std::runtime_error("error occurred");
    }

    dd::Edge result = nodesOnLevel[n_qubits-2][root_edge.p->e[0].p];

    dd->decRef(result);

    result.w = dd->cn.mulCached(result.w, root_edge.p->e[0].w);
    auto tmp = result.w;

    edges[1]=edges[3]=dd->DDzero;
    edges[0] = root_edge.p->e[0];
    edges[2] = result;
    result = dd->makeNonterminal(root_edge.p->v, edges);
    dd->cn.releaseCached(tmp);

    dd->setMode(dd::Vector);

    result.w = dd->cn.mulCached(result.w, root_edge.w);
    tmp = result.w;
    result.w = dd->cn.lookup(result.w);
    dd->cn.releaseCached(tmp);
    dd->decRef(root_edge);
    dd->incRef(result);
    root_edge = result;
    dd->garbageCollect();
    assert(dd->cn.cacheCount == cache_count_before);
}

void ShorFastSimulator::u_a_emulate2_rec(dd::Edge e) {
    if(dd->isTerminal(e) || nodesOnLevel[e.p->v].find(e.p) != nodesOnLevel[e.p->v].end()) {
        return;
    }
    nodesOnLevel[e.p->v][e.p]=dd->DDzero;
    u_a_emulate2_rec(e.p->e[0]);
    u_a_emulate2_rec(e.p->e[2]);
}

