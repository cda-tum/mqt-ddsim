#include <complex>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " file1 file2\n"
                  << "Calculates the fidelity between two vectors given as file (one complex number per line).\n"
                  << "Return code is 0 if fidelity is greater than 0.999 and 1 otherwise. 2 for errors.\n";
        return 2;
    }
    std::string vector1{argv[1]};
    std::string vector2{argv[2]};

    std::cout << "Vector 1 read from '" << argv[1] << "'\n";
    std::cout << "Vector 2 read from '" << argv[2] << "'\n";

    std::ifstream v1_stream(vector1);
    std::ifstream v2_stream(vector2);

    if (!v1_stream || !v2_stream) {
        std::cerr << "Failed to open either or both files.";
        return 2;
    }

    unsigned                  lines = 0;
    std::complex<long double> sum{};
    while (!v1_stream.eof() && !v2_stream.eof()) {
        std::string ars, ais, brs, bis;
        v1_stream >> ars >> ais;
        v2_stream >> brs >> bis;

        if (!v1_stream || !v2_stream) {
            break;
        }
        // istreams cannot read doubles in hexfloat format so we have to use strtold
        long double ar = std::strtold(ars.c_str(), nullptr);
        long double ai = std::strtold(ais.c_str(), nullptr);
        long double br = std::strtold(brs.c_str(), nullptr);
        long double bi = std::strtold(bis.c_str(), nullptr);

        std::complex<long double> a(ar, ai);
        std::complex<long double> b(br, bi);
        sum += a * std::conj(b);

        lines++;
    }
    long double fidelity = std::norm(sum);

    std::cout << "Lines: " << lines << " (Should be a power of two)\n";
    std::cout << "Fidelity: " << fidelity << "\n";
    if (std::abs(1.0l - fidelity) < 0.001) {
        return 0;
    } else {
        return 1;
    }
}