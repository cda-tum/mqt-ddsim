set(MQT_DDSIM_QIR_TARGET_NAME ${PROJECT_NAME}-qir)

if(NOT TARGET ${MQT_DDSIM_QIR_TARGET_NAME})
  file(GLOB_RECURSE QIR_HEADERS ${PROJECT_SOURCE_DIR}/include/qir/*.h
       ${PROJECT_SOURCE_DIR}/include/qir/*.hpp)
  file(GLOB_RECURSE QIR_SOURCES *.c *.cpp)

  add_library(${MQT_DDSIM_QIR_TARGET_NAME} STATIC ${QIR_HEADERS} ${QIR_SOURCES})

  # set include directories
  target_include_directories(
    ${MQT_DDSIM_QIR_TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include
                                        ${PROJECT_BINARY_DIR}/include)

  # link to the MQT::Core libraries
  target_link_libraries(${MQT_DDSIM_QIR_TARGET_NAME} PUBLIC MQT::CoreDD)

  add_library(MQT::DDsimQIR ALIAS ${MQT_DDSIM_QIR_TARGET_NAME})
endif()
