find_package( Boost 1.50 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

add_executable(${PROJECT_NAME}_simple ${CMAKE_CURRENT_SOURCE_DIR}/simple.cpp)
target_compile_features(${PROJECT_NAME}_simple PUBLIC cxx_std_14)
set_target_properties(${PROJECT_NAME}_simple PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(${PROJECT_NAME}_simple PUBLIC -mtune=native -march=native -pipe)
target_link_libraries(${PROJECT_NAME}_simple PRIVATE ${PROJECT_NAME} boost_program_options)


execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_simple ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_simple)
