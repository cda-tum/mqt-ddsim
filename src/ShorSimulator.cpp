#include "ShorSimulator.hpp"

#include "dd/ComplexNumbers.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

template<class Config>
std::map<std::string, std::size_t> ShorSimulator<Config>::Simulate([[maybe_unused]] std::size_t shots) {
    if (verbose) {
        std::clog << "Simulate Shor's algorithm for n=" << compositeN;
    }

    if (emulate) {
        nQubits                     = static_cast<dd::QubitCount>(3 * requiredBits);
        Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(nQubits);
        Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);
        //Initialize qubits
        //TODO: other init method where the initial value can be chosen
        ApplyGate(dd::Xmat, 0);

    } else {
        nQubits                     = static_cast<dd::QubitCount>(2 * requiredBits + 3);
        Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(nQubits);
        Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);
        //Initialize qubits
        //TODO: other init method where the initial value can be chosen

        ApplyGate(dd::Xmat, static_cast<dd::Qubit>(nQubits - 1));
    }

    if (verbose) {
        std::clog << " (requires " << nQubits << " qubits):\n";
    }

    if (coprimeA != 0 && gcd(coprimeA, compositeN) != 1) {
        std::clog << "Warning: gcd(a=" << coprimeA << ", n=" << compositeN << ") != 1 --> choosing a new value for a\n";
        coprimeA = 0;
    }

    if (coprimeA == 0) {
        std::uniform_int_distribution<std::size_t> distribution(1, compositeN - 1); // range is inclusive
        do {
            coprimeA = distribution(Simulator<Config>::mt);
        } while (gcd(coprimeA, compositeN) != 1 || coprimeA == 1);
    }

    if (verbose) {
        std::clog << "Find a coprime to N=" << compositeN << ": " << coprimeA << "\n";
    }

    auto* as                 = new std::uint64_t[2 * requiredBits];
    as[2 * requiredBits - 1] = coprimeA;
    std::uint64_t new_a      = coprimeA;
    for (std::int32_t i = 2 * requiredBits - 2; i >= 0; i--) {
        new_a = new_a * new_a;
        new_a = new_a % compositeN;
        as[i] = new_a;
    }

    for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
        ApplyGate(dd::Hmat, (nQubits - 1) - i);
    }
    const std::int32_t mod = std::ceil(2 * requiredBits / 6.0); // log_0.9(0.5) is about 6
    auto               t1  = std::chrono::steady_clock::now();

    if (emulate) {
        for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
            if (verbose) {
                std::clog << "[ " << (i + 1) << "/" << 2 * requiredBits << " ] u_a_emulate(" << as[i] << ", " << i
                          << ") " << std::chrono::duration<float>(std::chrono::steady_clock::now() - t1).count() << "\n"
                          << std::flush;
            }
            u_a_emulate(as[i], i);
        }
    } else {
        for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
            if (verbose) {
                std::clog << "[ " << (i + 1) << "/" << 2 * requiredBits << " ] u_a(" << as[i] << ", " << compositeN << ", " << 0
                          << ") " << std::chrono::duration<float>(std::chrono::steady_clock::now() - t1).count() << "\n"
                          << std::flush;
            }
            u_a(as[i], compositeN, 0);
        }
    }

    if (verbose) {
        std::clog << "Nodes before QFT: " << Simulator<Config>::dd->size(Simulator<Config>::rootEdge) << "\n";
    }

    //EXACT QFT
    for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
        if (verbose) {
            std::clog << "[ " << i + 1 << "/" << 2 * requiredBits << " ] QFT Pass. dd size=" << Simulator<Config>::dd->size(Simulator<Config>::rootEdge)
                      << "\n";
        }
        double q = 2;

        for (std::int32_t j = i - 1; j >= 0; j--) {
            double         q_r = QMDDcos(1, -q);
            double         q_i = QMDDsin(1, -q);
            dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
            ApplyGate(Qm, nQubits - 1 - i, dd::Control{static_cast<dd::Qubit>(nQubits - 1 - j)});
            q *= 2;
        }

        if (approximate && (i + 1) % mod == 0) {
            finalFidelity *= Simulator<Config>::ApproximateByFidelity(stepFidelity, false, true);
            approximationRuns++;
        }

        ApplyGate(dd::Hmat, nQubits - 1 - i);
    }

    delete[] as;

    // Non-Quantum Post Processing

    // measure result (involves randomness)
    {
        std::string sample_reversed = Simulator<Config>::MeasureAll(false);
        std::string sample{sample_reversed.rbegin(), sample_reversed.rend()};
        simFactors = post_processing(sample);
        if (simFactors.first != 0 && simFactors.second != 0) {
            simResult =
                    std::string("SUCCESS(") + std::to_string(simFactors.first) + "*" +
                    std::to_string(simFactors.second) + ")";
        } else {
            simResult = "FAILURE";
        }
    }

    // path of least resistance result (does not involve randomness)
    {
        std::pair<dd::ComplexValue, std::string> polr_pair = Simulator<Config>::getPathOfLeastResistance();
        //std::clog << polr_pair.first << " " << polr_pair.second << "\n";
        std::string polr_reversed = polr_pair.second;
        std::string polr          = {polr_reversed.rbegin(), polr_reversed.rend()};
        polrFactors               = post_processing(polr);

        if (polrFactors.first != 0 && polrFactors.second != 0) {
            polrResult = std::string("SUCCESS(") + std::to_string(polrFactors.first) + "*" +
                         std::to_string(polrFactors.second) + ")";
        } else {
            polrResult = "FAILURE";
        }
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
std::pair<std::uint32_t, std::uint32_t> ShorSimulator<Config>::post_processing(const std::string& sample) const {
    std::ostream log{nullptr};
    if (verbose) {
        log.rdbuf(std::clog.rdbuf());
    }
    std::uint64_t res = 0;

    log << "measurement: ";
    for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
        log << sample.at(2 * requiredBits - 1 - i);
        res = (res << 1u) + (sample.at(requiredBits + i) == '1');
    }

    log << " = " << res << "\n";

    if (res == 0) {
        log << "Factorization failed (measured 0)!\n";
        return {0, 0};
    }
    std::vector<std::uint64_t> cf;
    auto                       denom     = 1ull << (2 * requiredBits);
    const auto                 old_denom = denom;
    const auto                 old_res   = res;

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

    for (std::uint32_t i = 0; i < cf.size(); i++) {
        //determine candidate
        std::uint64_t denominator = cf[i];
        std::uint64_t numerator   = 1;

        for (std::int32_t j = i - 1; j >= 0; j--) {
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
dd::mEdge ShorSimulator<Config>::limitTo(std::uint64_t a) {
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero};

    if (a & 1u) {
        edges[0] = edges[3] = dd::mEdge::one;
    } else {
        edges[0] = dd::mEdge::one;
    }
    dd::Edge f = Simulator<Config>::dd->makeDDNode(0, edges, false);

    edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero;

    for (std::uint32_t p = 1; p < requiredBits + 1; p++) {
        if ((a >> p) & 1u) {
            edges[0] = Simulator<Config>::dd->makeIdent(0, p - 1);
            edges[3] = f;
        } else {
            edges[0] = f;
        }
        f        = Simulator<Config>::dd->makeDDNode(p, edges, false);
        edges[3] = dd::mEdge::zero;
    }

    return f;
}

template<class Config>
dd::mEdge ShorSimulator<Config>::addConst(std::uint64_t a) {
    dd::mEdge                f = dd::mEdge::one;
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero};

    std::uint32_t p = 0;
    while (!((a >> p) & 1u)) {
        edges[0] = f;
        edges[3] = f;
        f        = Simulator<Config>::dd->makeDDNode(p, edges, false);
        p++;
    }

    dd::mEdge right, left;

    edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero;
    edges[2]                                  = f;
    left                                      = Simulator<Config>::dd->makeDDNode(p, edges, false);
    edges[2]                                  = dd::mEdge::zero;
    edges[1]                                  = f;
    right                                     = Simulator<Config>::dd->makeDDNode(p, edges, false);
    p++;

    dd::mEdge new_left, new_right;
    for (; p < requiredBits; p++) {
        edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero;
        if ((a >> p) & 1u) {
            edges[2]  = left;
            new_left  = Simulator<Config>::dd->makeDDNode(p, edges, false);
            edges[2]  = dd::mEdge::zero;
            edges[0]  = right;
            edges[1]  = left;
            edges[3]  = right;
            new_right = Simulator<Config>::dd->makeDDNode(p, edges, false);
        } else {
            edges[1]  = right;
            new_right = Simulator<Config>::dd->makeDDNode(p, edges, false);
            edges[1]  = dd::mEdge::zero;
            edges[0]  = left;
            edges[2]  = right;
            edges[3]  = left;
            new_left  = Simulator<Config>::dd->makeDDNode(p, edges, false);
        }
        left  = new_left;
        right = new_right;
    }

    edges[0] = left;
    edges[1] = right;
    edges[2] = right;
    edges[3] = left;

    return Simulator<Config>::dd->makeDDNode(p, edges, false);
}

template<class Config>
dd::mEdge ShorSimulator<Config>::addConstMod(std::uint64_t a) {
    dd::Edge f = addConst(a);

    dd::Edge f2 = addConst(compositeN);

    dd::Edge f3 = limitTo(compositeN - 1);

    dd::Edge f4 = limitTo(compositeN - 1 - a);

    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge diff2 = Simulator<Config>::dd->add(f3, f4);
    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge tmp   = Simulator<Config>::dd->add(Simulator<Config>::dd->multiply(f, f4), Simulator<Config>::dd->multiply(Simulator<Config>::dd->multiply(Simulator<Config>::dd->transpose(f2), f), diff2));

    return tmp.p->e[0];
}

template<class Config>
dd::mEdge ShorSimulator<Config>::limitStateVector(dd::vEdge e) {
    if (e.p == dd::vEdge::zero.p) {
        if (e.w == dd::Complex::zero) {
            return dd::mEdge::zero;
        } else {
            return dd::mEdge::one;
        }
    }
    auto it = dagEdges.find(e.p);
    if (it != dagEdges.end()) {
        return it->second;
    }

    std::array<dd::mEdge, 4> edges{
            limitStateVector(e.p->e.at(0)),
            dd::mEdge::zero,
            dd::mEdge::zero,
            limitStateVector(e.p->e.at(1))};

    dd::Edge result = Simulator<Config>::dd->makeDDNode(e.p->v, edges, false);
    dagEdges[e.p]   = result;
    return result;
}

template<class Config>
void ShorSimulator<Config>::u_a_emulate(std::uint64_t a, std::int32_t q) {
    dd::mEdge limit = Simulator<Config>::dd->makeIdent(0, requiredBits - 1);

    dd::mEdge                f = dd::mEdge::one;
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero};

    for (std::uint32_t p = 0; p < requiredBits; ++p) {
        edges[0] = f;
        edges[1] = f;
        f        = Simulator<Config>::dd->makeDDNode(p, edges, false);
    }

    //TODO: limitTo?

    f = Simulator<Config>::dd->multiply(f, limit);

    edges[1] = dd::mEdge::zero;

    Simulator<Config>::dd->incRef(f);
    Simulator<Config>::dd->incRef(limit);

    std::uint64_t t = a;

    for (std::uint32_t i = 0; i < requiredBits; ++i) {
        dd::mEdge active = dd::mEdge::one;
        for (std::uint32_t p = 0; p < requiredBits; ++p) {
            if (p == i) {
                edges[3] = active;
                edges[0] = dd::mEdge::zero;
            } else {
                edges[0] = edges[3] = active;
            }
            active = Simulator<Config>::dd->makeDDNode(p, edges, false);
        }

        active.w          = Simulator<Config>::dd->cn.lookup(-1, 0);
        dd::mEdge passive = Simulator<Config>::dd->multiply(f, Simulator<Config>::dd->add(limit, active));
        active.w          = dd::Complex::one;
        active            = Simulator<Config>::dd->multiply(f, active);

        dd::mEdge tmp = addConstMod(t);
        active        = Simulator<Config>::dd->multiply(tmp, active);

        Simulator<Config>::dd->decRef(f);
        f = Simulator<Config>::dd->add(active, passive);
        Simulator<Config>::dd->incRef(f);
        Simulator<Config>::dd->garbageCollect();

        t = (2 * t) % compositeN;
    }

    Simulator<Config>::dd->decRef(limit);
    Simulator<Config>::dd->decRef(f);

    dd::mEdge e = f;

    for (std::int32_t i = 2 * requiredBits - 1; i >= 0; --i) {
        if (i == q) {
            edges[1] = edges[2] = dd::mEdge::zero;
            edges[0]            = Simulator<Config>::dd->makeIdent(0, nQubits - i - 2);
            edges[3]            = e;
            e                   = Simulator<Config>::dd->makeDDNode(nQubits - 1 - i, edges, false);
        } else {
            edges[1] = edges[2] = dd::mEdge::zero;
            edges[0] = edges[3] = e;
            e                   = Simulator<Config>::dd->makeDDNode(nQubits - 1 - i, edges, false);
        }
    }

    dd::vEdge tmp = Simulator<Config>::dd->multiply(e, Simulator<Config>::rootEdge);
    Simulator<Config>::dd->incRef(tmp);
    Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
    Simulator<Config>::rootEdge = tmp;

    Simulator<Config>::dd->garbageCollect();
}

template<class Config>
std::int32_t ShorSimulator<Config>::inverse_mod(std::int32_t a, std::int32_t n) {
    std::int32_t t    = 0;
    std::int32_t newt = 1;
    std::int32_t r    = n;
    std::int32_t newr = a;
    while (newr != 0) {
        std::int32_t quotient = r / newr;
        std::int32_t h        = t;
        t                     = newt;
        newt                  = h - quotient * newt;
        h                     = r;
        r                     = newr;
        newr                  = h - quotient * newr;
    }
    if (r > 1) {
        std::cerr << "ERROR: a=" << a << " with n=" << n << " is not invertible\n";
        std::exit(3);
    }
    if (t < 0) {
        t = t + n;
    }
    return t;
}

template<class Config>
void ShorSimulator<Config>::add_phi(std::uint64_t a, std::int32_t c1, std::int32_t c2) {
    for (auto i = static_cast<std::int32_t>(requiredBits); i >= 0; --i) {
        double        q   = 1;
        std::uint32_t fac = 0;
        for (std::int32_t j = i; j >= 0; --j) {
            if ((a >> j) & 1u) {
                fac |= 1u;
            }
            fac *= 2;
            q *= 2;
        }

        dd::Controls controls;
        if (c1 != std::numeric_limits<std::int32_t>::min()) {
            controls.emplace(dd::Control{static_cast<dd::Qubit>((nQubits - 1) - c1)});
        }
        if (c2 != std::numeric_limits<std::int32_t>::min()) {
            controls.emplace(dd::Control{static_cast<dd::Qubit>((nQubits - 1) - c2)});
        }

        double         q_r = QMDDcos(fac, q);
        double         q_i = QMDDsin(fac, q);
        dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};

        ApplyGate(Qm, static_cast<dd::Qubit>((nQubits - 1) - (1 + 2 * requiredBits - static_cast<std::uint32_t>(i))), controls);
    }
}

template<class Config>
void ShorSimulator<Config>::add_phi_inv(std::uint64_t a, std::int32_t c1, std::int32_t c2) {
    for (auto i = static_cast<std::int32_t>(requiredBits); i >= 0; --i) {
        double        q   = 1;
        std::uint32_t fac = 0;
        for (std::int32_t j = i; j >= 0; --j) {
            if ((a >> j) & 1u) {
                fac |= 1u;
            }
            fac *= 2;
            q *= 2;
        }
        dd::Controls controls;
        if (c1 != std::numeric_limits<std::int32_t>::min()) {
            controls.emplace(dd::Control{static_cast<dd::Qubit>((nQubits - 1) - c1)});
        }
        if (c2 != std::numeric_limits<std::int32_t>::min()) {
            controls.emplace(dd::Control{static_cast<dd::Qubit>((nQubits - 1) - c2)});
        }

        double         q_r = QMDDcos(fac, -q);
        double         q_i = QMDDsin(fac, -q);
        dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
        ApplyGate(Qm, static_cast<dd::Qubit>((nQubits - 1) - (1 + 2 * requiredBits - i)), controls);
    }
}

template<class Config>
void ShorSimulator<Config>::qft() {
    for (std::uint32_t i = requiredBits + 1; i < 2 * requiredBits + 2; i++) {
        ApplyGate(dd::Hmat, static_cast<dd::Qubit>((nQubits - 1) - i));

        double q = 2;
        for (std::uint32_t j = i + 1; j < 2 * requiredBits + 2; j++) {
            double         q_r = QMDDcos(1, q);
            double         q_i = QMDDsin(1, q);
            dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
            ApplyGate(Qm, static_cast<dd::Qubit>((nQubits - 1) - i), dd::Control{static_cast<dd::Qubit>((nQubits - 1) - j)});
            q *= 2;
        }
    }
}

template<class Config>
void ShorSimulator<Config>::qft_inv() {
    for (std::uint32_t i = 2 * requiredBits + 1; i >= requiredBits + 1; i--) {
        double q = 2;
        for (std::uint32_t j = i + 1; j < 2 * requiredBits + 2; j++) {
            double         q_r = QMDDcos(1, -q);
            double         q_i = QMDDsin(1, -q);
            dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
            ApplyGate(Qm, (nQubits - 1) - i, dd::Control{static_cast<dd::Qubit>((nQubits - 1) - j)});
            q *= 2;
        }
        ApplyGate(dd::Hmat, (nQubits - 1) - i);
    }
}

template<class Config>
void ShorSimulator<Config>::mod_add_phi(std::uint64_t a, std::uint32_t N, std::int32_t c1, std::int32_t c2) {
    add_phi(a, c1, c2);
    add_phi_inv(N, std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::min());

    qft_inv();

    ApplyGate(dd::Xmat, (nQubits - 1) - (2 * requiredBits + 2),
              dd::Control{static_cast<dd::Qubit>((nQubits - 1) - (requiredBits + 1))});

    qft();
    add_phi(N, 2 * requiredBits + 2, std::numeric_limits<std::int32_t>::min());
    add_phi_inv(a, c1, c2);
    qft_inv();

    ApplyGate(dd::Xmat, (nQubits - 1) - (2 * requiredBits + 2),
              dd::Control{static_cast<dd::Qubit>((nQubits - 1) - (requiredBits + 1)), dd::Control::Type::neg});

    qft();
    add_phi(a, c1, c2);
}

template<class Config>
void ShorSimulator<Config>::mod_add_phi_inv(std::int32_t a, std::int32_t N, std::int32_t c1, std::int32_t c2) {
    add_phi_inv(a, c1, c2);
    qft_inv();

    ApplyGate(dd::Xmat, static_cast<dd::Qubit>((nQubits - 1) - (2 * requiredBits + 2)), dd::Control{static_cast<dd::Qubit>((nQubits - 1) - (requiredBits + 1)), dd::Control::Type::neg});

    qft();
    add_phi(a, c1, c2);
    add_phi_inv(N, 2 * requiredBits + 2, std::numeric_limits<std::int32_t>::min());
    qft_inv();

    ApplyGate(dd::Xmat, (nQubits - 1) - (2 * requiredBits + 2), dd::Control{static_cast<dd::Qubit>((nQubits - 1) - (requiredBits + 1))});

    qft();
    add_phi(N, std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::min());
    add_phi_inv(a, c1, c2);
}

template<class Config>
void ShorSimulator<Config>::cmult(std::uint64_t a, std::uint32_t N, std::int32_t c) {
    qft();

    std::uint64_t t = a;
    for (std::uint32_t i = requiredBits; i >= 1; i--) {
        mod_add_phi(t, N, i, c);
        t = (2 * t) % N;
    }
    qft_inv();
}

template<class Config>
void ShorSimulator<Config>::cmult_inv(std::uint64_t a, std::uint32_t N, std::int32_t c) {
    qft();
    std::uint64_t t = a;
    for (std::uint32_t i = requiredBits; i >= 1; i--) {
        mod_add_phi_inv(t, N, i, c);
        t = (2 * t) % N;
    }
    qft_inv();
}

template<class Config>
void ShorSimulator<Config>::u_a(std::uint64_t a, std::int32_t N, std::int32_t c) {
    using namespace dd::literals;
    cmult(a, N, c);
    for (std::uint32_t i = 0; i < requiredBits; i++) {
        ApplyGate(dd::Xmat, static_cast<dd::Qubit>((nQubits - 1) - (i + 1)), dd::Control{static_cast<dd::Qubit>((nQubits - 1) - (requiredBits + 2 + i))});

        ApplyGate(dd::Xmat, static_cast<dd::Qubit>((nQubits - 1) - (requiredBits + 2 + i)), dd::Controls{{dd::Control{static_cast<dd::Qubit>(nQubits - 1 - (i + 1))}, dd::Control{static_cast<dd::Qubit>(nQubits - 1 - c)}}});

        ApplyGate(dd::Xmat, static_cast<dd::Qubit>((nQubits - 1) - (i + 1)), dd::Control{static_cast<dd::Qubit>((nQubits - 1) - (requiredBits + 2 + i))});
    }

    cmult_inv(inverse_mod(a, N), N, c);
}

template<class Config>
void ShorSimulator<Config>::ApplyGate(dd::GateMatrix matrix, dd::Qubit target) {
    dd::Edge gate = Simulator<Config>::dd->makeGateDD(matrix, nQubits, target);
    dd::Edge tmp  = Simulator<Config>::dd->multiply(gate, Simulator<Config>::rootEdge);
    Simulator<Config>::dd->incRef(tmp);
    Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
    Simulator<Config>::rootEdge = tmp;

    Simulator<Config>::dd->garbageCollect();
}

template<class Config>
void ShorSimulator<Config>::ApplyGate(dd::GateMatrix matrix, dd::Qubit target, dd::Control control) {
    dd::Edge gate = Simulator<Config>::dd->makeGateDD(matrix, nQubits, control, target);
    dd::Edge tmp  = Simulator<Config>::dd->multiply(gate, Simulator<Config>::rootEdge);
    Simulator<Config>::dd->incRef(tmp);
    Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
    Simulator<Config>::rootEdge = tmp;

    Simulator<Config>::dd->garbageCollect();
}

template<class Config>
void ShorSimulator<Config>::ApplyGate(dd::GateMatrix matrix, dd::Qubit target, const dd::Controls& controls) {
    dd::Edge gate = Simulator<Config>::dd->makeGateDD(matrix, nQubits, controls, target);
    dd::Edge tmp  = Simulator<Config>::dd->multiply(gate, Simulator<Config>::rootEdge);
    Simulator<Config>::dd->incRef(tmp);
    Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
    Simulator<Config>::rootEdge = tmp;

    Simulator<Config>::dd->garbageCollect();
}

template class ShorSimulator<dd::DDPackageConfig>;
