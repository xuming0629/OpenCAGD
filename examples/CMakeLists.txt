# ============================================================
# OpenCAGD Examples
# ============================================================

# ============================================================
# Helper functions
# ============================================================

# Core example:
#
#   Only depends on OpenCAGD core.
#
function(opencagd_add_example name source)
    add_executable(
        ${name}
        ${source}
    )

    target_link_libraries(
        ${name}
        PRIVATE
            OpenCAGD::OpenCAGD
    )
endfunction()


# Plotting example:
#
#   OpenCAGD::Plotting already PUBLIC depends on:
#
#       OpenCAGD::OpenCAGD
#       Python3::Python
#
# Therefore plotting examples only link OpenCAGD::Plotting.
# This avoids linking libopencagd.a twice on macOS.
#
function(opencagd_add_plotting_example name source)
    add_executable(
        ${name}
        ${source}
    )

    target_link_libraries(
        ${name}
        PRIVATE
            OpenCAGD::Plotting
    )
endfunction()


# ============================================================
# 00. Basic examples
# ============================================================

opencagd_add_example(
    opencagd_hello
    basic/00_hello.cpp
)


# ============================================================
# 01. Book learning track
#
# The learning track follows the mathematical dependency order
# used in the book.
# ============================================================

add_subdirectory(book)


# ============================================================
# 02. Visualization examples
#
# OpenCAGD::Plotting is created only when embedded Python / 
# Matplotlib support was configured successfully.
#
# Checking the target is more robust than checking an option
# alone, because Python development files may be unavailable.
# ============================================================

if(TARGET OpenCAGD::Plotting)

    # --------------------------------------------------------
    # 02.1 Basis / classic curve plotting examples
    # --------------------------------------------------------

    opencagd_add_plotting_example(
        opencagd_plot_bernstein
        visualization/00_plot_bernstein.cpp
    )

    opencagd_add_plotting_example(
        opencagd_plot_bezier
        visualization/01_plot_bezier.cpp
    )

    opencagd_add_plotting_example(
        opencagd_plot_bspline_basis
        visualization/02_plot_bspline_basis.cpp
    )

    opencagd_add_plotting_example(
        opencagd_plot_nurbs_circle
        visualization/03_plot_nurbs_circle.cpp
    )


    # --------------------------------------------------------
    # 02.2 Curve visualization examples
    # --------------------------------------------------------

    opencagd_add_plotting_example(
        opencagd_bezier_curve
        visualization/01_curve/01_bezier_curve.cpp
    )

    opencagd_add_plotting_example(
        opencagd_bspline_curve
        visualization/01_curve/02_bspline_curve.cpp
    )

    opencagd_add_plotting_example(
        opencagd_nurbs_quarter_circle
        visualization/01_curve/03_nurbs_quarter_circle.cpp
    )


    # --------------------------------------------------------
    # 02.3 Surface visualization examples
    # --------------------------------------------------------

    opencagd_add_plotting_example(
        opencagd_bezier_surface
        visualization/02_surface/10_bezier_surface.cpp
    )

    opencagd_add_plotting_example(
        opencagd_bspline_surface
        visualization/02_surface/11_bspline_surface.cpp
    )

    opencagd_add_plotting_example(
        opencagd_nurbs_surface
        visualization/02_surface/12_nurbs_surface.cpp
    )

    opencagd_add_plotting_example(
        opencagd_nurbs_quarter_cylinder
        visualization/02_surface/13_nurbs_quarter_cylinder.cpp
    )

else()

    message(
        STATUS
        "Skipping visualization examples because OpenCAGD::Plotting is unavailable"
    )

endif()


# ============================================================
# 03. MPI examples
# ============================================================

if(OPENCAGD_ENABLE_MPI)

    add_executable(
        opencagd_mpi_hello
        mpi/00_hello_mpi.cpp
    )

    target_link_libraries(
        opencagd_mpi_hello
        PRIVATE
            OpenCAGD::OpenCAGD
            OpenCAGD::MPI
    )


    add_executable(
        opencagd_mpi_reduce
        mpi/01_reduce.cpp
    )

    target_link_libraries(
        opencagd_mpi_reduce
        PRIVATE
            OpenCAGD::OpenCAGD
            OpenCAGD::MPI
    )

endif()
