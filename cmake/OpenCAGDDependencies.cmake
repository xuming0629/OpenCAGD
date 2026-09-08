# Dependency policy:
#   * Eigen is used as a portable header-only implementation dependency.
#   * The supplied OpenMPI and VTK trees are prebuilt macOS arm64 packages.
#     They are preferred only when OPENCAGD_USE_BUNDLED_BINARY_DEPS=ON.
#   * Linux/Windows normally resolve MPI/VTK from the system, package manager,
#     vcpkg, conda, or a user-provided CMAKE_PREFIX_PATH/VTK_DIR.

if(OPENCAGD_ENABLE_EIGEN)
    if(OPENCAGD_USE_BUNDLED_EIGEN AND NOT Eigen3_DIR AND
       EXISTS "${OPENCAGD_THIRD_PARTY_DIR}/eigen-5.0/share/eigen3/cmake/Eigen3Config.cmake")
        set(Eigen3_DIR
            "${OPENCAGD_THIRD_PARTY_DIR}/eigen-5.0/share/eigen3/cmake"
            CACHE PATH "Bundled Eigen3 package directory")
    endif()
    find_package(Eigen3 5.0 CONFIG REQUIRED)
endif()

if(OPENCAGD_ENABLE_MPI)
    if(OPENCAGD_USE_BUNDLED_BINARY_DEPS AND NOT MPI_CXX_COMPILER AND
       EXISTS "${OPENCAGD_THIRD_PARTY_DIR}/openmpi/bin/mpicxx")
        set(MPI_CXX_COMPILER
            "${OPENCAGD_THIRD_PARTY_DIR}/openmpi/bin/mpicxx"
            CACHE FILEPATH "Bundled MPI C++ compiler wrapper")
    endif()
    find_package(MPI REQUIRED COMPONENTS CXX)
endif()

if(OPENCAGD_ENABLE_PLOT)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
endif()

if(OPENCAGD_ENABLE_VTK)
    if(OPENCAGD_USE_BUNDLED_BINARY_DEPS AND NOT VTK_DIR AND
       EXISTS "${OPENCAGD_THIRD_PARTY_DIR}/vtk-9.4/lib/cmake/vtk-9.4/vtk-config.cmake")
        set(VTK_DIR
            "${OPENCAGD_THIRD_PARTY_DIR}/vtk-9.4/lib/cmake/vtk-9.4"
            CACHE PATH "Bundled VTK package directory")
    endif()
    find_package(VTK 9.4 REQUIRED COMPONENTS
        CommonCore
        CommonDataModel
        FiltersCore
        FiltersSources
        InteractionStyle
        IOImage
        RenderingCore
        RenderingOpenGL2
        RenderingAnnotation)
endif()

