add_library(opencagd_core
    domain.cpp
    numeric.cpp
    point3d.cpp
    power_basis.cpp
    vector3d.cpp)
opencagd_apply_library_defaults(opencagd_core OPENCAGD_CORE_EXPORTS Core)
