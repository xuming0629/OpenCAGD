#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include <opencagd/curve/bspline_basis.hpp>
#include <opencagd/geometry/point.hpp>
#include <opencagd/math/numeric.hpp>

namespace opencagd::surface
{
template <std::size_t Dim>
class NURBSSurface
{
public:
    using point_type = Point<double, Dim>;
    using control_net_type = std::vector<std::vector<point_type>>;
    using weight_net_type = std::vector<std::vector<double>>;

    NURBSSurface(std::size_t degree_u,
                 std::size_t degree_v,
                 std::vector<double> knots_u,
                 std::vector<double> knots_v,
                 control_net_type control_points,
                 weight_net_type weights)
        : knot_u_(degree_u, std::move(knots_u)),
          knot_v_(degree_v, std::move(knots_v)),
          control_points_(std::move(control_points)),
          weights_(std::move(weights))
    {
        validate();
    }

    [[nodiscard]] std::size_t degree_u() const noexcept { return knot_u_.degree(); }
    [[nodiscard]] std::size_t degree_v() const noexcept { return knot_v_.degree(); }
    [[nodiscard]] const control_net_type& control_points() const noexcept { return control_points_; }
    [[nodiscard]] const weight_net_type& weights() const noexcept { return weights_; }
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

        point_type numerator{};
        double denominator = 0.0;
        for (std::size_t a = 0; a <= degree_u(); ++a)
        {
            for (std::size_t b = 0; b <= degree_v(); ++b)
            {
                const std::size_t iu = first_u + a;
                const std::size_t iv = first_v + b;
                const double factor = Nu[a] * Nv[b] * weights_[iu][iv];
                numerator += control_points_[iu][iv] * factor;
                denominator += factor;
            }
        }
        if (std::abs(denominator) <= math::default_tolerance)
            throw std::runtime_error("NURBS surface denominator is numerically zero");
        return numerator / denominator;
    }

private:
    void validate() const
    {
        const std::size_t rows = knot_u_.num_basis_functions();
        const std::size_t cols = knot_v_.num_basis_functions();
        if (control_points_.size() != rows || weights_.size() != rows)
            throw std::invalid_argument("NURBS surface U control/weight count mismatch");
        for (std::size_t i = 0; i < rows; ++i)
        {
            if (control_points_[i].size() != cols || weights_[i].size() != cols)
                throw std::invalid_argument("NURBS surface V control/weight count mismatch");
            for (double w : weights_[i])
                if (w <= 0.0)
                    throw std::invalid_argument("This learning implementation requires positive NURBS weights");
        }
    }

    curve::KnotVector knot_u_;
    curve::KnotVector knot_v_;
    control_net_type control_points_;
    weight_net_type weights_;
};
using NURBSSurface3d = NURBSSurface<3>;
} // namespace opencagd::surface
