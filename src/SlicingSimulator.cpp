#include "SlicingSimulator.hpp"

std::size_t SlicingSimulator::getNDecisions(dd::Qubit split_qubit) {
    std::size_t ndecisions = 0;
    // calculate number of decisions
    for (const auto& op: *qc) {
        if (dynamic_cast<qc::StandardOperation*>(op.get())) {
            bool target_in_lower_slice = false, target_in_upper_slice = false;
            bool control_in_lower_slice = false, control_in_upper_slice = false;
            for (const auto& target: op->getTargets()) {
                target_in_lower_slice = target_in_lower_slice || target < split_qubit;
                target_in_upper_slice = target_in_upper_slice || target >= split_qubit;
            }
            for (const auto& control: op->getControls()) {
                control_in_lower_slice = control_in_lower_slice || control.qubit < split_qubit;
                control_in_upper_slice = control_in_upper_slice || control.qubit >= split_qubit;
            }
            if ((target_in_lower_slice && control_in_upper_slice) ||
                (target_in_upper_slice && control_in_lower_slice)) {
                ndecisions++;
            }
        } else if (op->isCompoundOperation()) {
            throw std::invalid_argument("Compound operations currently not supported in hybrid Schrodinger-Feynman simulation.");
        } else if (op->isClassicControlledOperation()) {
            throw std::invalid_argument("Classic-controlled operations currently not supported in hybrid Schrodinger-Feynman simulation.");
        } else if (op->getType() == qc::Measure || op->getType() == qc::Reset) {
            throw std::invalid_argument("Non-unitary operations currently not supported in hybrid Schrodinger-Feynman simulation.");
        }
    }
    return ndecisions;
}
std::tuple<qc::VectorDD, qc::VectorDD, qc::VectorDD> SlicingSimulator::SimulateSlicing(std::unique_ptr<dd::Package>& slice_dd, dd::Qubit split_qubit, std::size_t controls) {
    Slice lower(slice_dd, 0, static_cast<dd::Qubit>(split_qubit - 1), controls);
    Slice upper(slice_dd, split_qubit, static_cast<dd::Qubit>(getNumberOfQubits() - 1), controls);

    for (const auto& op: *qc) {
        if (op->isUnitary()) {
            [[maybe_unused]] auto l = lower.apply(slice_dd, op);
            [[maybe_unused]] auto u = upper.apply(slice_dd, op);
            assert(l == u);
        }
        slice_dd->garbageCollect();
    }

    auto result = slice_dd->kronecker(upper.edge, lower.edge, false);
    slice_dd->incRef(result);

    return {lower.edge, upper.edge, result};
}
bool SlicingSimulator::Slice::apply(std::unique_ptr<dd::Package>& slice_dd, const std::unique_ptr<qc::Operation>& op) {
    bool is_split_op = false;
    if (reinterpret_cast<qc::StandardOperation*>(op.get())) { // TODO change control and target if wrong direction
        qc::Targets  op_targets{};
        dd::Controls op_controls{};

        // check targets
        bool target_in_split = false, target_in_other_split = false;
        for (const auto& target: op->getTargets()) {
            if (start <= target && target <= end) {
                op_targets.push_back(target);
                target_in_split = true;
            } else {
                target_in_other_split = true;
            }
        }

        if (target_in_split && target_in_other_split && !op->getControls().empty()) {
            throw std::invalid_argument("Multiple Targets that are in different slices are not supported at the moment");
        }

        // check controls
        for (const auto& control: op->getControls()) {
            if (start <= control.qubit && control.qubit <= end) {
                op_controls.emplace(dd::Control{control.qubit, control.type});
            } else { // other controls are set to the corresponding value
                if (target_in_split) {
                    is_split_op       = true;
                    bool next_control = getNextControl();
                    if ((control.type == dd::Control::Type::pos && !next_control) || // break if control is not activated
                        (control.type == dd::Control::Type::neg && next_control)) {
                        nDecisionsExecuted++;
                        return true;
                    }
                }
            }
        }

        if (target_in_other_split && !op_controls.empty()) { // control slice for split
            if (op_controls.size() > 1) {
                throw std::invalid_argument("Multiple controls in control slice of operation are not supported at the moment");
            }

            is_split_op  = true;
            bool control = getNextControl();
            for (const auto& c: op_controls) {
                slice_dd->decRef(edge); // TODO incref and decref could be integrated in delete edge
                edge = slice_dd->deleteEdge(edge, c.qubit, control ? (c.type == dd::Control::Type::pos ? 0 : 1) : (c.type == dd::Control::Type::pos ? 1 : 0));
                slice_dd->incRef(edge);
            }
        } else if (target_in_split) { // target slice for split or operation in split
            const auto&           param = op->getParameter();
            qc::StandardOperation new_op(nqubits, op_controls, op_targets, op->getType(), param[0], param[1], param[2], start);
            slice_dd->decRef(edge);
            edge = slice_dd->multiply(new_op.getDD(slice_dd), edge, start);
            slice_dd->incRef(edge);
        }
    } else {
        throw std::invalid_argument("Only StandardOperations are supported for now.");
    }
    if (is_split_op) {
        nDecisionsExecuted++;
    }
    return is_split_op;
}
