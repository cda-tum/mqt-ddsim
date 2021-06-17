#include "UnitarySimulator.hpp"

#include <chrono>

void UnitarySimulator::Construct() {
    // carry out actual computation
    auto start = std::chrono::steady_clock::now();
    if (mode == Mode::Sequential) {
        e = qc->buildFunctionality(dd);
    } else if (mode == Mode::Recursive) {
        e = qc->buildFunctionalityRecursive(dd);
    }
    auto end         = std::chrono::steady_clock::now();
    constructionTime = std::chrono::duration<double>(end - start).count();
}
