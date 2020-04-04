# allow loader to find lib
if(APPLE)
	set(basePoint @loader_path)
else()
	set(basePoint $ORIGIN)
endif()
include(GNUInstallDirs)
file(RELATIVE_PATH relDir
     ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
     ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
     )
set(CMAKE_INSTALL_RPATH ${basePoint} ${basePoint}/${relDir})

add_library(${PROJECT_NAME}
		${PROJECT_SOURCE_DIR}/include/Simulator.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Simulator.cpp
		${PROJECT_SOURCE_DIR}/include/QFRSimulator.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/QFRSimulator.cpp
		${PROJECT_SOURCE_DIR}/include/GroverSimulator.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/GroverSimulator.cpp
		)
add_library(JKQ::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

option(TRY_USING_INSTALLED_QFR_LIBRARY "Attempt searching for installed version of QFR library and use it if found" ON)

if (TRY_USING_INSTALLED_QFR_LIBRARY)
	find_package(qfr 1.0 QUIET)
	if (qfr_FOUND)
		message(STATUS "using installed QFR library")
	else()
		message(STATUS "using QFR submodule")
		add_subdirectory("${PROJECT_SOURCE_DIR}/extern/qfr" "extern/qfr")
	endif()
else()
	message(STATUS "using QFR submodule")
	add_subdirectory("${PROJECT_SOURCE_DIR}/extern/qfr" "extern/qfr")
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC JKQ::qfr)

# add coverage compiler and linker flag if COVERAGE is set
if (COVERAGE)
	target_compile_options(${PROJECT_NAME} PRIVATE --coverage)
	target_link_libraries(${PROJECT_NAME} PRIVATE --coverage)
endif()

# check if interprocedural optimization is supported
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
	set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# set compiler flags depending on compiler
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE "/W4" "$<$<CONFIG:RELEASE>:/O2 /GL>")
	target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/LTCG>")
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall $<$<CONFIG:DEBUG>:-g3 -Og -Wextra -Wpedantic -pedantic> $<$<CONFIG:RELEASE>:-DNDEBUG -g0 -O3 -mtune=native -march=native>)
endif()

# installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME ${PROJECT_NAME})

install(DIRECTORY ${${PROJECT_NAME}_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE JKQ::
        DESTINATION ${INSTALL_CONFIGDIR}
        )

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${${PROJECT_NAME}_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
                              INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
                              )

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
        )

# exporting from the build tree
export(EXPORT ${PROJECT_NAME}-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake NAMESPACE JKQ::)

# Register package in user's package registry
export(PACKAGE ${PROJECT_NAME})
