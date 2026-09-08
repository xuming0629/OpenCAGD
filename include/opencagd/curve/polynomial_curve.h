#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/curve/parametric_curve.h>

namespace opencagd {
namespace curve {

class OPENCAGD_CURVE_API PolynomialCurve : public ParametricCurve
{
public:
    explicit PolynomialCurve(
        const std::vector<geometry::Point3D>& coefficients,
        const geometry::Interval& domain = geometry::Interval(0.0, 1.0));

    std::size_t degree() const;
    const std::vector<geometry::Point3D>& coefficients() const;

    geometry::Interval domain() const override;
    geometry::Point3D evaluate(double t) const override;
    geometry::Vector3D derivative(double t, std::size_t order = 1) const override;

private:
    std::vector<geometry::Point3D> coefficients_;
    geometry::Interval domain_;
};

} // namespace curve
} // namespace opencagd
