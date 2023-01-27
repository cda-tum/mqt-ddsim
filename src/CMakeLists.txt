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


add_library(MQT::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
