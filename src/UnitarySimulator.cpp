#include "UnitarySimulator.hpp"

#include "dd/FunctionalityConstruction.hpp"

#include <chrono>

template<class DDPackage>
void UnitarySimulator<DDPackage>::Construct() {
    // carry out actual computation
    auto start = std::chrono::steady_clock::now();
    if (mode == Mode::Sequential) {
        e = dd::buildFunctionality(CircuitSimulator<DDPackage>::qc.get(), Simulator<DDPackage>::dd);
    } else if (mode == Mode::Recursive) {
        e = dd::buildFunctionalityRecursive(CircuitSimulator<DDPackage>::qc.get(), Simulator<DDPackage>::dd);
    }
    auto end         = std::chrono::steady_clock::now();
    constructionTime = std::chrono::duration<double>(end - start).count();
}

template class UnitarySimulator<dd::Package<>>;
