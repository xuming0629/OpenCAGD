#include <opencagd/surface/parametric_surface.h>
#include <opencagd/math/numeric.h>

#include <stdexcept>

namespace opencagd {
namespace surface {

ParametricSurface::~ParametricSurface() {}

void ParametricSurface::require_parameter(double u, double v) const
{
    if (!domain().contains(u, v, math::tolerance()))
        throw std::out_of_range("Surface parameter lies outside the valid domain");
}

geometry::Vector3D ParametricSurface::derivative_u(double u, double v) const
{
    require_parameter(u, v);
    return derivative(u, v, 1, 0);
}

geometry::Vector3D ParametricSurface::derivative_v(double u, double v) const
{
    require_parameter(u, v);
    return derivative(u, v, 0, 1);
}

geometry::Vector3D ParametricSurface::normal(double u, double v) const
{
    require_parameter(u, v);
    const geometry::Vector3D su = derivative(u, v, 1, 0);
    const geometry::Vector3D sv = derivative(u, v, 0, 1);
    const geometry::Vector3D n = geometry::cross(su, sv);
    if (n.norm() <= math::tolerance())
        throw std::runtime_error(
            "Surface normal is undefined because Su and Sv are linearly dependent");
    return n.normalized();
}

} // namespace surface
} // namespace opencagd
