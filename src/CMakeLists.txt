set(MQT_DDSIM_TARGET_NAME mqt-ddsim)

add_library(
  ${MQT_DDSIM_TARGET_NAME}
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/Simulator.hpp
  Simulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/CircuitSimulator.hpp
  CircuitSimulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/GroverSimulator.hpp
  GroverSimulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/ShorSimulator.hpp
  ShorSimulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/ShorFastSimulator.hpp
  ShorFastSimulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/StochasticNoiseSimulator.hpp
  StochasticNoiseSimulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/DeterministicNoiseSimulator.hpp
  DeterministicNoiseSimulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/HybridSchrodingerFeynmanSimulator.hpp
  HybridSchrodingerFeynmanSimulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/UnitarySimulator.hpp
  UnitarySimulator.cpp
  ${MQT_DDSIM_INCLUDE_BUILD_DIR}/PathSimulator.hpp
  PathSimulator.cpp)

# set include directories
target_include_directories(${MQT_DDSIM_TARGET_NAME}
                           PUBLIC $<BUILD_INTERFACE:${MQT_DDSIM_INCLUDE_BUILD_DIR}>)

# link to the MQT::Core and Taskflow libraries
target_link_libraries(
  ${MQT_DDSIM_TARGET_NAME}
  PUBLIC MQT::CoreDD MQT::CoreCircuitOptimizer Taskflow
  PRIVATE MQT::ProjectWarnings MQT::ProjectOptions)

# the following sets the SYSTEM flag for the include dirs of the taskflow libs
set_target_properties(Taskflow PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
                                          $<TARGET_PROPERTY:Taskflow,INTERFACE_INCLUDE_DIRECTORIES>)

# add MQT alias
add_library(MQT::DDSim ALIAS ${MQT_DDSIM_TARGET_NAME})

if(BUILD_MQT_DDSIM_BINDINGS)
  add_subdirectory(python)
endif()
