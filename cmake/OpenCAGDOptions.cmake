option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries" OFF)
option(OPENCAGD_BUILD_EXAMPLES "Build examples" ON)
option(OPENCAGD_BUILD_TESTS "Build tests" ON)
option(OPENCAGD_FETCH_TEST_DEPS "Fetch GoogleTest if neither bundled nor installed" ON)
option(OPENCAGD_EXAMPLE_SHOW "Open interactive windows when visualization examples run" ON)

option(OPENCAGD_ENABLE_EIGEN "Build the Eigen-backed linear algebra adapter" ON)
option(OPENCAGD_ENABLE_MPI "Build the MPI adapter library" OFF)
option(OPENCAGD_ENABLE_PLOT "Build the Python/Matplotlib renderer backend" OFF)
option(OPENCAGD_ENABLE_VTK "Build the VTK renderer backend" OFF)

option(OPENCAGD_USE_BUNDLED_EIGEN "Prefer third_party/eigen-5.0" ON)
option(OPENCAGD_USE_BUNDLED_BINARY_DEPS
       "Prefer bundled binary MPI/VTK packages. The supplied binaries are macOS arm64 builds."
       ${APPLE})

set(OPENCAGD_THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/third_party"
    CACHE PATH "OpenCAGD third-party dependency root")

# Backward-compatible switch from the previous project layout.
if(DEFINED OPENCAGD_ENABLE_PYTHON_PLOTTING)
    if(OPENCAGD_ENABLE_PYTHON_PLOTTING)
        set(OPENCAGD_ENABLE_PLOT ON CACHE BOOL "Build Matplotlib renderer" FORCE)
    endif()
endif()
