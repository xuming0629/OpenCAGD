add_library(opencagd_plot
    matplotlib_renderer.cpp
    python_runtime.cpp)
target_include_directories(opencagd_plot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(opencagd_plot PUBLIC opencagd_visualization PRIVATE Python3::Python)
opencagd_apply_library_defaults(opencagd_plot OPENCAGD_PLOT_EXPORTS Plot)
