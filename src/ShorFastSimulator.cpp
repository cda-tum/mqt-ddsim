#include "ShorFastSimulator.hpp"

#include "Definitions.hpp"
#include "Simulator.hpp"
#include "dd/ComplexNumbers.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Edge.hpp"
#include "dd/GateMatrixDefinitions.hpp"
#include "dd/Node.hpp"

#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

template <class Config>
std::map<std::string, std::size_t>
ShorFastSimulator<Config>::simulate([[maybe_unused]] std::size_t shots) {
  if (verbose) {
    std::clog << "simulate Shor's algorithm for n=" << compositeN;
  }
  Simulator<Config>::rootEdge =
      Simulator<Config>::dd->makeZeroState(static_cast<dd::Qubit>(nQubits));
  Simulator<Config>::dd->incRef(Simulator<Config>::rootEdge);
  // Initialize qubits
  // TODO: other init method where the initial value can be chosen
  applyGate(dd::X_MAT, 0);

  if (verbose) {
    std::clog << " (requires " << +nQubits << " qubits):\n";
  }

  if (coprimeA != 0 && gcd(coprimeA, compositeN) != 1) {
    std::clog << "Warning: gcd(a=" << coprimeA << ", n=" << compositeN
              << ") != 1 --> choosing a new value for a\n";
    coprimeA = 0;
  }

  if (coprimeA == 0) {
    std::uniform_int_distribution<std::uint32_t> distribution(
        1, compositeN - 1); // range is inclusive
    do {
      coprimeA = distribution(Simulator<Config>::mt);
    } while (gcd(coprimeA, compositeN) != 1 || coprimeA == 1);
  }

  if (verbose) {
    std::clog << "Find a coprime to N=" << compositeN << ": " << coprimeA
              << "\n";
  }

  std::vector<std::uint64_t> as;
  as.resize(2 * requiredBits);
  assert(as.size() == 2 * requiredBits); // it's quite easy to get the vector
                                         // initialization wrong
  as[2 * requiredBits - 1] = coprimeA;
  std::uint64_t newA = coprimeA;
  for (auto i = static_cast<std::int64_t>(2 * requiredBits - 2); i >= 0; i--) {
    newA = newA * newA;
    newA = newA % compositeN;
    as[static_cast<std::size_t>(i)] = newA;
  }

  auto t1 = std::chrono::steady_clock::now();
  std::string measurements(2 * requiredBits, '0');

  for (std::size_t i = 0; i < 2 * requiredBits; i++) {
    applyGate(dd::H_MAT, static_cast<dd::Qubit>(nQubits - 1));

    if (verbose) {
      std::clog << "[ " << (i + 1) << "/" << 2 * requiredBits
                << " ] uAEmulate2(" << as[i] << ") "
                << std::chrono::duration<float>(
                       std::chrono::steady_clock::now() - t1)
                       .count()
                << "\n"
                << std::flush;
    }
    uAEmulate2(as[i]);

    if (verbose) {
      std::clog << "[ " << i + 1 << "/" << 2 * requiredBits
                << " ] QFT Pass. dd size=" << Simulator<Config>::rootEdge.size()
                << "\n";
    }

    double q = 2;
    for (int j = static_cast<int>(i - 1); j >= 0; j--) {
      if (measurements[static_cast<std::size_t>(j)] == '1') {
        double qR = cosine(1, -q);
        double qI = sine(1, -q);
        const dd::GateMatrix qm{1, 0, 0, {qR, qI}};
        applyGate(qm, static_cast<dd::Qubit>(nQubits - 1));
      }
      q *= 2;
    }

    applyGate(dd::H_MAT, static_cast<dd::Qubit>(nQubits - 1));

    measurements[i] = Simulator<Config>::measureOneCollapsing(
        static_cast<dd::Qubit>(nQubits - 1), false);
    Simulator<Config>::dd->garbageCollect();

    if (measurements[i] == '1') {
      applyGate(dd::X_MAT, static_cast<dd::Qubit>(nQubits - 1));
    }
  }

  // Non-Quantum Post Processing
  simFactors = postProcessing(measurements);
  if (simFactors.first != 0 && simFactors.second != 0) {
    simResult = "SUCCESS(" + std::to_string(simFactors.first) + "*" +
                std::to_string(simFactors.second) + ")";
  } else {
    simResult = "FAILURE";
  }

  return {};
}

/**
 * Post process the result of the simulation, i.e. try to find two non-trivial
 * factors
 * @tparam Config Configuration for the underlying DD package
 * @param sample string with the measurement results (consisting of only 0s and
 * 1s)
 * @return pair of integers with the factors in case of success or pair of zeros
 * in case of errors
 */
template <class Config>
std::pair<std::uint32_t, std::uint32_t>
ShorFastSimulator<Config>::postProcessing(const std::string& sample) const {
  std::ostream log{nullptr};
  if (verbose) {
    log.rdbuf(std::clog.rdbuf());
  }
  std::uint64_t res = 0;
  log << "measurement: ";
  for (std::uint32_t i = 0; i < 2 * requiredBits; i++) {
    log << sample.at(2 * requiredBits - 1 - i);
    const auto currentBit =
        static_cast<std::uint64_t>(sample.at(2 * requiredBits - 1 - i) == '1');
    res = (res << 1U) + currentBit;
  }

  log << " = " << res << "\n";
  if (res == 0) {
    log << "Factorization failed (measured 0)!\n";
    return {0, 0};
  }
  std::vector<std::uint64_t> cf;
  auto denom = 1ULL << (2 * requiredBits);
  const auto oldDenom = denom;
  const auto oldRes = res;
  log << "Continued fraction expansion of " << res << "/" << denom << " = ";
  while (res != 0) {
    cf.emplace_back(denom / res);
    const auto tmp = denom % res;
    denom = res;
    res = tmp;
  }

  for (const auto i : cf) {
    log << i << " ";
  }
  log << "\n";
  for (int i = 0; static_cast<std::size_t>(i) < cf.size(); i++) {
    // determine candidate
    std::uint64_t denominator = cf[static_cast<std::size_t>(i)];
    std::uint64_t numerator = 1;

    for (int j = i - 1; j >= 0; j--) {
      const auto tmp =
          numerator + cf[static_cast<std::size_t>(j)] * denominator;
      numerator = denominator;
      denominator = tmp;
    }

    log << "  Candidate " << numerator << "/" << denominator << ": ";
    if (denominator > compositeN) {
      log << " denominator too large (greater than " << compositeN
          << ")!\nFactorization failed!\n";
      return {0, 0};
    }

    const double delta =
        static_cast<double>(oldRes) / static_cast<double>(oldDenom) -
        static_cast<double>(numerator) / static_cast<double>(denominator);
    if (std::abs(delta) >= 1.0 / (2.0 * static_cast<double>(oldDenom))) {
      log << "delta is too big (" << delta << ")\n";
      continue;
    }

    std::uint64_t fact = 1;
    while (denominator * fact < compositeN &&
           modpow(coprimeA, denominator * fact, compositeN) != 1) {
      fact++;
    }

    if (modpow(coprimeA, denominator * fact, compositeN) != 1) {
      log << "failed\n";
      continue;
    }

    log << "found period: " << denominator << " * " << fact << " = "
        << (denominator * fact) << "\n";
    if (((denominator * fact) & 1U) > 0) {
      log << "Factorization failed (period is odd)!\n";
      return {0, 0};
    }

    auto f1 = modpow(coprimeA, (denominator * fact) / 2, compositeN);
    auto f2 = (f1 + 1) % compositeN;
    f1 = (f1 == 0) ? compositeN - 1 : f1 - 1;
    f1 = gcd(f1, compositeN);
    f2 = gcd(f2, compositeN);
    if (f1 == 1ULL || f2 == 1ULL) {
      log << "Factorization failed: found trivial factors " << f1 << " and "
          << f2 << "\n";
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

template <class Config>
dd::mEdge ShorFastSimulator<Config>::limitTo(std::uint64_t a) {
  std::array<dd::mEdge, 4> edges{dd::mEdge::zero(), dd::mEdge::zero(),
                                 dd::mEdge::zero(), dd::mEdge::zero()};

  if ((a & 1U) > 0) {
    edges[0] = edges[3] = dd::mEdge::one();
  } else {
    edges[0] = dd::mEdge::one();
  }
  dd::Edge f = Simulator<Config>::dd->makeDDNode(0, edges, false);

  edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero();

  for (std::uint32_t p = 1; p < requiredBits + 1; p++) {
    if (((a >> p) & 1U) > 0) {
      edges[0] = Simulator<Config>::dd->makeIdent();
      edges[3] = f;
    } else {
      edges[0] = f;
    }
    f = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges,
                                          false);
    edges[3] = dd::mEdge::zero();
  }

  return f;
}

template <class Config>
dd::mEdge ShorFastSimulator<Config>::addConst(std::uint64_t a) {
  dd::Edge f = dd::mEdge::one();
  std::array<dd::mEdge, 4> edges{dd::mEdge::zero(), dd::mEdge::zero(),
                                 dd::mEdge::zero(), dd::mEdge::zero()};

  std::uint32_t p = 0;
  while (((a >> p) & 1U) == 0) {
    edges[0] = f;
    edges[3] = f;
    f = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges,
                                          false);
    p++;
  }

  edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero();
  edges[2] = f;
  dd::Edge left = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p),
                                                    edges, false);
  edges[2] = dd::mEdge::zero();
  edges[1] = f;
  dd::Edge right = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p),
                                                     edges, false);
  p++;

  for (; p < requiredBits; p++) {
    edges[0] = edges[1] = edges[2] = edges[3] = dd::mEdge::zero();
    if (((a >> p) & 1U) > 0) {
      edges[2] = left;
      const dd::Edge newLeft = Simulator<Config>::dd->makeDDNode(
          static_cast<dd::Qubit>(p), edges, false);
      edges[2] = dd::mEdge::zero();
      edges[0] = right;
      edges[1] = left;
      edges[3] = right;
      const dd::Edge newRight = Simulator<Config>::dd->makeDDNode(
          static_cast<dd::Qubit>(p), edges, false);
      left = newLeft;
      right = newRight;
    } else {
      edges[1] = right;
      const dd::Edge newRight = Simulator<Config>::dd->makeDDNode(
          static_cast<dd::Qubit>(p), edges, false);
      edges[1] = dd::mEdge::zero();
      edges[0] = left;
      edges[2] = right;
      edges[3] = left;
      const dd::Edge newLeft = Simulator<Config>::dd->makeDDNode(
          static_cast<dd::Qubit>(p), edges, false);
      left = newLeft;
      right = newRight;
    }
  }

  edges[0] = left;
  edges[1] = right;
  edges[2] = right;
  edges[3] = left;

  return Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges,
                                           false);
}

template <class Config>
dd::mEdge ShorFastSimulator<Config>::addConstMod(std::uint64_t a) {
  const auto f = addConst(a);
  const auto f2 = addConst(compositeN);
  const auto f3 = limitTo(compositeN - 1);

  auto f4 = limitTo(compositeN - 1 - a);
  f4.w = dd::ComplexNumbers::neg(f4.w);

  const auto diff2 = Simulator<Config>::dd->add(f3, f4);

  f4.w = dd::ComplexNumbers::neg(f4.w);

  const auto simEdgeMultiply = Simulator<Config>::dd->multiply(
      Simulator<Config>::dd->conjugateTranspose(f2), f);
  const auto simEdgeResult = Simulator<Config>::dd->add(
      Simulator<Config>::dd->multiply(f, f4),
      Simulator<Config>::dd->multiply(simEdgeMultiply, diff2));

  assert(simEdgeResult.p != nullptr);
  return simEdgeResult.p->e[0];
}

template <class Config>
void ShorFastSimulator<Config>::applyGate(dd::GateMatrix matrix,
                                          dd::Qubit target) {
  numberOfOperations++;
  const dd::Edge gate = Simulator<Config>::dd->makeGateDD(matrix, target);
  const dd::Edge tmp =
      Simulator<Config>::dd->multiply(gate, Simulator<Config>::rootEdge);
  Simulator<Config>::dd->incRef(tmp);
  Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
  Simulator<Config>::rootEdge = tmp;

  Simulator<Config>::dd->garbageCollect();
}

/**
 * Emulate the modular exponentiation by directly building the appropriate
 * decision diagram for the operation
 * @tparam Config Configuration for the underlying DD package
 * @param a the exponent
 */
template <class Config>
void ShorFastSimulator<Config>::uAEmulate2(const std::uint64_t a) {
  dagEdges.clear();

  dd::vEdge f = dd::vEdge::one();
  std::array<dd::vEdge, dd::RADIX> edges{dd::vEdge::zero(), dd::vEdge::zero()};
  std::vector<std::uint64_t> ts;
  ts.resize(nQubits);

  std::uint64_t t = a;
  for (qc::Qubit p = 0; p < nQubits - 1; ++p) {
    edges[0] = f;
    f = Simulator<Config>::dd->makeDDNode(static_cast<dd::Qubit>(p), edges);
    ts[p] = t;
    t = (2 * t) % compositeN;
  }

  Simulator<Config>::dd->incRef(f);

  // clear nodesOnLevel. TODO: make it a local variable?
  for (auto& m : nodesOnLevel) {
    m = {};
  }

  // initialize nodesOnLevel
  uAEmulate2Rec(Simulator<Config>::rootEdge.p->e[0]);

  // special treatment for the nodes on first level
  for (const auto& entry : nodesOnLevel.at(0)) {
    auto left = dd::vCachedEdge::zero();
    if (!entry.first->e[0].w.exactlyZero()) {
      left = {f.p, f.w * entry.first->e[0].w};
    }

    auto right = dd::vCachedEdge::zero();
    if (!entry.first->e[1].w.exactlyZero()) {
      auto tmp = Simulator<Config>::dd->multiply(
          addConstMod(ts[static_cast<std::size_t>(entry.first->v)]), f);
      right = {tmp.p, tmp.w * entry.first->e[1].w};
    }

    nodesOnLevel[0][entry.first] = Simulator<Config>::dd->add2(left, right, 0);
  }

  // treat the nodes on the remaining levels
  for (std::size_t i = 1; i < nQubits - 1; i++) {
    for (auto it = nodesOnLevel.at(i).begin(); it != nodesOnLevel.at(i).end();
         it++) {
      auto left = dd::vCachedEdge::zero();
      if (!it->first->e.at(0).w.exactlyZero()) {
        left = nodesOnLevel.at(i - 1)[it->first->e.at(0).p];
        left.w = left.w * it->first->e.at(0).w;
      }

      auto right = dd::vCachedEdge::zero();
      if (!it->first->e.at(1).w.exactlyZero()) {
        auto node0 = addConstMod(ts.at(static_cast<std::size_t>(it->first->v)));
        auto& node1 = nodesOnLevel.at(i - 1)[it->first->e.at(1).p];
        auto node2 =
            dd::vEdge{node1.p, Simulator<Config>::dd->cn.lookup(node1.w)};
        auto res = Simulator<Config>::dd->multiply(node0, node2);
        right = {res.p, res.w * it->first->e.at(1).w};
      }

      dd::Qubit level = 0;
      if (left.p != nullptr) {
        level = left.p->v;
      }
      if (right.p != nullptr && right.p->v > level) {
        level = right.p->v;
      }

      nodesOnLevel.at(i)[it->first] =
          Simulator<Config>::dd->add2(left, right, level);
    }
    nodesOnLevel.at(i - 1).clear();
  }

  // there should be only one node at the top level
  if (nodesOnLevel.at(nQubits - 2).size() != 1) {
    throw std::runtime_error("error occurred");
  }

  auto result =
      nodesOnLevel.at(nQubits - 2)[Simulator<Config>::rootEdge.p->e[0].p];
  result.w = result.w * Simulator<Config>::rootEdge.p->e[0].w;
  auto tmp = dd::vCachedEdge{Simulator<Config>::rootEdge.p->e[0].p,
                             Simulator<Config>::rootEdge.p->e[0].w};
  result = Simulator<Config>::dd->makeDDNode(Simulator<Config>::rootEdge.p->v,
                                             std::array{tmp, result});

  result.w = result.w * Simulator<Config>::rootEdge.w;
  auto res = dd::vEdge{result.p, Simulator<Config>::dd->cn.lookup(result.w)};
  Simulator<Config>::dd->incRef(res);
  Simulator<Config>::dd->decRef(Simulator<Config>::rootEdge);
  Simulator<Config>::rootEdge = res;
  Simulator<Config>::dd->garbageCollect();
}

template <class Config>
void ShorFastSimulator<Config>::uAEmulate2Rec(dd::vEdge e) {
  if (e.isTerminal()) {
    return;
  }
  auto& nodes = nodesOnLevel.at(static_cast<std::size_t>(e.p->v));
  if (nodes.find(e.p) != nodes.end()) {
    return;
  }
  nodes[e.p] = dd::vCachedEdge::zero();
  uAEmulate2Rec(e.p->e[0]);
  uAEmulate2Rec(e.p->e[1]);
}

template class ShorFastSimulator<dd::DDPackageConfig>;
