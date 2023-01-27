#include "ShorFastSimulator.hpp"

#include <chrono>
#include <cmath>
#include <dd/ComplexNumbers.hpp>
#include <iostream>
#include <limits>
#include <random>

template<class Config>
std::map<std::string, std::size_t> ShorFastSimulator<Config>::Simulate([[maybe_unused]] std::size_t shots) {
    if (verbose) {
        std::clog << "Simulate Shor's algorithm for n=" << compositeN;
    }
    this->rootEdge = this->dd->makeZeroState(nQubits);
    this->dd->incRef(this->rootEdge);
    //Initialize qubits
    //TODO: other init method where the initial value can be chosen
    ApplyGate(dd::Xmat, 0);

    if (verbose) {
        std::clog << " (requires " << +nQubits << " qubits):\n";
    }

    if (coprimeA != 0 && gcd(coprimeA, compositeN) != 1) {
        std::clog << "Warning: gcd(a=" << coprimeA << ", n=" << compositeN << ") != 1 --> choosing a new value for a\n";
        coprimeA = 0;
    }

    if (coprimeA == 0) {
        std::uniform_int_distribution<std::uint32_t> distribution(1, compositeN - 1); // range is inclusive
        do {
            coprimeA = distribution(this->mt);
        } while (gcd(coprimeA, compositeN) != 1 || coprimeA == 1);
    }

    if (verbose) {
        std::clog << "Find a coprime to N=" << compositeN << ": " << coprimeA << "\n";
    }

    auto* as                  = new std::uint64_t[2 * requiredBits]; // replace with stl container
    as[2 * requiredBits - 1] = coprimeA;
    std::uint64_t new_a  = coprimeA;
    for (std::int32_t i = 2 * requiredBits - 2; i >= 0; i--) {
        new_a = new_a * new_a;
        new_a = new_a % compositeN;
        as[i] = new_a;
    }

    auto        t1 = std::chrono::steady_clock::now();
    std::string measurements(2 * requiredBits, '0');

    for (std::size_t i = 0; i < 2 * requiredBits; i++) {
        ApplyGate(dd::Hmat, nQubits - 1);

        if (verbose) {
            std::clog << "[ " << (i + 1) << "/" << 2 * requiredBits << " ] u_a_emulate2(" << as[i] << ") "
                      << std::chrono::duration<float>(std::chrono::steady_clock::now() - t1).count() << "\n"
                      << std::flush;
        }
        u_a_emulate2(as[i]);

        if (verbose) {
            std::clog << "[ " << i + 1 << "/" << 2 * requiredBits << " ] QFT Pass. dd size=" << this->dd->size(this->rootEdge)
                      << "\n";
        }

        double q = 2;
        for (int j = i - 1; j >= 0; j--) {
            if (measurements[j] == '1') {
                double         q_r = QMDDcos(1, -q);
                double         q_i = QMDDsin(1, -q);
                dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
                ApplyGate(Qm, nQubits - 1);
            }
            q *= 2;
        }

        ApplyGate(dd::Hmat, nQubits - 1);

        measurements[i] = this->MeasureOneCollapsing(nQubits - 1, false);
        this->dd->garbageCollect();

        if (measurements[i] == '1') {
            ApplyGate(dd::Xmat, nQubits - 1);
        }
    }

    delete[] as;

    // Non-Quantum Post Processing
    simFactors = post_processing(measurements);
    if (simFactors.first != 0 && simFactors.second != 0) {
        simResult = "SUCCESS(" + std::to_string(simFactors.first) + "*" + std::to_string(simFactors.second) + ")";
    } else {
        simResult = "FAILURE";
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
std::pair<std::uint32_t, std::uint32_t> ShorFastSimulator<Config>::post_processing(const std::string& sample) const {
    std::ostream log{nullptr};
    if (verbose) {
        log.rdbuf(std::clog.rdbuf());
    }
    std::uint64_t res = 0;

    log << "measurement: ";
    for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
        log << sample.at(2 * requiredBits - 1 - i);
        res = (res << 1u) + (sample.at(2 * requiredBits - 1 - i) == '1');
    }

    log << " = " << res << "\n";

    if (res == 0) {
        log << "Factorization failed (measured 0)!\n";
        return {0, 0};
    }
    std::vector<std::uint64_t> cf;
    auto                            denom     = 1ull << (2 * requiredBits);
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

    for (int i = 0; static_cast<std::size_t>(i) < cf.size(); i++) {
        //determine candidate
        std::uint64_t denominator = cf[i];
        std::uint64_t numerator   = 1;

        for (int j = i - 1; j >= 0; j--) {
            const auto tmp = numerator + cf[j] * denominator;
            numerator      = denominator;
            denominator    = tmp;
        }

        log << "  Candidate " << numerator << "/" << denominator << ": ";

        if (denominator > compositeN) {
            log << " denominator too large (greater than " << compositeN << ")!\nFactorization failed!\n";
            return {0, 0};
        }

        const double delta = static_cast<double>(old_res) / static_cast<double>(old_denom) - static_cast<double>(numerator) / static_cast<double>(denominator);
        if (std::abs(delta) >= 1.0 / (2.0 * static_cast<double>(old_denom))) {
            log << "delta is too big (" << delta << ")\n";
            continue;
        }

        std::uint64_t fact = 1;
        while (denominator * fact < compositeN && modpow(coprimeA, denominator * fact, compositeN) != 1) {
            fact++;
        }

        if (modpow(coprimeA, denominator * fact, compositeN) != 1) {
            log << "failed\n";
            continue;
        }

        log << "found period: " << denominator << " * " << fact << " = " << (denominator * fact) << "\n";

        if ((denominator * fact) & 1u) {
            log << "Factorization failed (period is odd)!\n";
            return {0, 0};
        }

        auto f1 = modpow(coprimeA, (denominator * fact) / 2, compositeN);
        auto f2 = (f1 + 1) % compositeN;
        f1      = (f1 == 0) ? compositeN - 1 : f1 - 1;
        f1      = gcd(f1, compositeN);
        f2      = gcd(f2, compositeN);

        if (f1 == 1ull || f2 == 1ull) {
            log << "Factorization failed: found trivial factors " << f1 << " and " << f2 << "\n";
            return {0, 0};
        }
        log << "Factorization succeeded! Non-trivial factors are: \n"
            << "  -- gcd(" << compositeN << "^(" << (denominator * fact) << "/2)-1"
            << "," << compositeN << ") = " << f1 << "\n"
            << "  -- gcd(" << compositeN << "^(" << (denominator * fact) << "/2)+1"
            << "," << compositeN << ") = " << f2 << "\n";
        return {f1, f2};
    }
    return {0, 0};
}

template<class Config>
dd::mEdge ShorFastSimulator<Config>::limitTo(std::uint64_t a) {
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

    for (std::uint32_t p = 1; p < requiredBits + 1; p++) {
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
dd::mEdge ShorFastSimulator<Config>::addConst(std::uint64_t a) {
    dd::Edge                 f = dd::mEdge::one;
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero, dd::mEdge::zero,
            dd::mEdge::zero, dd::mEdge::zero};

    std::uint32_t p = 0;
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

    for (; p < requiredBits; p++) {
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
dd::mEdge ShorFastSimulator<Config>::addConstMod(std::uint64_t a) {
    dd::mEdge f = addConst(a);

    dd::mEdge f2 = addConst(compositeN);

    dd::mEdge f3 = limitTo(compositeN - 1);

    dd::mEdge f4 = limitTo(compositeN - 1 - a);

    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge diff2 = this->dd->add(f3, f4);
    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge tmp   = this->dd->add(this->dd->multiply(f, f4), this->dd->multiply(this->dd->multiply(this->dd->transpose(f2), f), diff2));

    return tmp.p->e[0];
}

template<class Config>
void ShorFastSimulator<Config>::ApplyGate(dd::GateMatrix matrix, qc::Qubit target) {
    numberOfOperations++;
    dd::Edge gate = this->dd->makeGateDD(matrix, nQubits, target);
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
void ShorFastSimulator<Config>::u_a_emulate2(std::uint64_t a) {
    [[maybe_unused]] const std::size_t cache_count_before = this->dd->cn.cacheCount();
    dagEdges.clear();

    dd::vEdge                        f = dd::vEdge::one;
    std::array<dd::vEdge, dd::RADIX> edges{
            dd::vEdge::zero, dd::vEdge::zero};

    std::uint64_t t = a;
    for (qc::Qubit p = 0; p < nQubits - 1; ++p) {
        edges[0] = f;
        f        = this->dd->makeDDNode(p, edges);
        ts[p]    = t;
        t        = (2 * t) % compositeN;
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

    for (std::size_t i = 1; i < nQubits - 1; i++) {
        std::vector<dd::vEdge> saveEdges;
        for (auto it = nodesOnLevel.at(i).begin(); it != nodesOnLevel.at(i).end(); it++) {
            dd::vEdge left = dd::vEdge::zero;
            if (it->first->e.at(0).w != dd::Complex::zero) {
                left   = nodesOnLevel.at(i - 1)[it->first->e.at(0).p];
                left.w = this->dd->cn.mulCached(left.w, it->first->e.at(0).w);
            }

            dd::vEdge right = dd::vEdge::zero;
            if (it->first->e.at(1).w != dd::Complex::zero) {
                right   = this->dd->multiply(addConstMod(ts.at(static_cast<std::size_t>(it->first->v))), nodesOnLevel.at(i - 1)[it->first->e.at(1).p]);
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

    if (nodesOnLevel.at(nQubits - 2).size() != 1) {
        throw std::runtime_error("error occurred");
    }

    dd::vEdge result = nodesOnLevel.at(nQubits - 2)[this->rootEdge.p->e[0].p];

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