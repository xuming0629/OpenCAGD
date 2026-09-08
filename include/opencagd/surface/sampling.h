#pragma once

#include <cstddef>
#include <opencagd/core/export.h>
#include <opencagd/surface/parametric_surface.h>
#include <opencagd/surface/surface_grid.h>

namespace opencagd {
namespace surface {

OPENCAGD_SURFACE_API SurfaceSampleGrid sample_surface(
    const ParametricSurface& surface,
    std::size_t u_count,
    std::size_t v_count);

} // namespace surface
} // namespace opencagd
