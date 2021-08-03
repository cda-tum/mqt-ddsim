#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN = dd::ComplexNumbers;

qc::MatrixDD DeterministicNoiseSimulator::makeZeroDensityOperator(dd::QubitCount n) {
    auto f = qc::MatrixDD::one;
    assert((signed char)n == n);
    for (dd::Qubit p = 0; p < (signed char)n; p++) {
        f = dd->makeDDNode(p, std::array{f, qc::MatrixDD::zero, qc::MatrixDD::zero, qc::MatrixDD::zero});
    }
    return f;
}

dd::fp DeterministicNoiseSimulator::probForIndexToBeZero(qc::MatrixDD e, dd::Qubit index, dd::fp pathProb, dd::fp global_prob) {
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
    qc::MatrixDD tmp0 = dd->multiply(dd->multiply(f, density_root_edge), dd->conjugateTranspose(f));
    dd->incRef(tmp0);
    dd->decRef(density_root_edge);
    density_root_edge = tmp0;

    // Normalize the density matrix
    auto trace = dd->trace(density_root_edge);
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

    int op_count = 0;

    for (auto const& op: *qc) {
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
            if (dd->garbageCollect()) {
                NoiseTable.fill({});
            }
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
                dd_op   = classic_op->getOperation()->getDD(dd);
                targets = classic_op->getOperation()->getTargets();
                assert(targets.size() == 1);
                controls = classic_op->getOperation()->getControls();
                if (!conditionTrue) {
                    continue;
                }
            } else {
                dd_op   = op->getDD(dd);
                targets = op->getTargets();
                assert(targets.size() == 1);
                controls = op->getControls();
            }
            // Applying the operation to the density matrix
            qc::MatrixDD tmp0 = dd->multiply(dd->multiply(dd_op, density_root_edge), dd->conjugateTranspose(dd_op));
            dd->incRef(tmp0);
            dd->decRef(density_root_edge);
            density_root_edge = tmp0;

//            dd->printMatrix(density_root_edge);
            if (noiseProbability > 0) {
                if (sequentialApplyNoise) { // was `stochastic_runs == -2`
                    [[maybe_unused]] auto cache_size_before = dd->cn.cacheCount();

                    auto seq_targets = targets;
                    for (auto const& control: controls) {
                        seq_targets.push_back(control.qubit);
                    }
                    applyDetNoiseSequential(seq_targets);

                    [[maybe_unused]] auto cache_size_after = dd->cn.cacheCount();
                    assert(cache_size_after == cache_size_before);
                } else {
                    signed char maxDepth       = targets[0];
                    auto        control_qubits = op->getControls();
                    for (auto& control: control_qubits) {
                        if (control.qubit < maxDepth) {
                            maxDepth = control.qubit;
                        }
                    }
                    [[maybe_unused]] auto cache_size_before = dd->cn.cacheCount();
                    auto                  tmp2              = ApplyNoiseEffects(density_root_edge, op, 0);
                    [[maybe_unused]] auto cache_size_after  = dd->cn.cacheCount();
                    assert(cache_size_after == cache_size_before);

                    dd->incRef(tmp2);
                    dd->decRef(density_root_edge);
                    density_root_edge = tmp2;
                }
            }
            op_count += 1;
        }
    }
    if (dd->garbageCollect()) {
        NoiseTable.fill({});
    }
    //    dd->printMatrix(density_root_edge);
    return AnalyseState(n_qubits, false);
}

qc::MatrixDD DeterministicNoiseSimulator::ApplyNoiseEffects(qc::MatrixDD density_op, const std::unique_ptr<qc::Operation>& op, unsigned char maxDepth) {
    if (density_op.p->v < maxDepth || density_op.isTerminal()) {
        qc::MatrixDD tmp{};
        if (!density_op.w.approximatelyZero()) {
            //            tmp.w = dd->cn.getCached(dd::CTEntry::val(density_op.w.r), dd::CTEntry::val(density_op.w.i));
            tmp.w = dd->cn.lookup(dd::CTEntry::val(density_op.w.r), dd::CTEntry::val(density_op.w.i));
        } else {
            tmp.w = dd::Complex::zero;
        }
        if (density_op.isTerminal()) {
            return qc::MatrixDD::terminal(tmp.w);
        } else {
            tmp.p = density_op.p;
            return tmp;
        }
    }

    std::vector used_qubits = op->getTargets();
    for (auto control: op->getControls()) {
        used_qubits.push_back(control.qubit);
    }
    sort(used_qubits.begin(), used_qubits.end());

    std::array<qc::MatrixDD, 4> new_edges{};
    for (int i = 0; i < 4; i++) {
        if (density_op.p->e[i].w.approximatelyZero()) {
            // applyNoiseEffects2 returns a pointer to the terminal node with edge weight 0
            new_edges[i] = qc::MatrixDD::terminal(density_op.p->e[i].w);
            continue;
        }

        // Check if the target of the current edge is in the Compute table. Note that I check for the target node of
        // the current edge if noise needs to be applied or not
        qc::MatrixDD tmp = dd->densityNoiseOperations.lookup(density_op.p->e[i], used_qubits);

        if (tmp.p != nullptr) {
            new_edges[i] = tmp;
            continue;
        }

        new_edges[i] = ApplyNoiseEffects(density_op.p->e[i], op, maxDepth);


        // Adding the operation to the operation table
        dd->densityNoiseOperations.insert(density_op.p->e[i], new_edges[i], used_qubits);
    }

    [[maybe_unused]] auto cache_size_before = dd->cn.cacheCount();
    if (op->actsOn(density_op.p->v)) {
        for (auto const& type: gateNoiseTypes) {
            switch (type) {
                case 'A':
                    ApplyAmplitudeDampingToNode(new_edges);
                    break;
                case 'P':
                    ApplyPhaseFlipToNode(new_edges);
                    break;
                    //case 'B':
                    //    applyBitFlipToNode(new_edges);
                    //    break;
                case 'D':
                    ApplyDepolaritationToNode(new_edges);
                    break;
                default:
                    throw std::runtime_error(std::string("Unknown gate noise type '") + type + "'");
            }
        }
    }
    [[maybe_unused]] auto cache_size_after = dd->cn.cacheCount();
    assert(cache_size_after == cache_size_before);

    qc::MatrixDD tmp = dd->makeDDNode(density_op.p->v, new_edges, false);

    // Multiplying the old edge weight with the new one
    if (!tmp.w.approximatelyZero()) {
        auto tmp1 = dd->cn.getTemporary();
        CN::mul(tmp1, tmp.w, density_op.w);
        tmp.w = dd->cn.lookup(tmp1);
    } else {
        tmp.w = dd->cn.lookup(tmp.w);
    }
    return tmp;
}

void DeterministicNoiseSimulator::ApplyPhaseFlipToNode(std::array<qc::MatrixDD, 4>& e) {
    //e[0] = e[0]

    //e[1] = (1-2p)*e[1]
    if (!e[1].w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, oneMinusNoiseTwoProbFromTable, e[1].w);
        e[1].w = dd->cn.lookup(tmp);
    }

    //e[2] = (1-2p)*e[2]
    if (!e[2].w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, oneMinusNoiseTwoProbFromTable, e[2].w);
        e[2].w = dd->cn.lookup(tmp);
    }

    //e[3] = e[3]
}

void DeterministicNoiseSimulator::ApplyAmplitudeDampingToNode(std::array<qc::MatrixDD, 4>& e) {
    // e[0] = e[0] + p*e[3]
    if (!e[3].w.approximatelyZero()) {
        if (!e[0].w.approximatelyZero()) {
            auto tmp_edge = e[3];
            tmp_edge.w    = dd->cn.mulCached(noiseProbFromTableAmp, tmp_edge.w);
            dd::Edge tmp  = dd->add2(e[0], tmp_edge);
            dd->cn.returnToCache(tmp_edge.w);
            e[0].w = dd->cn.lookup(tmp.w);
            dd->cn.returnToCache(tmp.w);
            e[0].p = tmp.p;
        } else {
            auto tmp = dd->cn.getTemporary();
            CN::mul(tmp, noiseProbFromTableAmp, e[3].w);
            e[0].w = dd->cn.lookup(tmp);
            e[0].p = e[3].p;
        }
    }

    //e[1] = sqrt(1-p)*e[1]
    if (!e[1].w.approximatelyZero()) {
        //        CN::mul(e[1].w, complex_prob, e[1].w);
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, sqrtOneMinusNoiseProbFromTableAmp, e[1].w);
        e[1].w = dd->cn.lookup(tmp);
    }

    //e[2] = sqrt(1-p)*e[2]
    if (!e[2].w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, sqrtOneMinusNoiseProbFromTableAmp, e[2].w);
        e[2].w = dd->cn.lookup(tmp);
    }

    //e[3] = (1-p)*e[3]
    if (!e[3].w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, oneMinusNoiseProbFromTableAmp, e[3].w);
        e[3].w = dd->cn.lookup(tmp);
    }
}

void DeterministicNoiseSimulator::ApplyDepolaritationToNode(std::array<qc::MatrixDD, 4>& e) {
    dd::Edge old_edges_0 = e[0];
    dd::Edge old_edges_3 = e[3];

    //e[0] = 0.5*((2-p)*e[0] + p*e[3])
    if (!old_edges_0.w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, twoMinusNoiseProbFromTable, e[0].w);
        e[0].w = dd->cn.lookup(tmp);
    }
    if (!old_edges_3.w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, noiseProbFromTable, e[3].w);
        e[3].w = dd->cn.lookup(tmp);
    }
    e[0]      = dd->add(e[0], e[3]);
    auto tmp1 = dd->cn.getTemporary(0.5, 0);
    CN::mul(tmp1, tmp1, e[0].w);
    e[0].w = dd->cn.lookup(tmp1);

    //e[1]=(1-p)*e[1]
    if (!e[1].w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, oneMinusNoiseProbFromTable, e[1].w);
        e[1].w = dd->cn.lookup(tmp);
    }
    //e[2]=(1-p)*e[2]
    if (!e[2].w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, oneMinusNoiseProbFromTable, e[2].w);
        e[2].w = dd->cn.lookup(tmp);
    }

    //e[3] = 0.5*((2-p)*e[3] + p*e[0])
    if (!old_edges_3.w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, twoMinusNoiseProbFromTable, old_edges_3.w);
        e[3].w = dd->cn.lookup(tmp);
    }

    if (!old_edges_0.w.approximatelyZero()) {
        auto tmp = dd->cn.getTemporary();
        CN::mul(tmp, noiseProbFromTable, old_edges_0.w);
        old_edges_0.w = dd->cn.lookup(tmp);
    }
    e[3]      = dd->add(old_edges_0, e[3]);
    auto tmp2 = dd->cn.getTemporary(0.5, 0);
    CN::mul(tmp2, tmp2, e[3].w);
    e[3].w = dd->cn.lookup(tmp2);
}

std::map<std::string, double> DeterministicNoiseSimulator::AnalyseState(dd::QubitCount nr_qubits, bool full_state) const {
    std::map<std::string, double> measure_result = {};

    double p0, p1, imaginary;

    double long global_probability;

    auto delMe = dd::QubitCount();

    double measure_states = std::min((double)256, pow(2, nr_qubits));

    if (full_state) {
        measure_states = pow(2, nr_qubits);
    }

    for (int m = 0; m < measure_states; m++) {
        int current_result         = m;
        global_probability         = dd::CTEntry::val(density_root_edge.w.r);
        std::string  result_string = intToString(m, '1');
        qc::MatrixDD cur           = density_root_edge;
        for (int i = 0; i < nr_qubits; ++i) {
            if (cur.p->v != -1) {
                assert(dd::CTEntry::val(cur.p->e.at(0).w.i) + dd::CTEntry::val(cur.p->e[3].w.i) == 0);
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
        measure_result.insert({result_string, global_probability});
    }
    return measure_result;
}

void DeterministicNoiseSimulator::generateGate(qc::MatrixDD* pointer_for_matrices, char noise_type, dd::Qubit target) {
    std::array<dd::GateMatrix, 4> idle_noise_gate{};
    dd::ComplexValue              tmp = {};

    double probability = noiseProbability;

    switch (noise_type) {
        // bitflip
        //      (sqrt(1-probability)    0           )       (0      sqrt(probability))
        //  e0= (0            sqrt(1-probability)   ), e1=  (sqrt(probability)      0)
        //        case 'B': {
        //            tmp.r                 = std::sqrt(1 - probability) * dd::complex_one.r;
        //            idle_noise_gate[0][0] = idle_noise_gate[0][3] = tmp;
        //            idle_noise_gate[0][1] = idle_noise_gate[0][2] = dd::complex_zero;
        //            tmp.r                                         = std::sqrt(probability) * dd::complex_one.r;
        //            idle_noise_gate[1][1] = idle_noise_gate[1][2] = tmp;
        //            idle_noise_gate[1][0] = idle_noise_gate[1][3] = dd::complex_zero;
        //
        //            pointer_for_matrices[0] = dd->makeGateDD(idle_noise_gate[0], getNumberOfQubits(), target);
        //            pointer_for_matrices[1] = dd->makeGateDD(idle_noise_gate[1], getNumberOfQubits(), target);
        //            break;
        //        }
        // phase flip
        //      (sqrt(1-probability)    0           )       (sqrt(probability)      0)
        //  e0= (0            sqrt(1-probability)   ), e1=  (0      -sqrt(probability))
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
    qc::MatrixDD tmp = {};
    //    qc::MatrixDD ancillary_edge_1 = {};
    qc::MatrixDD idle_operation[4];

    // Iterate over qubits and check if the qubit had been used
    for (auto target_qubit: targets) {
        for (auto const& type: gateNoiseTypes) {
            generateGate(idle_operation, type, target_qubit);
            tmp.p = nullptr;
            //Apply all noise matrices of the current noise effect
            for (int m = 0; m < noiseEffects.find(type)->second; m++) {
                auto tmp0 = dd->multiply(dd->multiply(idle_operation[m], density_root_edge), dd->conjugateTranspose(idle_operation[m]));
                if (tmp.p == nullptr) {
                    tmp = tmp0;
                } else {
                    tmp = dd->add(tmp0, tmp);
                }
            }
            dd->incRef(tmp);
            dd->decRef(density_root_edge);
            density_root_edge = tmp;
        }
    }
}

std::string DeterministicNoiseSimulator::intToString(long target_number, char value) const {
    if (target_number < 0) {
        assert(target_number == -1);
        return (std::string("F"));
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

void DeterministicNoiseSimulator::noiseInsert(const qc::MatrixDD& a, const std::vector<signed char>& usedQubits, const qc::MatrixDD& r) {
    dd::ComplexValue    aw{dd::CTEntry::val(a.w.r), dd::CTEntry::val(a.w.i)};
    const unsigned long i = noiseHash(a.p, aw, usedQubits);

    assert(((std::uintptr_t)r.w.r & 1u) == 0 && ((std::uintptr_t)r.w.i & 1u) == 0);

    std::copy(usedQubits.begin(), usedQubits.end(), NoiseTable[i].usedQubits);
    NoiseTable[i].a    = a.p;
    NoiseTable[i].aw   = aw;
    NoiseTable[i].r    = r.p;
    NoiseTable[i].rw.r = r.w.r->value;
    NoiseTable[i].rw.i = r.w.i->value;
}

qc::MatrixDD DeterministicNoiseSimulator::noiseLookup(const qc::MatrixDD& a, const std::vector<signed char>& usedQubits) {
    // Lookup a computation in the compute table
    // return NULL if not a match else returns result of prior computation
    qc::MatrixDD r{nullptr, {nullptr, nullptr}};

    dd::ComplexValue aw{dd::CTEntry::val(a.w.r), dd::CTEntry::val(a.w.i)};

    const unsigned long i = noiseHash(a.p, aw, usedQubits);

    if (NoiseTable[i].a != a.p || NoiseTable[i].aw != aw) return r;
    if (!std::equal(usedQubits.begin(), usedQubits.end(), std::begin(NoiseTable[i].usedQubits))) {
        return r;
    }

    r.p = NoiseTable[i].r;

    if (std::abs(NoiseTable[i].rw.r) < dd::ComplexTable<>::tolerance() && std::fabs(NoiseTable[i].rw.i) < dd::ComplexTable<>::tolerance()) {
        return qc::MatrixDD::zero;
    } else {
        r.w = dd->cn.lookup(NoiseTable[i].rw.r, NoiseTable[i].rw.i);
    }
    return r;
}

unsigned long DeterministicNoiseSimulator::noiseHash(dd::Package::mNode* a, const dd::ComplexValue& aw, const std::vector<signed char>& usedQubits) {
    unsigned long i = 0;

    for (auto qubit: usedQubits) {
        i = (i << 3U) + i * qubit + qubit;
    }
    i += (reinterpret_cast<std::uintptr_t>(a)) + static_cast<unsigned long>(aw.r * 1000000 + aw.i * 2000000);
    return i & NoiseMASK;
}
