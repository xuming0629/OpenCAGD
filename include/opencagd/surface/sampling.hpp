#pragma once

#include <cstddef>
#include <stdexcept>
#include <type_traits>

#include <opencagd/surface/surface_grid.hpp>

namespace opencagd::surface
{
template <typename Surface>
[[nodiscard]] auto sample_surface(
    const Surface& surface,
    std::size_t u_count,
    std::size_t v_count)
{
    using point_type = std::remove_cvref_t<decltype(surface.evaluate(
        surface.domain_u_min(), surface.domain_v_min()))>;

    if (u_count < 2 || v_count < 2)
        throw std::invalid_argument("Surface sampling requires at least 2 x 2 samples");

    SurfaceSampleGrid<point_type> grid;
    grid.u_count = u_count;
    grid.v_count = v_count;
    grid.points.reserve(u_count * v_count);

    for (std::size_t i = 0; i < u_count; ++i)
    {
        const double au = static_cast<double>(i) / static_cast<double>(u_count - 1);
        const double u = surface.domain_u_min() +
                         (surface.domain_u_max() - surface.domain_u_min()) * au;
        for (std::size_t j = 0; j < v_count; ++j)
        {
            const double av = static_cast<double>(j) / static_cast<double>(v_count - 1);
            const double v = surface.domain_v_min() +
                             (surface.domain_v_max() - surface.domain_v_min()) * av;
            grid.points.push_back(surface.evaluate(u, v));
        }
    }
    return grid;
}
} // namespace opencagd::surface
