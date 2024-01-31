# set required cmake version
cmake_minimum_required(VERSION 3.19...3.28)

project(
  mqt-ddsim
  LANGUAGES CXX
  DESCRIPTION
    "MQT DDSIM - A quantum circuit simulator based on decision diagrams")

option(BUILD_MQT_DDSIM_TESTS "Also build tests for the MQT DDSIM project" ON)
option(BUILD_MQT_DDSIM_BINDINGS "Build the MQT DDSIM Python bindings" OFF)
option(BUILD_MQT_DDSIM_CLI "Build the MQT DDSIM command line interface" ON)

if(BUILD_MQT_DDSIM_BINDINGS)
  # ensure that the BINDINGS option is set
  set(BINDINGS
      ON
      CACHE INTERNAL "Enable settings related to Python bindings")
  # Some common settings for finding Python
  set(Python_FIND_VIRTUALENV
      FIRST
      CACHE STRING "Give precedence to virtualenvs when searching for Python")
  set(Python_FIND_FRAMEWORK
      LAST
      CACHE STRING "Prefer Brew/Conda to Apple framework Python")
  set(Python_ARTIFACTS_INTERACTIVE
      ON
      CACHE
        BOOL
        "Prevent multiple searches for Python and instead cache the results.")

  # top-level call to find Python
  find_package(
    Python 3.8 REQUIRED
    COMPONENTS Interpreter Development.Module
    OPTIONAL_COMPONENTS Development.SABIModule)
endif()

include(cmake/ExternalDependencies.cmake)

# set the include directory for the build tree
set(MQT_DDSIM_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(src)

if(BUILD_MQT_DDSIM_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(test)
endif()

if(BUILD_MQT_DDSIM_CLI)
  add_subdirectory(apps)
endif()
