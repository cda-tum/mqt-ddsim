add_library(
  ${PROJECT_NAME} STATIC
  ${PROJECT_SOURCE_DIR}/include/qir/DDsimQir.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DDsimQir.cpp)

# set include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include
                                                  ${PROJECT_BINARY_DIR}/include)

# link to the MQT::Core libraries
target_link_libraries(${PROJECT_NAME} PUBLIC MQT::CoreDD)
