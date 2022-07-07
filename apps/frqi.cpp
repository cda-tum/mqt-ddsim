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

int main(int argc, char** argv) {
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
    if (vm.count("help")) {
        std::cout << options.help();
        std::exit(0);
    }

    unsigned int numOfShots = vm["shots"].as<unsigned int>();
    std::string  filename   = vm["file"].as<std::string>()

                                   cv::Mat image,
                dest;
    cv::Mat        genimg;
    cv::Size       size(32, 32);
    dd::QubitCount nqubits = 11;

    image = imread(filename, cv::IMREAD_GRAYSCALE);
    if (!image.data) {
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

    cv::MatIterator_<double> it, end;
    for (it = dest.begin<double>(), end = dest.end<double>(); it != end; ++it) {
        *it = std::asin(*it);
    }

    const auto t0start = std::chrono::steady_clock::now();
    /* Preparation of the quantum circuit */
    std::unique_ptr<qc::QuantumComputation> qc = std::make_unique<qc::QuantumComputation>(nqubits);

    for (dd::Qubit i = 1; i < 11; i++) {
        qc->h(i); // add Hadamard gates
    }

    int cnt = 0; // start with the x/y location at 0000000000b
    for (it = dest.begin<double>(), end = dest.end<double>(); it != end; ++it) {
        if (*it != 0) {
            dd::Controls controls;
            for (int i = 0; i < 10; i++) {
                controls.insert({static_cast<dd::Qubit>(i + 1), (cnt & 1LL << i) ? dd::Control::Type::pos : dd::Control::Type::neg});
            }
            qc->ry(0, controls, (*it) * 2);
        }
        cnt++;
    }
    const auto t0end = std::chrono::steady_clock::now();

    if (vm.count("verbose")) {
        qc->print(std::cout);
    }

    std::clog << "Elapsed time for quantum circuit preparation: " << std::chrono::duration<float>(t0end - t0start).count() << "s" << std::endl;

    const auto       t1start = std::chrono::steady_clock::now();
    CircuitSimulator ddsim(std::move(qc));

    std::vector<unsigned int> counts(1024, 0);
    int                       cols    = dest.cols;
    int                       rows    = dest.rows;
    int                       colrows = cols * rows;

    std::map<std::string, std::size_t> m_counter = ddsim.Simulate(numOfShots);

    const auto t1end = std::chrono::steady_clock::now();

    std::clog << "Elapsed time for quantum simulation and measurement " << std::chrono::duration<float>(t1end - t1start).count() << " s" << std::endl;
    std::clog << "size of image: " << colrows << std::endl;

    unsigned int result = 0;
    for (auto& elem: m_counter) {
        result += elem.second;
    }
    std::clog << "Number of distinct measurements: " << m_counter.size() << "\n";
    std::clog << "RESULT: " << result << std::endl;

    std::vector<double> prob(1024, 0);

    for (auto& elem: m_counter) {
        std::size_t fixed_index = std::stoi(elem.first.substr(0, 10), nullptr, 2);
        double      tmp         = std::sqrt(static_cast<double>(elem.second) / numOfShots) * 32. * 255.;
        if (vm.count("verbose")) {
            std::clog << elem.first << " (" << fixed_index << ") : " << elem.second << " (" << tmp << ")\n";
        }
        prob.at(fixed_index) = tmp;
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
