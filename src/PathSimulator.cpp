#include "PathSimulator.hpp"

#include <iterator>
#include <utility>

PathSimulator::SimulationPath::SimulationPath(std::size_t nleaves, PathSimulator::SimulationPath::Path path, const qc::QuantumComputation* qc, bool assumeCorrectOrder):
    path(std::move(path)), nleaves(nleaves), qc(qc) {
    steps.reserve(nleaves);
    // create empty vector of steps
    for (std::size_t id = 0; id < nleaves; ++id) {
        steps.emplace_back(id, std::vector{id});
    }

    for (auto& [leftID, rightID]: this->path) {
        if (leftID >= steps.size())
            throw std::runtime_error("Left contraction path index out of range.");
        if (rightID >= steps.size())
            throw std::runtime_error("Right contraction path index out of range");

        auto& leftStep  = steps.at(leftID);
        auto& rightStep = steps.at(rightID);

        const std::size_t resultID = steps.size();
        leftStep.parent            = resultID;
        rightStep.parent           = resultID;

        std::vector<std::size_t> operations{};
        if (!assumeCorrectOrder) {
            // move all operations from the left and the right to this result
            auto& leftOps  = leftStep.operations;
            auto& rightOps = rightStep.operations;

            // consider each operation from the left and check whether any operation from the right must precede it
            bool leftIsActuallyLeft = true;
            //        std::cout << "Checking whether ID " << leftID << " is actually left of ID " << rightID << std::endl;
            for (const auto& leftOpID: leftOps) {
                // if the initial state is included in the left, this has to be left
                if (leftOpID == 0) {
                    //                std::cout << "Left contains initial state, so it has to be left" << std::endl;
                    break;
                }
                const auto& leftOp = qc->at(leftOpID - 1);

                for (const auto& rightOpID: rightOps) {
                    // if the initial state is included in the right, the right side should be left
                    if (rightOpID == 0) {
                        //                    std::cout << "Right contains initial state, so it has to be left" << std::endl;
                        leftIsActuallyLeft = false;
                        break;
                    }

                    // in case the left ID is smaller than the right ID, everything is in order
                    if (leftOpID < rightOpID) {
                        //                    std::cout << "Operation IDs " << leftOpID << " and " << rightOpID << " are in order." << std::endl;
                        continue;
                    }

                    const auto& rightOp = qc->at(rightOpID - 1);

                    // operation on the right occurs before operation on the left in qc
                    // if they share any qubits, then right should actually be left
                    std::set<dd::Qubit> qubits{};
                    for (const auto& target: leftOp->getTargets())
                        qubits.emplace(target);
                    for (const auto& control: leftOp->getControls())
                        qubits.emplace(control.qubit);

                    for (const auto& qubit: qubits) {
                        if (rightOp->actsOn(qubit)) {
                            //                        std::cout << "Right operation ID " << rightOpID << " must precede left ID " << leftOpID << std::endl;
                            leftIsActuallyLeft = false;
                            break;
                        }
                    }
                    if (!leftIsActuallyLeft)
                        break;
                    //                std::cout << "Right operation ID " << rightOpID << " does not conflict with left ID " << leftOpID << std::endl;
                }
                if (!leftIsActuallyLeft)
                    break;
            }

            if (!leftIsActuallyLeft) {
                std::swap(leftID, rightID);
            }

            operations.reserve(leftOps.size() + rightOps.size());
            operations.insert(operations.end(), std::make_move_iterator(leftOps.begin()), std::make_move_iterator(leftOps.end()));
            leftOps = {};
            operations.insert(operations.end(), std::make_move_iterator(rightOps.begin()), std::make_move_iterator(rightOps.end()));
            rightOps = {};
        }
        //        std::cout << "Concluded that the right order is: [" << leftID << ", " << rightID << "] -> " << resultID << std::endl;
        steps.emplace_back(resultID, std::move(operations), Step::UNKNOWN, std::pair{leftID, rightID});
    }
}

std::map<std::string, std::size_t> PathSimulator::Simulate(unsigned int shots) {
    // build task graph from contraction plan
    constructTaskGraph();
    //std::cout<< *qc << std::endl;
    /// Enable the following statements to generate a .dot file of the resulting taskflow
    //        std::ofstream ofs("taskflow.dot");
    //        taskflow.dump(ofs);

    // perform simulation
    executor.run(taskflow).wait();

    // measure resulting DD
    return MeasureAllNonCollapsing(shots);
}

void PathSimulator::generateSequentialSimulationPath() {
    SimulationPath::Path path{};
    path.reserve(qc->getNops());

    for (std::size_t i = 0; i < qc->getNops(); ++i) {
        if (i == 0)
            path.emplace_back(0, 1);
        else
            path.emplace_back(qc->getNops() + i, i + 1);
    }
    setSimulationPath(path, true);
}

void PathSimulator::generateSequentialSimulationPathFalseOrder() {
    // Assumption of correct order is set to false
    SimulationPath::Path path{};
    path.reserve(qc->getNops());

    for (std::size_t i = 0; i < qc->getNops(); ++i) {
        if (i == 0)
            path.emplace_back(0, 1);
        else
            path.emplace_back(qc->getNops() + i, i + 1);
    }
    setSimulationPath(path, false);
}

void PathSimulator::generatePairwiseRecursiveGroupingSimulationPath() {
    SimulationPath::Path path{};
    path.reserve(qc->getNops());

    std::size_t nleaves = qc->getNops() + 1;
    auto        depth   = static_cast<std::size_t>(std::ceil(std::log2(nleaves)));

    std::size_t id     = nleaves;
    std::size_t offset = 0;

    std::size_t strayID                = 0;
    bool        strayElementLeft       = false;
    bool        eliminatedStrayElement = false;
    std::size_t elements               = nleaves;

    for (std::size_t l = 0; l < depth; ++l) {
        if (eliminatedStrayElement) {
            id++;
            elements++;
            eliminatedStrayElement = false;
        }

        for (std::size_t i = 0; i < elements - 1; i += 2) {
            path.emplace_back(offset + i, offset + i + 1);
        }

        if (elements % 2 == 1) {
            if (strayElementLeft) {
                path.emplace_back(offset + elements - 1, strayID);
                strayElementLeft       = false;
                eliminatedStrayElement = true;
            } else {
                strayElementLeft = true;
                strayID          = offset + elements - 1;
            }
        }

        offset = id;
        elements >>= 1;
        id += elements;
    }
    if (strayElementLeft) {
        path.emplace_back(offset, strayID);
    }
    setSimulationPath(path, true);
}

void PathSimulator::generateBracketSimulationPath(std::size_t bracketSize) {
    SimulationPath::Path path{};
    path.reserve(qc->getNops());
    bool        rightSingle      = false;
    std::size_t startElemBracket = bracketSize + 1;
    std::size_t strayElem        = 0;
    std::size_t memoryLeft       = 0;
    std::size_t bracketMemory    = 0;
    std::size_t opMemory         = 0;
    //std::cout << warmupDepth << std::endl;
    for (std::size_t i = 0; i < bracketSize; i++) {
        if (i == 0)
            path.emplace_back(0, 1);
        else {
            //std::cout << qc->getNops() + i << " " << 1+i << std::endl;
            path.emplace_back(qc->getNops() + i, 1 + i);
        }
    }
    memoryLeft = qc->getNops() + bracketSize;
    while (!rightSingle) {
        for (auto i = 0U; i < bracketSize - 1; i++) {
            if (startElemBracket == qc->getNops()) {
                rightSingle = true;
                strayElem   = startElemBracket;
                break;
            }
            if (i == 0) {
                //std::cout << startElemBracket << " " << startElemBracket+1 << std::endl;
                path.emplace_back(startElemBracket, startElemBracket + 1);
                opMemory++;
                if (startElemBracket + 1 == qc->getNops()) {
                    strayElem   = qc->getNops() + bracketSize + 1 + (bracketSize * bracketMemory - bracketMemory);
                    rightSingle = true;
                    break;
                }
            } else {
                //  std::cout << qc->getNops() + warmupDepth+i+(bracketSize*bracketMemory-bracketMemory) << " " << startElemBracket+1+i << std::endl;
                path.emplace_back(qc->getNops() + bracketSize + i + (bracketSize * bracketMemory - bracketMemory), startElemBracket + 1 + i);
                opMemory++;
                if (startElemBracket + 1 + i >= qc->getNops()) {
                    strayElem   = qc->getNops() + bracketSize + i + (bracketSize * bracketMemory - bracketMemory) + 1;
                    rightSingle = true;
                    break;
                }
            }
        }
        if (rightSingle) {
            break;
        }
        opMemory = 0;
        startElemBracket += bracketSize;
        bracketMemory++;
    }
    for (auto i = 0U; i < bracketMemory; i++) {
        if (i == 0) {
            //std::cout << memoryLeft << " " << memoryLeft+(bracketSize-1) << std::endl;
            path.emplace_back(memoryLeft, memoryLeft + (bracketSize - 1));
        } else {
            //std::cout << memoryLeft + (bracketSize-1)*bracketMemory+opMemory +i << " " << memoryLeft+(bracketSize-1)*(i+1) << std::endl;
            path.emplace_back(memoryLeft + (bracketSize - 1) * bracketMemory + opMemory + i, memoryLeft + (bracketSize - 1) * (i + 1));
        }
    }
    if (rightSingle) {
        //std::cout << memoryLeft + (bracketSize)*bracketMemory+opMemory << " " << strayElem << std::endl;
        path.emplace_back(memoryLeft + (bracketSize)*bracketMemory + opMemory, strayElem);
    }
    setSimulationPath(path, true);
}

void PathSimulator::constructTaskGraph() {
    const auto& path  = simulationPath.path;
    const auto& steps = simulationPath.steps;

    if (path.empty())
        return;

    const std::size_t nleaves = qc->getNops() + 1;

    for (std::size_t i = 0; i < path.size(); ++i) {
        const auto [leftID, rightID] = path.at(i);
        const auto& resultStep       = steps.at(nleaves + i);

        // basic construction steps of matrices and initial state
        if (leftID < nleaves) {
            if (leftID == 0) {
                // initial state
                qc::VectorDD zeroState = dd->makeZeroState(qc->getNqubits());
                dd->incRef(zeroState);
                results.emplace(leftID, zeroState);
            } else {
                const auto&  op   = qc->at(leftID - 1);
                qc::MatrixDD opDD = op->getDD(dd);
                dd->incRef(opDD);
                results.emplace(leftID, opDD);
            }
        }

        if (rightID < nleaves) {
            if (rightID == 0) {
                throw std::runtime_error("Initial state must not appear on right side of contraction step.");
            } else {
                const auto&  op   = qc->at(rightID - 1);
                qc::MatrixDD opDD = op->getDD(dd);
                dd->incRef(opDD);
                results.emplace(rightID, opDD);
            }
        }

        // add MxV / MxM task
        addContractionTask(leftID, rightID, resultStep.id);

        // create dependencies
        if (leftID >= nleaves) {
            auto& leftTask   = tasks.at(leftID);
            auto& resultTask = tasks.at(resultStep.id);
            leftTask.precede(resultTask);
        }

        if (rightID >= nleaves) {
            auto& rightTask  = tasks.at(rightID);
            auto& resultTask = tasks.at(resultStep.id);
            rightTask.precede(resultTask);
        }

        // add final task for storing the result
        if (i == path.size() - 1) {
            const auto runner = [this, resultStep]() {
                if (auto res = std::get_if<qc::VectorDD>(&results.at(resultStep.id))) {
                    root_edge = *res;
                } else {
                    throw std::runtime_error("Expected vector DD as result.");
                }
            };
            auto storeResultTask = taskflow.emplace(runner).name("store result");
            auto preceedingTask  = tasks.at(resultStep.id);
            preceedingTask.precede(storeResultTask);
        }
    }
}

void PathSimulator::addContractionTask(std::size_t leftID, std::size_t rightID, std::size_t resultID) {
    const auto runner = [this, leftID, rightID, resultID]() {
        /// Enable the following statement for printing execution order
        //            std::cout << "Executing " << leftID << " " << rightID << " -> " << resultID << std::endl;
        const auto& leftDD  = results.at(leftID);
        const auto& rightDD = results.at(rightID);

        const auto leftIsVector  = std::holds_alternative<qc::VectorDD>(leftDD);
        const auto rightIsVector = std::holds_alternative<qc::VectorDD>(rightDD);

        if (rightIsVector) {
            throw std::runtime_error("Right element in contraction is a vector. This should not happen!");
        }

        if (leftIsVector) {
            // matrix-vector multiplication
            const auto& vector   = *std::get_if<qc::VectorDD>(&leftDD);
            const auto& matrix   = *std::get_if<qc::MatrixDD>(&rightDD);
            auto        resultDD = dd->multiply(matrix, vector);
            dd->incRef(resultDD);
            dd->decRef(vector);
            dd->decRef(matrix);
            results.emplace(resultID, resultDD);
        } else {
            // matrix-matrix multiplication
            const auto& leftMatrix  = *std::get_if<qc::MatrixDD>(&leftDD);
            const auto& rightMatrix = *std::get_if<qc::MatrixDD>(&rightDD);
            auto        resultDD    = dd->multiply(rightMatrix, leftMatrix);
            dd->incRef(resultDD);
            dd->decRef(leftMatrix);
            dd->decRef(rightMatrix);
            results.emplace(resultID, resultDD);
        }
        dd->garbageCollect();
        results.erase(leftID);
        results.erase(rightID);
    };

    const auto resultTask = taskflow.emplace(runner).name(std::to_string(resultID));
    tasks.emplace(resultID, resultTask);
}
