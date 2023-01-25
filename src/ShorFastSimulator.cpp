#include "ShorFastSimulator.hpp"

#include <chrono>
#include <cmath>
#include <dd/ComplexNumbers.hpp>
#include <iostream>
#include <limits>
#include <random>

template<class Config>
std::map<std::string, std::size_t> ShorFastSimulator<Config>::Simulate(unsigned int shots) {
    if (verbose) {
        std::clog << "Simulate Shor's algorithm for n=" << n;
    }
    this->rootEdge = this->dd->makeZeroState(n_qubits);
    this->dd->incRef(this->rootEdge);
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
            coprime_a = distribution(this->mt);
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
            std::clog << "[ " << i + 1 << "/" << 2 * required_bits << " ] QFT Pass. dd size=" << this->dd->size(this->rootEdge)
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

        measurements[i] = this->MeasureOneCollapsing(n_qubits - 1, false);
        this->dd->garbageCollect();

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

/**
 * Post process the result of the simulation, i.e. try to find two non-trivial factors
 * @tparam Config Configuration for the underlying DD package
 * @param sample string with the measurement results (consisting of only 0s and 1s)
 * @return pair of integers with the factors in case of success or pair of zeros in case of errors
 */
template<class Config>
std::pair<unsigned int, unsigned int> ShorFastSimulator<Config>::post_processing(const std::string& sample) const {
    std::ostream log{nullptr};
    if (verbose) {
        log.rdbuf(std::clog.rdbuf());
    }
    unsigned long long res = 0;

    log << "measurement: ";
    for (unsigned int i = 0; i < 2 * required_bits; i++) {
        log << sample.at(2 * required_bits - 1 - i);
        res = (res << 1u) + (sample.at(2 * required_bits - 1 - i) == '1');
    }

    log << " = " << res << "\n";

    if (res == 0) {
        log << "Factorization failed (measured 0)!\n";
        return {0, 0};
    }
    std::vector<unsigned long long> cf;
    auto                            denom     = 1ull << (2 * required_bits);
    const auto                      old_denom = denom;
    const auto                      old_res   = res;

    log << "Continued fraction expansion of " << res << "/" << denom << " = " << std::flush;

    while (res != 0) {
        cf.push_back(denom / res);
        const auto tmp = denom % res;
        denom          = res;
        res            = tmp;
    }

    for (const auto i: cf) {
        log << i << " ";
    }
    log << "\n";

    for (unsigned int i = 0; i < cf.size(); i++) {
        //determine candidate
        unsigned long long denominator = cf[i];
        unsigned long long numerator   = 1;

        for (int j = i - 1; j >= 0; j--) {
            const auto tmp = numerator + cf[j] * denominator;
            numerator      = denominator;
            denominator    = tmp;
        }

        log << "  Candidate " << numerator << "/" << denominator << ": ";

        if (denominator > n) {
            log << " denominator too large (greater than " << n << ")!\nFactorization failed!\n";
            return {0, 0};
        }

        const double delta = static_cast<double>(old_res) / static_cast<double>(old_denom) - static_cast<double>(numerator) / static_cast<double>(denominator);
        if (std::abs(delta) >= 1.0 / (2.0 * static_cast<double>(old_denom))) {
            log << "delta is too big (" << delta << ")\n";
            continue;
        }

        unsigned long long fact = 1;
        while (denominator * fact < n && modpow(coprime_a, denominator * fact, n) != 1) {
            fact++;
        }

        if (modpow(coprime_a, denominator * fact, n) != 1) {
            log << "failed\n";
            continue;
        }

        log << "found period: " << denominator << " * " << fact << " = " << (denominator * fact) << "\n";

        if ((denominator * fact) & 1u) {
            log << "Factorization failed (period is odd)!\n";
            return {0, 0};
        }

        auto f1 = modpow(coprime_a, (denominator * fact) / 2, n);
        auto f2 = (f1 + 1) % n;
        f1      = (f1 == 0) ? n - 1 : f1 - 1;
        f1      = gcd(f1, n);
        f2      = gcd(f2, n);

        if (f1 == 1ull || f2 == 1ull) {
            log << "Factorization failed: found trivial factors " << f1 << " and " << f2 << "\n";
            return {0, 0};
        }
        log << "Factorization succeeded! Non-trivial factors are: \n"
            << "  -- gcd(" << n << "^(" << (denominator * fact) << "/2)-1" << "," << n << ") = " << f1 << "\n"
            << "  -- gcd(" << n << "^(" << (denominator * fact) << "/2)+1" << "," << n << ") = " << f2 << "\n";
        return {f1, f2};
    }
    return {0, 0};
}

template<class Config>
dd::mEdge ShorFastSimulator<Config>::limitTo(unsigned long long a) {
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero, dd::mEdge::zero,
            dd::mEdge::zero, dd::mEdge::zero};

    if (a & 1u) {
        edges[0] = edges[3] = dd::mEdge::one;
    } else {
        edges[0] = dd::mEdge::one;
    }
    dd::Edge f = this->dd->makeDDNode(0, edges, false);

    edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero;

    for (unsigned int p = 1; p < required_bits + 1; p++) {
        if ((a >> p) & 1u) {
            edges[0] = this->dd->makeIdent(0, p - 1);
            edges[3] = f;
        } else {
            edges[0] = f;
        }
        f        = this->dd->makeDDNode(p, edges, false);
        edges[3] = dd::mEdge::zero;
    }

    return f;
}

template<class Config>
dd::mEdge ShorFastSimulator<Config>::addConst(unsigned long long a) {
    dd::Edge                 f = dd::mEdge::one;
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero, dd::mEdge::zero,
            dd::mEdge::zero, dd::mEdge::zero};

    unsigned int p = 0;
    while (!((a >> p) & 1u)) {
        edges[0] = f;
        edges[3] = f;
        f        = this->dd->makeDDNode(p, edges, false);
        p++;
    }

    edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero;
    edges[2]                                  = f;
    dd::Edge left                             = this->dd->makeDDNode(p, edges, false);
    edges[2]                                  = dd::mEdge::zero;
    edges[1]                                  = f;
    dd::Edge right                            = this->dd->makeDDNode(p, edges, false);
    p++;

    for (; p < required_bits; p++) {
        edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero;
        if ((a >> p) & 1u) {
            edges[2]           = left;
            dd::Edge new_left  = this->dd->makeDDNode(p, edges, false);
            edges[2]           = dd::mEdge::zero;
            edges[0]           = right;
            edges[1]           = left;
            edges[3]           = right;
            dd::Edge new_right = this->dd->makeDDNode(p, edges, false);
            left               = new_left;
            right              = new_right;
        } else {
            edges[1]           = right;
            dd::Edge new_right = this->dd->makeDDNode(p, edges, false);
            edges[1]           = dd::mEdge::zero;
            edges[0]           = left;
            edges[2]           = right;
            edges[3]           = left;
            dd::Edge new_left  = this->dd->makeDDNode(p, edges, false);
            left               = new_left;
            right              = new_right;
        }
    }

    edges[0] = left;
    edges[1] = right;
    edges[2] = right;
    edges[3] = left;

    return this->dd->makeDDNode(p, edges, false);
}

template<class Config>
dd::mEdge ShorFastSimulator<Config>::addConstMod(unsigned long long a) {
    dd::mEdge f = addConst(a);

    dd::mEdge f2 = addConst(n);

    dd::mEdge f3 = limitTo(n - 1);

    dd::mEdge f4 = limitTo(n - 1 - a);

    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge diff2 = this->dd->add(f3, f4);
    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge tmp   = this->dd->add(this->dd->multiply(f, f4), this->dd->multiply(this->dd->multiply(this->dd->transpose(f2), f), diff2));

    return tmp.p->e[0];
}

template<class Config>
void ShorFastSimulator<Config>::ApplyGate(dd::GateMatrix matrix, dd::Qubit target) {
    number_of_operations++;
    dd::Edge gate = this->dd->makeGateDD(matrix, n_qubits, target);
    dd::Edge tmp  = this->dd->multiply(gate, this->rootEdge);
    this->dd->incRef(tmp);
    this->dd->decRef(this->rootEdge);
    this->rootEdge = tmp;

    this->dd->garbageCollect();
}

/**
 * Emulate the modular exponentiation by directly building the appropriate decision diagram for the operation
 * @tparam Config Configuration for the underlying DD package
 * @param a the exponent
 */
template<class Config>
void ShorFastSimulator<Config>::u_a_emulate2(unsigned long long int a) {
    [[maybe_unused]] const std::size_t cache_count_before = this->dd->cn.cacheCount();
    dag_edges.clear();

    dd::vEdge                        f = dd::vEdge::one;
    std::array<dd::vEdge, dd::RADIX> edges{
            dd::vEdge::zero, dd::vEdge::zero};

    unsigned long long t = a;
    for (dd::Qubit p = 0; p < n_qubits - 1; ++p) {
        edges[0] = f;
        f        = this->dd->makeDDNode(p, edges);
        ts[p]    = t;
        t        = (2 * t) % n;
    }

    this->dd->incRef(f);

    for (auto& m: nodesOnLevel) {
        m = std::map<dd::vNode*, dd::vEdge>();
    }

    u_a_emulate2_rec(this->rootEdge.p->e[0]);

    for (const auto& entry: nodesOnLevel.at(0)) {
        dd::vEdge left = f;
        if (entry.first->e[0].w == dd::Complex::zero) {
            left = dd::vEdge::zero;
        } else {
            left.w = this->dd->cn.mulCached(left.w, entry.first->e[0].w);
        }

        dd::vEdge right = this->dd->multiply(addConstMod(ts[entry.first->v]), f);

        if (entry.first->e[1].w == dd::Complex::zero) {
            right = dd::vEdge::zero;
        } else {
            right.w = this->dd->cn.mulCached(right.w, entry.first->e[1].w);
        }

        dd::vEdge result = this->dd->add(left, right);

        if (left.w != dd::Complex::zero) {
            this->dd->cn.returnToCache(left.w);
        }
        if (right.w != dd::Complex::zero) {
            this->dd->cn.returnToCache(right.w);
        }

        this->dd->incRef(result);

        nodesOnLevel[0][entry.first] = result;
    }

    for (dd::QubitCount i = 1; i < n_qubits - 1; i++) {
        std::vector<dd::vEdge> saveEdges;
        for (auto it = nodesOnLevel.at(i).begin(); it != nodesOnLevel.at(i).end(); it++) {
            dd::vEdge left = dd::vEdge::zero;
            if (it->first->e.at(0).w != dd::Complex::zero) {
                left   = nodesOnLevel.at(i - 1)[it->first->e.at(0).p];
                left.w = this->dd->cn.mulCached(left.w, it->first->e.at(0).w);
            }

            dd::vEdge right = dd::vEdge::zero;
            if (it->first->e.at(1).w != dd::Complex::zero) {
                right   = this->dd->multiply(addConstMod(ts.at(it->first->v)), nodesOnLevel.at(i - 1)[it->first->e.at(1).p]);
                right.w = this->dd->cn.mulCached(right.w, it->first->e.at(1).w);
            }

            dd::vEdge result = this->dd->add(left, right);

            if (left.w != dd::Complex::zero) {
                this->dd->cn.returnToCache(left.w);
            }
            if (right.w != dd::Complex::zero) {
                this->dd->cn.returnToCache(right.w);
            }

            this->dd->incRef(result);
            nodesOnLevel.at(i)[it->first] = result;
            saveEdges.push_back(result);
        }
        for (auto& it: nodesOnLevel.at(i - 1)) {
            this->dd->decRef(it.second);
        }
        this->dd->garbageCollect();
        saveEdges.push_back(this->rootEdge);
        nodesOnLevel.at(i - 1).clear();
    }

    if (nodesOnLevel.at(n_qubits - 2).size() != 1) {
        throw std::runtime_error("error occurred");
    }

    dd::vEdge result = nodesOnLevel.at(n_qubits - 2)[this->rootEdge.p->e[0].p];

    this->dd->decRef(result);

    result.w = this->dd->cn.mulCached(result.w, this->rootEdge.p->e[0].w);
    auto tmp = result.w;

    result = this->dd->makeDDNode(this->rootEdge.p->v, std::array<dd::vEdge, dd::RADIX>{this->rootEdge.p->e[0], result});
    this->dd->cn.returnToCache(tmp);

    result.w = this->dd->cn.mulCached(result.w, this->rootEdge.w);
    tmp      = result.w;
    result.w = this->dd->cn.lookup(result.w);
    this->dd->cn.returnToCache(tmp);
    this->dd->decRef(this->rootEdge);
    this->dd->incRef(result);
    this->rootEdge = result;
    this->dd->garbageCollect();
    assert(this->dd->cn.cacheCount() == cache_count_before);
}

template<class Config>
void ShorFastSimulator<Config>::u_a_emulate2_rec(dd::vEdge e) {
    if (e.isTerminal() || nodesOnLevel.at(e.p->v).find(e.p) != nodesOnLevel.at(e.p->v).end()) {
        return;
    }
    nodesOnLevel.at(e.p->v)[e.p] = dd::vEdge::zero;
    u_a_emulate2_rec(e.p->e[0]);
    u_a_emulate2_rec(e.p->e[1]);
}

template class ShorFastSimulator<dd::DDPackageConfig>;