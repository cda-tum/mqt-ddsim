#include "DeterministicNoiseSimulator.hpp"

#include "dd/Export.hpp"

using CN    = dd::ComplexNumbers;
using dEdge = dd::dEdge;
using dNode = dd::dNode;
using mEdge = dd::mEdge;

dEdge DeterministicNoiseSimulator::makeZeroDensityOperator(dd::QubitCount n) {
    auto f = dEdge::one;
    assert((signed char)n == n);
    for (dd::Qubit p = 0; p < (signed char)n; p++) {
        f = dd->makeDDNode(p, std::array{f, dEdge::zero, dEdge::zero, dEdge::zero});
    }
    return f;
}

std::map<std::string, double> DeterministicNoiseSimulator::DeterministicSimulate() {
    const unsigned short         nQubits = qc->getNqubits();
    std::map<unsigned int, bool> classic_values;

    densityRootEdge = makeZeroDensityOperator(nQubits);
    dd->incRef(densityRootEdge);

    for (auto const& op: *qc) {
        dd->garbageCollect();
        if (!op->isUnitary() && !(op->isClassicControlledOperation())) {
            if (auto* nu_op = dynamic_cast<qc::NonUnitaryOperation*>(op.get())) {
                if (op->getType() == qc::Measure) {
                    throw std::invalid_argument("Measurements are currently not supported");
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
            qc::MatrixDD operation = {};
            qc::Targets  targets;
            dd::Controls controls;
            if (op->isClassicControlledOperation()) {
                throw std::runtime_error("Classical controlled operations are not supported.");
            } else {
                operation = dd::getDD(op.get(), dd);
                targets   = op->getTargets();
                controls  = op->getControls();
            }

            // Applying the operation to the density matrix
            auto tmp0 = dd->conjugateTranspose(operation);
            auto tmp1 = dd->multiply(reinterpret_cast<dEdge&>(densityRootEdge), reinterpret_cast<dEdge&>(tmp0), 0, false);
            auto tmp2 = dd->multiply(reinterpret_cast<dEdge&>(operation), reinterpret_cast<dEdge&>(tmp1), 0, useDensityMatrixType);
            dd->incRef(tmp2);
            dEdge::alignDensityEdge(&densityRootEdge);
            dd->decRef(densityRootEdge);
            densityRootEdge = tmp2;

            if (useDensityMatrixType) {
                dEdge::setDensityMatrixTrue(&densityRootEdge);
            }

            std::vector usedQubits = targets;
            for (auto control: controls) {
                usedQubits.push_back(control.qubit);
            }

            if (sequentiallyApplyNoise) {
                [[maybe_unused]] auto cacheSizeBefore = dd->cn.cacheCount();

                applyDetNoiseSequential(usedQubits);

                [[maybe_unused]] auto cache_size_after = dd->cn.cacheCount();
                assert(cache_size_after == cacheSizeBefore);
            } else {
                sort(usedQubits.begin(), usedQubits.end(), std::greater<>());

                [[maybe_unused]] auto cacheSizeBefore = dd->cn.cacheCount();

                dEdge nodeAfterNoise = {};
                if (useDensityMatrixType) {
                    dEdge::applyDmChangesToEdges(&densityRootEdge, nullptr);
                    nodeAfterNoise = applyNoiseEffects(densityRootEdge, usedQubits, false);
                    dEdge::revertDmChangesToEdges(&densityRootEdge, nullptr);
                } else {
                    nodeAfterNoise = applyNoiseEffects(densityRootEdge, usedQubits, true);
                }

                [[maybe_unused]] auto cacheSizeAfter = dd->cn.cacheCount();
                assert(cacheSizeAfter == cacheSizeBefore);

                dd->incRef(nodeAfterNoise);

                dEdge::alignDensityEdge(&densityRootEdge);
                dd->decRef(densityRootEdge);
                densityRootEdge = nodeAfterNoise;
                if (useDensityMatrixType) {
                    dEdge::setDensityMatrixTrue(&densityRootEdge);
                }
            }
        }
    }
    return AnalyseState(nQubits, false);
}

dEdge DeterministicNoiseSimulator::applyNoiseEffects(dEdge& originalEdge, const std::vector<dd::Qubit>& usedQubits, bool firstPathEdge) {
    if (originalEdge.p->v < usedQubits.back() || originalEdge.isTerminal()) {
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

    //    auto noiseLookUpResult = dd->densityNoise.lookup(originalCopy, usedQubits);
    //
    //    if (noiseLookUpResult.p != nullptr) {
    //        auto tmpComplexValue = dd->cn.getCached();
    //        CN::mul(tmpComplexValue, noiseLookUpResult.w, originalEdge.w);
    //        noiseLookUpResult.w = dd->cn.lookup(tmpComplexValue);
    //        dd->cn.returnToCache(tmpComplexValue);
    //        return noiseLookUpResult;
    //    }

    std::array<dEdge, 4> new_edges{};
    for (short i = 0; i < 4; i++) {
        if (firstPathEdge || i == 1) {
            // If I am to the useDensityMatrix I cannot minimize the necessary operations anymore
            dEdge::applyDmChangesToEdges(&originalCopy.p->e[i], nullptr);
            new_edges[i] = applyNoiseEffects(originalCopy.p->e[i], usedQubits, true);
            dEdge::revertDmChangesToEdges(&originalCopy.p->e[i], nullptr);
        } else if (i == 2) {
            // Size e[1] == e[2] (due to density matrix representation), I can skip calculating e[2]
            new_edges[2].p = new_edges[1].p;
            new_edges[2].w = new_edges[1].w;
        } else {
            dEdge::applyDmChangesToEdges(&originalCopy.p->e[i], nullptr);
            new_edges[i] = applyNoiseEffects(originalCopy.p->e[i], usedQubits, false);
            dEdge::revertDmChangesToEdges(&originalCopy.p->e[i], nullptr);
        }
    }
    dEdge e = {};
    if (std::count(usedQubits.begin(), usedQubits.end(), originalCopy.p->v)) {
        for (auto& new_edge: new_edges) {
            if (new_edge.w.approximatelyZero()) {
                new_edge.w = dd::Complex::zero;
            } else {
                new_edge.w = dd->cn.getCached(dd::CTEntry::val(new_edge.w.r), dd::CTEntry::val(new_edge.w.i));
            }
        }

        for (auto const& type: noiseEffects) {
            switch (type) {
                case dd::amplitudeDamping:
                    applyAmplitudeDampingToEdges(new_edges, (usedQubits.size() == 1) ? ampDampingProbSingleQubit : ampDampingProbMultiQubit);
                    break;
                case dd::phaseFlip:
                    applyPhaseFlipToEdges(new_edges, (usedQubits.size() == 1) ? noiseProbSingleQubit : noiseProbMultiQubit);
                    break;
                case dd::depolarization:
                    applyDepolarisationToEdges(new_edges, (usedQubits.size() == 1) ? noiseProbSingleQubit : noiseProbMultiQubit);
                    break;
                case dd::identity:
                    continue;
            }
        }

        for (auto& new_edge: new_edges) {
            if (new_edge.w.approximatelyZero()) {
                if (!new_edge.w.exactlyZero()) {
                    dd->cn.returnToCache(new_edge.w);
                    new_edge.w = dd::Complex::zero;
                }
            } else {
                dd::Complex c = dd->cn.lookup(new_edge.w);
                dd->cn.returnToCache(new_edge.w);
                new_edge.w = c;
            }
        }
    }

    e = dd->makeDDNode(originalCopy.p->v, new_edges, false, firstPathEdge);

    // Adding the noise operation to the cache, note that e.w is from the complex number table
    //    dd->densityNoise.insert(originalCopy, e, usedQubits);

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

void DeterministicNoiseSimulator::applyPhaseFlipToEdges(std::array<dEdge, std::tuple_size_v<decltype(dd::dNode::e)>>& e, double probability) {
    dd::Complex complexProb = dd->cn.getCached();

    //e[0] = e[0]

    //e[1] = (1-2p)*e[1]
    if (!e[1].w.approximatelyZero()) {
        complexProb.r->value = 1 - 2 * probability;
        complexProb.i->value = 0;
        CN::mul(e[1].w, complexProb, e[1].w);
    }

    //e[2] = (1-2p)*e[2]
    if (!e[2].w.approximatelyZero()) {
        if (e[1].w.approximatelyZero()) {
            complexProb.r->value = 1 - 2 * probability;
            complexProb.i->value = 0;
        }
        CN::mul(e[2].w, complexProb, e[2].w);
    }

    //e[3] = e[3]

    dd->cn.returnToCache(complexProb);
}

void DeterministicNoiseSimulator::applyAmplitudeDampingToEdges(std::array<dEdge, std::tuple_size_v<decltype(dd::dNode::e)>>& e, double probability) {
    dd::Complex complexProb = dd->cn.getCached();
    dEdge       helperEdge;
    helperEdge.w = dd->cn.getCached();

    // e[0] = e[0] + p*e[3]
    if (!e[3].w.approximatelyZero()) {
        complexProb.r->value = probability;
        complexProb.i->value = 0;
        if (!e[0].w.approximatelyZero()) {
            CN::mul(helperEdge.w, complexProb, e[3].w);
            helperEdge.p = e[3].p;
            dd::Edge tmp = dd->add2(e[0], helperEdge);
            if (!e[0].w.exactlyZero()) {
                dd->cn.returnToCache(e[0].w);
            }
            e[0] = tmp;
        } else {
            e[0].w = dd->cn.getCached();
            CN::mul(e[0].w, complexProb, e[3].w);
            e[0].p = e[3].p;
        }
    }

    //e[1] = sqrt(1-p)*e[1]
    if (!e[1].w.approximatelyZero()) {
        complexProb.r->value = std::sqrt(1 - probability);
        complexProb.i->value = 0;
        CN::mul(e[1].w, complexProb, e[1].w);
    }

    //e[2] = sqrt(1-p)*e[2]
    if (!e[2].w.approximatelyZero()) {
        if (e[1].w.approximatelyZero()) {
            complexProb.r->value = std::sqrt(1 - probability);
            complexProb.i->value = 0;
        }
        CN::mul(e[2].w, complexProb, e[2].w);
    }

    //e[3] = (1-p)*e[3]
    if (!e[3].w.approximatelyZero()) {
        complexProb.r->value = 1 - probability;
        CN::mul(e[3].w, complexProb, e[3].w);
    }

    dd->cn.returnToCache(helperEdge.w);
    dd->cn.returnToCache(complexProb);
}

void DeterministicNoiseSimulator::applyDepolarisationToEdges(std::array<dEdge, std::tuple_size_v<decltype(dd::dNode::e)>>& e, double probability) {
    dEdge       helperEdge[2];
    dd::Complex complexProb = dd->cn.getCached();
    complexProb.i->value    = 0;

    dEdge oldE0Edge;
    oldE0Edge.w = dd->cn.getCached(dd::CTEntry::val(e[0].w.r), dd::CTEntry::val(e[0].w.i));
    oldE0Edge.p = e[0].p;

    //e[0] = 0.5*((2-p)*e[0] + p*e[3])
    {
        helperEdge[0].w = dd::Complex::zero;
        helperEdge[1].w = dd::Complex::zero;

        //helperEdge[0] = 0.5*((2-p)*e[0]
        if (!e[0].w.approximatelyZero()) {
            helperEdge[0].w      = dd->cn.getCached();
            complexProb.r->value = (2 - probability) * 0.5;
            CN::mul(helperEdge[0].w, complexProb, e[0].w);
            helperEdge[0].p = e[0].p;
        }

        //helperEdge[1] = 0.5*p*e[3]
        if (!e[3].w.approximatelyZero()) {
            helperEdge[1].w      = dd->cn.getCached();
            complexProb.r->value = probability * 0.5;
            CN::mul(helperEdge[1].w, complexProb, e[3].w);
            helperEdge[1].p = e[3].p;
        }

        //e[0] = helperEdge[0] + helperEdge[1]
        if (!e[0].w.exactlyZero()) {
            dd->cn.returnToCache(e[0].w);
        }
        e[0] = dd->add2(helperEdge[0], helperEdge[1]);

        if (!helperEdge[0].w.exactlyZero()) {
            dd->cn.returnToCache(helperEdge[0].w);
        }
        if (!helperEdge[1].w.exactlyZero()) {
            dd->cn.returnToCache(helperEdge[1].w);
        }
    }

    //e[1]=1-p*e[1]
    if (!e[1].w.approximatelyZero()) {
        complexProb.r->value = 1 - probability;
        CN::mul(e[1].w, e[1].w, complexProb);
    }
    //e[2]=1-p*e[2]
    if (!e[2].w.approximatelyZero()) {
        if (e[1].w.approximatelyZero()) {
            complexProb.r->value = 1 - probability;
        }
        CN::mul(e[2].w, e[2].w, complexProb);
    }

    //e[3] = 0.5*((2-p)*e[3]) + 0.5*(p*e[0])
    {
        helperEdge[0].w = dd::Complex::zero;
        helperEdge[1].w = dd::Complex::zero;

        //helperEdge[0] = 0.5*((2-p)*e[3])
        if (!e[3].w.approximatelyZero()) {
            helperEdge[0].w      = dd->cn.getCached();
            complexProb.r->value = (2 - probability) * 0.5;
            CN::mul(helperEdge[0].w, complexProb, e[3].w);
            helperEdge[0].p = e[3].p;
        }

        //helperEdge[1] = 0.5*p*e[0]
        if (!oldE0Edge.w.approximatelyZero()) {
            helperEdge[1].w      = dd->cn.getCached();
            complexProb.r->value = probability * 0.5;
            CN::mul(helperEdge[1].w, complexProb, oldE0Edge.w);
            helperEdge[1].p = oldE0Edge.p;
        }

        //e[3] = helperEdge[0] + helperEdge[1]
        if (!e[3].w.exactlyZero()) {
            dd->cn.returnToCache(e[3].w);
        }
        e[3] = dd->add2(helperEdge[0], helperEdge[1]);

        if (!helperEdge[0].w.exactlyZero()) {
            dd->cn.returnToCache(helperEdge[0].w);
        }
        if (!helperEdge[1].w.exactlyZero()) {
            dd->cn.returnToCache(helperEdge[1].w);
        }
    }
    dd->cn.returnToCache(oldE0Edge.w);
    dd->cn.returnToCache(complexProb);
}

std::map<std::string, double> DeterministicNoiseSimulator::AnalyseState(dd::QubitCount nQubits, bool fullState = false) {
    std::map<std::string, double> measuredResult = {};
    double                        p0, p1;
    double                        globalProbability;
    double                        statesToMeasure;

    dEdge::alignDensityEdge(&densityRootEdge);

    if (fullState) {
        statesToMeasure = std::pow(2, nQubits);
    } else {
        statesToMeasure = std::min((double)256, std::pow(2, nQubits));
    }

    for (int m = 0; m < statesToMeasure; m++) {
        int currentResult         = m;
        globalProbability         = dd::CTEntry::val(densityRootEdge.w.r);
        std::string  resultString = intToString(m, '1');
        qc::MatrixDD cur          = reinterpret_cast<const dd::mEdge&>(densityRootEdge);
        for (int i = 0; i < nQubits; ++i) {
            if (cur.p->v != -1) {
                assert(dd::CTEntry::approximatelyZero(cur.p->e.at(0).w.i) && dd::CTEntry::approximatelyZero(cur.p->e.at(3).w.i));
                p0 = dd::CTEntry::val(cur.p->e.at(0).w.r);
                p1 = dd::CTEntry::val(cur.p->e.at(3).w.r);
            } else {
                globalProbability = 0;
                break;
            }

            if (currentResult % 2 == 0) {
                cur = cur.p->e.at(0);
                globalProbability *= p0;
            } else {
                cur = cur.p->e.at(3);
                globalProbability *= p1;
            }
            currentResult = currentResult >> 1;
        }
        if (globalProbability > measurementThreshold) {
            measuredResult.insert({resultString, globalProbability});
        }
    }
    return measuredResult;
}

void DeterministicNoiseSimulator::generateGate(qc::MatrixDD* pointerForMatrices, dd::NoiseOperations noiseType, dd::Qubit target, double probability) {
    std::array<dd::GateMatrix, 4> idleNoiseGate{};
    dd::ComplexValue              tmp = {};

    switch (noiseType) {
        case dd::phaseFlip: {
            tmp.r               = std::sqrt(1 - probability) * dd::complex_one.r;
            idleNoiseGate[0][0] = idleNoiseGate[0][3] = tmp;
            idleNoiseGate[0][1] = idleNoiseGate[0][2] = dd::complex_zero;
            tmp.r                                     = std::sqrt(probability) * dd::complex_one.r;
            idleNoiseGate[1][0]                       = tmp;
            tmp.r *= -1;
            idleNoiseGate[1][3] = tmp;
            idleNoiseGate[1][1] = idleNoiseGate[1][2] = dd::complex_zero;

            pointerForMatrices[0] = dd->makeGateDD(idleNoiseGate[0], getNumberOfQubits(), target);
            pointerForMatrices[1] = dd->makeGateDD(idleNoiseGate[1], getNumberOfQubits(), target);

            break;
        }
            // amplitude damping
            //      (1      0           )       (0      sqrt(probability))
            //  e0= (0      sqrt(1-probability)   ), e1=  (0      0      )
        case dd::amplitudeDamping: {
            tmp.r               = std::sqrt(1 - probability) * dd::complex_one.r;
            idleNoiseGate[0][0] = dd::complex_one;
            idleNoiseGate[0][1] = idleNoiseGate[0][2] = dd::complex_zero;
            idleNoiseGate[0][3]                       = tmp;

            tmp.r               = std::sqrt(probability) * dd::complex_one.r;
            idleNoiseGate[1][0] = idleNoiseGate[1][3] = idleNoiseGate[1][2] = dd::complex_zero;
            idleNoiseGate[1][1]                                             = tmp;

            pointerForMatrices[0] = dd->makeGateDD(idleNoiseGate[0], getNumberOfQubits(), target);
            pointerForMatrices[1] = dd->makeGateDD(idleNoiseGate[1], getNumberOfQubits(), target);
            break;
        }
            // depolarization
        case dd::depolarization: {
            tmp.r = std::sqrt(1 - ((3 * probability) / 4)) * dd::complex_one.r;
            //                   (1 0)
            // sqrt(1- ((3p)/4))*(0 1)
            idleNoiseGate[0][0] = idleNoiseGate[0][3] = tmp;
            idleNoiseGate[0][1] = idleNoiseGate[0][2] = dd::complex_zero;

            pointerForMatrices[0] = dd->makeGateDD(idleNoiseGate[0], getNumberOfQubits(), target);

            //            (0 1)
            // sqrt(probability/4))*(1 0)
            tmp.r               = std::sqrt(probability / 4) * dd::complex_one.r;
            idleNoiseGate[1][1] = idleNoiseGate[1][2] = tmp;
            idleNoiseGate[1][0] = idleNoiseGate[1][3] = dd::complex_zero;

            pointerForMatrices[1] = dd->makeGateDD(idleNoiseGate[1], getNumberOfQubits(), target);

            //            (1 0)
            // sqrt(probability/4))*(0 -1)
            tmp.r               = std::sqrt(probability / 4) * dd::complex_one.r;
            idleNoiseGate[2][0] = tmp;
            tmp.r               = tmp.r * -1;
            idleNoiseGate[2][3] = tmp;
            idleNoiseGate[2][1] = idleNoiseGate[2][2] = dd::complex_zero;

            pointerForMatrices[3] = dd->makeGateDD(idleNoiseGate[2], getNumberOfQubits(), target);

            //            (0 -i)
            // sqrt(probability/4))*(i 0)
            tmp.r               = dd::complex_zero.r;
            tmp.i               = std::sqrt(probability / 4) * 1;
            idleNoiseGate[3][2] = tmp;
            tmp.i               = tmp.i * -1;
            idleNoiseGate[3][1] = tmp;
            idleNoiseGate[3][0] = idleNoiseGate[3][3] = dd::complex_zero;

            pointerForMatrices[2] = dd->makeGateDD(idleNoiseGate[3], getNumberOfQubits(), target);
            break;
        }
        default:
            throw std::runtime_error("Unknown noise effect received.");
    }
}

void DeterministicNoiseSimulator::applyDetNoiseSequential(const qc::Targets& targets) {
    dd::dEdge tmp = {};
    //    qc::MatrixDD ancillary_edge_1 = {};
    qc::MatrixDD idleOperation[4];

    // Iterate over qubits and check if the qubit had been used
    for (auto targetQubit: targets) {
        for (auto const& type: noiseEffects) {
            generateGate(idleOperation, type, targetQubit, getNoiseProbability(type, targets));
            tmp.p = nullptr;
            //Apply all noise matrices of the current noise effect
            for (int m = 0; m < sequentialNoiseMap.find(type)->second; m++) {
                auto tmp0 = dd->conjugateTranspose(idleOperation[m]);
                auto tmp1 = dd->multiply(densityRootEdge, reinterpret_cast<dEdge&>(tmp0), 0, false);
                auto tmp2 = dd->multiply(reinterpret_cast<dEdge&>(idleOperation[m]), tmp1, 0, useDensityMatrixType);
                if (tmp.p == nullptr) {
                    tmp = tmp2;
                } else {
                    tmp = dd->add(tmp2, tmp);
                }
            }
            dd->incRef(tmp);
            dEdge::alignDensityEdge(&densityRootEdge);
            dd->decRef(densityRootEdge);
            densityRootEdge = tmp;
            if (useDensityMatrixType) {
                dEdge::setDensityMatrixTrue(&densityRootEdge);
            }
        }
    }
}

double DeterministicNoiseSimulator::getNoiseProbability(const dd::NoiseOperations type, const qc::Targets& targets) {
    if (type == dd::amplitudeDamping) {
        return (targets.size() == 1) ? ampDampingProbSingleQubit : ampDampingProbMultiQubit;
    } else {
        return (targets.size() == 1) ? noiseProbSingleQubit : noiseProbMultiQubit;
    }
}

std::string DeterministicNoiseSimulator::intToString(long targetNumber, char value) const {
    if (targetNumber < 0) {
        assert(targetNumber == -1);
        return {"F"};
    }
    auto        qubits = getNumberOfQubits();
    std::string path(qubits, '0');
    auto        number = (unsigned long)targetNumber;
    for (int i = 1; i <= qubits; i++) {
        if (number % 2) {
            path[qubits - i] = value;
        }
        number = number >> 1u;
    }
    return path;
}
