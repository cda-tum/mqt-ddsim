#include "PathSimulator.hpp"

#include <iterator>
#include <utility>

template<class Config>
PathSimulator<Config>::SimulationPath::SimulationPath(std::size_t nleaves_, PathSimulator::SimulationPath::Components components_, const qc::QuantumComputation* qc_, bool assumeCorrectOrder):
    components(std::move(components_)), nleaves(nleaves_), qc(qc_) {
    steps.reserve(nleaves_);
    // create empty vector of steps
    for (std::size_t id = 0; id < nleaves; ++id) {
        steps.emplace_back(id, std::vector{id});
    }

    for (auto& [leftID, rightID]: components) {
        if (leftID >= steps.size()) {
            throw std::runtime_error("Left simulation path index out of range.");
        }
        if (rightID >= steps.size()) {
            throw std::runtime_error("Right simulation path index out of range");
        }

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
                    std::set<qc::Qubit> qubits{};
                    for (const auto& target: leftOp->getTargets()) {
                        qubits.emplace(target);
                    }
                    for (const auto& control: leftOp->getControls()) {
                        qubits.emplace(control.qubit);
                    }

                    for (const auto& qubit: qubits) {
                        if (rightOp->actsOn(qubit)) {
                            //                        std::cout << "Right operation ID " << rightOpID << " must precede left ID " << leftOpID << std::endl;
                            leftIsActuallyLeft = false;
                            break;
                        }
                    }
                    if (!leftIsActuallyLeft) {
                        break;
                    }
                    //                std::cout << "Right operation ID " << rightOpID << " does not conflict with left ID " << leftOpID << std::endl;
                }
                if (!leftIsActuallyLeft) {
                    break;
                }
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

template<class Config>
std::map<std::string, std::size_t> PathSimulator<Config>::simulate(std::size_t shots) {
    // build task graph from simulation path
    constructTaskGraph();
    //std::cout<< *qc << std::endl;
    /// Enable the following statements to generate a .dot file of the resulting taskflow
    //        std::ofstream ofs("taskflow.dot");
    //        taskflow.dump(ofs);

    // perform simulation
    executor.run(taskflow).wait();

    // measure resulting DD
    return Simulator<Config>::measureAllNonCollapsing(shots);
}

template<class Config>
void PathSimulator<Config>::generateSequentialSimulationPath() {
    typename SimulationPath::Components components{};
    components.reserve(CircuitSimulator<Config>::qc->getNops());

    for (std::size_t i = 0; i < CircuitSimulator<Config>::qc->getNops(); ++i) {
        if (i == 0) {
            components.emplace_back(0, 1);
        } else {
            components.emplace_back(CircuitSimulator<Config>::qc->getNops() + i, i + 1);
        }
    }
    setSimulationPath(components, true);
}

template<class Config>
void PathSimulator<Config>::generatePairwiseRecursiveGroupingSimulationPath() {
    typename SimulationPath::Components components{};
    components.reserve(CircuitSimulator<Config>::qc->getNops());

    const std::size_t nleaves = CircuitSimulator<Config>::qc->getNops() + 1;
    auto              depth   = static_cast<std::size_t>(std::ceil(std::log2(nleaves)));

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
        // Pairwise adding elements
        for (std::size_t i = 0; i < elements - 1; i += 2) {
            components.emplace_back(offset + i, offset + i + 1);
        }
        // Checking if the number of elements is even
        if (elements % 2 == 1) {
            // Adding the stray element
            if (strayElementLeft) {
                components.emplace_back(offset + elements - 1, strayID);
                strayElementLeft       = false;
                eliminatedStrayElement = true;
            }
            // Setting the stray element
            else {
                strayElementLeft = true;
                strayID          = offset + elements - 1;
            }
        }

        offset = id;
        elements >>= 1;
        id += elements;
    }
    // Adding the remaining element
    if (strayElementLeft) {
        components.emplace_back(offset, strayID);
    }
    setSimulationPath(components, true);
}

template<class Config>
void PathSimulator<Config>::generateBracketSimulationPath(std::size_t bracketSize) {
    typename SimulationPath::Components components{};
    components.reserve(CircuitSimulator<Config>::qc->getNops());
    bool        rightSingle      = false;
    std::size_t startElemBracket = bracketSize + 1;
    std::size_t strayElem        = 0;
    std::size_t memoryLeft       = 0;
    std::size_t bracketMemory    = 0;
    std::size_t opMemory         = 0;
    // Sequentially adding tasks for the first bracket
    for (std::size_t i = 0; i < bracketSize; i++) {
        if (i == 0) {
            components.emplace_back(0, 1);
        } else {
            components.emplace_back(CircuitSimulator<Config>::qc->getNops() + i, 1 + i);
        }
    }
    memoryLeft = CircuitSimulator<Config>::qc->getNops() + bracketSize;
    //Creating the brackets by sequentially adding the individual operations
    while (!rightSingle) {
        for (auto i = 0U; i < bracketSize - 1; i++) {
            //Checking for stray elements
            if (startElemBracket == CircuitSimulator<Config>::qc->getNops()) {
                rightSingle = true;
                strayElem   = startElemBracket;
                break;
            }
            //Connecting operations sequentially
            if (i == 0) {
                components.emplace_back(startElemBracket, startElemBracket + 1);
                opMemory++;
                if (startElemBracket + 1 == CircuitSimulator<Config>::qc->getNops()) {
                    strayElem   = CircuitSimulator<Config>::qc->getNops() + bracketSize + 1 + (bracketSize * bracketMemory - bracketMemory);
                    rightSingle = true;
                    break;
                }
            } else {
                components.emplace_back(CircuitSimulator<Config>::qc->getNops() + bracketSize + i + (bracketSize * bracketMemory - bracketMemory), startElemBracket + 1 + i);
                opMemory++;
                if (startElemBracket + 1 + i >= CircuitSimulator<Config>::qc->getNops()) {
                    strayElem   = CircuitSimulator<Config>::qc->getNops() + bracketSize + i + (bracketSize * bracketMemory - bracketMemory) + 1;
                    rightSingle = true;
                    break;
                }
            }
        }
        if (rightSingle) {
            break;
        }
        // Counting the number of brackets and moving the start element for the next bracket forward
        opMemory = 0;
        startElemBracket += bracketSize;
        bracketMemory++;
    }
    //Adding the individual brackets in sequential order
    for (auto i = 0U; i < bracketMemory; i++) {
        if (i == 0) {
            components.emplace_back(memoryLeft, memoryLeft + (bracketSize - 1));
        } else {
            components.emplace_back(memoryLeft + (bracketSize - 1) * bracketMemory + opMemory + i, memoryLeft + (bracketSize - 1) * (i + 1));
        }
    }
    //Adding the last stray element on the right-hand side
    if (rightSingle) {
        components.emplace_back(memoryLeft + (bracketSize)*bracketMemory + opMemory, strayElem);
    }
    setSimulationPath(components, true);
}

template<class Config>
void PathSimulator<Config>::generateAlternatingSimulationPath(std::size_t startingPoint) {
    typename SimulationPath::Components components{};
    components.reserve(CircuitSimulator<Config>::qc->getNops());
    const std::size_t startElem = startingPoint;
    components.emplace_back(startElem, startElem + 1);
    std::size_t       leftID   = startElem - 1;
    const std::size_t leftEnd  = 0;
    std::size_t       rightID  = startElem + 2;
    const std::size_t rightEnd = CircuitSimulator<Config>::qc->getNops() + 1;
    std::size_t       nextID   = rightEnd;
    //Alternating between left and right-hand side
    while (leftID != leftEnd && rightID != rightEnd) {
        components.emplace_back(leftID, nextID);
        nextID++;
        components.emplace_back(nextID, rightID);
        nextID++;
        leftID--;
        rightID++;
    }

    //Finish the left-hand side
    while (leftID != leftEnd) {
        components.emplace_back(leftID, nextID);
        nextID++;
        leftID--;
    }

    //Finish the right-hand side
    while (rightID != rightEnd) {
        components.emplace_back(nextID, rightID);
        nextID++;
        rightID++;
    }

    //Add the remaining matrix-vector multiplication
    components.emplace_back(0, nextID);
    setSimulationPath(components, true);
}

template<class Config>
void PathSimulator<Config>::generateGatecostSimulationPath(const std::size_t startingPoint, std::list<std::size_t>& gateCosts) {
    typename SimulationPath::Components components{};
    components.reserve(CircuitSimulator<Config>::qc->getNops());

    const std::size_t leftEnd  = 0;
    const std::size_t rightEnd = CircuitSimulator<Config>::qc->getNops() + 1;

    components.emplace_back(startingPoint, startingPoint + 1);

    std::size_t leftID  = startingPoint - 1;
    std::size_t rightID = startingPoint + 2;
    std::size_t nextID  = rightEnd;

    if (leftID != leftEnd && rightID != rightEnd) {
        for (auto i = 0U; i < gateCosts.front() - 1; ++i) {
            components.emplace_back(nextID, rightID);
            ++nextID;
            ++rightID;
        }
        gateCosts.pop_front();
    }

    while (leftID != leftEnd && rightID != rightEnd) {
        components.emplace_back(leftID, nextID);
        ++nextID;
        --leftID;
        for (auto i = 0U; i < gateCosts.front(); ++i) {
            components.emplace_back(nextID, rightID);
            ++nextID;
            ++rightID;
            if (rightID == rightEnd) {
                break;
            }
        }
        gateCosts.pop_front();
        if (rightID == rightEnd) {
            break;
        }
    }

    //Finish the left-hand side
    while (leftID != leftEnd) {
        components.emplace_back(leftID, nextID);
        ++nextID;
        --leftID;
    }

    //Finish the right-hand side
    while (rightID != rightEnd) {
        components.emplace_back(nextID, rightID);
        ++nextID;
        ++rightID;
    }

    //Add the remaining matrix-vector multiplication
    components.emplace_back(0, nextID);
    setSimulationPath(components, true);
}

template<class Config>
void PathSimulator<Config>::constructTaskGraph() {
    const auto& path  = simulationPath.components;
    const auto& steps = simulationPath.steps;

    if (path.empty()) {
        Simulator<Config>::rootEdge = Simulator<Config>::dd->makeZeroState(static_cast<dd::Qubit>(CircuitSimulator<Config>::qc->getNqubits()));
        return;
    }

    const std::size_t nleaves = CircuitSimulator<Config>::qc->getNops() + 1;

    for (std::size_t i = 0; i < path.size(); ++i) {
        const auto [leftID, rightID] = path.at(i);
        const auto& resultStep       = steps.at(nleaves + i);

        // basic construction steps of matrices and initial state
        if (leftID < nleaves) {
            if (leftID == 0) {
                // initial state
                qc::VectorDD zeroState = Simulator<Config>::dd->makeZeroState(static_cast<dd::Qubit>(CircuitSimulator<Config>::qc->getNqubits()));
                Simulator<Config>::dd->incRef(zeroState);
                results.emplace(leftID, zeroState);
            } else {
                const auto&  op   = CircuitSimulator<Config>::qc->at(leftID - 1);
                qc::MatrixDD opDD = dd::getDD(op.get(), Simulator<Config>::dd);
                Simulator<Config>::dd->incRef(opDD);
                results.emplace(leftID, opDD);
            }
        }

        if (rightID < nleaves) {
            if (rightID == 0) {
                throw std::runtime_error("Initial state must not appear on right side of the simulation path member.");
            }
            const auto&  op   = CircuitSimulator<Config>::qc->at(rightID - 1);
            qc::MatrixDD opDD = dd::getDD(op.get(), Simulator<Config>::dd);
            Simulator<Config>::dd->incRef(opDD);
            results.emplace(rightID, opDD);
        }

        // add MxV / MxM task
        addSimulationTask(leftID, rightID, resultStep.id);

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
                    Simulator<Config>::rootEdge = *res;
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

template<class Config>
void PathSimulator<Config>::addSimulationTask(std::size_t leftID, std::size_t rightID, std::size_t resultID) {
    const auto runner = [this, leftID, rightID, resultID]() {
        /// Enable the following statement for printing execution order
        //            std::cout << "Executing " << leftID << " " << rightID << " -> " << resultID << std::endl;
        const auto& leftDD  = results.at(leftID);
        const auto& rightDD = results.at(rightID);

        const auto leftIsVector  = std::holds_alternative<qc::VectorDD>(leftDD);
        const auto rightIsVector = std::holds_alternative<qc::VectorDD>(rightDD);

        if (rightIsVector) {
            throw std::runtime_error("Right element in this simulation path member is a vector. This should not happen!");
        }

        if (leftIsVector) {
            // matrix-vector multiplication
            const auto& vector   = *std::get_if<qc::VectorDD>(&leftDD);
            const auto& matrix   = *std::get_if<qc::MatrixDD>(&rightDD);
            auto        resultDD = Simulator<Config>::dd->multiply(matrix, vector);
            Simulator<Config>::dd->incRef(resultDD);
            Simulator<Config>::dd->decRef(vector);
            Simulator<Config>::dd->decRef(matrix);
            results.emplace(resultID, resultDD);
        } else {
            // matrix-matrix multiplication
            const auto& leftMatrix  = *std::get_if<qc::MatrixDD>(&leftDD);
            const auto& rightMatrix = *std::get_if<qc::MatrixDD>(&rightDD);
            auto        resultDD    = Simulator<Config>::dd->multiply(rightMatrix, leftMatrix);
            Simulator<Config>::dd->incRef(resultDD);
            Simulator<Config>::dd->decRef(leftMatrix);
            Simulator<Config>::dd->decRef(rightMatrix);
            results.emplace(resultID, resultDD);
        }
        Simulator<Config>::dd->garbageCollect();
        results.erase(leftID);
        results.erase(rightID);
    };

    const auto resultTask = taskflow.emplace(runner).name(std::to_string(resultID));
    tasks.emplace(resultID, resultTask);
}

template class PathSimulator<dd::DDPackageConfig>;
