#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include <opencagd/curve/bspline_basis.hpp>
#include <opencagd/geometry/point.hpp>

namespace opencagd::surface
{
template <std::size_t Dim>
class BSplineSurface
{
public:
    using point_type = Point<double, Dim>;
    using control_net_type = std::vector<std::vector<point_type>>;

    BSplineSurface(std::size_t degree_u,
                   std::size_t degree_v,
                   std::vector<double> knots_u,
                   std::vector<double> knots_v,
                   control_net_type control_points)
        : knot_u_(degree_u, std::move(knots_u)),
          knot_v_(degree_v, std::move(knots_v)),
          control_points_(std::move(control_points))
    {
        validate_control_net();
    }

    [[nodiscard]] std::size_t degree_u() const noexcept { return knot_u_.degree(); }
    [[nodiscard]] std::size_t degree_v() const noexcept { return knot_v_.degree(); }
    [[nodiscard]] const curve::KnotVector& knot_vector_u() const noexcept { return knot_u_; }
    [[nodiscard]] const curve::KnotVector& knot_vector_v() const noexcept { return knot_v_; }
    [[nodiscard]] const control_net_type& control_points() const noexcept { return control_points_; }
    [[nodiscard]] double domain_u_min() const noexcept { return knot_u_.domain_min(); }
    [[nodiscard]] double domain_u_max() const noexcept { return knot_u_.domain_max(); }
    [[nodiscard]] double domain_v_min() const noexcept { return knot_v_.domain_min(); }
    [[nodiscard]] double domain_v_max() const noexcept { return knot_v_.domain_max(); }

    [[nodiscard]] point_type evaluate(double u, double v) const
    {
        const auto span_u = knot_u_.find_span(u);
        const auto span_v = knot_v_.find_span(v);
        const auto Nu = curve::basis_functions(knot_u_, span_u, u);
        const auto Nv = curve::basis_functions(knot_v_, span_v, v);
        const auto first_u = span_u - degree_u();
        const auto first_v = span_v - degree_v();

        point_type point{};
        for (std::size_t a = 0; a <= degree_u(); ++a)
            for (std::size_t b = 0; b <= degree_v(); ++b)
                point += control_points_[first_u + a][first_v + b] * (Nu[a] * Nv[b]);
        return point;
    }

private:
    void validate_control_net() const
    {
        if (control_points_.size() != knot_u_.num_basis_functions())
            throw std::invalid_argument("B-spline surface U control-point count mismatch");
        if (control_points_.empty())
            throw std::invalid_argument("B-spline surface requires a non-empty control net");
        const std::size_t cols = knot_v_.num_basis_functions();
        for (const auto& row : control_points_)
            if (row.size() != cols)
                throw std::invalid_argument("B-spline surface V control-point count mismatch");
    }

    curve::KnotVector knot_u_;
    curve::KnotVector knot_v_;
    control_net_type control_points_;
};
using BSplineSurface3d = BSplineSurface<3>;
} // namespace opencagd::surface
