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
  test_path_sim.cpp)

target_link_libraries(mqt-ddsim-test PRIVATE MQT::CoreAlgorithms)
