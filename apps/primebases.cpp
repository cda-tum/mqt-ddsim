#include "cxxopts.hpp"

#include <cmath>
#include <iostream>
#include <string>

static std::uint64_t gcd(std::uint64_t a, std::uint64_t b) {
    while (a != 0) {
        const std::uint64_t c = a;
        a                     = b % a;
        b                     = c;
    }
    return b;
}

static bool isPrime(std::uint64_t number) {
    const auto upperLimit = static_cast<std::uint64_t>(std::floor(std::sqrt(number)));

    for (std::uint64_t a = 2; a <= upperLimit; a++) {
        if (number % a == 0) {
            return false;
        }
    }
    return true;
}

void outputCoprimes(const std::uint64_t compositeNumber, const std::uint64_t length) {
    std::uint64_t outputLength  = 0;
    std::uint64_t currentNumber = 2;

    while (outputLength < length) {
        if (gcd(currentNumber, compositeNumber) == 1) {
            std::cout << currentNumber << "\n";
            outputLength++;
        }
        currentNumber++;
    }
}

void outputPrimes(const std::uint64_t compositeNumber, const std::uint64_t length) {
    std::uint64_t outputLength  = 0;
    std::uint64_t currentNumber = 2;

    while (outputLength < length) {
        if (isPrime(currentNumber) && gcd(currentNumber, compositeNumber) == 1) {
            std::cout << currentNumber << "\n";
            outputLength++;
        }
        currentNumber++;
    }
}

int main(int argc, char** argv) { // NOLINT(bugprone-exception-escape)
    cxxopts::Options options("MQT DDSIM", "see for more information https://www.cda.cit.tum.de/");
    // clang-format off
    options.add_options()
        ("h,help", "produce help message")
        ("N,composite_number", "number of measurements on the final quantum state", cxxopts::value<unsigned int>())
        ("S,strategy", "strategy for prime base generation (primes, coprimes)", cxxopts::value<std::string>())
        ("L,length", "how many bases to generate", cxxopts::value<unsigned int>());
    // clang-format on

    auto vm = options.parse(argc, argv);
    if (vm.count("help") > 0) {
        std::cout << options.help();
        std::exit(0);
    }

    if (vm["strategy"].as<std::string>() == "coprimes") {
        outputCoprimes(vm["composite_number"].as<unsigned int>(), vm["length"].as<unsigned int>());
    } else if (vm["strategy"].as<std::string>() == "primes") {
        outputPrimes(vm["composite_number"].as<unsigned int>(), vm["length"].as<unsigned int>());
    } else {
        std::cerr << "Invalid strategy.\n";
    }
}
