# Declare all external dependencies and make sure that they are available.

include(FetchContent)
set(FETCH_PACKAGES "")

if(BUILD_MQT_DDSIM_BINDINGS)
  if(NOT SKBUILD)
    # Manually detect the installed pybind11 package.
    execute_process(
      COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE pybind11_DIR)

    # Add the detected directory to the CMake prefix path.
    list(APPEND CMAKE_PREFIX_PATH "${pybind11_DIR}")
  endif()

  # add pybind11 library
  find_package(pybind11 2.13.5 CONFIG REQUIRED)
endif()

# cmake-format: off
set(MQT_CORE_VERSION 2.7.0
    CACHE STRING "MQT Core version")
set(MQT_CORE_REV "be654c753e98e9062d796143dd3a591366370b2d"
    CACHE STRING "MQT Core identifier (tag, branch or commit hash)")
set(MQT_CORE_REPO_OWNER "cda-tum"
    CACHE STRING "MQT Core repository owner (change when using a fork)")
# cmake-format: on
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
  FetchContent_Declare(
    mqt-core
    GIT_REPOSITORY https://github.com/${MQT_CORE_REPO_OWNER}/mqt-core.git
    GIT_TAG ${MQT_CORE_REV}
    FIND_PACKAGE_ARGS ${MQT_CORE_VERSION})
  list(APPEND FETCH_PACKAGES mqt-core)
else()
  find_package(mqt-core ${MQT_CORE_VERSION} QUIET)
  if(NOT mqt-core_FOUND)
    FetchContent_Declare(
      mqt-core
      GIT_REPOSITORY https://github.com/${MQT_CORE_REPO_OWNER}/mqt-core.git
      GIT_TAG ${MQT_CORE_REV})
    list(APPEND FETCH_PACKAGES mqt-core)
  endif()
endif()

if(BUILD_MQT_DDSIM_TESTS)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  set(GTEST_VERSION
      1.14.0
      CACHE STRING "Google Test version")
  set(GTEST_URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.tar.gz)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    FetchContent_Declare(googletest URL ${GTEST_URL} FIND_PACKAGE_ARGS ${GTEST_VERSION} NAMES GTest)
    list(APPEND FETCH_PACKAGES googletest)
  else()
    find_package(googletest ${GTEST_VERSION} QUIET NAMES GTest)
    if(NOT googletest_FOUND)
      FetchContent_Declare(googletest URL ${GTEST_URL})
      list(APPEND FETCH_PACKAGES googletest)
    endif()
  endif()
endif()

set(TF_BUILD_TESTS
    OFF
    CACHE INTERNAL "")
set(TF_BUILD_EXAMPLES
    OFF
    CACHE INTERNAL "")
set(TF_BUILD_PROFILER
    OFF
    CACHE INTERNAL "")
set(TF_VERSION
    3.7.0
    CACHE STRING "Taskflow version")
set(TF_URL https://github.com/taskflow/taskflow/archive/refs/tags/v${TF_VERSION}.tar.gz)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
  FetchContent_Declare(taskflow URL ${TF_URL} FIND_PACKAGE_ARGS)
  list(APPEND FETCH_PACKAGES taskflow)
else()
  find_package(taskflow ${TF_VERSION} QUIET)
  if(NOT taskflow_FOUND)
    FetchContent_Declare(taskflow URL ${TF_URL})
    list(APPEND FETCH_PACKAGES taskflow)
  endif()
endif()

if(BUILD_MQT_DDSIM_CLI)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads)
  link_libraries(Threads::Threads)

  set(CXXOPTS_VERSION
      3.1.1
      CACHE STRING "cxxopts version")
  set(CXXOPTS_URL https://github.com/jarro2783/cxxopts/archive/refs/tags/v${CXXOPTS_VERSION}.tar.gz)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    FetchContent_Declare(cxxopts URL ${CXXOPTS_URL} FIND_PACKAGE_ARGS ${CXXOPTS_VERSION})
    list(APPEND FETCH_PACKAGES cxxopts)
  else()
    find_package(cxxopts ${CXXOPTS_VERSION} QUIET)
    if(NOT cxxopts_FOUND)
      FetchContent_Declare(cxxopts URL ${CXXOPTS_URL})
      list(APPEND FETCH_PACKAGES cxxopts)
    endif()
  endif()
endif()

if(BUILD_MQT_DDSIM_BINDINGS)
  # add pybind11_json library
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    FetchContent_Declare(
      pybind11_json
      GIT_REPOSITORY https://github.com/pybind/pybind11_json
      FIND_PACKAGE_ARGS)
    list(APPEND FETCH_PACKAGES pybind11_json)
  else()
    find_package(pybind11_json QUIET)
    if(NOT pybind11_json_FOUND)
      FetchContent_Declare(pybind11_json GIT_REPOSITORY https://github.com/pybind/pybind11_json)
      list(APPEND FETCH_PACKAGES pybind11_json)
    endif()
  endif()
endif()

# Make all declared dependencies available.
FetchContent_MakeAvailable(${FETCH_PACKAGES})
