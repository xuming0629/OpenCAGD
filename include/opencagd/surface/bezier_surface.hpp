#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include <opencagd/curve/bernstein.hpp>
#include <opencagd/geometry/point.hpp>

namespace opencagd::surface
{
template <std::size_t Dim>
class BezierSurface
{
public:
    using point_type = Point<double, Dim>;
    using control_net_type = std::vector<std::vector<point_type>>;

    explicit BezierSurface(control_net_type control_points)
        : control_points_(std::move(control_points))
    {
        validate_control_net();
    }

    [[nodiscard]] std::size_t degree_u() const noexcept { return control_points_.size() - 1; }
    [[nodiscard]] std::size_t degree_v() const noexcept { return control_points_.front().size() - 1; }
    [[nodiscard]] constexpr double domain_u_min() const noexcept { return 0.0; }
    [[nodiscard]] constexpr double domain_u_max() const noexcept { return 1.0; }
    [[nodiscard]] constexpr double domain_v_min() const noexcept { return 0.0; }
    [[nodiscard]] constexpr double domain_v_max() const noexcept { return 1.0; }
    [[nodiscard]] const control_net_type& control_points() const noexcept { return control_points_; }

    [[nodiscard]] point_type evaluate(double u, double v) const
    {
        if (u < 0.0 || u > 1.0 || v < 0.0 || v > 1.0)
            throw std::out_of_range("BezierSurface parameters must be in [0,1] x [0,1]");
        const auto bu = curve::bernstein_basis(degree_u(), u);
        const auto bv = curve::bernstein_basis(degree_v(), v);
        point_type point{};
        for (std::size_t i = 0; i <= degree_u(); ++i)
            for (std::size_t j = 0; j <= degree_v(); ++j)
                point += control_points_[i][j] * (bu[i] * bv[j]);
        return point;
    }

private:
    void validate_control_net() const
    {
        if (control_points_.empty() || control_points_.front().empty())
            throw std::invalid_argument("BezierSurface requires a non-empty control net");
        const std::size_t cols = control_points_.front().size();
        for (const auto& row : control_points_)
            if (row.size() != cols)
                throw std::invalid_argument("BezierSurface control net must be rectangular");
    }

    control_net_type control_points_;
};
using BezierSurface3d = BezierSurface<3>;
} // namespace opencagd::surface
