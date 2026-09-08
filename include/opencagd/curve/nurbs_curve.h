#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/curve/knot_vector.h>
#include <opencagd/curve/parametric_curve.h>

namespace opencagd {
namespace curve {

class OPENCAGD_CURVE_API NURBSCurve : public ParametricCurve
{
public:
    NURBSCurve(std::size_t degree,
               const std::vector<double>& knots,
               const std::vector<geometry::Point3D>& control_points,
               const std::vector<double>& weights);

    std::size_t degree() const;
    const KnotVector& knot_vector() const;
    const std::vector<geometry::Point3D>& control_points() const;
    const std::vector<double>& weights() const;

    geometry::Interval domain() const override;
    geometry::Point3D evaluate(double u) const override;
    geometry::Vector3D derivative(double u, std::size_t order = 1) const override;

    std::vector<double> rational_basis_functions(double u) const;

private:
    void validate() const;

    KnotVector knot_vector_;
    std::vector<geometry::Point3D> control_points_;
    std::vector<double> weights_;
};

} // namespace curve
} // namespace opencagd
