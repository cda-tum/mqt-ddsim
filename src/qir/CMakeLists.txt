set(MQT_DDSIM_QIR_TARGET_NAME ${PROJECT_NAME}-qir)

if(NOT TARGET ${MQT_DDSIM_QIR_TARGET_NAME})
  file(GLOB_RECURSE QIR_HEADERS ${PROJECT_SOURCE_DIR}/include/qir/*.h
       ${PROJECT_SOURCE_DIR}/include/qir/*.hpp)
  file(GLOB_RECURSE QIR_SOURCES *.c *.cpp)

  add_library(${MQT_DDSIM_QIR_TARGET_NAME} STATIC ${QIR_HEADERS} ${QIR_SOURCES})

  message(STATUS "QIR Headers: ${QIR_HEADERS}")
  message(STATUS "QIR Sources: ${QIR_SOURCES}")
  message(STATUS "Project Source Dir: ${PROJECT_SOURCE_DIR}")
  message(STATUS "Project Binary Dir: ${PROJECT_BINARY_DIR}")

  # set include directories
  target_include_directories(
    ${MQT_DDSIM_QIR_TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include
                                        ${PROJECT_BINARY_DIR}/include)

  # link to the MQT::Core libraries
  target_link_libraries(${MQT_DDSIM_QIR_TARGET_NAME} PUBLIC MQT::CoreDD)

  add_library(MQT::DDsimQIR ALIAS ${MQT_DDSIM_QIR_TARGET_NAME})

  # Define the path to the bell_qir.ll file
  set(BELL_QIR_LL_FILE ${PROJECT_SOURCE_DIR}/scripts/qir/examples/BellPair.ll)
  message(STATUS "Path to BellPair.ll ${BELL_QIR_LL_FILE}")

  # Ensure the file exists
  if(NOT EXISTS ${BELL_QIR_LL_FILE})
    message(FATAL_ERROR "File not found: ${BELL_QIR_LL_FILE}")
  endif()
 
  # NOTE: The following step depends on the clang version. It has been tested with clang 15.0.0 and 
  # worked. However, with the newer version 18.6.0 there was an error for an "Unsupported stack probing method"
  # and the request to submit a pull request with this error.

  # Add a custom command to compile the .ll file to .o
  add_custom_command(
    OUTPUT bell_qir.o
    # COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-as -o bell_qir.bc ${BELL_QIR_LL_FILE}
    # COMMAND ${LLVM_TOOLS_BINARY_DIR}/llc -filetype=obj -o bell_qir.o bell_qir.bc
    # COMMAND ${LLVM_TOOLS_BINARY_DIR}/clang -c ${BELL_QIR_LL_FILE} -o bell_qir.o
    COMMAND clang -c ${BELL_QIR_LL_FILE} -o bell_qir.o
    DEPENDS ${BELL_QIR_LL_FILE}
    COMMENT "Compiling ${BELL_QIR_LL_FILE} to bell_qir.o"
  )


  add_executable(bellPair bell_qir.o)

  target_link_libraries(bellPair PRIVATE MQT::DDsimQIR)
endif()
