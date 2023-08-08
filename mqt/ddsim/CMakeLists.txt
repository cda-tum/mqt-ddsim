pybind11_add_module(py${PROJECT_NAME} bindings.cpp)
target_link_libraries(py${PROJECT_NAME} PUBLIC ${PROJECT_NAME} MQT::CorePython)
target_compile_definitions(py${PROJECT_NAME}
                           PRIVATE VERSION_INFO=${DDSIM_VERSION_INFO})
