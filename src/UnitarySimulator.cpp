#include "UnitarySimulator.hpp"

void UnitarySimulator::Construct() {
    // carry out actual computation
    auto start_construction = std::chrono::high_resolution_clock::now();
    if (mode == Mode::Sequential) {
        e = qc->buildFunctionality(dd);
    } else if (mode == Mode::Recursive) {
        e = qc->buildFunctionalityRecursive(dd);
    }
    auto end_construction = std::chrono::high_resolution_clock::now();
    constructionTime      = std::chrono::duration<double>(end_construction - start_construction).count();
}
