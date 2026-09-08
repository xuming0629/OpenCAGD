#include <opencagd/surface/sampling.h>

#include <stdexcept>
#include <vector>

namespace opencagd {
namespace surface {

SurfaceSampleGrid sample_surface(
    const ParametricSurface& surface,
    std::size_t u_count,
    std::size_t v_count)
{
    if (u_count < 2 || v_count < 2)
        throw std::invalid_argument("Surface sampling requires at least a 2 x 2 grid");

    const geometry::SurfaceDomain d = surface.domain();
    std::vector<geometry::Point3D> points;
    points.reserve(u_count * v_count);

    for (std::size_t i = 0; i < u_count; ++i)
    {
        const double au = static_cast<double>(i) / static_cast<double>(u_count - 1);
        const double u = d.u.min + d.u.length() * au;
        for (std::size_t j = 0; j < v_count; ++j)
        {
            const double av = static_cast<double>(j) / static_cast<double>(v_count - 1);
            const double v = d.v.min + d.v.length() * av;
            points.push_back(surface.evaluate(u, v));
        }
    }
    return SurfaceSampleGrid(u_count, v_count, points);
}

} // namespace surface
} // namespace opencagd
