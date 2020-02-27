set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.50 COMPONENTS program_options REQUIRED )
message(STATUS "Boost version: ${Boost_VERSION}")

if(NOT TARGET Boost::program_options)
	add_library(Boost::program_options IMPORTED INTERFACE)
	set_property(TARGET Boost::program_options PROPERTY
	             INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
	set_property(TARGET Boost::program_options PROPERTY
	             INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
endif()

add_executable(${PROJECT_NAME}_simple ${CMAKE_CURRENT_SOURCE_DIR}/simple.cpp)
target_compile_features(${PROJECT_NAME}_simple PUBLIC cxx_std_14)
set_target_properties(${PROJECT_NAME}_simple PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(${PROJECT_NAME}_simple PUBLIC ${PROJECT_NAME} Boost::program_options)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_simple ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_simple)
if(MSVC)
	target_compile_options(${PROJECT_NAME}_simple PUBLIC "/W4" "$<$<CONFIG:RELEASE>:/O2 /GL>")
	target_link_options(${PROJECT_NAME}_simple PUBLIC "$<$<CONFIG:Release>:/LTCG>")
else()
	target_compile_options(${PROJECT_NAME}_simple PUBLIC -Wall -DNDEBUG -g0 $<$<CONFIG:DEBUG>:-Og -Wextra -Wpedantic -pedantic> $<$<CONFIG:RELEASE>:-O3 -mtune=native -march=native>)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_app ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_app)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}_simple
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set_target_properties(${PROJECT_NAME}_simple PROPERTIES EXPORT_NAME ${PROJECT_NAME}_simple)
