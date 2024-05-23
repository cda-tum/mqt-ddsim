if(TARGET ${PROJECT_NAME}-qir)
  file(GLOB_RECURSE QIR_TEST_SOURCES *.cpp)
  package_add_test(mqt-ddsim-qir-test MQT::DDsimQIR ${QIR_TEST_SOURCES})
endif()
