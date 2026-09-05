# ============================================================
# OpenCAGD geometry / CAGD core
# Header-heavy by design while algorithms are learned and verified.
# ============================================================
add_library(
    opencagd
    STATIC
        opencagd.cpp
)

add_library(OpenCAGD::OpenCAGD ALIAS opencagd)

target_include_directories(
    opencagd
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
)

target_compile_features(opencagd PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(opencagd PRIVATE /W4)
else()
    target_compile_options(opencagd PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ============================================================
# Optional embedded Python + Matplotlib adapter
# This dependency never leaks into OpenCAGD::OpenCAGD.
# ============================================================
if(OPENCAGD_ENABLE_PYTHON_PLOTTING)
    find_package(Python3 COMPONENTS Interpreter Development QUIET)

    if(Python3_FOUND AND TARGET Python3::Python)
        add_library(
            opencagd_plotting
            STATIC
                visualization/matplotlib.cpp
        )
        add_library(OpenCAGD::Plotting ALIAS opencagd_plotting)

        target_include_directories(
            opencagd_plotting
            PUBLIC
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
        )
        target_link_libraries(
            opencagd_plotting
            PUBLIC OpenCAGD::OpenCAGD
            PRIVATE Python3::Python
        )
        target_compile_features(opencagd_plotting PUBLIC cxx_std_20)

        if(MSVC)
            target_compile_options(opencagd_plotting PRIVATE /W4)
        else()
            target_compile_options(opencagd_plotting PRIVATE -Wall -Wextra -Wpedantic)
        endif()

        message(STATUS "Python plotting enabled: ${Python3_EXECUTABLE}")
    else()
        message(WARNING
            "Python3 development files were not found. Core OpenCAGD will still build, "
            "but OpenCAGD::Plotting and plotting examples are disabled.")
    endif()
endif()
