#pragma once

#include <algorithm>
#include <cmath>

namespace opencagd::math
{

inline constexpr double default_tolerance = 1.0e-12;

[[nodiscard]] inline bool nearly_equal(
    double a,
    double b,
    double tolerance = default_tolerance) noexcept
{
    const double scale = std::max({1.0, std::abs(a), std::abs(b)});
    return std::abs(a - b) <= tolerance * scale;
}

[[nodiscard]] inline double clamp_parameter(double u, double umin, double umax) noexcept
{
    return std::clamp(u, umin, umax);
}

} // namespace opencagd::math
