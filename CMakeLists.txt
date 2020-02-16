cmake_minimum_required(VERSION 3.10)
project(ddsim
        VERSION 1.0.0
        DESCRIPTION "A front end to utilize the dd_package and qfr for simulation"
        LANGUAGES CXX)

option(GIT_SUBMODULE "Check submodules during build" ON)
macro(handle_submodule MODULENAME)
	find_package(Git QUIET)
	if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git" AND GIT_SUBMODULE)
		message(STATUS "${MODULENAME} update")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote -- extern/${MODULENAME}
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		                RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init --recursive --remote -- extern/${MODULENAME} failed with ${GIT_SUBMOD_RESULT}.")
		endif()
	endif()

	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/${MODULENAME}/CMakeLists.txt")
		message(FATAL_ERROR "${MODULENAME} failed to download! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
	endif()
endmacro()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

SET(COVERAGE OFF CACHE BOOL "Coverage")

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
	    STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
	             "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

handle_submodule(qfr)

add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	handle_submodule(googletest)
	enable_testing()
	include(GoogleTest)
	add_subdirectory(test)

	add_subdirectory(apps)
endif()

