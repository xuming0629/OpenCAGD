# ============================================================
# GoogleTest discovery
# The bundled package is a macOS Apple-Silicon binary build.
# ============================================================
set(_opencagd_has_gtest OFF)

if(APPLE AND OPENCAGD_USE_BUNDLED_DEPS)
    set(_opencagd_gtest_config_dir "${OPENCAGD_GTEST_ROOT}/lib/cmake/GTest")
    if(EXISTS "${_opencagd_gtest_config_dir}/GTestConfig.cmake")
        find_package(GTest CONFIG QUIET PATHS "${_opencagd_gtest_config_dir}" NO_DEFAULT_PATH)
    endif()
else()
    find_package(GTest QUIET)
endif()

if(GTest_FOUND)
    set(_opencagd_has_gtest ON)
else()
    message(WARNING
        "GoogleTest was not found for this platform. OpenCAGD library/examples remain buildable; "
        "install GTest or use the bundled macOS arm64 package to enable tests.")
endif()

if(NOT _opencagd_has_gtest)
    return()
endif()

add_executable(
    opencagd_tests
    core/test_basic.cpp
    curve/test_bezier.cpp
    curve/test_bspline.cpp
    curve/test_nurbs.cpp
    surface/test_differential_geometry.cpp
)

target_link_libraries(
    opencagd_tests
    PRIVATE
        OpenCAGD::OpenCAGD
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(opencagd_tests DISCOVERY_TIMEOUT 30)

# MPI tests are deliberately isolated from normal unit tests.
if(OPENCAGD_ENABLE_MPI)
    add_executable(
        opencagd_mpi_tests
        test_main.cpp
        parallel/test_mpi.cpp
    )
    target_link_libraries(
        opencagd_mpi_tests
        PRIVATE
            OpenCAGD::OpenCAGD
            OpenCAGD::MPI
            GTest::gtest
    )

    add_test(
        NAME OpenCAGD.MPI2
        COMMAND
            "${OPENCAGD_MPIEXEC_EXECUTABLE}"
            "${OPENCAGD_MPIEXEC_NUMPROC_FLAG}" 2
            $<TARGET_FILE:opencagd_mpi_tests>
            --gtest_filter=OpenCAGDMPI.MultiRank
    )
    set_tests_properties(OpenCAGD.MPI2 PROPERTIES PROCESSORS 2)

    if(APPLE AND OPENCAGD_USE_BUNDLED_DEPS)
        set(
            _opencagd_mpi2_test_env
            "PATH=${OPENCAGD_MPI_ROOT}/bin:$ENV{PATH}"
            "DYLD_LIBRARY_PATH=${OPENCAGD_MPI_ROOT}/lib:$ENV{DYLD_LIBRARY_PATH}"
            "OPAL_PREFIX=${OPENCAGD_MPI_ROOT}"
            "OPENCAGD_EXPECT_MPI_SIZE=2"
        )
        set_property(TEST OpenCAGD.MPI2 PROPERTY ENVIRONMENT "${_opencagd_mpi2_test_env}")
    else()
        set_property(TEST OpenCAGD.MPI2 PROPERTY ENVIRONMENT "OPENCAGD_EXPECT_MPI_SIZE=2")
    endif()
endif()
