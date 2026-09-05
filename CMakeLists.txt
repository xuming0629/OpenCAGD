cmake_minimum_required(VERSION 3.20)

# ============================================================
# Version
# VERSION is the single source of truth for OpenCAGD.
# ============================================================
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" OPENCAGD_VERSION_FILE)
string(STRIP "${OPENCAGD_VERSION_FILE}" OPENCAGD_VERSION)

project(
    OpenCAGD
    VERSION ${OPENCAGD_VERSION}
    DESCRIPTION "A modern C++20 learning and research library for CAGD, B-Spline and NURBS"
    LANGUAGES CXX
)

message(STATUS "OpenCAGD version: ${PROJECT_VERSION}")

# ============================================================
# Global build settings
# ============================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if(APPLE)
    set(_opencagd_default_bundled_deps ON)
    set(_opencagd_default_mpi ON)
else()
    set(_opencagd_default_bundled_deps OFF)
    set(_opencagd_default_mpi OFF)
endif()

option(OPENCAGD_BUILD_EXAMPLES "Build OpenCAGD examples" ON)
option(OPENCAGD_BUILD_TESTS "Build OpenCAGD tests" ON)
option(OPENCAGD_ENABLE_MPI "Enable optional MPI layer/examples/tests" ${_opencagd_default_mpi})
option(OPENCAGD_ENABLE_PYTHON_PLOTTING "Enable embedded Python/Matplotlib plotting" ON)
option(OPENCAGD_USE_BUNDLED_DEPS "Use compatible dependencies under third_party" ${_opencagd_default_bundled_deps})

set(
    OPENCAGD_MPI_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/openmpi"
    CACHE PATH
    "Open MPI installation root"
)

set(
    OPENCAGD_GTEST_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest"
    CACHE PATH
    "GoogleTest installation root"
)

# ============================================================
# Generated version header
# ============================================================
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/opencagd/core/version.hpp"
    @ONLY
)

# ============================================================
# Optional MPI dependency
# Geometry / B-Spline / NURBS never depends on MPI.
# ============================================================
if(OPENCAGD_ENABLE_MPI)
    add_library(opencagd_mpi INTERFACE)
    add_library(OpenCAGD::MPI ALIAS opencagd_mpi)

    if(OPENCAGD_USE_BUNDLED_DEPS)
        if(NOT APPLE)
            message(FATAL_ERROR
                "The bundled Open MPI in this repository is a macOS Apple-Silicon build. "
                "Use -DOPENCAGD_ENABLE_MPI=OFF or -DOPENCAGD_USE_BUNDLED_DEPS=OFF on this platform.")
        endif()

        if(NOT EXISTS "${OPENCAGD_MPI_ROOT}/include/mpi.h")
            message(FATAL_ERROR "Bundled MPI header not found: ${OPENCAGD_MPI_ROOT}/include/mpi.h")
        endif()

        set(OPENCAGD_MPI_LIBRARY "${OPENCAGD_MPI_ROOT}/lib/libmpi.dylib")
        if(NOT EXISTS "${OPENCAGD_MPI_LIBRARY}")
            message(FATAL_ERROR "Bundled MPI library not found: ${OPENCAGD_MPI_LIBRARY}")
        endif()

        target_include_directories(opencagd_mpi SYSTEM INTERFACE "${OPENCAGD_MPI_ROOT}/include")
        target_link_libraries(opencagd_mpi INTERFACE "${OPENCAGD_MPI_LIBRARY}")

        set(OPENCAGD_MPIEXEC_EXECUTABLE "${OPENCAGD_MPI_ROOT}/bin/mpiexec")
        set(OPENCAGD_MPIEXEC_NUMPROC_FLAG "-n")
        message(STATUS "Using bundled Open MPI: ${OPENCAGD_MPI_ROOT}")
    else()
        find_package(MPI REQUIRED COMPONENTS CXX)
        target_link_libraries(opencagd_mpi INTERFACE MPI::MPI_CXX)
        set(OPENCAGD_MPIEXEC_EXECUTABLE "${MPIEXEC_EXECUTABLE}")
        set(OPENCAGD_MPIEXEC_NUMPROC_FLAG "${MPIEXEC_NUMPROC_FLAG}")
        message(STATUS "Using system MPI: ${MPI_CXX_VERSION}")
    endif()
endif()

# ============================================================
# Project modules
# ============================================================
add_subdirectory(src)

if(OPENCAGD_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(OPENCAGD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
