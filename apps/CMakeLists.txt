set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.50 COMPONENTS program_options REQUIRED )
message(STATUS "Boost version: ${Boost_VERSION}")

if(NOT TARGET Boost::program_options)
	add_library(Boost::program_options IMPORTED INTERFACE)
	set_property(TARGET Boost::program_options PROPERTY
	             INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
	set_property(TARGET Boost::program_options PROPERTY
	             INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
endif()

include(GNUInstallDirs)

macro(add_sim_executable APPNAME)
	add_executable(${PROJECT_NAME}_${APPNAME} ${CMAKE_CURRENT_SOURCE_DIR}/${APPNAME}.cpp)
	target_compile_features(${PROJECT_NAME}_${APPNAME} PUBLIC cxx_std_14)
	set_target_properties(${PROJECT_NAME}_${APPNAME} PROPERTIES CXX_EXTENSIONS OFF)
	target_link_libraries(${PROJECT_NAME}_${APPNAME} PUBLIC ${PROJECT_NAME} ${ARGN})
	add_custom_command(TARGET ${PROJECT_NAME}_${APPNAME}
	                   POST_BUILD
	                   COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_DIR:${PROJECT_NAME}_${APPNAME}>/${PROJECT_NAME}_${APPNAME} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${APPNAME}
	                   COMMENT "Creating symlink for ${PROJECT_NAME}_${APPNAME}"
	                   VERBATIM)

	if(MSVC)
		target_compile_options(${PROJECT_NAME}_${APPNAME} PUBLIC "/W4" "$<$<CONFIG:RELEASE>:/O2 /GL>")
		target_link_options(${PROJECT_NAME}_${APPNAME} PUBLIC "$<$<CONFIG:Release>:/LTCG>")
	else()
		target_compile_options(${PROJECT_NAME}_${APPNAME} PUBLIC -Wall $<$<CONFIG:DEBUG>:-Og -Wextra -Wpedantic -pedantic> $<$<CONFIG:RELEASE>:-DNDEBUG -g0 -O3 -mtune=native -march=native>)
	endif()

	install(TARGETS ${PROJECT_NAME}_${APPNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	set_target_properties(${PROJECT_NAME}_${APPNAME} PROPERTIES EXPORT_NAME ${PROJECT_NAME}_${APPNAME})
endmacro()

add_sim_executable(simple Boost::program_options)
add_sim_executable(benchmark benchmark::benchmark_main)