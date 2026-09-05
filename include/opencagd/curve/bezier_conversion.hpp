#pragma once

#include <cstddef>
#include <vector>

#include <opencagd/curve/bezier_curve.hpp>
#include <opencagd/curve/polynomial_curve.hpp>
#include <opencagd/math/binomial.hpp>

namespace opencagd::curve
{

template <std::size_t Dim>
[[nodiscard]] PolynomialCurve<Dim> to_power_basis(const BezierCurve<Dim>& curve)
{
    using point_type = typename BezierCurve<Dim>::point_type;
    const std::size_t n = curve.degree();
    std::vector<point_type> coefficients(n + 1, point_type{});

    // B_i^n(t) = C(n,i) t^i (1-t)^(n-i)
    // Expand (1-t)^(n-i), then collect equal powers of t.
    for (std::size_t i = 0; i <= n; ++i)
    {
        for (std::size_t j = 0; j <= n - i; ++j)
        {
            const std::size_t k = i + j;
            const double sign = (j % 2 == 0) ? 1.0 : -1.0;
            const double factor =
                static_cast<double>(opencagd::math::binomial(n, i)) *
                static_cast<double>(opencagd::math::binomial(n - i, j)) * sign;
            coefficients[k] += curve.control_points()[i] * factor;
        }
    }

    return PolynomialCurve<Dim>(std::move(coefficients));
}

} // namespace opencagd::curve
