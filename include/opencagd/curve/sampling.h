#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/curve/parametric_curve.h>

namespace opencagd {
namespace curve {

OPENCAGD_CURVE_API std::vector<geometry::Point3D> sample_curve(
    const ParametricCurve& curve,
    std::size_t count);

} // namespace curve
} // namespace opencagd
