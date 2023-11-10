#include "ShorSimulator.hpp"

#include "dd/ComplexNumbers.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

template<class Config>
std::map<std::string, std::size_t> ShorSimulator<Config>::simulate([[maybe_unused]] std::size_t shots) {
    if (verbose) {
        std::clog << "simulate Shor's algorithm for n=" << compositeN;
    }

    nQubits                     = static_cast<dd::Qubit>(3 * requiredBits);
    Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(static_cast<dd::Qubit>(nQubits));
    Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);
    //Initialize qubits
    //TODO: other init method where the initial value can be chosen
    applyGate(dd::X_MAT, 0);

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

    std::vector<std::uint64_t> as;
    as.resize(2 * requiredBits);
    assert(as.size() == 2 * requiredBits); // it's quite easy to get the vector initialization wrong
    as[2 * requiredBits - 1] = coprimeA;
    std::uint64_t newA       = coprimeA;
    for (auto i = static_cast<std::int64_t>(2 * requiredBits - 2); i >= 0; i--) {
        newA                            = newA * newA;
        newA                            = newA % compositeN;
        as[static_cast<std::size_t>(i)] = newA;
    }

    for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
        applyGate(dd::H_MAT, static_cast<dd::Qubit>((nQubits - 1) - i));
    }
    const auto mod = static_cast<std::int32_t>(std::ceil(2.0 * static_cast<double>(requiredBits) / 6.0)); // log_0.9(0.5) is about 6
    const auto t1  = std::chrono::steady_clock::now();

    for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
        if (verbose) {
            std::clog << "[ " << (i + 1) << "/" << 2 * requiredBits << " ] uAEmulate(" << as[i] << ", " << i
                      << ") " << std::chrono::duration<float>(std::chrono::steady_clock::now() - t1).count() << "\n"
                      << std::flush;
        }
        uAEmulate(as[i], static_cast<std::int32_t>(i));
    }

    if (verbose) {
        std::clog << "Nodes before QFT: " << Simulator<Config>::rootEdge.size() << "\n";
    }

    //EXACT QFT
    for (std::int32_t i = 0; i < static_cast<std::int32_t>(2 * requiredBits); i++) {
        if (verbose) {
            std::clog << "[ " << i + 1 << "/" << 2 * requiredBits << " ] QFT Pass. dd size=" << Simulator<Config>::rootEdge.size()
                      << "\n";
        }
        double q = 2;

        for (std::int32_t j = i - 1; j >= 0; j--) {
            double               qR = cosine(1, -q);
            double               qI = sine(1, -q);
            const dd::GateMatrix qm{1, 0, 0, {qR, qI}};
            applyGate(qm, static_cast<dd::Qubit>(nQubits - 1 - static_cast<std::size_t>(i)), qc::Control{static_cast<qc::Qubit>(nQubits - 1 - static_cast<std::size_t>(j))});
            q *= 2;
        }

        if (approximate && (i + 1) % mod == 0) {
            finalFidelity *= static_cast<long double>(Simulator<Config>::approximateByFidelity(stepFidelity, false, true));
            approximationRuns++;
        }

        applyGate(dd::H_MAT, static_cast<dd::Qubit>(nQubits - 1 - static_cast<std::size_t>(i)));
    }

    // Non-Quantum Post Processing

    // measure result (involves randomness)
    {
        std::string       sampleReversed = Simulator<Config>::measureAll(false);
        const std::string sample{sampleReversed.rbegin(), sampleReversed.rend()};
        simFactors = postProcessing(sample);
        if (simFactors.first != 0 && simFactors.second != 0) {
            simResult =
                    std::string("SUCCESS(") + std::to_string(simFactors.first) + "*" +
                    std::to_string(simFactors.second) + ")";
        } else {
            simResult = "FAILURE";
        }
    }

    // the path of least resistance result (does not involve randomness)
    {
        const std::pair<dd::ComplexValue, std::string> polrPair = Simulator<Config>::getPathOfLeastResistance();
        std::clog << polrPair.first << " " << polrPair.second << "\n";
        std::string       polrReversed = polrPair.second;
        const std::string polr         = {polrReversed.rbegin(), polrReversed.rend()};
        polrFactors                    = postProcessing(polr);

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
std::pair<std::uint32_t, std::uint32_t> ShorSimulator<Config>::postProcessing(const std::string& sample) const {
    std::ostream log{nullptr};
    if (verbose) {
        log.rdbuf(std::clog.rdbuf());
    }
    std::uint64_t res = 0;

    log << "measurement: ";
    for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
        log << sample.at(2 * requiredBits - 1 - i);
        res = (res << 1U) + (sample.at(requiredBits + i) == '1' ? 1 : 0);
    }

    log << " = " << res << "\n";

    if (res == 0) {
        log << "Factorization failed (measured 0)!\n";
        return {0, 0};
    }
    std::vector<std::uint64_t> cf;
    auto                       denom    = 1ULL << (2 * requiredBits);
    const auto                 oldDenom = denom;
    const auto                 oldRes   = res;

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

        for (std::int32_t j = static_cast<std::int32_t>(i) - 1; j >= 0; j--) {
            const auto tmp = numerator + cf[static_cast<std::size_t>(j)] * denominator;
            numerator      = denominator;
            denominator    = tmp;
        }

        log << "  Candidate " << numerator << "/" << denominator << ": ";

        if (denominator > compositeN) {
            log << " denominator too large (greater than " << compositeN << ")!\nFactorization failed!\n";
            return {0, 0};
        }

        const double delta = static_cast<double>(oldRes) / static_cast<double>(oldDenom) - static_cast<double>(numerator) / static_cast<double>(denominator);
        if (std::abs(delta) >= 1.0 / (2.0 * static_cast<double>(oldDenom))) {
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

        if (((denominator * fact) & 1U) > 0) {
            log << "Factorization failed (period is odd)!\n";
            return {0, 0};
        }

        auto f1 = modpow(coprimeA, (denominator * fact) / 2, compositeN);
        auto f2 = (f1 + 1) % compositeN;
        f1      = (f1 == 0) ? compositeN - 1 : f1 - 1;
        f1      = gcd(f1, compositeN);
        f2      = gcd(f2, compositeN);

        if (f1 == 1ULL || f2 == 1ULL) {
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
            dd::mEdge::zero(),
            dd::mEdge::zero(),
            dd::mEdge::zero(),
            dd::mEdge::zero()};

    if ((a & 1U) > 0) {
        edges[0] = edges[3] = dd::mEdge::one();
    } else {
        edges[0] = dd::mEdge::one();
    }
    dd::Edge f = Simulator<Config>::dd->makeDDNode(0, edges, false);

    edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero();

    for (std::uint32_t p = 1; p < requiredBits + 1; p++) {
        if (((a >> p) & 1U) > 0) {
            edges[0] = Simulator<Config>::dd->makeIdent(0, static_cast<dd::Qubit>(p - 1));
            edges[3] = f;
        } else {
            edges[0] = f;
        }
        f        = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
        edges[3] = dd::mEdge::zero();
    }

    return f;
}

template<class Config>
dd::mEdge ShorSimulator<Config>::addConst(std::uint64_t a) {
    dd::mEdge                f = dd::mEdge::one();
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero(),
            dd::mEdge::zero(),
            dd::mEdge::zero(),
            dd::mEdge::zero()};

    std::uint32_t p = 0;
    while (((a >> p) & 1U) == 0U) {
        edges[0] = f;
        edges[3] = f;
        f        = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
        p++;
    }

    dd::mEdge right;
    dd::mEdge left;

    edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero();
    edges[2]                                  = f;
    left                                      = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
    edges[2]                                  = dd::mEdge::zero();
    edges[1]                                  = f;
    right                                     = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
    p++;

    dd::mEdge newLeft;
    dd::mEdge newRight;
    for (; p < requiredBits; p++) {
        edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero();
        if (((a >> p) & 1U) != 0U) {
            edges[2] = left;
            newLeft  = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
            edges[2] = dd::mEdge::zero();
            edges[0] = right;
            edges[1] = left;
            edges[3] = right;
            newRight = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
        } else {
            edges[1] = right;
            newRight = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
            edges[1] = dd::mEdge::zero();
            edges[0] = left;
            edges[2] = right;
            edges[3] = left;
            newLeft  = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
        }
        left  = newLeft;
        right = newRight;
    }

    edges[0] = left;
    edges[1] = right;
    edges[2] = right;
    edges[3] = left;

    return Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
}

template<class Config>
dd::mEdge ShorSimulator<Config>::addConstMod(std::uint64_t a) {
    const auto f  = addConst(a);
    const auto f2 = addConst(compositeN);
    const auto f3 = limitTo(compositeN - 1);

    auto f4 = limitTo(compositeN - 1 - a);
    f4.w    = dd::ComplexNumbers::neg(f4.w);

    const auto diff2 = Simulator<Config>::dd->add(f3, f4);

    f4.w = dd::ComplexNumbers::neg(f4.w);

    const auto simEdgeMultiply = Simulator<Config>::dd->multiply(Simulator<Config>::dd->conjugateTranspose(f2), f);
    const auto simEdgeResult   = Simulator<Config>::dd->add(Simulator<Config>::dd->multiply(f, f4), Simulator<Config>::dd->multiply(simEdgeMultiply, diff2));

    return simEdgeResult.p->e[0]; // NOLINT(clang-analyzer-core.CallAndMessage) Function Pointer is not null
}

template<class Config>
void ShorSimulator<Config>::uAEmulate(std::uint64_t a, std::int32_t q) {
    const dd::mEdge limit = Simulator<Config>::dd->makeIdent(0, static_cast<dd::Qubit>(requiredBits - 1));

    dd::mEdge                f = dd::mEdge::one();
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero(),
            dd::mEdge::zero(),
            dd::mEdge::zero(),
            dd::mEdge::zero()};

    for (std::uint32_t p = 0; p < requiredBits; ++p) {
        edges[0] = f;
        edges[1] = f;
        f        = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
    }

    //TODO: limitTo?

    f = Simulator<Config>::dd->multiply(f, limit);

    edges[1] = dd::mEdge::zero();

    Simulator<Config>::dd->incRef(f);
    Simulator<Config>::dd->incRef(limit);

    std::uint64_t t = a;

    for (std::uint32_t i = 0; i < requiredBits; ++i) {
        dd::mEdge active = dd::mEdge::one();
        for (std::uint32_t p = 0; p < requiredBits; ++p) {
            if (p == i) {
                edges[3] = active;
                edges[0] = dd::mEdge::zero();
            } else {
                edges[0] = edges[3] = active;
            }
            active = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges, false);
        }

        active.w                = Simulator<Config>::dd->cn.lookup(-1, 0);
        const dd::mEdge passive = Simulator<Config>::dd->multiply(f, Simulator<Config>::dd->add(limit, active));
        active.w                = dd::Complex::one();
        active                  = Simulator<Config>::dd->multiply(f, active);

        const dd::mEdge tmp = addConstMod(t);
        active              = Simulator<Config>::dd->multiply(tmp, active);

        Simulator<Config>::dd->decRef(f);
        f = Simulator<Config>::dd->add(active, passive);
        Simulator<Config>::dd->incRef(f);
        Simulator<Config>::dd->garbageCollect();

        t = (2 * t) % compositeN;
    }

    Simulator<Config>::dd->decRef(limit);
    Simulator<Config>::dd->decRef(f);

    dd::mEdge e = f;

    for (auto i = static_cast<std::int32_t>(2 * requiredBits - 1); i >= 0; --i) {
        if (i == q) {
            edges[1] = edges[2] = dd::mEdge::zero();
            edges[0]            = Simulator<Config>::dd->makeIdent(0, static_cast<dd::Qubit>(nQubits - static_cast<std::size_t>(i) - 2));
            edges[3]            = e;
            e                   = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(nQubits - 1 - static_cast<std::size_t>(i)), edges, false);
        } else {
            edges[1] = edges[2] = dd::mEdge::zero();
            edges[0] = edges[3] = e;
            e                   = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(nQubits - 1 - static_cast<std::size_t>(i)), edges, false);
        }
    }

    const dd::vEdge tmp = Simulator<Config>::dd->multiply(e, Simulator<Config>::rootEdge);
    Simulator<Config>::dd->incRef(tmp);
    Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
    Simulator<Config>::rootEdge = tmp;

    Simulator<Config>::dd->garbageCollect();
}

template<class Config>
void ShorSimulator<Config>::applyGate(dd::GateMatrix matrix, dd::Qubit target) {
    applyGate(matrix, target, qc::Controls{});
}

template<class Config>
void ShorSimulator<Config>::applyGate(dd::GateMatrix matrix, dd::Qubit target, qc::Control control) {
    applyGate(matrix, target, qc::Controls{control});
}

template<class Config>
void ShorSimulator<Config>::applyGate(dd::GateMatrix matrix, dd::Qubit target, const qc::Controls& controls) {
    const dd::Edge gate = Simulator<Config>::dd->makeGateDD(matrix, static_cast<dd::Qubit>(nQubits), controls, target);
    const dd::Edge tmp  = Simulator<Config>::dd->multiply(gate, Simulator<Config>::rootEdge);
    Simulator<Config>::dd->incRef(tmp);
    Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
    Simulator<Config>::rootEdge = tmp;

    Simulator<Config>::dd->garbageCollect();
}

template class ShorSimulator<dd::DDPackageConfig>;
