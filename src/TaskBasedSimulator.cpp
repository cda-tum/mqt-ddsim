//
// Created by Lukas Burgholzer on 09.08.21.
//

#include "TaskBasedSimulator.hpp"

TaskBasedSimulator::ContractionPlan::ContractionPlan(std::size_t nleaves, const TaskBasedSimulator::ContractionPlan::Path& path):
    path(path), nleaves(nleaves) {
    steps.reserve(nleaves);
    // create empty vector of steps
    for (std::size_t id = 0; id < nleaves; ++id) {
        steps.emplace_back(id);
    }

    for (const auto& [leftID, rightID]: path) {
        if (leftID >= steps.size())
            throw std::runtime_error("Left contraction path index out of range.");
        if (rightID >= steps.size())
            throw std::runtime_error("Right contraction path index out of range");

        auto& leftStep  = steps.at(leftID);
        auto& rightStep = steps.at(rightID);

        const std::size_t resultID = steps.size();
        leftStep.parent            = resultID;
        rightStep.parent           = resultID;

        steps.emplace_back(resultID, Step::UNKNOWN, std::pair{leftID, rightID});
    }
}

std::map<std::string, std::size_t> TaskBasedSimulator::Simulate(unsigned int shots) {
    // build task graph from contraction plan
    constructTaskGraph();

    /// Enable the following statements to generate a .dot file of the resulting taskflow
    //        std::ofstream ofs("taskflow.dot");
    //        taskflow.dump(ofs);

    // perform simulation
    executor.run(taskflow).wait();

    // measure resulting DD
    return MeasureAllNonCollapsing(shots);
}

void TaskBasedSimulator::generateSequentialContractionPlan() {
    ContractionPlan::Path path{};
    path.reserve(qc->getNops());

    for (std::size_t i = 0; i < qc->getNops(); ++i) {
        if (i == 0)
            path.emplace_back(0, 1);
        else
            path.emplace_back(qc->getNops() + i, i + 1);
    }
    setContractionPlan(path);
}

void TaskBasedSimulator::generatePairwiseRecursiveGroupingContractionPlan() {
    ContractionPlan::Path path{};
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
    setContractionPlan(path);
}

void TaskBasedSimulator::constructTaskGraph() {
    const auto& path  = contractionPlan.path;
    const auto& steps = contractionPlan.steps;

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
                root_edge = std::get<qc::VectorDD>(results.at(resultStep.id));
            };
            auto storeResultTask = taskflow.emplace(runner).name("store result");
            auto preceedingTask  = tasks.at(resultStep.id);
            preceedingTask.precede(storeResultTask);
        }
    }
}

void TaskBasedSimulator::addContractionTask(std::size_t leftID, std::size_t rightID, std::size_t resultID) {
    const auto runner = [this, leftID, rightID, resultID]() {
        /// Enable the following statement for printing execution order
        //            std::cout << "Executing " << leftID << " " << rightID << " -> " << resultID << std::endl;
        const auto& leftDD  = results.at(leftID);
        const auto& rightDD = results.at(rightID);
        if (auto vector = std::get_if<qc::VectorDD>(&leftDD)) {
            auto& matrix   = std::get<qc::MatrixDD>(rightDD);
            auto  resultDD = dd->multiply(matrix, *vector);
            dd->incRef(resultDD);
            dd->decRef(*vector);
            dd->decRef(matrix);
            results.emplace(resultID, resultDD);
        } else {
            const auto& leftMatrix  = std::get<qc::MatrixDD>(leftDD);
            const auto& rightMatrix = std::get<qc::MatrixDD>(rightDD);
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
