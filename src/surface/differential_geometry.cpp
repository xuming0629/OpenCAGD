#include <opencagd/surface/differential_geometry.h>
#include <opencagd/math/numeric.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace opencagd {
namespace surface {

SurfaceDifferentialGeometry::SurfaceDifferentialGeometry()
    : point(), du(), dv(), duu(), duv(), dvv(), normal(),
      E(0.0), F(0.0), G(0.0), e(0.0), f(0.0), g(0.0),
      gaussian_curvature(0.0), mean_curvature(0.0),
      principal_curvature_max(0.0), principal_curvature_min(0.0)
{
}

SurfaceDifferentialGeometry differential_geometry(
    const ParametricSurface& surface,
    double u,
    double v)
{
    SurfaceDifferentialGeometry result;
    result.point = surface.evaluate(u, v);
    result.du = surface.derivative(u, v, 1, 0);
    result.dv = surface.derivative(u, v, 0, 1);
    result.duu = surface.derivative(u, v, 2, 0);
    result.duv = surface.derivative(u, v, 1, 1);
    result.dvv = surface.derivative(u, v, 0, 2);
    result.normal = surface.normal(u, v);

    result.E = geometry::dot(result.du, result.du);
    result.F = geometry::dot(result.du, result.dv);
    result.G = geometry::dot(result.dv, result.dv);
    result.e = geometry::dot(result.normal, result.duu);
    result.f = geometry::dot(result.normal, result.duv);
    result.g = geometry::dot(result.normal, result.dvv);

    const double metric = result.E * result.G - result.F * result.F;
    if (std::abs(metric) <= math::tolerance())
        throw std::runtime_error("Surface curvature is undefined at a singular parameterization");

    result.gaussian_curvature =
        (result.e * result.g - result.f * result.f) / metric;
    result.mean_curvature =
        (result.E * result.g - 2.0 * result.F * result.f + result.G * result.e) /
        (2.0 * metric);

    const double discriminant = std::max(
        0.0,
        result.mean_curvature * result.mean_curvature - result.gaussian_curvature);
    const double root = std::sqrt(discriminant);
    result.principal_curvature_max = result.mean_curvature + root;
    result.principal_curvature_min = result.mean_curvature - root;
    return result;
}

} // namespace surface
} // namespace opencagd
