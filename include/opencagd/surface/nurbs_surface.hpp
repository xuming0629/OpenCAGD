#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include <opencagd/curve/bspline_basis.hpp>
#include <opencagd/curve/bspline_derivatives.hpp>
#include <opencagd/geometry/point.hpp>
#include <opencagd/math/binomial.hpp>
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
    [[nodiscard]] const curve::KnotVector& knot_vector_u() const noexcept { return knot_u_; }
    [[nodiscard]] const curve::KnotVector& knot_vector_v() const noexcept { return knot_v_; }
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

    /**
     * @brief 计算 NURBS 曲面的混合偏导。
     *
     * 将曲面写成：
     *
     *      S(u,v) = A(u,v) / W(u,v)
     *
     * 先计算 A 与 W 的各阶偏导，再由二维乘积求导公式递推恢复
     * 有理曲面的导数。
     */
    [[nodiscard]]
    point_type derivative(
        double u,
        double v,
        std::size_t order_u,
        std::size_t order_v) const
    {
        const auto span_u = knot_u_.find_span(u);
        const auto span_v = knot_v_.find_span(v);

        const auto Nu = curve::basis_function_derivatives(
            knot_u_, span_u, u, order_u);
        const auto Nv = curve::basis_function_derivatives(
            knot_v_, span_v, v, order_v);

        const auto first_u = span_u - degree_u();
        const auto first_v = span_v - degree_v();

        std::vector<std::vector<point_type>> A(
            order_u + 1,
            std::vector<point_type>(order_v + 1, point_type{}));

        std::vector<std::vector<double>> W(
            order_u + 1,
            std::vector<double>(order_v + 1, 0.0));

        for (std::size_t ku = 0; ku <= order_u; ++ku)
        {
            for (std::size_t kv = 0; kv <= order_v; ++kv)
            {
                for (std::size_t a = 0; a <= degree_u(); ++a)
                {
                    for (std::size_t b = 0; b <= degree_v(); ++b)
                    {
                        const std::size_t iu = first_u + a;
                        const std::size_t iv = first_v + b;
                        const double factor =
                            Nu[ku][a] * Nv[kv][b] * weights_[iu][iv];

                        A[ku][kv] += control_points_[iu][iv] * factor;
                        W[ku][kv] += factor;
                    }
                }
            }
        }

        if (std::abs(W[0][0]) <= math::default_tolerance)
        {
            throw std::runtime_error(
                "NURBS surface derivative denominator is numerically zero");
        }

        std::vector<std::vector<point_type>> S(
            order_u + 1,
            std::vector<point_type>(order_v + 1, point_type{}));

        // 由 A = W * S 的二维 Leibniz 公式递推。
        // 按 ku、kv 递增时，右侧所需的 S(ku-i, kv-j) 均已计算。
        for (std::size_t ku = 0; ku <= order_u; ++ku)
        {
            for (std::size_t kv = 0; kv <= order_v; ++kv)
            {
                point_type value = A[ku][kv];

                for (std::size_t i = 0; i <= ku; ++i)
                {
                    for (std::size_t j = 0; j <= kv; ++j)
                    {
                        if (i == 0 && j == 0)
                        {
                            continue;
                        }

                        const double coefficient =
                            static_cast<double>(math::binomial(ku, i)) *
                            static_cast<double>(math::binomial(kv, j)) *
                            W[i][j];

                        value -= S[ku - i][kv - j] * coefficient;
                    }
                }

                S[ku][kv] = value / W[0][0];
            }
        }

        return S[order_u][order_v];
    }

    [[nodiscard]] point_type partial_u(double u, double v) const
    {
        return derivative(u, v, 1, 0);
    }

    [[nodiscard]] point_type partial_v(double u, double v) const
    {
        return derivative(u, v, 0, 1);
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
