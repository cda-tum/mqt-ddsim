add_subdirectory("${PROJECT_SOURCE_DIR}/extern/cxxopts" "extern/cxxopts"
                 EXCLUDE_FROM_ALL)
# the following sets the SYSTEM flag for the include dirs of the cxxopts libs to
# cmake-lint: disable=C0307
set_target_properties(
  cxxopts PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
                     $<TARGET_PROPERTY:cxxopts,INTERFACE_INCLUDE_DIRECTORIES>)

# macro to add a executable with the given libraries
macro(ADD_SIM_EXECUTABLE appname)
  add_executable(${PROJECT_NAME}_${appname}
                 ${CMAKE_CURRENT_SOURCE_DIR}/${appname}.cpp)
  target_link_libraries(${PROJECT_NAME}_${appname} PRIVATE ${PROJECT_NAME}
                                                           ${ARGN})
endmacro()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
link_libraries(Threads::Threads)

add_sim_executable(simple cxxopts::cxxopts)
add_sim_executable(primebases cxxopts::cxxopts)
if(Threads_FOUND)
  add_sim_executable(noise_aware cxxopts::cxxopts)
  target_link_libraries(${PROJECT_NAME}_noise_aware PUBLIC Threads::Threads)
endif()
find_package(OpenCV QUIET)
if(OpenCV_FOUND)
  add_sim_executable(frqi cxxopts::cxxopts ${OpenCV_LIBRARIES})
endif()
add_sim_executable(vectors)
