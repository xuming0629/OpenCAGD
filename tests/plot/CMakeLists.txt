opencagd_add_gtest(opencagd_test_plot test_plot.cpp plot)
target_link_libraries(opencagd_test_plot PRIVATE OpenCAGD::Plot)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests/output")
target_compile_definitions(opencagd_test_plot PRIVATE OPENCAGD_TEST_OUTPUT_DIR="${CMAKE_BINARY_DIR}/tests/output")
