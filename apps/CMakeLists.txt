add_subdirectory("${PROJECT_SOURCE_DIR}/extern/cxxopts" "extern/cxxopts")

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

add_sim_executable(simple cxxopts::cxxopts)
add_sim_executable(primebases cxxopts::cxxopts)
if (Threads_FOUND)
    add_sim_executable(noise_aware cxxopts::cxxopts)
    target_link_libraries(${PROJECT_NAME}_noise_aware PUBLIC Threads::Threads)
endif ()
find_package(OpenCV QUIET)
if (OpenCV_FOUND)
    add_sim_executable(frqi cxxopts::cxxopts ${OpenCV_LIBRARIES})
endif()
add_sim_executable(vectors)

