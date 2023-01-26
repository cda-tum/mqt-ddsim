add_subdirectory("${PROJECT_SOURCE_DIR}/extern/qfr" "extern/qfr" EXCLUDE_FROM_ALL)


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
        ${PROJECT_SOURCE_DIR}/include/StochasticNoiseSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/StochasticNoiseSimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/DeterministicNoiseSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DeterministicNoiseSimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/HybridSchrodingerFeynmanSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/HybridSchrodingerFeynmanSimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/UnitarySimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UnitarySimulator.cpp
        ${PROJECT_SOURCE_DIR}/include/PathSimulator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PathSimulator.cpp
        )
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>)
# set required C++ standard and disable compiler specific extensions
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

set(JSON_MultipleHeaders OFF CACHE INTERNAL "")
target_link_libraries(${PROJECT_NAME} PUBLIC MQT::qfr_dd)

set(TF_BUILD_TESTS OFF CACHE BOOL "")
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "")
set(TF_BUILD_PROFILER OFF CACHE BOOL "")
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/taskflow" "extern/taskflow")
target_link_libraries(${PROJECT_NAME} PUBLIC Taskflow)
# the following sets the SYSTEM flag for the include dirs of the taskflow libs to suppress warnings
# cmake-lint: disable=C0307
set_target_properties(
        Taskflow PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
        $<TARGET_PROPERTY:Taskflow,INTERFACE_INCLUDE_DIRECTORIES>)

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
# set compiler flags depending on compiler
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4" "/utf-8")
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

    if (BINDINGS)
        # adjust visibility settings for building Python bindings
        include(CheckPIESupported)
        check_pie_supported()
        set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
        target_compile_options(${PROJECT_NAME} PUBLIC -fvisibility=hidden)
    endif ()
    if (NOT DEPLOY)
        # only include machine-specific optimizations when building for the host machine
        target_compile_options(${PROJECT_NAME} PUBLIC -mtune=native)
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(-march=native HAS_MARCH_NATIVE)
        if (HAS_MARCH_NATIVE)
            target_compile_options(${PROJECT_NAME} PUBLIC -march=native)
        endif ()
    endif ()
endif ()

add_library(MQT::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
