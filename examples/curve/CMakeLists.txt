set(OPENCAGD_CURVE_EXAMPLES
    00_bernstein_basis
    01_bezier_curve
    02_bspline_curve
    03_nurbs_quarter_circle
    04_polymorphic_curves)
foreach(name IN LISTS OPENCAGD_CURVE_EXAMPLES)
    opencagd_add_example(opencagd_curve_${name} ${name}.cpp curve)
    target_link_libraries(opencagd_curve_${name} PRIVATE OpenCAGD::Curve)
endforeach()
