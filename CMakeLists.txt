cmake_minimum_required(VERSION 3.16)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" OPENCAGD_VERSION_FILE)
string(STRIP "${OPENCAGD_VERSION_FILE}" OPENCAGD_VERSION)

project(OpenCAGD
    VERSION ${OPENCAGD_VERSION}
    DESCRIPTION "Portable C++14 CAGD, B-Spline and NURBS learning library"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(OpenCAGDOptions)
include(OpenCAGDHelpers)
include(OpenCAGDDependencies)

opencagd_configure_output_directories()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/opencagd/core/version.h"
    @ONLY)

add_subdirectory(src)

if(OPENCAGD_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(OPENCAGD_BUILD_TESTS)
    enable_testing()
    include(CTest)

    # OpenCAGD is intentionally C++14. GoogleTest 1.18+ requires C++17,
    # therefore tests are pinned to the C++14-compatible 1.14 release line.
    if(EXISTS "${OPENCAGD_THIRD_PARTY_DIR}/googletest/CMakeLists.txt")
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        add_subdirectory("${OPENCAGD_THIRD_PARTY_DIR}/googletest"
                         "${CMAKE_BINARY_DIR}/third_party/googletest" EXCLUDE_FROM_ALL)
    else()
        if(EXISTS "${OPENCAGD_THIRD_PARTY_DIR}/googletest/lib/cmake/GTest/GTestConfig.cmake")
            find_package(GTest 1.14 EXACT CONFIG QUIET
                PATHS "${OPENCAGD_THIRD_PARTY_DIR}/googletest/lib/cmake/GTest"
                NO_DEFAULT_PATH)
        endif()

        if(NOT TARGET GTest::gtest_main)
            find_package(GTest 1.14 EXACT CONFIG QUIET)
        endif()

        if(NOT TARGET GTest::gtest_main)
            if(OPENCAGD_FETCH_TEST_DEPS)
                include(FetchContent)
                FetchContent_Declare(googletest
                    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
                    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
                if(MSVC)
                    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
                endif()
                set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
                set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
                FetchContent_MakeAvailable(googletest)
            else()
                message(FATAL_ERROR
                    "GoogleTest 1.14 is required for the C++14 test suite. "
                    "The previously bundled GoogleTest 1.18 requires C++17. "
                    "Provide GoogleTest 1.14 or enable OPENCAGD_FETCH_TEST_DEPS.")
            endif()
        endif()
    endif()

    include(GoogleTest)
    add_subdirectory(tests)
endif()

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated/opencagd"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT OpenCAGDTargets
    FILE OpenCAGDTargets.cmake
    NAMESPACE OpenCAGD::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenCAGD)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OpenCAGDConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/OpenCAGDConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenCAGD)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/OpenCAGDConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/OpenCAGDConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/OpenCAGDConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenCAGD)
