#pragma once

#include <opencagd/core/export.h>
#include <opencagd/curve/bezier_curve.h>
#include <opencagd/curve/polynomial_curve.h>

namespace opencagd {
namespace curve {

OPENCAGD_CURVE_API PolynomialCurve to_power_basis(const BezierCurve& curve);

} // namespace curve
} // namespace opencagd
