#include <complex>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " file1 file2\n"
                  << "Calculates the fidelity between two vectors given as file (one complex number per line).\n"
                  << "Return code is 0 if fidelity is greater than 0.999 and 1 otherwise. 2 for errors.\n";
        return 2;
    }
    const std::string vector1{argv[1]};
    const std::string vector2{argv[2]};

    std::cout << "Vector 1 read from '" << argv[1] << "'\n";
    std::cout << "Vector 2 read from '" << argv[2] << "'\n";
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    std::ifstream v1Stream(vector1);
    std::ifstream v2Stream(vector2);

    if (!v1Stream || !v2Stream) {
        std::cerr << "Failed to open either or both files.";
        return 2;
    }

    unsigned                  lines = 0;
    std::complex<long double> sum{};
    while (!v1Stream.eof() && !v2Stream.eof()) {
        std::string ars;
        std::string ais;
        std::string brs;
        std::string bis;
        v1Stream >> ars >> ais;
        v2Stream >> brs >> bis;

        if (!v1Stream || !v2Stream) {
            break;
        }
        // istreams cannot read doubles in hexfloat format, so we have to use strtold
        const long double ar = std::strtold(ars.c_str(), nullptr);
        const long double ai = std::strtold(ais.c_str(), nullptr);
        const long double br = std::strtold(brs.c_str(), nullptr);
        const long double bi = std::strtold(bis.c_str(), nullptr);

        const std::complex<long double> a(ar, ai);
        const std::complex<long double> b(br, bi);
        sum += a * std::conj(b);

        lines++;
    }
    const long double fidelity = std::norm(sum);

    std::cout << "Lines: " << lines << " (Should be a power of two)\n";
    std::cout << "Fidelity: " << fidelity << "\n";
    if (std::abs(1.0L - fidelity) < 0.001L) {
        return 0;
    }
    return 1;
}
