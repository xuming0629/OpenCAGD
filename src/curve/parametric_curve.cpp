#include <opencagd/curve/parametric_curve.h>
#include <opencagd/math/numeric.h>

#include <cmath>
#include <stdexcept>

namespace opencagd {
namespace curve {

ParametricCurve::~ParametricCurve() {}

void ParametricCurve::require_parameter(double u) const
{
    if (!domain().contains(u, math::tolerance()))
        throw std::out_of_range("Curve parameter lies outside the valid domain");
}

double ParametricCurve::speed(double u) const
{
    require_parameter(u);
    return derivative(u, 1).norm();
}

geometry::Vector3D ParametricCurve::tangent(double u) const
{
    require_parameter(u);
    return derivative(u, 1).normalized();
}

double ParametricCurve::curvature(double u) const
{
    require_parameter(u);
    const geometry::Vector3D d1 = derivative(u, 1);
    const geometry::Vector3D d2 = derivative(u, 2);
    const double speed_value = d1.norm();
    if (speed_value <= math::tolerance())
        throw std::runtime_error("Curve curvature is undefined because the first derivative is zero");

    return geometry::cross(d1, d2).norm() /
           (speed_value * speed_value * speed_value);
}

} // namespace curve
} // namespace opencagd
