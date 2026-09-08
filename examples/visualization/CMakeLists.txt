opencagd_add_example(opencagd_visualization_scene_data scene_data.cpp visualization)
target_link_libraries(opencagd_visualization_scene_data PRIVATE OpenCAGD::Visualization OpenCAGD::Surface)

set(OPENCAGD_VISUAL_EXAMPLES
    00_bernstein_basis
    01_bezier_curve
    02_bezier_de_casteljau
    03_bezier_subdivision
    04_bspline_basis
    05_bspline_curve
    06_nurbs_circle
    07_bezier_surface
    08_bspline_surface
    09_nurbs_cylinder_normals)
set(OPENCAGD_EXAMPLE_OUTPUT_ROOT "${CMAKE_BINARY_DIR}/examples/output")
file(MAKE_DIRECTORY "${OPENCAGD_EXAMPLE_OUTPUT_ROOT}/plot")
file(MAKE_DIRECTORY "${OPENCAGD_EXAMPLE_OUTPUT_ROOT}/vtk")
foreach(example_name IN LISTS OPENCAGD_VISUAL_EXAMPLES)
    if(OPENCAGD_ENABLE_PLOT)
        set(target_name "opencagd_plot_${example_name}")
        opencagd_add_example(${target_name} ${example_name}.cpp plot)
        target_include_directories(${target_name} PRIVATE "${PROJECT_SOURCE_DIR}/examples/common")
        target_compile_definitions(${target_name} PRIVATE OPENCAGD_EXAMPLE_BACKEND_PLOT=1 OPENCAGD_EXAMPLE_OUTPUT_DIR="${OPENCAGD_EXAMPLE_OUTPUT_ROOT}" OPENCAGD_EXAMPLE_SHOW=$<BOOL:${OPENCAGD_EXAMPLE_SHOW}>)
        target_link_libraries(${target_name} PRIVATE OpenCAGD::Plot OpenCAGD::Curve OpenCAGD::Surface)
    endif()
    if(OPENCAGD_ENABLE_VTK)
        set(target_name "opencagd_vtk_${example_name}")
        opencagd_add_example(${target_name} ${example_name}.cpp vtk)
        target_include_directories(${target_name} PRIVATE "${PROJECT_SOURCE_DIR}/examples/common")
        target_compile_definitions(${target_name} PRIVATE OPENCAGD_EXAMPLE_BACKEND_VTK=1 OPENCAGD_EXAMPLE_OUTPUT_DIR="${OPENCAGD_EXAMPLE_OUTPUT_ROOT}" OPENCAGD_EXAMPLE_SHOW=$<BOOL:${OPENCAGD_EXAMPLE_SHOW}>)
        target_link_libraries(${target_name} PRIVATE OpenCAGD::VTK OpenCAGD::Curve OpenCAGD::Surface)
        if(COMMAND vtk_module_autoinit)
            vtk_module_autoinit(TARGETS ${target_name} MODULES ${VTK_LIBRARIES})
        endif()
    endif()
endforeach()
if(OPENCAGD_ENABLE_PLOT AND OPENCAGD_ENABLE_VTK)
    opencagd_add_example(opencagd_backend_switch 10_backend_switch.cpp visualization)
    target_compile_definitions(opencagd_backend_switch PRIVATE OPENCAGD_EXAMPLE_OUTPUT_DIR="${OPENCAGD_EXAMPLE_OUTPUT_ROOT}" OPENCAGD_EXAMPLE_SHOW=$<BOOL:${OPENCAGD_EXAMPLE_SHOW}>)
    target_link_libraries(opencagd_backend_switch PRIVATE OpenCAGD::Plot OpenCAGD::VTK OpenCAGD::Curve OpenCAGD::Surface)
    if(COMMAND vtk_module_autoinit)
        vtk_module_autoinit(TARGETS opencagd_backend_switch MODULES ${VTK_LIBRARIES})
    endif()
endif()
