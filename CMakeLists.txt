# set required cmake version
cmake_minimum_required(VERSION 3.19)

# This avoids googletest complaining that this (IPO) policy is not set
cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

project(ddsim
        LANGUAGES CXX
        DESCRIPTION "MQT DDSIM - A quantum simulator based on decision diagrams"
        )

macro(check_submodule_present MODULENAME)
    if (NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/${MODULENAME}/CMakeLists.txt")
        message(FATAL_ERROR "${MODULENAME} submodule not cloned properly. Please run `git submodule update --init --recursive` from the main project directory")
    endif ()
endmacro()

check_submodule_present(qfr)
check_submodule_present(taskflow)

add_subdirectory(src)

option(BUILD_DDSIM_TESTS "Also build tests and benchmarks for DDSIM project" OFF)
if (BUILD_DDSIM_TESTS)
    check_submodule_present(qfr/extern/dd_package/extern/benchmark)

    enable_testing()
    include(GoogleTest)
    if (NOT TARGET benchmark::benchmark_main)
        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Skip gbenchmark self tests")
        add_subdirectory("extern/qfr/extern/dd_package/extern/benchmark" EXCLUDE_FROM_ALL)
    endif ()
    add_subdirectory(test)
endif ()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    check_submodule_present(cxxopts)
    add_subdirectory(apps)
endif ()

option(BINDINGS "Configure for building Python bindings")
if (BINDINGS)
    # add Python binding code
    add_subdirectory(mqt/ddsim)
endif ()
