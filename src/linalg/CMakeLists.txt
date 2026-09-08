add_library(opencagd_linalg
    dense_matrix.cpp
    linear_solver.cpp)
get_target_property(OPENCAGD_EIGEN_INCLUDE_DIRS Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(opencagd_linalg SYSTEM PRIVATE ${OPENCAGD_EIGEN_INCLUDE_DIRS})
target_link_libraries(opencagd_linalg PUBLIC opencagd_core)
opencagd_apply_library_defaults(opencagd_linalg OPENCAGD_LINALG_EXPORTS LinearAlgebra)
