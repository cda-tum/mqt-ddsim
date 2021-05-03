add_library(${PROJECT_NAME}
        ${PROJECT_SOURCE_DIR}/include/Simulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Simulator.cpp
        ${PROJECT_SOURCE_DIR}/include/CircuitSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CircuitSimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/GroverSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GroverSimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/ShorSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ShorSimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/ShorFastSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ShorFastSimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/SlicingSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SlicingSimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/HybridSchroedingerFeynmanSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/HybridSchroedingerFeynmanSimulator.cpp
        )
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>)
# set required C++ standard and disable compiler specific extensions
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/qfr" "extern/qfr")
target_link_libraries(${PROJECT_NAME} PUBLIC JKQ::qfr)

# add coverage compiler and linker flag if COVERAGE is set
if (COVERAGE)
    target_compile_options(${PROJECT_NAME} PUBLIC --coverage)
    target_link_libraries(${PROJECT_NAME} PUBLIC --coverage)
endif ()

# check if interprocedural optimization is supported
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if (ipo_supported)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

find_package(OpenMP REQUIRED)
if(NOT TARGET OpenMP::OpenMP_CXX)
	add_library(OpenMP_TARGET INTERFACE)
	add_library(OpenMP::OpenMP_CXX ALIAS OpenMP_TARGET)
	target_compile_options(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
	find_package(Threads REQUIRED)
	target_link_libraries(OpenMP_TARGET INTERFACE Threads::Threads)
	target_link_libraries(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)

# set compiler flags depending on compiler
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4" "/utf-8")
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall $<$<CONFIG:DEBUG>:-g3 -Og -Wextra -Wpedantic -pedantic> $<$<CONFIG:RELEASE>:-g0 -mtune=native -march=native>)

    if (BINDINGS)
        # adjust visibility settings for building Python bindings
        target_compile_options(${PROJECT_NAME} PUBLIC -fvisibility=hidden)
    endif ()
    if (NOT DEPLOY)
        # only include machine-specific optimizations when building for the host machine
        target_compile_options(${PROJECT_NAME} PUBLIC -mtune=native -march=native)
    endif ()
endif ()

if (GENERATE_POSITION_INDEPENDENT_CODE)
    include(CheckPIESupported)
    check_pie_supported()
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif ()

add_library(JKQ::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
