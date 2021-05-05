#include "HybridSchrodingerFeynmanSimulator.hpp"
std::map<std::string, std::size_t> HybridSchrodingerFeynmanSimulator::Simulate(unsigned int shots) {
    auto nqubits    = getNumberOfQubits();
    auto splitQubit = static_cast<dd::Qubit>(nqubits / 2);
    if (mode == Mode::DD) {
        SimulateParallel(splitQubit);
        return MeasureAllNonCollapsing(shots);
    } else {
        SimulateParallelAmplitudes(splitQubit);

        if (shots > 0) {
            return SampleFromAmplitudeVectorInPlace(finalAmplitudes, shots);
        } else {
            // in case no shots were requested, the final amplitudes remain untouched
            return {};
        }
    }
}

void HybridSchrodingerFeynmanSimulator::addAmplitudes(std::unique_ptr<dd::Package>& dd, const std::string& filename1, const std::string& filename2, const std::string& resultfile, bool binary) {
    std::string complex_real_regex = R"(([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?(?![ \d\.]*(?:[eE][+-])?\d*[iI]))?)";
    std::string complex_imag_regex = R"(( ?[+-]? ?(?:(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)?[iI])?)";
    std::regex  complex_weight_regex(complex_real_regex + complex_imag_regex);
    std::smatch m;

    auto ifs1 = std::ifstream(filename1);
    if (!ifs1.good()) {
        std::cerr << "Cannot open amplitude file: " << filename1 << std::endl;
        exit(1);
    }

    auto ifs2 = std::ifstream(filename2);
    if (!ifs2.good()) {
        std::cerr << "Cannot open amplitude file: " << filename2 << std::endl;
        exit(1);
    }

    if (ifs1.peek() == std::ifstream::traits_type::eof()) {
        rename(filename2.c_str(), resultfile.c_str());
        return;
    }

    if (ifs2.peek() == std::ifstream::traits_type::eof()) {
        rename(filename1.c_str(), resultfile.c_str());
        return;
    }

    std::ostringstream oss{};
    addAmplitudes(dd, ifs1, ifs2, oss, binary);

    std::ofstream init(resultfile);
    init << oss.str() << std::flush;
    init.close();
}
void HybridSchrodingerFeynmanSimulator::addAmplitudes(std::unique_ptr<dd::Package>& dd, std::istream& ifs1, std::istream& ifs2, std::ostream& oss, bool binary) {
    if (binary) {
        dd::fp temp_r, temp_i;
        while (ifs1.read(reinterpret_cast<char*>(&temp_r), sizeof(temp_r))) {
            ifs1.read(reinterpret_cast<char*>(&temp_i), sizeof(temp_i));

            dd::ComplexValue amplitude1{temp_r, temp_i};
            dd::ComplexValue amplitude2{};
            amplitude2.readBinary(ifs2);

            auto temp = dd->cn.addCached(dd->cn.lookup(amplitude1), dd->cn.lookup(amplitude2));
            temp.writeBinary(oss);
            dd->cn.returnToCache(temp);
        }
    } else {
        std::string complex_real_regex = R"(([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?(?![ \d\.]*(?:[eE][+-])?\d*[iI]))?)";
        std::string complex_imag_regex = R"(( ?[+-]? ?(?:(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)?[iI])?)";
        std::regex  complex_weight_regex(complex_real_regex + complex_imag_regex);
        std::smatch m;
        std::string line1, line2;

        while (std::getline(ifs1, line1)) {
            if (!std::getline(ifs2, line2)) {
                throw std::runtime_error("Too few lines in second stream");
            }

            std::regex_match(line1, m, complex_weight_regex);
            dd::ComplexValue amplitude1{};
            amplitude1.from_string(m.str(1), m.str(2));
            std::regex_match(line2, m, complex_weight_regex);
            dd::ComplexValue amplitude2{};
            amplitude2.from_string(m.str(1), m.str(2));

            dd::Complex temp = dd->cn.addCached(dd->cn.lookup(amplitude1), dd->cn.lookup(amplitude2));
            oss << temp.toString(false, 16) << "\n";
            dd->cn.returnToCache(temp);
        }
        if (std::getline(ifs2, line2)) {
            throw std::runtime_error("Too few lines in first stream");
        }
    }
}
void HybridSchrodingerFeynmanSimulator::addAmplitudes(std::vector<dd::ComplexValue>& amp1, std::vector<dd::ComplexValue>& amp2) {
    for (std::size_t i = 0; i < amp1.size(); i++) {
        amp1[i].r += amp2[i].r;
        amp1[i].i += amp2[i].i;
    }
}

void HybridSchrodingerFeynmanSimulator::SimulateParallel(dd::Qubit split_qubit) {
    auto               ndecisions  = getNDecisions(split_qubit);
    const std::int64_t max_control = 1LL << ndecisions;

    int actuallyUsedThreads = static_cast<int>(nthreads);
    if (static_cast<std::size_t>(max_control) < nthreads) {
        actuallyUsedThreads = static_cast<int>(max_control);
    }
    omp_set_num_threads(actuallyUsedThreads);
    root_edge                    = qc::VectorDD::zero;
    std::int64_t nslices_at_once = 16;
    nslices_at_once              = std::min(nslices_at_once, max_control / static_cast<std::int64_t>(actuallyUsedThreads));
    std::stack<std::pair<std::int64_t, std::int64_t>> stack{};
    for (auto i = max_control - 1; i >= 0; i -= nslices_at_once) {
        stack.emplace(0, i);
    }
    std::vector<std::vector<bool>> computed(ndecisions, std::vector<bool>(max_control, false));

    omp_lock_t stacklock;
    omp_init_lock(&stacklock);
#pragma omp parallel
    {
        bool                                  isempty;
        std::pair<std::int64_t, std::int64_t> current{};
        omp_set_lock(&stacklock);
        isempty = stack.empty();
        if (!isempty) {
            current = stack.top();
            stack.pop();
        }
        omp_unset_lock(&stacklock);

        while (!isempty) {
            if (current.first == 0) { // slice
                std::unique_ptr<dd::Package> old_dd;
                qc::VectorDD                 edge{};
                for (std::int64_t i = 0; i < nslices_at_once; i++) {
                    auto slice_dd = std::make_unique<dd::Package>(getNumberOfQubits());
                    auto edges    = SimulateSlicing(slice_dd, split_qubit, current.second + i);
                    if (i > 0) {
                        edge = slice_dd->add(slice_dd->transfer(edge), std::get<2>(edges));
                    } else {
                        edge = std::get<2>(edges);
                    }
                    old_dd = std::move(slice_dd);
                }

                current.first = static_cast<std::int64_t>(std::log2(nslices_at_once));
                current.second /= nslices_at_once;
                dd::serialize(edge, "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd", true);
            } else { // adding
                auto        slice_dd       = std::make_unique<dd::Package>(getNumberOfQubits());
                std::string filename       = "slice_" + std::to_string(current.first - 1) + "_";
                std::string filename_left  = filename + std::to_string(current.second * 2) + ".dd";
                std::string filename_right = filename + std::to_string(current.second * 2 + 1) + ".dd";

                auto result  = slice_dd->deserialize<dd::Package::vNode>(filename_left, true);
                auto result2 = slice_dd->deserialize<dd::Package::vNode>(filename_right, true);
                result       = slice_dd->add(result, result2);
                dd::serialize(result, "slice_" + std::to_string(current.first) + "_" + std::to_string(current.second) + ".dd", true);

                remove(filename_left.c_str());
                remove(filename_right.c_str());
            }

            omp_set_lock(&stacklock);
            if (current.first < static_cast<std::int64_t>(ndecisions)) {
                computed[current.first][current.second] = true;

                if (computed[current.first][current.second + (current.second % 2 ? (-1) : 1)]) {
                    stack.push(std::make_pair(current.first + 1, current.second / 2));
                }
            }
            isempty = stack.empty();
            if (!isempty) {
                current = stack.top();
                stack.pop();
            }
            omp_unset_lock(&stacklock);
        }
    }
    omp_destroy_lock(&stacklock);
    root_edge = dd->deserialize<dd::Package::vNode>("slice_" + std::to_string(ndecisions) + "_0.dd", true);
    dd->incRef(root_edge);
}
void HybridSchrodingerFeynmanSimulator::SimulateParallelAmplitudes(dd::Qubit split_qubit) {
    auto               ndecisions  = getNDecisions(split_qubit);
    const std::int64_t max_control = 1LL << ndecisions;

    int actuallyUsedThreads = static_cast<int>(nthreads);
    if (static_cast<std::size_t>(max_control) < nthreads) {
        actuallyUsedThreads = static_cast<int>(max_control);
    }
    omp_set_num_threads(actuallyUsedThreads);

    std::vector<std::vector<dd::ComplexValue>> amplitudes(actuallyUsedThreads);
    std::vector<bool>                          initialized(actuallyUsedThreads, false);

    root_edge                         = qc::VectorDD::zero;
    std::int64_t   nslices_on_one_cpu = std::min(static_cast<std::int64_t>(64), static_cast<std::int64_t>(max_control / actuallyUsedThreads));
    dd::QubitCount nqubits            = getNumberOfQubits();

#pragma omp parallel for schedule(dynamic, 1)
    for (std::int64_t control = 0; control < max_control; control += nslices_on_one_cpu) {
        auto                           current_thread     = omp_get_thread_num();
        std::vector<dd::ComplexValue>& thread_amplitudes  = amplitudes[current_thread];
        bool                           thread_initialized = initialized[current_thread];
        for (std::int64_t local_control = 0; local_control < nslices_on_one_cpu; local_control++) {
            std::int64_t                 total_control = control + local_control;
            std::unique_ptr<dd::Package> slice_dd      = std::make_unique<dd::Package>(getNumberOfQubits());
            auto                         edges         = SimulateSlicing(slice_dd, split_qubit, total_control);

            if (thread_initialized) {
                slice_dd->addAmplitudes(std::get<2>(edges), thread_amplitudes, nqubits);
            } else {
                initialized[current_thread] = true;
                thread_initialized          = true;
                thread_amplitudes           = std::vector<dd::ComplexValue>(1 << nqubits);
                slice_dd->exportAmplitudes(std::get<2>(edges), thread_amplitudes, nqubits);
            }
        }
    }

    std::size_t old_increment = 1;
    for (unsigned short level = 0; level < static_cast<unsigned short>(std::log2(actuallyUsedThreads)); ++level) {
        const int increment = 2 << level;
        for (int idx = 0; idx < actuallyUsedThreads; idx += increment) {
            addAmplitudes(amplitudes[idx], amplitudes[idx + old_increment]);
        }
        old_increment = increment;
    }
    finalAmplitudes = std::move(amplitudes[0]);
}
