add_library(opencagd_visualization
    builder.cpp
    render_options.cpp
    renderer.cpp)
target_link_libraries(opencagd_visualization PUBLIC opencagd_surface opencagd_core)
opencagd_apply_library_defaults(opencagd_visualization OPENCAGD_VISUALIZATION_EXPORTS Visualization)
