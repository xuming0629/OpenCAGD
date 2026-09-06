# # ============================================================
# # OpenCAGD geometry / CAGD core
# # Header-heavy by design while algorithms are learned and verified.
# # ============================================================
# add_library(
#     opencagd
#     STATIC
#         opencagd.cpp
# )

# add_library(OpenCAGD::OpenCAGD ALIAS opencagd)

# target_include_directories(
#     opencagd
#     PUBLIC
#         $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
#         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
# )

# target_compile_features(opencagd PUBLIC cxx_std_20)

# if(MSVC)
#     target_compile_options(opencagd PRIVATE /W4)
# else()
#     target_compile_options(opencagd PRIVATE -Wall -Wextra -Wpedantic)
# endif()

# # ============================================================
# # Optional embedded Python + Matplotlib adapter
# # This dependency never leaks into OpenCAGD::OpenCAGD.
# # ============================================================
# if(OPENCAGD_ENABLE_PYTHON_PLOTTING)
#     find_package(Python3 COMPONENTS Interpreter Development QUIET)

#     if(Python3_FOUND AND TARGET Python3::Python)
#         add_library(
#             opencagd_plotting
#             STATIC
#                 visualization/matplotlib.cpp
#         )
#         add_library(OpenCAGD::Plotting ALIAS opencagd_plotting)

#         target_include_directories(
#             opencagd_plotting
#             PUBLIC
#                 $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
#                 $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
#         )
#         target_link_libraries(
#             opencagd_plotting
#             PUBLIC OpenCAGD::OpenCAGD
#             PRIVATE Python3::Python
#         )
#         target_compile_features(opencagd_plotting PUBLIC cxx_std_20)

#         if(MSVC)
#             target_compile_options(opencagd_plotting PRIVATE /W4)
#         else()
#             target_compile_options(opencagd_plotting PRIVATE -Wall -Wextra -Wpedantic)
#         endif()

#         message(STATUS "Python plotting enabled: ${Python3_EXECUTABLE}")
#     else()
#         message(WARNING
#             "Python3 development files were not found. Core OpenCAGD will still build, "
#             "but OpenCAGD::Plotting and plotting examples are disabled.")
#     endif()
# endif()
# ============================================================
# OpenCAGD geometry / CAGD core
# Header-heavy by design while algorithms are learned and verified.
# ============================================================

add_library(
    opencagd
    STATIC
        opencagd.cpp
)

add_library(
    OpenCAGD::OpenCAGD
    ALIAS
    opencagd
)

target_include_directories(
    opencagd
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
)

target_compile_features(
    opencagd
    PUBLIC
        cxx_std_20
)

if(MSVC)
    target_compile_options(
        opencagd
        PRIVATE
            /W4
    )
else()
    target_compile_options(
        opencagd
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()


# ============================================================
# Optional embedded Python + Matplotlib adapter
#
# Core:
#
#     OpenCAGD::OpenCAGD
#
# does NOT depend on Python.
#
# Only:
#
#     OpenCAGD::Plotting
#
# depends on Python.
# ============================================================

if(OPENCAGD_ENABLE_PYTHON_PLOTTING)

    find_package(
        Python3
        COMPONENTS
            Interpreter
            Development
        QUIET
    )

    if(Python3_FOUND AND TARGET Python3::Python)

        # ----------------------------------------------------
        # Python / Matplotlib visualization backend
        #
        # IMPORTANT:
        #
        # matplotlib.cpp uses PythonRuntime, therefore
        # python_runtime.cpp MUST be part of this library.
        # ----------------------------------------------------

        add_library(
            opencagd_plotting
            STATIC
                visualization/python_runtime.cpp
                visualization/matplotlib.cpp
                visualization/matplotlib_figure.cpp
        )

        # 如果已经实现 MatplotlibFigure，再加入：
        #
        # 

        add_library(
            OpenCAGD::Plotting
            ALIAS
            opencagd_plotting
        )


        # ----------------------------------------------------
        # Include directories
        # ----------------------------------------------------

        target_include_directories(
            opencagd_plotting
            PUBLIC
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
        )


        # ----------------------------------------------------
        # Dependencies
        #
        # Plotting
        #    │
        #    ├── OpenCAGD Core
        #    │
        #    └── CPython
        #
        # Python dependency does NOT enter OpenCAGD::OpenCAGD.
        # ----------------------------------------------------

        target_link_libraries(
            opencagd_plotting
            PUBLIC
                OpenCAGD::OpenCAGD
                Python3::Python
        )


        # ----------------------------------------------------
        # C++ standard
        # ----------------------------------------------------

        target_compile_features(
            opencagd_plotting
            PUBLIC
                cxx_std_20
        )


        # ----------------------------------------------------
        # Compiler warnings
        # ----------------------------------------------------

        if(MSVC)

            target_compile_options(
                opencagd_plotting
                PRIVATE
                    /W4
            )

        else()

            target_compile_options(
                opencagd_plotting
                PRIVATE
                    -Wall
                    -Wextra
                    -Wpedantic
            )

        endif()


        # ----------------------------------------------------
        # Compile-time capability flag
        # ----------------------------------------------------

        target_compile_definitions(
            opencagd_plotting
            PUBLIC
                OPENCAGD_HAS_PYTHON_PLOTTING=1
        )


        message(
            STATUS
            "Python plotting enabled: ${Python3_EXECUTABLE}"
        )

        message(
            STATUS
            "Python version: ${Python3_VERSION}"
        )

        message(
            STATUS
            "Python include dirs: ${Python3_INCLUDE_DIRS}"
        )

    else()

        message(
            WARNING
            "Python3 development files were not found. "
            "Core OpenCAGD will still build, "
            "but OpenCAGD::Plotting and plotting examples are disabled."
        )

    endif()

endif()