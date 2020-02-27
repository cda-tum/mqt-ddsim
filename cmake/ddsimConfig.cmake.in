get_filename_component(DDSIM_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

find_dependency(qfr 1.0 REQUIRED CONFIG)

if(NOT TARGET JKQ::ddsim)
	include("${DDSIM_CMAKE_DIR}/ddsimTargets.cmake")
endif()

set(ddsim_LIBRARIES JKQ::ddsim)
