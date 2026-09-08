opencagd_add_example(opencagd_plot_renderer_demo renderer_demo.cpp plot)
target_link_libraries(opencagd_plot_renderer_demo PRIVATE OpenCAGD::Plot OpenCAGD::Curve)
target_compile_definitions(opencagd_plot_renderer_demo PRIVATE OPENCAGD_EXAMPLE_SHOW=$<BOOL:${OPENCAGD_EXAMPLE_SHOW}> OPENCAGD_EXAMPLE_OUTPUT_DIR="${CMAKE_BINARY_DIR}/examples/output/plot")
