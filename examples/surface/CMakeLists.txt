set(OPENCAGD_SURFACE_EXAMPLES
    00_bezier_surface
    01_bspline_surface
    02_nurbs_quarter_cylinder
    03_differential_geometry)
foreach(name IN LISTS OPENCAGD_SURFACE_EXAMPLES)
    opencagd_add_example(opencagd_surface_${name} ${name}.cpp surface)
    target_link_libraries(opencagd_surface_${name} PRIVATE OpenCAGD::Surface)
endforeach()
