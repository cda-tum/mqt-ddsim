add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")
mark_as_advanced(
		BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
		gmock_build_tests gtest_build_samples gtest_build_tests
		gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)
set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

macro(package_add_test TESTNAME)
	# create an exectuable in which the tests will be stored
	add_executable(${TESTNAME} ${ARGN})
	# link the Google test infrastructure, mocking library, and a default main fuction to
	# the test executable.  Remove g_test_main if writing your own main function.
	target_link_libraries(${TESTNAME} PRIVATE ${PROJECT_NAME} gtest gmock gtest_main)
	target_compile_options(${TESTNAME} PUBLIC -mtune=native -march=native -pipe)
	set_property(TARGET ${TESTNAME} PROPERTY CXX_EXTENSIONS OFF)
	if (COVERAGE)
		target_compile_options(${TESTNAME} PRIVATE --coverage)
		target_link_libraries(${TESTNAME} PRIVATE --coverage)
	endif()

	# gtest_discover_tests replaces gtest_add_tests,
	# see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
	gtest_discover_tests(${TESTNAME}
	                     # set a working directory so your project root so that you can find test data via paths relative to the project root
	                     WORKING_DIRECTORY ${PROJECT_DIR}
	                     PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
	                     )
	set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

file(COPY        ${CMAKE_CURRENT_SOURCE_DIR}/circuits
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

package_add_test(ddsim_test ${CMAKE_CURRENT_SOURCE_DIR}/test_simple.cpp)

