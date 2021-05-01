#include <boost/program_options.hpp>
#include <cmath>
#include <iostream>
#include <string>

static unsigned long long gcd(unsigned long long a, unsigned long long b) {
    unsigned long long c;
    while (a != 0) {
        c = a;
        a = b % a;
        b = c;
    }
    return b;
}

static bool is_prime(unsigned int number) {
    const unsigned int upper_limit = std::floor(std::sqrt(number));

    for (unsigned int a = 2; a <= upper_limit; a++) {
        if (number % a == 0) {
            return false;
        }
    }
    return true;
}

void output_coprimes(const unsigned int composite_number, const unsigned int length) {
    unsigned output_length  = 0;
    unsigned current_number = 2;

    while (output_length < length) {
        if (gcd(current_number, composite_number) == 1) {
            std::cout << current_number << "\n";
            output_length++;
        }
        current_number++;
    }
}

void output_primes(const unsigned int composite_number, const unsigned int length) {
    unsigned output_length  = 0;
    unsigned current_number = 2;

    while (output_length < length) {
        if (is_prime(current_number) && gcd(current_number, composite_number) == 1) {
            std::cout << current_number << "\n";
            output_length++;
        }
        current_number++;
    }
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;
    po::options_description description("JKQ DDSIM by https://iic.jku.at/eda/ -- Allowed options");
    description.add_options()("help,h", "produce help message")("composite_number,N", po::value<unsigned int>()->required(),
                                                                "number of measurements on the final quantum state")("strategy,S", po::value<std::string>()->required(),
                                                                                                                     "strategy for prime base generation (primes, coprimes)")("length,L", po::value<unsigned int>()->required(), "how many bases to generate");
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, description), vm);
        if (vm.count("help")) {
            std::cout << description;
            return 0;
        }
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "[ERROR] " << e.what() << "! Try option '--help' for available commandline options.\n";
        std::exit(1);
    }

    if (vm["strategy"].as<std::string>() == "coprimes") {
        output_coprimes(vm["composite_number"].as<unsigned int>(), vm["length"].as<unsigned int>());
    } else if (vm["strategy"].as<std::string>() == "primes") {
        output_primes(vm["composite_number"].as<unsigned int>(), vm["length"].as<unsigned int>());
    } else {
        std::cerr << "Invalid stragety.\n";
    }
}