# set required cmake version
cmake_minimum_required(VERSION 3.19)

# This avoids googletest complaining that this (IPO) policy is not set
cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

project(
  ddsim
  LANGUAGES CXX
  DESCRIPTION "MQT DDSIM - A quantum simulator based on decision diagrams")

# check whether `modulename` is correctly cloned in the `extern` directory.
macro(CHECK_SUBMODULE_PRESENT modulename)
  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/${modulename}/CMakeLists.txt")
    message(
      FATAL_ERROR
        "${modulename} submodule not cloned properly. \
        Please run `git submodule update --init --recursive` \
        from the main project directory")
  endif()
endmacro()

if(NOT BUILD_DDSIM_TESTS)
  set(BUILD_MQT_CORE_TESTS
      OFF
      CACHE BOOL "")
endif()

check_submodule_present(mqt-core)
check_submodule_present(taskflow)

add_subdirectory(src)

option(BUILD_DDSIM_TESTS "Also build tests and benchmarks for DDSIM project"
       OFF)
if(BUILD_DDSIM_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  check_submodule_present(cxxopts)
  add_subdirectory(apps)
endif()

option(BINDINGS "Configure for building Python bindings")
if(BINDINGS)
  # add Python binding code
  add_subdirectory(mqt/ddsim)
endif()
