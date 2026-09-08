#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/curve/knot_vector.h>
#include <opencagd/curve/parametric_curve.h>

namespace opencagd {
namespace curve {

class OPENCAGD_CURVE_API BSplineCurve : public ParametricCurve
{
public:
    BSplineCurve(std::size_t degree,
                 const std::vector<double>& knots,
                 const std::vector<geometry::Point3D>& control_points);

    std::size_t degree() const;
    const KnotVector& knot_vector() const;
    const std::vector<geometry::Point3D>& control_points() const;

    geometry::Interval domain() const override;
    geometry::Point3D evaluate(double u) const override;
    geometry::Vector3D derivative(double u, std::size_t order = 1) const override;

private:
    KnotVector knot_vector_;
    std::vector<geometry::Point3D> control_points_;
};

} // namespace curve
} // namespace opencagd
