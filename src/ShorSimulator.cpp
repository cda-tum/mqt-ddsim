#include "ShorSimulator.hpp"

#include "dd/ComplexNumbers.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

std::map<std::string, std::size_t> ShorSimulator::Simulate([[maybe_unused]] unsigned int shots) {
    if (verbose) {
        std::clog << "Simulate Shor's algorithm for n=" << n;
    }

    if (emulate) {
        n_qubits  = 3 * required_bits;
        root_edge = dd->makeZeroState(n_qubits);
        dd->incRef(root_edge);
        //Initialize qubits
        //TODO: other init method where the initial value can be chosen
        ApplyGate(dd::Xmat, 0);

    } else {
        n_qubits  = 2 * required_bits + 3;
        root_edge = dd->makeZeroState(n_qubits);
        dd->incRef(root_edge);
        //Initialize qubits
        //TODO: other init method where the initial value can be chosen

        ApplyGate(dd::Xmat, n_qubits - 1);
    }

    if (verbose) {
        std::clog << " (requires " << n_qubits << " qubits):\n";
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
        std::clog << "Nodes before QFT: " << dd->size(root_edge) << "\n";
    }

    //EXACT QFT
    for (unsigned int i = 0; i < 2 * required_bits; i++) {
        if (verbose) {
            std::clog << "[ " << i + 1 << "/" << 2 * required_bits << " ] QFT Pass. dd size=" << dd->size(root_edge)
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
            final_fidelity *= ApproximateByFidelity(step_fidelity, false, true);
            approximation_runs++;
        }

        ApplyGate(dd::Hmat, n_qubits - 1 - i);
    }

    delete[] as;

    // Non-Quantum Post Processing

    // measure result (involves randomness)
    {
        std::string sample_reversed = MeasureAll(false);
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
        std::pair<dd::ComplexValue, std::string> polr_pair = getPathOfLeastResistance();
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

std::pair<unsigned int, unsigned int> ShorSimulator::post_processing(const std::string& sample) const {
    unsigned long long res = 0;
    if (verbose) {
        std::clog << "measurement: ";
    }
    for (unsigned int i = 0; i < 2 * required_bits; i++) {
        if (verbose) {
            std::clog << sample[required_bits + i];
        }
        res = (res << 1u) + (sample[required_bits + i] == '1');
    }

    if (verbose) {
        std::clog << " = " << res << "\n";
    }
    unsigned long long denom = 1ull << (2 * required_bits);

    bool               success = false;
    unsigned long long f1{0}, f2{0};
    if (res == 0) {
        if (verbose) {
            std::clog << "Factorization failed (measured 0)!" << std::endl;
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
                            f1 = modpow(coprime_a, (denominator * fact) >> 1u, n);
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

dd::Package::mEdge ShorSimulator::limitTo(unsigned long long a) {
    std::array<dd::Package::mEdge, 4> edges{
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero};

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

dd::Package::mEdge ShorSimulator::addConst(unsigned long long a) {
    dd::Package::mEdge                f = dd::Package::mEdge::one;
    std::array<dd::Package::mEdge, 4> edges{
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero};

    unsigned int p = 0;
    while (!((a >> p) & 1u)) {
        edges[0] = f;
        edges[3] = f;
        f        = dd->makeDDNode(p, edges, false);
        p++;
    }

    dd::Package::mEdge right, left;

    edges[0] = edges[1] = edges[2] = edges[3] = dd::Package::mEdge::zero;
    edges[2]                                  = f;
    left                                      = dd->makeDDNode(p, edges, false);
    edges[2]                                  = dd::Package::mEdge::zero;
    edges[1]                                  = f;
    right                                     = dd->makeDDNode(p, edges, false);
    p++;

    dd::Package::mEdge new_left, new_right;
    for (; p < required_bits; p++) {
        edges[0] = edges[1] = edges[2] = edges[3] = dd::Package::mEdge::zero;
        if ((a >> p) & 1u) {
            edges[2]  = left;
            new_left  = dd->makeDDNode(p, edges, false);
            edges[2]  = dd::Package::mEdge::zero;
            edges[0]  = right;
            edges[1]  = left;
            edges[3]  = right;
            new_right = dd->makeDDNode(p, edges, false);
        } else {
            edges[1]  = right;
            new_right = dd->makeDDNode(p, edges, false);
            edges[1]  = dd::Package::mEdge::zero;
            edges[0]  = left;
            edges[2]  = right;
            edges[3]  = left;
            new_left  = dd->makeDDNode(p, edges, false);
        }
        left  = new_left;
        right = new_right;
    }

    edges[0] = left;
    edges[1] = right;
    edges[2] = right;
    edges[3] = left;

    return dd->makeDDNode(p, edges, false);
}

dd::Package::mEdge ShorSimulator::addConstMod(unsigned long long a) {
    dd::Edge f = addConst(a);

    dd::Edge f2 = addConst(n);

    dd::Edge f3 = limitTo(n - 1);

    dd::Edge f4 = limitTo(n - 1 - a);

    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge diff2 = dd->add(f3, f4);
    f4.w           = dd::ComplexNumbers::neg(f4.w);
    dd::Edge tmp   = dd->add(dd->multiply(f, f4), dd->multiply(dd->multiply(dd->transpose(f2), f), diff2));

    return tmp.p->e[0];
}

dd::Package::mEdge ShorSimulator::limitStateVector(dd::Package::vEdge e) {
    if (e.p == dd::Package::vEdge::zero.p) {
        if (e.w == dd::Complex::zero) {
            return dd::Package::mEdge::zero;
        } else {
            return dd::Package::mEdge::one;
        }
    }
    auto it = dag_edges.find(e.p);
    if (it != dag_edges.end()) {
        return it->second;
    }

    std::array<dd::Package::mEdge, 4> edges{
            limitStateVector(e.p->e.at(0)),
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero,
            limitStateVector(e.p->e.at(1))};

    dd::Edge result = dd->makeDDNode(e.p->v, edges, false);
    dag_edges[e.p]  = result;
    return result;
}

void ShorSimulator::u_a_emulate(unsigned long long a, int q) {
    dd::Package::mEdge limit = dd->makeIdent(0, required_bits - 1);

    dd::Package::mEdge                f = dd::Package::mEdge::one;
    std::array<dd::Package::mEdge, 4> edges{
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero,
            dd::Package::mEdge::zero};

    for (unsigned int p = 0; p < required_bits; ++p) {
        edges[0] = f;
        edges[1] = f;
        f        = dd->makeDDNode(p, edges, false);
    }

    //TODO: limitTo?

    f = dd->multiply(f, limit);

    edges[1] = dd::Package::mEdge::zero;

    dd->incRef(f);
    dd->incRef(limit);

    unsigned long t = a;

    for (unsigned int i = 0; i < required_bits; ++i) {
        dd::Package::mEdge active = dd::Package::mEdge::one;
        for (unsigned int p = 0; p < required_bits; ++p) {
            if (p == i) {
                edges[3] = active;
                edges[0] = dd::Package::mEdge::zero;
            } else {
                edges[0] = edges[3] = active;
            }
            active = dd->makeDDNode(p, edges, false);
        }

        active.w                   = dd->cn.lookup(-1, 0);
        dd::Package::mEdge passive = dd->multiply(f, dd->add(limit, active));
        active.w                   = dd::Complex::one;
        active                     = dd->multiply(f, active);

        dd::Package::mEdge tmp = addConstMod(t);
        active                 = dd->multiply(tmp, active);

        dd->decRef(f);
        f = dd->add(active, passive);
        dd->incRef(f);
        dd->garbageCollect();

        t = (2 * t) % n;
    }

    dd->decRef(limit);
    dd->decRef(f);

    dd::Package::mEdge e = f;

    for (int i = 2 * required_bits - 1; i >= 0; --i) {
        if (i == q) {
            edges[1] = edges[2] = dd::Package::mEdge::zero;
            edges[0]            = dd->makeIdent(0, n_qubits - i - 2);
            edges[3]            = e;
            e                   = dd->makeDDNode(n_qubits - 1 - i, edges, false);
        } else {
            edges[1] = edges[2] = dd::Package::mEdge::zero;
            edges[0] = edges[3] = e;
            e                   = dd->makeDDNode(n_qubits - 1 - i, edges, false);
        }
    }

    dd::Package::vEdge tmp = dd->multiply(e, root_edge);
    dd->incRef(tmp);
    dd->decRef(root_edge);
    root_edge = tmp;

    dd->garbageCollect();
}

int ShorSimulator::inverse_mod(int a, int n) {
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

void ShorSimulator::add_phi(int a, int c1, int c2) {
    for (int i = required_bits; i >= 0; --i) {
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

        ApplyGate(Qm, (n_qubits - 1) - (1 + 2 * required_bits - i), controls);
    }
}

void ShorSimulator::add_phi_inv(int a, int c1, int c2) {
    for (int i = required_bits; i >= 0; --i) {
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
        ApplyGate(Qm, (n_qubits - 1) - (1 + 2 * required_bits - i), controls);
    }
}

void ShorSimulator::qft() {
    for (unsigned int i = required_bits + 1; i < 2 * required_bits + 2; i++) {
        ApplyGate(dd::Hmat, (n_qubits - 1) - i);

        double q = 2;
        for (unsigned int j = i + 1; j < 2 * required_bits + 2; j++) {
            double         q_r = QMDDcos(1, q);
            double         q_i = QMDDsin(1, q);
            dd::GateMatrix Qm{dd::complex_one, dd::complex_zero, dd::complex_zero, {q_r, q_i}};
            ApplyGate(Qm, (n_qubits - 1) - i, dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - j)});
            q *= 2;
        }
    }
}

void ShorSimulator::qft_inv() {
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

void ShorSimulator::mod_add_phi(int a, int N, int c1, int c2) {
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

void ShorSimulator::mod_add_phi_inv(int a, int N, int c1, int c2) {
    add_phi_inv(a, c1, c2);
    qft_inv();

    ApplyGate(dd::Xmat, (n_qubits - 1) - (2 * required_bits + 2),
              dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 1)), dd::Control::Type::neg});

    qft();
    add_phi(a, c1, c2);
    add_phi_inv(N, 2 * required_bits + 2, std::numeric_limits<int>::min());
    qft_inv();

    ApplyGate(dd::Xmat, (n_qubits - 1) - (2 * required_bits + 2),
              dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 1))});

    qft();
    add_phi(N, std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    add_phi_inv(a, c1, c2);
}

void ShorSimulator::cmult(int a, int N, int c) {
    qft();

    int t = a;
    for (int i = required_bits; i >= 1; i--) {
        mod_add_phi(t, N, i, c);
        t = (2 * t) % N;
    }
    qft_inv();
}

void ShorSimulator::cmult_inv(int a, int N, int c) {
    qft();
    int t = a;
    for (int i = required_bits; i >= 1; i--) {
        mod_add_phi_inv(t, N, i, c);
        t = (2 * t) % N;
    }
    qft_inv();
}

void ShorSimulator::u_a(unsigned long long a, int N, int c) {
    using namespace dd::literals;
    cmult(a, N, c);
    for (unsigned int i = 0; i < required_bits; i++) {
        ApplyGate(dd::Xmat, (n_qubits - 1) - (i + 1),
                  dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 2 + i))});

        ApplyGate(dd::Xmat, (n_qubits - 1) - (required_bits + 2 + i), dd::Controls{{dd::Control{static_cast<dd::Qubit>(n_qubits - 1 - (i + 1))}, dd::Control{static_cast<dd::Qubit>(n_qubits - 1 - c)}}});

        ApplyGate(dd::Xmat, (n_qubits - 1) - (i + 1),
                  dd::Control{static_cast<dd::Qubit>((n_qubits - 1) - (required_bits + 2 + i))});
    }

    cmult_inv(inverse_mod(a, N), N, c);
}

void ShorSimulator::ApplyGate(dd::GateMatrix matrix, dd::Qubit target) {
    dd::Edge gate = dd->makeGateDD(matrix, n_qubits, target);
    dd::Edge tmp  = dd->multiply(gate, root_edge);
    dd->incRef(tmp);
    dd->decRef(root_edge);
    root_edge = tmp;

    dd->garbageCollect();
}

void ShorSimulator::ApplyGate(dd::GateMatrix matrix, dd::Qubit target, dd::Control control) {
    dd::Edge gate = dd->makeGateDD(matrix, n_qubits, control, target);
    dd::Edge tmp  = dd->multiply(gate, root_edge);
    dd->incRef(tmp);
    dd->decRef(root_edge);
    root_edge = tmp;

    dd->garbageCollect();
}

void ShorSimulator::ApplyGate(dd::GateMatrix matrix, dd::Qubit target, const dd::Controls& controls) {
    dd::Edge gate = dd->makeGateDD(matrix, n_qubits, controls, target);
    dd::Edge tmp  = dd->multiply(gate, root_edge);
    dd->incRef(tmp);
    dd->decRef(root_edge);
    root_edge = tmp;

    dd->garbageCollect();
}
