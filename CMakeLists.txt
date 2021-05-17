cmake_minimum_required(VERSION 3.14...3.19)

project(ddsim
        LANGUAGES CXX
        VERSION 1.5.0
        DESCRIPTION "DDSIM - A JKQ quantum simulator based on decision diagrams"
        )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
option(COVERAGE "Configure for coverage report generation")
option(GENERATE_POSITION_INDEPENDENT_CODE "Generate position independent code")
option(BINDINGS "Configure for building Python bindings")
option(DEPLOY "Configure for deployment")

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
	    STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

macro(check_submodule_present MODULENAME)
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/${MODULENAME}/CMakeLists.txt")
		message(FATAL_ERROR "${MODULENAME} submodule not cloned properly. Please run `git submodule update --init --recursive` from the main project directory")
	endif()
endmacro()

check_submodule_present(qfr)

add_subdirectory(src)

option(BUILD_DDSIM_TESTS "Also build tests and benchmarks for DDSIM project" OFF)
if(BUILD_DDSIM_TESTS)
	check_submodule_present(benchmark)

	enable_testing()
	include(GoogleTest)
	set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Skip gbenchmark self tests")
	add_subdirectory(extern/benchmark)
	add_subdirectory(test)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	add_subdirectory(apps)
endif()

# add Python binding code
if(BINDINGS)
	add_subdirectory(jkq/ddsim)
endif()

