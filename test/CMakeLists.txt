if(NOT TARGET gtest OR NOT TARGET gmock)
  # Prevent overriding the parent project's compiler/linker settings on Windows
  # cmake-lint: disable=C0103
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  add_subdirectory(
    "${PROJECT_SOURCE_DIR}/extern/qfr/extern/dd_package/extern/googletest"
    "extern/qfr/extern/dd_package/extern/googletest" EXCLUDE_FROM_ALL)
endif()

package_add_test(
  ${PROJECT_NAME}_test
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/test_circuit_sim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_shor_sim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_fast_shor_sim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_grover_sim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_hybridsim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_stoch_noise_sim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_det_noise_sim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_unitary_sim.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_path_sim.cpp)

add_custom_command(
  TARGET ${PROJECT_NAME}_test
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    $<TARGET_FILE_DIR:${PROJECT_NAME}_test>/${PROJECT_NAME}_test
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_test
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/circuits
    $<TARGET_FILE_DIR:${PROJECT_NAME}_test>/circuits
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    $<TARGET_FILE_DIR:${PROJECT_NAME}_test>/circuits
    ${CMAKE_BINARY_DIR}/circuits
  COMMENT "Copying circuits and creating symlinks for ${PROJECT_NAME}_test"
  VERBATIM)

if(NOT TARGET benchmark::benchmark)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  set(BENCHMARK_ENABLE_TESTING
      OFF
      CACHE BOOL "Skip google benchmark self tests")
  add_subdirectory(
    "${PROJECT_SOURCE_DIR}/extern/qfr/extern/dd_package/extern/benchmark"
    "extern/qfr/extern/dd_package/extern/benchmark" EXCLUDE_FROM_ALL)
endif()

add_executable(ddsim_benchmark EXCLUDE_FROM_ALL
               ${CMAKE_CURRENT_SOURCE_DIR}/benchmark.cpp)
target_link_libraries(ddsim_benchmark PRIVATE ${PROJECT_NAME}
                                              benchmark::benchmark_main)
set_target_properties(ddsim_benchmark PROPERTIES EXPORT_NAME ddsim_benchmark)

add_custom_command(
  TARGET ${PROJECT_NAME}_benchmark
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    $<TARGET_FILE_DIR:${PROJECT_NAME}_test>/${PROJECT_NAME}_benchmark
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_benchmark
  COMMENT "Creating symlinks for ${PROJECT_NAME}_benchmark"
  VERBATIM)
