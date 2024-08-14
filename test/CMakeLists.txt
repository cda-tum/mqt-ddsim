package_add_test(
  mqt-ddsim-test
  MQT::DDSim
  test_circuit_sim.cpp
  test_shor_sim.cpp
  test_fast_shor_sim.cpp
  test_grover_sim.cpp
  test_hybridsim.cpp
  test_stoch_noise_sim.cpp
  test_det_noise_sim.cpp
  test_unitary_sim.cpp
  test_path_sim.cpp
  test_output_ddvis.cpp)

target_link_libraries(mqt-ddsim-test PRIVATE MQT::CoreAlgorithms)

add_custom_command(
  TARGET mqt-ddsim-test
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_DIR:mqt-ddsim-test>/mqt-ddsim-test
          ${CMAKE_BINARY_DIR}/mqt-ddsim-test
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/circuits
          $<TARGET_FILE_DIR:mqt-ddsim-test>/circuits
  COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_DIR:mqt-ddsim-test>/circuits
          ${CMAKE_BINARY_DIR}/circuits
  COMMENT "Copying circuits and creating symlinks for mqt-ddsim-test"
  VERBATIM)
