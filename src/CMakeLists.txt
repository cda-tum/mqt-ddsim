add_library(${PROJECT_NAME} STATIC
        ${PROJECT_SOURCE_DIR}/include/SimpleSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SimpleSimulator.cpp
            )

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/qfr" "extern/qfr")
target_link_libraries(${PROJECT_NAME} PUBLIC QFR)
if (COVERAGE)
	target_compile_options(${PROJECT_NAME} PRIVATE --coverage)
	target_link_libraries(${PROJECT_NAME} PRIVATE --coverage)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
	set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -mtune=native -march=native -pipe)
