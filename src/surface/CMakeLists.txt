add_library(opencagd_surface
    bezier_surface.cpp
    bspline_surface.cpp
    differential_geometry.cpp
    nurbs_surface.cpp
    parametric_surface.cpp
    sampling.cpp
    surface_grid.cpp)
target_link_libraries(opencagd_surface PUBLIC opencagd_curve opencagd_core)
opencagd_apply_library_defaults(opencagd_surface OPENCAGD_SURFACE_EXPORTS Surface)
