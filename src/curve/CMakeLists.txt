add_library(opencagd_curve
    bernstein.cpp
    bezier_conversion.cpp
    bezier_curve.cpp
    bspline_basis.cpp
    bspline_curve.cpp
    knot_vector.cpp
    nurbs_curve.cpp
    parametric_curve.cpp
    polynomial_curve.cpp
    sampling.cpp)
target_link_libraries(opencagd_curve PUBLIC opencagd_core)
opencagd_apply_library_defaults(opencagd_curve OPENCAGD_CURVE_EXPORTS Curve)
