#pragma once

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace opencagd::curve
{
template <typename Evaluator>
[[nodiscard]] auto sample_parameter_range(
    Evaluator&& evaluator,
    double umin,
    double umax,
    std::size_t sample_count)
{
    using point_type = std::remove_cvref_t<decltype(evaluator(umin))>;
    if (sample_count < 2)
        throw std::invalid_argument("Curve sampling requires at least two samples");

    std::vector<point_type> points;
    points.reserve(sample_count);
    for (std::size_t i = 0; i < sample_count; ++i)
    {
        const double alpha = static_cast<double>(i) / static_cast<double>(sample_count - 1);
        const double u = umin + (umax - umin) * alpha;
        points.push_back(evaluator(u));
    }
    return points;
}

template <typename Curve>
[[nodiscard]] auto sample_curve(const Curve& curve, std::size_t sample_count)
{
    return sample_parameter_range(
        [&curve](double u) { return curve.evaluate(u); },
        curve.domain_min(), curve.domain_max(), sample_count);
}
} // namespace opencagd::curve
