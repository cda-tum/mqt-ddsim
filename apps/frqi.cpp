/*
 * Based on code by Christoph Brandner, MedUni Wien
 */
#include "CircuitSimulator.hpp"
#include "cxxopts.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <opencv2/opencv.hpp>
#include <string>

using namespace dd::literals;

int main(int argc, char** argv) { // NOLINT(bugprone-exception-escape)
    cxxopts::Options options("FRQI", "with MQT DDSIM by https://www.cda.cit.tum.de/ -- Allowed options");
    // clang-format off
    options.add_options()
            ("h,help", "produce help message")
            ("shots", "number of measurements (if the algorithm does not contain non-unitary gates, weak simulation is used)", cxxopts::value<unsigned int>()->default_value("1000000"))
            ("verbose", "Causes some simulators to print additional information to STDERR")
            ("file", "simulate a quantum circuit given by file (detection by the file extension)", cxxopts::value<std::string>())
            ;
    // clang-format on
    auto vm = options.parse(argc, argv);
    if (vm.count("help") > 0) {
        std::cout << options.help();
        std::exit(0);
    }

    const unsigned int numOfShots = vm["shots"].as<unsigned int>();
    const std::string  filename   = vm["file"].as<std::string>();

    cv::Mat              image;
    cv::Mat              dest;
    const cv::Mat        genimg;
    const cv::Size       size(32, 32);
    const dd::QubitCount nqubits = 11;

    image = imread(filename, cv::IMREAD_GRAYSCALE);
    if (image.data == nullptr) {
        std::cerr << "Could not open or find the image '" << filename << "'" << std::endl;
        return -1;
    }
    cv::resize(image, dest, size, 0, 0, cv::INTER_AREA);

    dest.reshape(1, 1);
    dest.convertTo(dest, CV_64FC1);
    dest /= 255.;

    cv::namedWindow("original", cv::WINDOW_NORMAL);
    cv::resizeWindow("original", 128, 128);
    cv::imshow("original", dest);

    cv::MatIterator_<double> it;
    cv::MatIterator_<double> end;

    for (it = dest.begin<double>(), end = dest.end<double>(); it != end; ++it) {
        *it = std::asin(*it);
    }

    const auto t0start = std::chrono::steady_clock::now();
    /* Preparation of the quantum circuit */
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(nqubits);

    for (qc::Qubit i = 1; i < 11; i++) {
        qc->h(i); // add Hadamard gates
    }

    int cnt = 0; // start with the x/y location at 0000000000b
    for (it = dest.begin<double>(), end = dest.end<double>(); it != end; ++it) {
        if (*it != 0) {
            qc::Controls controls;
            for (int i = 0; i < 10; i++) {
                controls.insert({static_cast<qc::Qubit>(i + 1), (cnt & 1LL << i) != 0 ? qc::Control::Type::Pos : qc::Control::Type::Neg});
            }
            qc->ry(0, controls, (*it) * 2);
        }
        cnt++;
    }
    const auto t0end = std::chrono::steady_clock::now();

    if (vm.count("verbose") > 0) {
        qc->print(std::cout);
    }

    std::clog << "Elapsed time for quantum circuit preparation: " << std::chrono::duration<float>(t0end - t0start).count() << "s" << std::endl;

    const auto         t1start = std::chrono::steady_clock::now();
    CircuitSimulator<> ddsim(std::move(qc));

    const int cols    = dest.cols;
    const int rows    = dest.rows;
    const int colrows = cols * rows;

    const std::map<std::string, std::size_t> mCounter = ddsim.simulate(numOfShots);

    const auto t1end = std::chrono::steady_clock::now();

    std::clog << "Elapsed time for quantum simulation and measurement " << std::chrono::duration<float>(t1end - t1start).count() << " s" << std::endl;
    std::clog << "size of image: " << colrows << std::endl;

    unsigned int result = 0;
    for (const auto& [bitstring, count]: mCounter) {
        result += count;
    }
    std::clog << "Number of distinct measurements: " << mCounter.size() << "\n";
    std::clog << "RESULT: " << result << std::endl;

    std::vector<double> prob(1024, 0);

    for (const auto& [bitstring, count]: mCounter) {
        const std::size_t fixedIndex = std::stoul(bitstring.substr(0, 10), nullptr, 2);
        const double      tmp        = std::sqrt(static_cast<double>(count) / numOfShots) * 32. * 255.;
        if (vm.count("verbose") > 0) {
            std::clog << bitstring << " (" << fixedIndex << ") : " << count << " (" << tmp << ")\n";
        }
        prob.at(fixedIndex) = tmp;
    }

    cv::Mat final = cv::Mat(prob).reshape(0, 32);
    final.convertTo(final, CV_8UC1);

    cv::namedWindow("reconstructed", cv::WINDOW_NORMAL);
    cv::resizeWindow("reconstructed", 128, 128);
    cv::imshow("reconstructed", final);

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
