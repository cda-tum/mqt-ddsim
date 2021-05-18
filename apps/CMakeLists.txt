set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.50 COMPONENTS program_options)

if (Boost_FOUND)
    message(STATUS "Boost version: ${Boost_VERSION}")
else ()
    message(STATUS "Boost not found. Certain targets may not be available.")
endif ()

macro(add_sim_executable APPNAME)
    add_executable(${PROJECT_NAME}_${APPNAME} ${CMAKE_CURRENT_SOURCE_DIR}/${APPNAME}.cpp)
    target_link_libraries(${PROJECT_NAME}_${APPNAME} PUBLIC ${PROJECT_NAME} ${ARGN})
    add_custom_command(TARGET ${PROJECT_NAME}_${APPNAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_DIR:${PROJECT_NAME}_${APPNAME}>/${PROJECT_NAME}_${APPNAME} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_${APPNAME}
                       COMMENT "Creating symlink for ${PROJECT_NAME}_${APPNAME}"
                       VERBATIM)

    set_target_properties(${PROJECT_NAME}_${APPNAME} PROPERTIES EXPORT_NAME ${PROJECT_NAME}_${APPNAME})
endmacro()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
link_libraries(Threads::Threads)

if (Boost_FOUND)
	add_sim_executable(simple Boost::program_options)
	add_sim_executable(primebases Boost::program_options)
	if (Threads_FOUND)
		add_sim_executable(noise_aware Boost::program_options)
		#		target_link_libraries(${PROJECT_NAME}_noise_aware PUBLIC Threads::Threads)
	endif ()
else ()
	message(WARNING "Did not find Boost! Commandline interface will not be an available target!")
endif ()
add_sim_executable(vectors)
