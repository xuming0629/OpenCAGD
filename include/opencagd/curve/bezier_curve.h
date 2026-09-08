#pragma once

#include <cstddef>
#include <utility>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/curve/parametric_curve.h>

namespace opencagd {
namespace curve {

class OPENCAGD_CURVE_API BezierCurve : public ParametricCurve
{
public:
    explicit BezierCurve(const std::vector<geometry::Point3D>& control_points);

    std::size_t degree() const;
    const std::vector<geometry::Point3D>& control_points() const;

    geometry::Interval domain() const override;
    geometry::Point3D evaluate(double t) const override;
    geometry::Point3D evaluate_de_casteljau(double t) const;
    std::vector<std::vector<geometry::Point3D> > construction_levels(double t) const;
    geometry::Vector3D derivative(double t, std::size_t order = 1) const override;

    BezierCurve derivative_curve() const;
    std::pair<BezierCurve, BezierCurve> subdivide(double t) const;

private:
    std::vector<geometry::Point3D> control_points_;
};

} // namespace curve
} // namespace opencagd
