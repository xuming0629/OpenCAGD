add_library(opencagd_vtk
    vtk_renderer.cpp)
target_link_libraries(opencagd_vtk
    PUBLIC opencagd_visualization
    PRIVATE ${VTK_LIBRARIES})
opencagd_apply_library_defaults(opencagd_vtk OPENCAGD_VTK_EXPORTS VTK)

if(COMMAND vtk_module_autoinit)
    vtk_module_autoinit(TARGETS opencagd_vtk MODULES ${VTK_LIBRARIES})
endif()
