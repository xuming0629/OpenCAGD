add_subdirectory(core)
add_subdirectory(curve)
add_subdirectory(surface)

if(OPENCAGD_ENABLE_EIGEN)
    add_subdirectory(linalg)
endif()

add_subdirectory(visualization)

if(OPENCAGD_ENABLE_PLOT)
    add_subdirectory(plot)
endif()

if(OPENCAGD_ENABLE_VTK)
    add_subdirectory(vtk)
endif()

if(OPENCAGD_ENABLE_MPI)
    add_subdirectory(parallel)
endif()

add_library(opencagd_all INTERFACE)
add_library(OpenCAGD::OpenCAGD ALIAS opencagd_all)
set_target_properties(opencagd_all PROPERTIES EXPORT_NAME OpenCAGD)
target_link_libraries(opencagd_all INTERFACE
    opencagd_core
    opencagd_curve
    opencagd_surface
    opencagd_visualization)
if(OPENCAGD_ENABLE_EIGEN)
    target_link_libraries(opencagd_all INTERFACE opencagd_linalg)
endif()
install(TARGETS opencagd_all EXPORT OpenCAGDTargets)
