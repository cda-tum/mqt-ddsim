pybind11_add_module(
  pyddsim
  # Prefer thin LTO if available
  THIN_LTO
  # Optimize the bindings for size
  OPT_SIZE
  # Source code goes here
  bindings.cpp)
target_link_libraries(
  pyddsim PRIVATE MQT::DDSim MQT::ProjectOptions MQT::ProjectWarnings
                  MQT::CorePython pybind11_json)

# Install directive for scikit-build-core
install(
  TARGETS pyddsim
  DESTINATION .
  COMPONENT mqt-ddsim_Python)
