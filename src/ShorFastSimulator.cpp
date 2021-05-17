#include "ShorFastSimulator.hpp"

#include <chrono>
#include <cmath>
#include <dd/ComplexNumbers.hpp>
#include <iostream>
#include <limits>
#include <random>

std::map<std::string, std::size_t> ShorFastSimulator::Simulate([[maybe_unused]] unsigned int shots) {
    if (verbose) {
        std::clog << "Simulate Shor's algorithm for n=" << n;
    }
    root_edge = dd->makeZeroState(n_qubits);
    dd->incRef(root_edge);
    //Initialize qubits
    //TODO: other init method where the initial value can be chosen
    ApplyGate(dd::Xmat, 0);

    if (verbose) {
        std::clog << " (requires " << +n_qubits << " qubits):\n";
    }

    if (coprime_a != 0 && gcd(coprime_a, n) != 1) {
        std::clog << "Warning: gcd(a=" << coprime_a << ", n=" << n << ") != 1 --> choosing a new value for a\n";
        coprime_a = 0;
    }

    if (coprime_a == 0) {
        std::uniform_int_distribution<unsigned int> distribution(1, n - 1); // range is inclusive
        do {
            coprime_a = distribution(mt);
        } while (gcd(coprime_a, n) != 1 || coprime_a == 1);
    }

    if (verbose) {
        std::clog << "Find a coprime to N=" << n << ": " << coprime_a << "\n";
    }

    auto* as                  = new unsigned long long[2 * required_bits];
    as[2 * required_bits - 1] = coprime_a;
    unsigned long long new_a  = coprime_a;
    for (int i = 2 * required_bits - 2; i >= 0; i--) {
        new_a = new_a * new_a;
        new_a = new_a % n;
        as[i] = new_a;
    }

    auto        t1 = std::chrono::steady_clock::now();
    std::string measurements(2 * required_bits, '0');

    for (unsigned int i = 0; i < 2 * required_bits; i++) {
        ApplyGate(dd::Hmat, n_qubits - 1);

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

        double q = 2;
        for (int j = i - 1; j >= 0; j--) {
            if (measurements[j] == '1') {
                double         q_r = QMDDcos(1, -q);
                double         q_i = QMDDsin(1, -q);
                dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
                ApplyGate(Qm, n_qubits - 1);
            }
            q *= 2;
        }

        ApplyGate(dd::Hmat, n_qubits - 1);

        measurements[i] = MeasureOneCollapsing(n_qubits - 1, false);
        dd->garbageCollect();

        if (measurements[i] == '1') {
            ApplyGate(dd::Xmat, n_qubits - 1);
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
    for (unsigned int i = 0; i < 2 * required_bits; i++) {
        if (verbose) {
            std::clog << sample.at(2 * required_bits - 1 - i);
        }
        res = (res << 1u) + (sample.at(2 * required_bits - 1 - i) == '1');
    }

    if (verbose) {
        std::clog << " = " << res << "\n";
    }
    unsigned long long denom = 1ull << (2 * required_bits);

    bool               success = false;
    unsigned long long f1{0}, f2{0};
    if (res == 0) {
        if (verbose) {
            std::clog << "Factorization failed (measured 0)!\n";
        }
    } else {
        if (verbose) {
            std::clog << "Continued fraction expansion of " << res << "/" << denom << " = " << std::flush;
        }
        std::vector<unsigned long long> cf;

        unsigned long long old_res   = res;
        unsigned long long old_denom = denom;
        while (res != 0) {
            cf.push_back(denom / res);
            unsigned long long tmp = denom % res;
            denom                  = res;
            res                    = tmp;
        }

        if (verbose) {
            for (const auto i: cf) {
                std::clog << i << " ";
            }
            std::clog << "\n";
        }

        for (unsigned int i = 0; i < cf.size(); i++) {
            //determine candidate
            unsigned long long denominator = cf[i];
            unsigned long long numerator   = 1;

            for (int j = i - 1; j >= 0; j--) {
                unsigned long long tmp = numerator + cf[j] * denominator;
                numerator              = denominator;
                denominator            = tmp;
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
                double delta = (double)old_res / (double)old_denom - (double)numerator / (double)denominator;
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
                            f1 = modpow(coprime_a, (denominator * fact) / 2, n);
                            f2 = (f1 + 1) % n;
                            f1 = (f1 == 0) ? n - 1 : f1 - 1;
                            f1 = gcd(f1, n);
                            f2 = gcd(f2, n);

                            if (f1 == 1ull || f2 == 1ull) {
                                if (verbose) {
                                    std::clog << "Factorization failed: found trivial factors " << f1 << " and " << f2
                                              << "\n";
                                }
                            } else {
                                if (verbose) {
                                    std::clog << "Factorization succeeded! Non-trivial factors are: \n"
                                              << "  -- gcd(" << n << "^(" << (denominator * fact) << "/2)-1"
                                              << "," << n
                                              << ") = " << f1 << "\n"
                                              << "  -- gcd(" << n << "^(" << (denominator * fact) << "/2)+1"
                                              << "," << n
                                              << ") = " << f2 << "\n";
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
        return {0, 0};
    }
}

dd::Package::mEdge ShorFastSimulator::limitTo(unsigned long long a) {
    std::array<dd::Package::mEdge, 4> edges{
            dd::Package::mEdge::zero, dd::Package::mEdge::zero,
            dd::Package::mEdge::zero, dd::Package::mEdge::zero};

    if (a & 1u) {
        edges[0] = edges[3] = dd::Package::mEdge::one;
    } else {
        edges[0] = dd::Package::mEdge::one;
    }
    dd::Edge f = dd->makeDDNode(0, edges, false);

    edges[0] = edges[1] = edges[2] = edges[3] = dd::Package::mEdge::zero;

    for (unsigned int p = 1; p < required_bits + 1; p++) {
        if ((a >> p) & 1u) {
            edges[0] = dd->makeIdent(0, p - 1);
            edges[3] = f;
        } else {
            edges[0] = f;
        }
        f        = dd->makeDDNode(p, edges, false);
        edges[3] = dd::Package::mEdge::zero;
    }

    return f;
}

dd::Package::mEdge ShorFastSimulator::addConst(unsigned long long a) {
    dd::Edge                          f = dd::Package::mEdge::one;
    std::array<dd::Package::mEdge, 4> edges{
            dd::Package::mEdge::zero, dd::Package::mEdge::zero,
            dd::Package::mEdge::zero, dd::Package::mEdge::zero};

    unsigned int p = 0;
    while (!((a >> p) & 1u)) {
        edges[0] = f;
        edges[3] = f;
        f        = dd->makeDDNode(p, edges, false);
        p++;
    }

    edges[0] = edges[1] = edges[2] = edges[3] = dd::Package::mEdge::zero;
    edges[2]                                  = f;
    dd::Edge left                             = dd->makeDDNode(p, edges, false);
    edges[2]                                  = dd::Package::mEdge::zero;
    edges[1]                                  = f;
    dd::Edge right                            = dd->makeDDNode(p, edges, false);
    p++;

    for (; p < required_bits; p++) {
        edges[0] = edges[1] = edges[2] = edges[3] = dd::Package::mEdge::zero;
        if ((a >> p) & 1u) {
            edges[2]           = left;
            dd::Edge new_left  = dd->makeDDNode(p, edges, false);
            edges[2]           = dd::Package::mEdge::zero;
            edges[0]           = right;
            edges[1]           = left;
            edges[3]           = right;
            dd::Edge new_right = dd->makeDDNode(p, edges, false);
            left               = new_left;
            right              = new_right;
        } else {
            edges[1]           = right;
            dd::Edge new_right = dd->makeDDNode(p, edges, false);
            edges[1]           = dd::Package::mEdge::zero;
            edges[0]           = left;
            edges[2]           = right;
            edges[3]           = left;
            dd::Edge new_left  = dd->makeDDNode(p, edges, false);
            left               = new_left;
            right              = new_right;
        }
    }

    edges[0] = left;
    edges[1] = right;
    edges[2] = right;
    edges[3] = left;

    return dd->makeDDNode(p, edges, false);
}

dd::Package::mEdge ShorFastSimulator::addConstMod(unsigned long long a) {
    dd::Package::mEdge f = addConst(a);

    dd::Package::mEdge f2 = addConst(n);

    dd::Package::mEdge f3 = limitTo(n - 1);

    dd::Package::mEdge f4 = limitTo(n - 1 - a);

    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge diff2 = dd->add(f3, f4);
    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge tmp   = dd->add(dd->multiply(f, f4), dd->multiply(dd->multiply(dd->transpose(f2), f), diff2));

    return tmp.p->e[0];
}

void ShorFastSimulator::ApplyGate(dd::GateMatrix matrix, dd::Qubit target) {
    number_of_operations++;
    dd::Edge gate = dd->makeGateDD(matrix, n_qubits, target);
    dd::Edge tmp  = dd->multiply(gate, root_edge);
    dd->incRef(tmp);
    dd->decRef(root_edge);
    root_edge = tmp;

    dd->garbageCollect();
}

void ShorFastSimulator::u_a_emulate2(unsigned long long int a) {
    [[maybe_unused]] const std::size_t cache_count_before = dd->cn.cacheCount();
    dag_edges.clear();

    dd::Package::vEdge                        f = dd::Package::vEdge::one;
    std::array<dd::Package::vEdge, dd::RADIX> edges{
            dd::Package::vEdge::zero, dd::Package::vEdge::zero};

    unsigned long long t = a;
    for (dd::Qubit p = 0; p < n_qubits - 1; ++p) {
        edges[0] = f;
        f        = dd->makeDDNode(p, edges);
        ts[p]    = t;
        t        = (2 * t) % n;
    }

    dd->incRef(f);

    for (auto& m: nodesOnLevel) {
        m = std::map<dd::Package::vNode*, dd::Package::vEdge>();
    }

    u_a_emulate2_rec(root_edge.p->e[0]);

    //dd->setMode(dd::Matrix);

    for (const auto& entry: nodesOnLevel.at(0)) {
        dd::Package::vEdge left = f;
        if (entry.first->e[0].w == dd::Complex::zero) {
            left = dd::Package::vEdge::zero;
        } else {
            left.w = dd->cn.mulCached(left.w, entry.first->e[0].w);
        }

        dd::Package::vEdge right = dd->multiply(addConstMod(ts[entry.first->v]), f);

        if (entry.first->e[1].w == dd::Complex::zero) {
            right = dd::Package::vEdge::zero;
        } else {
            right.w = dd->cn.mulCached(right.w, entry.first->e[1].w);
        }

        dd::Package::vEdge result = dd->add(left, right);

        if (left.w != dd::Complex::zero) {
            dd->cn.returnToCache(left.w);
        }
        if (right.w != dd::Complex::zero) {
            dd->cn.returnToCache(right.w);
        }

        dd->incRef(result);

        nodesOnLevel[0][entry.first] = result;
    }

    for (int i = 1; i < n_qubits - 1; i++) {
        std::vector<dd::Package::vEdge> saveEdges;
        for (auto it = nodesOnLevel.at(i).begin(); it != nodesOnLevel.at(i).end(); it++) {
            dd::Package::vEdge left = dd::Package::vEdge::zero;
            if (it->first->e.at(0).w != dd::Complex::zero) {
                left   = nodesOnLevel.at(i - 1)[it->first->e.at(0).p];
                left.w = dd->cn.mulCached(left.w, it->first->e.at(0).w);
            }

            dd::Package::vEdge right = dd::Package::vEdge::zero;
            if (it->first->e.at(1).w != dd::Complex::zero) {
                right   = dd->multiply(addConstMod(ts.at(it->first->v)), nodesOnLevel.at(i - 1)[it->first->e.at(1).p]);
                right.w = dd->cn.mulCached(right.w, it->first->e.at(1).w);
            }

            dd::Package::vEdge result = dd->add(left, right);

            if (left.w != dd::Complex::zero) {
                dd->cn.returnToCache(left.w);
            }
            if (right.w != dd::Complex::zero) {
                dd->cn.returnToCache(right.w);
            }

            dd->incRef(result);
            nodesOnLevel.at(i)[it->first] = result;
            saveEdges.push_back(result);
        }
        for (auto& it: nodesOnLevel.at(i - 1)) {
            dd->decRef(it.second);
        }
        dd->garbageCollect();
        saveEdges.push_back(root_edge);
        nodesOnLevel.at(i - 1).clear();
    }

    if (nodesOnLevel.at(n_qubits - 2).size() != 1) {
        throw std::runtime_error("error occurred");
    }

    dd::Package::vEdge result = nodesOnLevel.at(n_qubits - 2)[root_edge.p->e[0].p];

    dd->decRef(result);

    result.w = dd->cn.mulCached(result.w, root_edge.p->e[0].w);
    auto tmp = result.w;

    result = dd->makeDDNode(root_edge.p->v, std::array<dd::Package::vEdge, dd::RADIX>{root_edge.p->e[0], result});
    dd->cn.returnToCache(tmp);

    //dd->setMode(dd::Vector);

    result.w = dd->cn.mulCached(result.w, root_edge.w);
    tmp      = result.w;
    result.w = dd->cn.lookup(result.w);
    dd->cn.returnToCache(tmp);
    dd->decRef(root_edge);
    dd->incRef(result);
    root_edge = result;
    dd->garbageCollect();
    assert(dd->cn.cacheCount() == cache_count_before);
}

void ShorFastSimulator::u_a_emulate2_rec(dd::Package::vEdge e) {
    if (e.isTerminal() || nodesOnLevel.at(e.p->v).find(e.p) != nodesOnLevel.at(e.p->v).end()) {
        return;
    }
    nodesOnLevel.at(e.p->v)[e.p] = dd::Package::vEdge::zero;
    u_a_emulate2_rec(e.p->e[0]);
    u_a_emulate2_rec(e.p->e[1]);
}
