# macro to add a executable with the given libraries
macro(ADD_SIM_EXECUTABLE appname)
  add_executable(mqt-ddsim-${appname} ${appname}.cpp)
  target_link_libraries(mqt-ddsim-${appname} PRIVATE MQT::DDSim MQT::CoreQASM ${ARGN})
endmacro()

add_sim_executable(simple cxxopts::cxxopts)
target_link_libraries(mqt-ddsim-simple PRIVATE MQT::CoreAlgorithms)

add_sim_executable(primebases cxxopts::cxxopts)
if(Threads_FOUND)
  add_sim_executable(noise_aware cxxopts::cxxopts)
  target_link_libraries(mqt-ddsim-noise_aware PRIVATE Threads::Threads)
endif()
