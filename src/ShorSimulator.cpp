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
        std::clog << "Simulate Shor's algorithm for n=" << n;
    }

    if (emulate) {
        n_qubits       = static_cast<dd::QubitCount>(3 * required_bits);
        this->rootEdge = this->dd->makeZeroState(n_qubits);
        this->dd->incRef(this->rootEdge);
        //Initialize qubits
        //TODO: other init method where the initial value can be chosen
        ApplyGate(dd::Xmat, 0);

    } else {
        n_qubits       = static_cast<dd::QubitCount>(2 * required_bits + 3);
        this->rootEdge = this->dd->makeZeroState(n_qubits);
        this->dd->incRef(this->rootEdge);
        //Initialize qubits
        //TODO: other init method where the initial value can be chosen

        ApplyGate(dd::Xmat, static_cast<dd::Qubit>(n_qubits - 1));
    }

    if (verbose) {
        std::clog << " (requires " << n_qubits << " qubits):\n";
    }

    if (coprime_a != 0 && gcd(coprime_a, n) != 1) {
        std::clog << "Warning: gcd(a=" << coprime_a << ", n=" << n << ") != 1 --> choosing a new value for a\n";
        coprime_a = 0;
    }

    if (coprime_a == 0) {
        std::uniform_int_distribution<std::size_t> distribution(1, n - 1); // range is inclusive
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

    for (unsigned int i = 0; i < 2 * required_bits; i++) {
        ApplyGate(dd::Hmat, (n_qubits - 1) - i);
    }
    const int mod = std::ceil(2 * required_bits / 6.0); // log_0.9(0.5) is about 6
    auto      t1  = std::chrono::steady_clock::now();

    if (emulate) {
        for (unsigned int i = 0; i < 2 * required_bits; i++) {
            if (verbose) {
                std::clog << "[ " << (i + 1) << "/" << 2 * required_bits << " ] u_a_emulate(" << as[i] << ", " << i
                          << ") " << std::chrono::duration<float>(std::chrono::steady_clock::now() - t1).count() << "\n"
                          << std::flush;
            }
            u_a_emulate(as[i], i);
        }
    } else {
        for (unsigned int i = 0; i < 2 * required_bits; i++) {
            if (verbose) {
                std::clog << "[ " << (i + 1) << "/" << 2 * required_bits << " ] u_a(" << as[i] << ", " << n << ", " << 0
                          << ") " << std::chrono::duration<float>(std::chrono::steady_clock::now() - t1).count() << "\n"
                          << std::flush;
            }
            u_a(as[i], n, 0);
        }
    }

    if (verbose) {
        std::clog << "Nodes before QFT: " << this->dd->size(this->rootEdge) << "\n";
    }

    //EXACT QFT
    for (unsigned int i = 0; i < 2 * required_bits; i++) {
        if (verbose) {
            std::clog << "[ " << i + 1 << "/" << 2 * required_bits << " ] QFT Pass. dd size=" << this->dd->size(this->rootEdge)
                      << "\n";
        }
        double q = 2;

        for (int j = i - 1; j >= 0; j--) {
            double         q_r = QMDDcos(1, -q);
            double         q_i = QMDDsin(1, -q);
            dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
            ApplyGate(Qm, n_qubits - 1 - i, dd::Control{static_cast<dd::Qubit>(n_qubits - 1 - j)});
            q *= 2;
        }

        if (approximate && (i + 1) % mod == 0) {
            final_fidelity *= this->ApproximateByFidelity(step_fidelity, false, true);
            approximation_runs++;
        }

        ApplyGate(dd::Hmat, n_qubits - 1 - i);
    }

    delete[] as;

    // Non-Quantum Post Processing

    // measure result (involves randomness)
    {
        std::string sample_reversed = this->MeasureAll(false);
        std::string sample{sample_reversed.rbegin(), sample_reversed.rend()};
        sim_factors = post_processing(sample);
        if (sim_factors.first != 0 && sim_factors.second != 0) {
            sim_result =
                    std::string("SUCCESS(") + std::to_string(sim_factors.first) + "*" +
                    std::to_string(sim_factors.second) + ")";
        } else {
            sim_result = "FAILURE";
        }
    }

    // path of least resistance result (does not involve randomness)
    {
        std::pair<dd::ComplexValue, std::string> polr_pair = this->getPathOfLeastResistance();
        //std::clog << polr_pair.first << " " << polr_pair.second << "\n";
        std::string polr_reversed = polr_pair.second;
        std::string polr          = {polr_reversed.rbegin(), polr_reversed.rend()};
        polr_factors              = post_processing(polr);

        if (polr_factors.first != 0 && polr_factors.second != 0) {
            polr_result = std::string("SUCCESS(") + std::to_string(polr_factors.first) + "*" +
                          std::to_string(polr_factors.second) + ")";
        } else {
            polr_result = "FAILURE";
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
std::pair<unsigned int, unsigned int> ShorSimulator<Config>::post_processing(const std::string& sample) const {
    std::ostream log{nullptr};
    if (verbose) {
        log.rdbuf(std::clog.rdbuf());
    }
    unsigned long long res = 0;

    log << "measurement: ";
    for (unsigned int i = 0; i < 2 * required_bits; i++) {
        log << sample.at(2 * required_bits - 1 - i);
        res = (res << 1u) + (sample.at(required_bits + i) == '1');
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
            << "  -- gcd(" << n << "^(" << (denominator * fact) << "/2)-1"
            << "," << n << ") = " << f1 << "\n"
            << "  -- gcd(" << n << "^(" << (denominator * fact) << "/2)+1"
            << "," << n << ") = " << f2 << "\n";
        return {f1, f2};
    }
    return {0, 0};
}

template<class Config>
dd::mEdge ShorSimulator<Config>::limitTo(unsigned long long a) {
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
dd::mEdge ShorSimulator<Config>::addConst(unsigned long long a) {
    dd::mEdge                f = dd::mEdge::one;
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero};

    unsigned int p = 0;
    while (!((a >> p) & 1u)) {
        edges[0] = f;
        edges[3] = f;
        f        = this->dd->makeDDNode(p, edges, false);
        p++;
    }

    dd::mEdge right, left;

    edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero;
    edges[2]                                  = f;
    left                                      = this->dd->makeDDNode(p, edges, false);
    edges[2]                                  = dd::mEdge::zero;
    edges[1]                                  = f;
    right                                     = this->dd->makeDDNode(p, edges, false);
    p++;

    dd::mEdge new_left, new_right;
    for (; p < required_bits; p++) {
        edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero;
        if ((a >> p) & 1u) {
            edges[2]  = left;
            new_left  = this->dd->makeDDNode(p, edges, false);
            edges[2]  = dd::mEdge::zero;
            edges[0]  = right;
            edges[1]  = left;
            edges[3]  = right;
            new_right = this->dd->makeDDNode(p, edges, false);
        } else {
            edges[1]  = right;
            new_right = this->dd->makeDDNode(p, edges, false);
            edges[1]  = dd::mEdge::zero;
            edges[0]  = left;
            edges[2]  = right;
            edges[3]  = left;
            new_left  = this->dd->makeDDNode(p, edges, false);
        }
        left  = new_left;
        right = new_right;
    }

    edges[0] = left;
    edges[1] = right;
    edges[2] = right;
    edges[3] = left;

    return this->dd->makeDDNode(p, edges, false);
}

template<class Config>
dd::mEdge ShorSimulator<Config>::addConstMod(unsigned long long a) {
    dd::Edge f = addConst(a);

    dd::Edge f2 = addConst(n);

    dd::Edge f3 = limitTo(n - 1);

    dd::Edge f4 = limitTo(n - 1 - a);

    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge diff2 = this->dd->add(f3, f4);
    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge tmp   = this->dd->add(this->dd->multiply(f, f4), this->dd->multiply(this->dd->multiply(this->dd->transpose(f2), f), diff2));

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
    auto it = dag_edges.find(e.p);
    if (it != dag_edges.end()) {
        return it->second;
    }

    std::array<dd::mEdge, 4> edges{
            limitStateVector(e.p->e.at(0)),
            dd::mEdge::zero,
            dd::mEdge::zero,
            limitStateVector(e.p->e.at(1))};

    dd::Edge result = this->dd->makeDDNode(e.p->v, edges, false);
    dag_edges[e.p]  = result;
    return result;
}

template<class Config>
void ShorSimulator<Config>::u_a_emulate(unsigned long long a, int q) {
    dd::mEdge limit = this->dd->makeIdent(0, required_bits - 1);

    dd::mEdge                f = dd::mEdge::one;
    std::array<dd::mEdge, 4> edges{
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero,
            dd::mEdge::zero};

    for (unsigned int p = 0; p < required_bits; ++p) {
        edges[0] = f;
        edges[1] = f;
        f        = this->dd->makeDDNode(p, edges, false);
    }

    //TODO: limitTo?

    f = this->dd->multiply(f, limit);

    edges[1] = dd::mEdge::zero;

    this->dd->incRef(f);
    this->dd->incRef(limit);

    unsigned long t = a;

    for (unsigned int i = 0; i < required_bits; ++i) {
        dd::mEdge active = dd::mEdge::one;
        for (unsigned int p = 0; p < required_bits; ++p) {
            if (p == i) {
                edges[3] = active;
                edges[0] = dd::mEdge::zero;
            } else {
                edges[0] = edges[3] = active;
            }
            active = this->dd->makeDDNode(p, edges, false);
        }

        active.w          = this->dd->cn.lookup(-1, 0);
        dd::mEdge passive = this->dd->multiply(f, this->dd->add(limit, active));
        active.w          = dd::Complex::one;
        active            = this->dd->multiply(f, active);

        dd::mEdge tmp = addConstMod(t);
        active        = this->dd->multiply(tmp, active);

        this->dd->decRef(f);
        f = this->dd->add(active, passive);
        this->dd->incRef(f);
        this->dd->garbageCollect();

        t = (2 * t) % n;
    }

    this->dd->decRef(limit);
    this->dd->decRef(f);

    dd::mEdge e = f;

    for (int i = 2 * required_bits - 1; i >= 0; --i) {
        if (i == q) {
            edges[1] = edges[2] = dd::mEdge::zero;
            edges[0]            = this->dd->makeIdent(0, n_qubits - i - 2);
            edges[3]            = e;
            e                   = this->dd->makeDDNode(n_qubits - 1 - i, edges, false);
        } else {
            edges[1] = edges[2] = dd::mEdge::zero;
            edges[0] = edges[3] = e;
            e                   = this->dd->makeDDNode(n_qubits - 1 - i, edges, false);
        }
    }

    dd::vEdge tmp = this->dd->multiply(e, this->rootEdge);
    this->dd->incRef(tmp);
    this->dd->decRef(this->rootEdge);
    this->rootEdge = tmp;

    this->dd->garbageCollect();
}

template<class Config>
int ShorSimulator<Config>::inverse_mod(int a, int n) {
    int t    = 0;
    int newt = 1;
    int r    = n;
    int newr = a;
    while (newr != 0) {
        int quotient = r / newr;
        int h        = t;
        t            = newt;
        newt         = h - quotient * newt;
        h            = r;
        r            = newr;
        newr         = h - quotient * newr;
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
void ShorSimulator<Config>::add_phi(unsigned long long int a, int c1, int c2) {
    for (int i = static_cast<int>(required_bits); i >= 0; --i) {
        double       q   = 1;
        unsigned int fac = 0;
        for (int j = i; j >= 0; --j) {
            if ((a >> j) & 1u) {
                fac |= 1u;
            }
            fac *= 2;
            q *= 2;
        }

        dd::Controls controls;
        if (c1 != std::numeric_limits<int>::min()) {
            controls.emplace(dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - c1)});
        }
        if (c2 != std::numeric_limits<int>::min()) {
            controls.emplace(dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - c2)});
        }

        double         q_r = QMDDcos(fac, q);
        double         q_i = QMDDsin(fac, q);
        dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};

        ApplyGate(Qm, static_cast<dd::Qubit>((n_qubits - 1) - (1 + 2 * required_bits - static_cast<unsigned int>(i))), controls);
    }
}

template<class Config>
void ShorSimulator<Config>::add_phi_inv(unsigned long long int a, int c1, int c2) {
    for (int i = static_cast<int>(required_bits); i >= 0; --i) {
        double       q   = 1;
        unsigned int fac = 0;
        for (int j = i; j >= 0; --j) {
            if ((a >> j) & 1u) {
                fac |= 1u;
            }
            fac *= 2;
            q *= 2;
        }
        dd::Controls controls;
        if (c1 != std::numeric_limits<int>::min()) {
            controls.emplace(dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - c1)});
        }
        if (c2 != std::numeric_limits<int>::min()) {
            controls.emplace(dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - c2)});
        }

        double         q_r = QMDDcos(fac, -q);
        double         q_i = QMDDsin(fac, -q);
        dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
        ApplyGate(Qm, static_cast<dd::Qubit>((n_qubits - 1) - (1 + 2 * required_bits - i)), controls);
    }
}

template<class Config>
void ShorSimulator<Config>::qft() {
    for (unsigned int i = required_bits + 1; i < 2 * required_bits + 2; i++) {
        ApplyGate(dd::Hmat, static_cast<dd::Qubit>((n_qubits - 1) - i));

        double q = 2;
        for (unsigned int j = i + 1; j < 2 * required_bits + 2; j++) {
            double         q_r = QMDDcos(1, q);
            double         q_i = QMDDsin(1, q);
            dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
            ApplyGate(Qm, static_cast<dd::Qubit>((n_qubits - 1) - i), dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - j)});
            q *= 2;
        }
    }
}

template<class Config>
void ShorSimulator<Config>::qft_inv() {
    for (unsigned int i = 2 * required_bits + 1; i >= required_bits + 1; i--) {
        double q = 2;
        for (unsigned int j = i + 1; j < 2 * required_bits + 2; j++) {
            double         q_r = QMDDcos(1, -q);
            double         q_i = QMDDsin(1, -q);
            dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
            ApplyGate(Qm, (n_qubits - 1) - i, dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - j)});
            q *= 2;
        }
        ApplyGate(dd::Hmat, (n_qubits - 1) - i);
    }
}

template<class Config>
void ShorSimulator<Config>::mod_add_phi(unsigned long long int a, unsigned int N, int c1, int c2) {
    add_phi(a, c1, c2);
    add_phi_inv(N, std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

    qft_inv();

    ApplyGate(dd::Xmat, (n_qubits - 1) - (2 * required_bits + 2),
              dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 1))});

    qft();
    add_phi(N, 2 * required_bits + 2, std::numeric_limits<int>::min());
    add_phi_inv(a, c1, c2);
    qft_inv();

    ApplyGate(dd::Xmat, (n_qubits - 1) - (2 * required_bits + 2),
              dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 1)), dd::Control::Type::neg});

    qft();
    add_phi(a, c1, c2);
}

template<class Config>
void ShorSimulator<Config>::mod_add_phi_inv(int a, int N, int c1, int c2) {
    add_phi_inv(a, c1, c2);
    qft_inv();

    ApplyGate(dd::Xmat, static_cast<dd::Qubit>((n_qubits - 1) - (2 * required_bits + 2)), dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 1)), dd::Control::Type::neg});

    qft();
    add_phi(a, c1, c2);
    add_phi_inv(N, 2 * required_bits + 2, std::numeric_limits<int>::min());
    qft_inv();

    ApplyGate(dd::Xmat, (n_qubits - 1) - (2 * required_bits + 2), dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 1))});

    qft();
    add_phi(N, std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    add_phi_inv(a, c1, c2);
}

template<class Config>
void ShorSimulator<Config>::cmult(unsigned long long int a, unsigned int N, int c) {
    qft();

    unsigned long long int t = a;
    for (unsigned int i = required_bits; i >= 1; i--) {
        mod_add_phi(t, N, i, c);
        t = (2 * t) % N;
    }
    qft_inv();
}

template<class Config>
void ShorSimulator<Config>::cmult_inv(unsigned long long int a, unsigned int N, int c) {
    qft();
    unsigned long long int t = a;
    for (unsigned int i = required_bits; i >= 1; i--) {
        mod_add_phi_inv(t, N, i, c);
        t = (2 * t) % N;
    }
    qft_inv();
}

template<class Config>
void ShorSimulator<Config>::u_a(unsigned long long a, int N, int c) {
    using namespace dd::literals;
    cmult(a, N, c);
    for (unsigned int i = 0; i < required_bits; i++) {
        ApplyGate(dd::Xmat, static_cast<dd::Qubit>((n_qubits - 1) - (i + 1)), dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 2 + i))});

        ApplyGate(dd::Xmat, static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 2 + i)), dd::Controls{{dd::Control{static_cast<dd::Qubit>(n_qubits - 1 - (i + 1))}, dd::Control{static_cast<dd::Qubit>(n_qubits - 1 - c)}}});

        ApplyGate(dd::Xmat, static_cast<dd::Qubit>((n_qubits - 1) - (i + 1)), dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 2 + i))});
    }

    cmult_inv(inverse_mod(a, N), N, c);
}

template<class Config>
void ShorSimulator<Config>::ApplyGate(dd::GateMatrix matrix, dd::Qubit target) {
    dd::Edge gate = this->dd->makeGateDD(matrix, n_qubits, target);
    dd::Edge tmp  = this->dd->multiply(gate, this->rootEdge);
    this->dd->incRef(tmp);
    this->dd->decRef(this->rootEdge);
    this->rootEdge = tmp;

    this->dd->garbageCollect();
}

template<class Config>
void ShorSimulator<Config>::ApplyGate(dd::GateMatrix matrix, dd::Qubit target, dd::Control control) {
    dd::Edge gate = this->dd->makeGateDD(matrix, n_qubits, control, target);
    dd::Edge tmp  = this->dd->multiply(gate, this->rootEdge);
    this->dd->incRef(tmp);
    this->dd->decRef(this->rootEdge);
    this->rootEdge = tmp;

    this->dd->garbageCollect();
}

template<class Config>
void ShorSimulator<Config>::ApplyGate(dd::GateMatrix matrix, dd::Qubit target, const dd::Controls& controls) {
    dd::Edge gate = this->dd->makeGateDD(matrix, n_qubits, controls, target);
    dd::Edge tmp  = this->dd->multiply(gate, this->rootEdge);
    this->dd->incRef(tmp);
    this->dd->decRef(this->rootEdge);
    this->rootEdge = tmp;

    this->dd->garbageCollect();
}

template class ShorSimulator<dd::DDPackageConfig>;