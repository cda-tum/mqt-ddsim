#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN    = dd::ComplexNumbers;
using dEdge = dd::dEdge;
using Edge  = dd::Edge<dd::dNode>;

dEdge DeterministicNoiseSimulator::makeZeroDensityOperator(dd::QubitCount n) {
    auto f = dEdge::one;
    assert((signed char)n == n);
    for (dd::Qubit p = 0; p < (signed char)n; p++) {
        f = dd->makeDDNode(p, std::array{f, dEdge::zero, dEdge::zero, dEdge::zero});
    }
    return f;
}

dd::fp DeterministicNoiseSimulator::probForIndexToBeZero(dEdge e, dd::Qubit index, dd::fp pathProb, dd::fp global_prob) {
    if (Edge::isDensityMatrix((long)e.p)) {
        dEdge newEdge{dd->dUniqueTable.getNode(), e.w};
        e = Edge::getAlignedDensityNodeCopy(e, newEdge);
    }

    if (e.w.approximatelyZero() || (e.p->v == index && e.p->e[0].w.approximatelyZero())) {
        return 0;
    }

    assert(!e.isTerminal());
    assert(dd::CTEntry::val(e.w.i) == 0);
    pathProb = pathProb * dd::CTEntry::val(e.w.r);

    if (e.p->v > index) {
        global_prob += probForIndexToBeZero(e.p->e[0], index, pathProb, global_prob);
        global_prob += probForIndexToBeZero(e.p->e[3], index, pathProb, global_prob);
        return global_prob;
    } else {
        return pathProb;
    }
}

char DeterministicNoiseSimulator::MeasureOneCollapsing(dd::Qubit index) {
    char                                   result   = 'n';
    auto                                   f        = qc::MatrixDD::one;
    auto                                   n_qubits = getNumberOfQubits();
    std::uniform_real_distribution<dd::fp> dist(0.0, 1.0L);
    dd::fp                                 n = dist(mt);

    auto prob_zero = probForIndexToBeZero(density_root_edge, index, 1, 0);

    for (dd::Qubit p = 0; p < (signed char)n_qubits; p++) {
        if (p == index) {
            if (prob_zero >= n) {
                // Build the operation that it sets index to 0
                result = '0';
                f      = dd->makeDDNode(static_cast<dd::Qubit>(p), std::array{f, qc::MatrixDD::zero, qc::MatrixDD::zero, qc::MatrixDD::zero});
            } else {
                result = '1';
                // Build the operation that it sets index to 1
                f = dd->makeDDNode(static_cast<dd::Qubit>(p), std::array{qc::MatrixDD::zero, qc::MatrixDD::zero, qc::MatrixDD::zero, f});
            }
        } else {
            f = dd->makeDDNode(static_cast<dd::Qubit>(p), std::array{f, qc::MatrixDD::zero, qc::MatrixDD::zero, f});
        }
    }
    //    qc::MatrixDD tmp0 = dd->multiply(dd->multiply(f, density_root_edge), dd->conjugateTranspose(f));
    auto tmp0 = dd->conjugateTranspose(f);
    auto tmp1 = dd->multiply(density_root_edge, reinterpret_cast<dEdge&>(tmp0), 0, false);
    auto tmp2 = dd->multiply(reinterpret_cast<dEdge&>(f), tmp1, 0, true);

    dd->incRef(tmp0);
    dd->decRef(density_root_edge);
    density_root_edge = reinterpret_cast<dEdge&>(tmp0);

    // Normalize the density matrix
    auto trace = dd->trace(reinterpret_cast<dd::mEdge&>(density_root_edge));
    assert(trace.i == 0);
    auto tmp_complex = dd->cn.getTemporary(1 / trace.r, 0);
    dd::ComplexNumbers::mul(tmp_complex, density_root_edge.w, tmp_complex);
    dd::ComplexNumbers::decRef(density_root_edge.w);
    density_root_edge.w = dd->cn.lookup(tmp_complex);
    return result;
}

std::map<std::string, double> DeterministicNoiseSimulator::DeterministicSimulate() {
    const unsigned short         n_qubits = qc->getNqubits();
    std::map<unsigned int, bool> classic_values;

    density_root_edge = makeZeroDensityOperator(n_qubits);
    dd->incRef(density_root_edge);

    for (auto const& op: *qc) {
        //        opCounter++;
        dd->garbageCollect();
        if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
            if (auto* nu_op = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (op->getType() == qc::Measure) {
                    auto quantum = nu_op->getTargets();
                    auto classic = nu_op->getClassics();

                    if (quantum.size() != classic.size()) {
                        throw std::runtime_error("Measurement: Sizes of quantum and classic register mismatch.");
                    }

                    for (unsigned int i = 0; i < quantum.size(); ++i) {
                        auto result = MeasureOneCollapsing(quantum.at(i));
                        assert(result == '0' || result == '1');
                        classic_values[classic.at(i)] = (result == '1');
                    }
                } else if (op->getType() == qc::Reset) {
                    //                    // Reset qubit
                    throw std::runtime_error(std::string{"Unsupported non-unitary functionality: \""} + nu_op->getName() + "\"");
                } else {
                    //Skipping barrier
                    if (op->getType() == qc::Barrier) {
                        continue;
                    }
                    throw std::runtime_error(std::string{"Unsupported non-unitary functionality: \""} + nu_op->getName() + "\"");
                }
            } else {
                throw std::runtime_error("Dynamic cast to NonUnitaryOperation failed.");
            }
            dd->garbageCollect();
        } else {
            qc::MatrixDD dd_op = {};
            qc::Targets  targets;
            dd::Controls controls;
            if (op->isClassicControlledOperation()) {
                // Check if the operation is controlled by a classical register
                auto* classic_op    = dynamic_cast<qc::ClassicControlledOperation*>(op.get());
                bool  conditionTrue = true;
                auto  expValue      = classic_op->getExpectedValue();

                for (unsigned int i = classic_op->getControlRegister().first;
                     i < classic_op->getControlRegister().second; i++) {
                    if (classic_values[i] != (expValue % 2)) {
                        conditionTrue = false;
                        break;
                    } else {
                        expValue = expValue >> 1u;
                    }
                }
                dd_op   = dd::getDD(classic_op->getOperation(), dd);
                targets = classic_op->getOperation()->getTargets();
                assert(targets.size() == 1);
                controls = classic_op->getOperation()->getControls();
                if (!conditionTrue) {
                    continue;
                }
            } else {
                dd_op   = dd::getDD(op.get(), dd);
                targets = op->getTargets();
                assert(targets.size() == 1);
                controls = op->getControls();
            }

            // Applying the operation to the density matrix
            auto tmp0 = dd->conjugateTranspose(dd_op);
            auto tmp1 = dd->multiply(reinterpret_cast<dEdge&>(density_root_edge), reinterpret_cast<dEdge&>(tmp0), 0, false);
            auto tmp2 = dd->multiply(reinterpret_cast<dEdge&>(dd_op), reinterpret_cast<dEdge&>(tmp1), 0, use_density_matrix_type);
            dd->incRef(tmp2);
            density_root_edge.p = Edge::getAlignedDensityNode(density_root_edge.p);
            dd->decRef(density_root_edge);
            density_root_edge = tmp2;

            if (use_density_matrix_type) {
                density_root_edge.p = Edge::setDensityMatrixTrue(density_root_edge.p);
            }

            if (noiseProbability > 0) {
                std::vector used_qubits = op->getTargets();
                for (auto control: op->getControls()) {
                    used_qubits.push_back(control.qubit);
                }

                if (sequentialApplyNoise) { // was `stochastic_runs == -2`
                    [[maybe_unused]] auto cache_size_before = dd->cn.cacheCount();

                    applyDetNoiseSequential(used_qubits);

                    [[maybe_unused]] auto cache_size_after = dd->cn.cacheCount();
                    assert(cache_size_after == cache_size_before);
                } else {
//                    signed char maxDepth       = targets[0];
//                    auto        control_qubits = op->getControls();
//                    for (auto& control: control_qubits) {
//                        if (control.qubit < maxDepth) {
//                            maxDepth = control.qubit;
//                        }
//                    }

                    //todo I only must check array elements <=current_v, for the caching
                    sort(used_qubits.begin(), used_qubits.end(), std::greater<>());

                    if (used_qubits.size() > 1) {
                        noiseProbability *= 2;
                    }

                    [[maybe_unused]] auto cache_size_before = dd->cn.cacheCount();

                    dd::Edge<dd::dNode> nodeAfterNoise = {};
                    if (use_density_matrix_type) {
                        Edge::applyDmChangesToEdges(&density_root_edge, nullptr);
                        nodeAfterNoise = ApplyNoiseEffects(density_root_edge, used_qubits, false);
                        Edge::revertDmChangesToEdges(&density_root_edge, nullptr);
                    } else {
                        nodeAfterNoise = ApplyNoiseEffects(density_root_edge, used_qubits, true);
                    }
                    if (used_qubits.size() > 1) {
                        noiseProbability /= 2;
                    }

                    [[maybe_unused]] auto cache_size_after = dd->cn.cacheCount();
                    assert(cache_size_after == cache_size_before);

                    dd->incRef(nodeAfterNoise);

                    density_root_edge.p = Edge::getAlignedDensityNode(density_root_edge.p);
                    dd->decRef(density_root_edge);
                    density_root_edge = nodeAfterNoise;
                    if (use_density_matrix_type) {
                        density_root_edge.p = Edge::setDensityMatrixTrue(density_root_edge.p);
                    }
                }
            }
        }
    }
    dd->garbageCollect(true);
    printf("Multiplication cache");
    dd->densityMul.printStatistics();
    printf("Add cache\n");
    dd->densityAdd.printStatistics();
    printf("Noise cache\n");
    dd->densityNoise.printStatistics();
    return AnalyseState(n_qubits, false);
}

dEdge DeterministicNoiseSimulator::ApplyNoiseEffects(dEdge& originalEdge, const std::vector<dd::Qubit>& used_qubits, bool firstPathEdge) {
    if (originalEdge.p->v < used_qubits.back() || originalEdge.isTerminal()) {
        dEdge tmp{};
        if (originalEdge.w.approximatelyZero()) {
            tmp.w = dd::Complex::zero;
        } else {
            tmp.w = originalEdge.w;
            //            noiseLookUpResult.w = dd->cn.getCached(dd::CTEntry::val(originalEdge.w.r), dd::CTEntry::val(originalEdge.w.i));
        }
        if (originalEdge.isTerminal()) {
            return dEdge::terminal(tmp.w);
        } else {
            tmp.p = originalEdge.p;
            return tmp;
        }
    }
    auto originalCopy = originalEdge;
    originalCopy.w    = dd::Complex::one;

    // Check if the target of the current edge is in the "compute table".
    auto noiseLookUpResult = dd->densityNoise.lookup(originalCopy, used_qubits);

    if (noiseLookUpResult.p != nullptr) {
        auto tmpComplexValue = dd->cn.getCached();
        CN::mul(tmpComplexValue, noiseLookUpResult.w, originalEdge.w);
        noiseLookUpResult.w = dd->cn.lookup(tmpComplexValue);
        dd->cn.returnToCache(tmpComplexValue);
        return noiseLookUpResult;
    }

    std::array<dEdge, 4> new_edges{};
    for (short i = 0; i < 4; i++) {
        if (firstPathEdge || i == 1) {
            // If I am to the firstPathEdge I cannot minimize the necessary operations anymore
            Edge::applyDmChangesToEdges(&originalCopy.p->e[i], nullptr);
            new_edges[i] = ApplyNoiseEffects(originalCopy.p->e[i], used_qubits, true);
            Edge::revertDmChangesToEdges(&originalCopy.p->e[i], nullptr);
        } else if (i == 2) {
            // Size e[1] == e[2] (due to density matrix representation), I can skip calculating e[2]
            new_edges[2].p = new_edges[1].p;
            new_edges[2].w = new_edges[1].w;
        } else {
            Edge::applyDmChangesToEdges(&originalCopy.p->e[i], nullptr);
            new_edges[i] = ApplyNoiseEffects(originalCopy.p->e[i], used_qubits, false);
            Edge::revertDmChangesToEdges(&originalCopy.p->e[i], nullptr);
        }
    }
    dd::Edge<dd::dNode> e = {};
    if (std::count(used_qubits.begin(), used_qubits.end(), originalCopy.p->v)) {
//    if (op->actsOn(originalCopy.p->v)) {
        //        [[maybe_unused]] auto cache_size_before = dd->cn.cacheCount();
        //        applyNoiseCall++;
        for (auto& new_edge: new_edges) {
            if (new_edge.w.approximatelyZero()) {
                new_edge.w = dd::Complex::zero;
            } else {
                new_edge.w = dd->cn.getCached(dd::CTEntry::val(new_edge.w.r), dd::CTEntry::val(new_edge.w.i));
            }
        }

        //        unsigned long cacheCount0 = dd->cn.cacheCount();
        //        unsigned long cacheCountA = 222;
        //        unsigned long cacheCountP = 222;
        //        unsigned long cacheCountD = 222;

        for (auto const& type: gateNoiseTypes) {
            switch (type) {
                case 'A':
                    ApplyAmplitudeDampingToNode(new_edges);
                    //                    cacheCountA = dd->cn.cacheCount();
                    break;
                case 'P':
                    ApplyPhaseFlipToNode(new_edges);
                    //                    cacheCountP = dd->cn.cacheCount();
                    break;
                case 'D':
                    ApplyDepolarisationToNode(new_edges);
                    //                    cacheCountD = dd->cn.cacheCount();
                    break;
                default:
                    throw std::runtime_error(std::string("Unknown gate noise type '") + type + "'");
            }
        }

        for (auto& new_edge: new_edges) {
            if (new_edge.w.approximatelyZero()) {
                if (new_edge.w != dd::Complex::zero) {
                    dd->cn.returnToCache(new_edge.w);
                    new_edge.w = dd::Complex::zero;
                }
            } else {
                dd::Complex c = dd->cn.lookup(new_edge.w);
                dd->cn.returnToCache(new_edge.w);
                new_edge.w = c;
            }
        }
        //        [[maybe_unused]] auto cache_size_after = dd->cn.cacheCount();
        //        if (cache_size_after != cache_size_before){
        //            std::cout << "before: " << cache_size_before << "\n" << std::endl;
        //            std::cout << "0: " << cacheCount0 << "\n" << std::endl;
        //            std::cout << "A: " << cacheCountA << "\n" << std::endl;
        //            std::cout << "P: " << cacheCountP << "\n" << std::endl;
        //            std::cout << "D: " << cacheCountD << "\n" << std::endl;
        //            std::cout << "after: " << cache_size_after << "\n" << std::endl;
        //            assert(false);
        //        }
    }

    e = dd->makeDDNode(originalCopy.p->v, new_edges, false, firstPathEdge);

    // Adding the noise operation to the cache, note that e.w is from the complex number table
    dd->densityNoise.insert(originalCopy, e, used_qubits);

    // Multiplying the old edge weight with the new one and looking up in the complex numbers table
    if (!e.w.approximatelyZero()) {
        if (e.w.approximatelyOne()) {
            e.w = originalEdge.w;
        } else {
            auto tmpComplexValue = dd->cn.getCached();
            CN::mul(tmpComplexValue, e.w, originalEdge.w);
            e.w = dd->cn.lookup(tmpComplexValue);
            dd->cn.returnToCache(tmpComplexValue);
        }
    }
    return e;
}

void DeterministicNoiseSimulator::ApplyPhaseFlipToNode(std::array<dEdge, 4>& e) {
    double      probability  = noiseProbability;
    dd::Complex complex_prob = dd->cn.getCached();

    //e[0] = e[0]

    //e[1] = (1-2p)*e[1]
    if (!e[1].w.approximatelyZero()) {
        complex_prob.r->value = 1 - 2 * probability;
        complex_prob.i->value = 0;
        CN::mul(e[1].w, complex_prob, e[1].w);
    }

    //e[2] = (1-2p)*e[2]
    if (!e[2].w.approximatelyZero()) {
        if (e[1].w.approximatelyZero()) {
            complex_prob.r->value = 1 - 2 * probability;
            complex_prob.i->value = 0;
        }
        CN::mul(e[2].w, complex_prob, e[2].w);
    }

    //e[3] = e[3]

    dd->cn.returnToCache(complex_prob);
}

void DeterministicNoiseSimulator::ApplyAmplitudeDampingToNode(std::array<dEdge, 4>& e) {
    double      probability  = noiseProbability * 2;
    dd::Complex complex_prob = dd->cn.getCached();
    dEdge       helper_edge[1];
    helper_edge[0].w = dd->cn.getCached();

    // e[0] = e[0] + p*e[3]
    if (!e[3].w.approximatelyZero()) {
        complex_prob.r->value = probability;
        complex_prob.i->value = 0;
        if (!e[0].w.approximatelyZero()) {
            CN::mul(helper_edge[0].w, complex_prob, e[3].w);
            helper_edge[0].p = e[3].p;
            dd::Edge tmp     = dd->add2(e[0], helper_edge[0]);
            if (e[0].w != dd::Complex::zero) {
                dd->cn.returnToCache(e[0].w);
            }
            e[0] = tmp;
        } else {
            e[0].w = dd->cn.getCached();
            CN::mul(e[0].w, complex_prob, e[3].w);
            e[0].p = e[3].p;
        }
    }

    //e[1] = sqrt(1-p)*e[1]
    if (!e[1].w.approximatelyZero()) {
        complex_prob.r->value = std::sqrt(1 - probability);
        complex_prob.i->value = 0;
        CN::mul(e[1].w, complex_prob, e[1].w);
    }

    //e[2] = sqrt(1-p)*e[2]
    if (!e[2].w.approximatelyZero()) {
        if (e[1].w.approximatelyZero()) {
            complex_prob.r->value = std::sqrt(1 - probability);
            complex_prob.i->value = 0;
        }
        CN::mul(e[2].w, complex_prob, e[2].w);
    }

    //e[3] = (1-p)*e[3]
    if (!e[3].w.approximatelyZero()) {
        complex_prob.r->value = 1 - probability;
        CN::mul(e[3].w, complex_prob, e[3].w);
    }

    dd->cn.returnToCache(helper_edge[0].w);
    dd->cn.returnToCache(complex_prob);
}

void DeterministicNoiseSimulator::ApplyDepolarisationToNode(std::array<dEdge, 4>& e) {
    double      probability = noiseProbability;
    dEdge       helper_edge[2];
    dd::Complex complex_prob = dd->cn.getCached();
    complex_prob.i->value    = 0;

    dEdge old_e0_edge;
    old_e0_edge.w = dd->cn.getCached(dd::CTEntry::val(e[0].w.r), dd::CTEntry::val(e[0].w.i));
    old_e0_edge.p = e[0].p;

    //e[0] = 0.5*((2-p)*e[0] + p*e[3])
    {
        helper_edge[0].w = dd::Complex::zero;
        helper_edge[1].w = dd::Complex::zero;

        //helper_edge[0] = 0.5*((2-p)*e[0]
        if (!e[0].w.approximatelyZero()) {
            helper_edge[0].w      = dd->cn.getCached();
            complex_prob.r->value = (2 - probability) * 0.5;
            CN::mul(helper_edge[0].w, complex_prob, e[0].w);
            helper_edge[0].p = e[0].p;
        }

        //helper_edge[1] = 0.5*p*e[3]
        if (!e[3].w.approximatelyZero()) {
            helper_edge[1].w      = dd->cn.getCached();
            complex_prob.r->value = probability * 0.5;
            CN::mul(helper_edge[1].w, complex_prob, e[3].w);
            helper_edge[1].p = e[3].p;
        }

        //e[0] = helper_edge[0] + helper_edge[1]
        if (e[0].w != dd::Complex::zero) {
            dd->cn.returnToCache(e[0].w);
        }
        e[0] = dd->add2(helper_edge[0], helper_edge[1]);

        if (helper_edge[0].w != dd::Complex::zero) {
            dd->cn.returnToCache(helper_edge[0].w);
        }
        if (helper_edge[1].w != dd::Complex::zero) {
            dd->cn.returnToCache(helper_edge[1].w);
        }
    }

    //e[1]=1-p*e[1]
    if (!e[1].w.approximatelyZero()) {
        complex_prob.r->value = 1 - probability;
        CN::mul(e[1].w, e[1].w, complex_prob);
    }
    //e[2]=1-p*e[2]
    if (!e[2].w.approximatelyZero()) {
        if (e[1].w.approximatelyZero()) {
            complex_prob.r->value = 1 - probability;
        }
        CN::mul(e[2].w, e[2].w, complex_prob);
    }

    //e[3] = 0.5*((2-p)*e[3]) + 0.5*(p*e[0])
    {
        helper_edge[0].w = dd::Complex::zero;
        helper_edge[1].w = dd::Complex::zero;

        //helper_edge[0] = 0.5*((2-p)*e[3])
        if (!e[3].w.approximatelyZero()) {
            helper_edge[0].w      = dd->cn.getCached();
            complex_prob.r->value = (2 - probability) * 0.5;
            CN::mul(helper_edge[0].w, complex_prob, e[3].w);
            helper_edge[0].p = e[3].p;
        }

        //helper_edge[1] = 0.5*p*e[0]
        if (!old_e0_edge.w.approximatelyZero()) {
            helper_edge[1].w      = dd->cn.getCached();
            complex_prob.r->value = probability * 0.5;
            CN::mul(helper_edge[1].w, complex_prob, old_e0_edge.w);
            helper_edge[1].p = old_e0_edge.p;
        }

        //e[3] = helper_edge[0] + helper_edge[1]
        if (e[3].w != dd::Complex::zero) {
            dd->cn.returnToCache(e[3].w);
        }
        e[3] = dd->add2(helper_edge[0], helper_edge[1]);

        if (helper_edge[0].w != dd::Complex::zero) {
            dd->cn.returnToCache(helper_edge[0].w);
        }
        if (helper_edge[1].w != dd::Complex::zero) {
            dd->cn.returnToCache(helper_edge[1].w);
        }
    }
    dd->cn.returnToCache(old_e0_edge.w);
    dd->cn.returnToCache(complex_prob);
}

std::map<std::string, double> DeterministicNoiseSimulator::AnalyseState(dd::QubitCount nr_qubits, bool full_state) {
    std::map<std::string, double> measuredResult = {};
    double                        p0, p1;
    double long                   global_probability;
    double                        statesToMeasure;

    density_root_edge.p = Edge::getAlignedDensityNode(density_root_edge.p);

    if (full_state) {
        statesToMeasure = pow(2, nr_qubits);
    } else {
        statesToMeasure = std::min((double)256, pow(2, nr_qubits));
    }

    for (int m = 0; m < statesToMeasure; m++) {
        int current_result         = m;
        global_probability         = dd::CTEntry::val(density_root_edge.w.r);
        std::string  result_string = intToString(m, '1');
        qc::MatrixDD cur           = reinterpret_cast<const dd::mEdge&>(density_root_edge);
        for (int i = 0; i < nr_qubits; ++i) {
            if (cur.p->v != -1) {
                assert(dd::CTEntry::approximatelyZero(cur.p->e.at(0).w.i) && dd::CTEntry::approximatelyZero(cur.p->e.at(3).w.i));
                p0 = dd::CTEntry::val(cur.p->e.at(0).w.r);
                p1 = dd::CTEntry::val(cur.p->e.at(3).w.r);
            } else {
                global_probability = 0;
                break;
            }

            if (current_result % 2 == 0) {
                cur = cur.p->e.at(0);
                global_probability *= p0;
            } else {
                cur = cur.p->e.at(3);
                global_probability *= p1;
            }
            current_result = current_result >> 1;
        }
        if (global_probability > 0.01) {
            measuredResult.insert({result_string, global_probability});
        }
    }
    return measuredResult;
}

void DeterministicNoiseSimulator::generateGate(qc::MatrixDD* pointer_for_matrices, char noise_type, dd::Qubit target) {
    std::array<dd::GateMatrix, 4> idle_noise_gate{};
    dd::ComplexValue              tmp = {};

    double probability = noiseProbability;

    switch (noise_type) {
        case 'P': {
            tmp.r                 = std::sqrt(1 - probability) * dd::complex_one.r;
            idle_noise_gate[0][0] = idle_noise_gate[0][3] = tmp;
            idle_noise_gate[0][1] = idle_noise_gate[0][2] = dd::complex_zero;
            tmp.r                                         = std::sqrt(probability) * dd::complex_one.r;
            idle_noise_gate[1][0]                         = tmp;
            tmp.r *= -1;
            idle_noise_gate[1][3] = tmp;
            idle_noise_gate[1][1] = idle_noise_gate[1][2] = dd::complex_zero;

            pointer_for_matrices[0] = dd->makeGateDD(idle_noise_gate[0], getNumberOfQubits(), target);
            pointer_for_matrices[1] = dd->makeGateDD(idle_noise_gate[1], getNumberOfQubits(), target);

            break;
        }
            // amplitude damping
            //      (1      0           )       (0      sqrt(probability))
            //  e0= (0      sqrt(1-probability)   ), e1=  (0      0      )
        case 'A': {
            tmp.r                 = std::sqrt(1 - probability * 2) * dd::complex_one.r;
            idle_noise_gate[0][0] = dd::complex_one;
            idle_noise_gate[0][1] = idle_noise_gate[0][2] = dd::complex_zero;
            idle_noise_gate[0][3]                         = tmp;

            tmp.r                 = std::sqrt(probability * 2) * dd::complex_one.r;
            idle_noise_gate[1][0] = idle_noise_gate[1][3] = idle_noise_gate[1][2] = dd::complex_zero;
            idle_noise_gate[1][1]                                                 = tmp;

            pointer_for_matrices[0] = dd->makeGateDD(idle_noise_gate[0], getNumberOfQubits(), target);
            pointer_for_matrices[1] = dd->makeGateDD(idle_noise_gate[1], getNumberOfQubits(), target);
            break;
        }
            // depolarization
        case 'D': {
            tmp.r = std::sqrt(1 - ((3 * probability) / 4)) * dd::complex_one.r;
            //                   (1 0)
            // sqrt(1- ((3p)/4))*(0 1)
            idle_noise_gate[0][0] = idle_noise_gate[0][3] = tmp;
            idle_noise_gate[0][1] = idle_noise_gate[0][2] = dd::complex_zero;

            pointer_for_matrices[0] = dd->makeGateDD(idle_noise_gate[0], getNumberOfQubits(), target);

            //            (0 1)
            // sqrt(probability/4))*(1 0)
            tmp.r                 = std::sqrt(probability / 4) * dd::complex_one.r;
            idle_noise_gate[1][1] = idle_noise_gate[1][2] = tmp;
            idle_noise_gate[1][0] = idle_noise_gate[1][3] = dd::complex_zero;

            pointer_for_matrices[1] = dd->makeGateDD(idle_noise_gate[1], getNumberOfQubits(), target);

            //            (1 0)
            // sqrt(probability/4))*(0 -1)
            tmp.r                 = std::sqrt(probability / 4) * dd::complex_one.r;
            idle_noise_gate[2][0] = tmp;
            tmp.r                 = tmp.r * -1;
            idle_noise_gate[2][3] = tmp;
            idle_noise_gate[2][1] = idle_noise_gate[2][2] = dd::complex_zero;

            pointer_for_matrices[3] = dd->makeGateDD(idle_noise_gate[2], getNumberOfQubits(), target);

            //            (0 -i)
            // sqrt(probability/4))*(i 0)
            tmp.r                 = dd::complex_zero.r;
            tmp.i                 = std::sqrt(probability / 4) * 1;
            idle_noise_gate[3][2] = tmp;
            tmp.i                 = tmp.i * -1;
            idle_noise_gate[3][1] = tmp;
            idle_noise_gate[3][0] = idle_noise_gate[3][3] = dd::complex_zero;

            pointer_for_matrices[2] = dd->makeGateDD(idle_noise_gate[3], getNumberOfQubits(), target);
            break;
        }
        default:
            throw std::runtime_error("Unknown noise effect received.");
    }
}

void DeterministicNoiseSimulator::applyDetNoiseSequential(const qc::Targets& targets) {
    dd::dEdge tmp = {};
    //    qc::MatrixDD ancillary_edge_1 = {};
    qc::MatrixDD idle_operation[4];

    // Iterate over qubits and check if the qubit had been used
    for (auto target_qubit: targets) {
        for (auto const& type: gateNoiseTypes) {
            generateGate(idle_operation, type, target_qubit);
            tmp.p = nullptr;
            //Apply all noise matrices of the current noise effect
            for (int m = 0; m < noiseEffects.find(type)->second; m++) {
                auto tmp0 = dd->conjugateTranspose(idle_operation[m]);
                auto tmp1 = dd->multiply(density_root_edge, reinterpret_cast<dEdge&>(tmp0), 0, false);
                auto tmp2 = dd->multiply(reinterpret_cast<dEdge&>(idle_operation[m]), tmp1, 0, use_density_matrix_type);

                //                auto tmp0 = dd->multiply(dd->multiply(idle_operation[m], density_root_edge), dd->conjugateTranspose(idle_operation[m]));
                if (tmp.p == nullptr) {
                    tmp = tmp2;
                } else {
                    tmp = dd->add(tmp2, tmp);
                }
            }
            dd->incRef(tmp);
            density_root_edge.p = Edge::getAlignedDensityNode(density_root_edge.p);
            dd->decRef(density_root_edge);
            density_root_edge = tmp;
            if (use_density_matrix_type) {
                density_root_edge.p = Edge::setDensityMatrixTrue(density_root_edge.p);
            }
        }
    }
}

std::string DeterministicNoiseSimulator::intToString(long target_number, char value) const {
    if (target_number < 0) {
        assert(target_number == -1);
        return {"F"};
    }
    auto        qubits = getNumberOfQubits();
    std::string path(qubits, '0');
    auto        number = (unsigned long)target_number;
    for (int i = 1; i <= qubits; i++) {
        if (number % 2) {
            path[qubits - i] = value;
        }
        number = number >> 1u;
    }
    return path;
}