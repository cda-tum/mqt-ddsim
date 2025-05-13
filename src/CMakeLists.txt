# Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
# Copyright (c) 2025 Munich Quantum Software Company GmbH
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

set(MQT_DDSIM_TARGET_NAME mqt-ddsim)

if(NOT TARGET MQT::DDSim)
  # collect headers and source files
  file(GLOB_RECURSE MQT_DDSIM_HEADERS ${MQT_DDSIM_INCLUDE_BUILD_DIR}/*.hpp)
  file(GLOB_RECURSE MQT_DDSIM_SOURCES **.cpp)
  list(FILTER MQT_DDSIM_SOURCES EXCLUDE REGEX ".*/python/.*$")

  # add DDSim Package library
  add_library(${MQT_DDSIM_TARGET_NAME} ${MQT_DDSIM_HEADERS} ${MQT_DDSIM_SOURCES})

  # set include directories
  target_include_directories(${MQT_DDSIM_TARGET_NAME}
                             PUBLIC $<BUILD_INTERFACE:${MQT_DDSIM_INCLUDE_BUILD_DIR}>)

  # link to the MQT::Core and Taskflow libraries
  target_link_libraries(
    ${MQT_DDSIM_TARGET_NAME}
    PUBLIC MQT::CoreDD MQT::CoreCircuitOptimizer Taskflow
    PRIVATE MQT::ProjectWarnings MQT::ProjectOptions)

  # the following sets the SYSTEM flag for the include dirs of the taskflow libs
  set_target_properties(
    Taskflow PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
                        $<TARGET_PROPERTY:Taskflow,INTERFACE_INCLUDE_DIRECTORIES>)

  # add MQT alias
  add_library(MQT::DDSim ALIAS ${MQT_DDSIM_TARGET_NAME})
endif()

if(BUILD_MQT_DDSIM_BINDINGS)
  add_subdirectory(python)
endif()
