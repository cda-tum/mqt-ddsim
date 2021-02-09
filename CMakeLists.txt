cmake_minimum_required(VERSION 3.10...3.16)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(ddsim
        VERSION 1.0.2
        DESCRIPTION "A front end to utilize the dd_package and qfr for simulation"
        LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(COVERAGE OFF CACHE BOOL "Coverage")
option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" OFF)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
	    STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(GIT_SUBMODULE "Check submodules during build" OFF)
macro(handle_submodule MODULENAME)
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/${MODULENAME}/CMakeLists.txt")
		message(FATAL_ERROR "${MODULENAME} was not found. Please init/update submodules (git submodule update --init --recursive) and try again.")
	endif()
endmacro()

handle_submodule(qfr)

add_subdirectory(src)

option(BUILD_TESTS "Also build tests for DDSIM project" OFF)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR BUILD_TESTS)
	handle_submodule(benchmark)

	enable_testing()
	include(GoogleTest)
	add_subdirectory(test)

	set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Skip gbenchmark self tests")
	add_subdirectory(extern/benchmark)
	add_subdirectory(apps)
endif()

