function(opencagd_add_plot_example target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE OpenCAGD::OpenCAGD OpenCAGD::Plotting)
endfunction()

opencagd_add_plot_example(opencagd_plot_bernstein 00_plot_bernstein.cpp)
opencagd_add_plot_example(opencagd_plot_bezier 01_plot_bezier.cpp)
opencagd_add_plot_example(opencagd_plot_bspline_basis 02_plot_bspline_basis.cpp)
opencagd_add_plot_example(opencagd_plot_nurbs_circle 03_plot_nurbs_circle.cpp)
