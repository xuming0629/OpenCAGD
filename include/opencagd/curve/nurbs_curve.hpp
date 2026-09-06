#pragma once

#include <algorithm>    // std::max
#include <cmath>        // std::abs, std::pow, std::sqrt
#include <cstddef>      // std::size_t
#include <stdexcept>    // std::invalid_argument, std::runtime_error
#include <utility>      // std::move
#include <vector>       // std::vector

#include <opencagd/curve/bspline_basis.hpp>
#include <opencagd/curve/bspline_derivatives.hpp>
#include <opencagd/geometry/point.hpp>
#include <opencagd/math/binomial.hpp>
#include <opencagd/math/numeric.hpp>

namespace opencagd::curve
{

/**
 * @brief 任意维度的 NURBS 曲线
 *
 * NURBS：
 *
 *      Non-Uniform Rational B-Spline
 *
 * 即：
 *
 *      非均匀有理 B 样条曲线
 *
 * NURBS 曲线定义为：
 *
 *             Σ N_{i,p}(u) w_i P_i
 *             i=0...n
 *      C(u) = ------------------------
 *               Σ N_{i,p}(u) w_i
 *               i=0...n
 *
 * 其中：
 *
 *      P_i        : 控制点
 *      w_i        : 控制点权重
 *      N_{i,p}(u) : p 次 B-spline 基函数
 *      p          : 曲线次数
 *      U          : 节点向量
 *      u          : 参数
 *
 * 定义有理基函数：
 *
 *                     N_{i,p}(u) w_i
 *      R_{i,p}(u) = -------------------
 *                    Σ N_{j,p}(u) w_j
 *
 * 那么 NURBS 曲线也可以写成：
 *
 *      C(u) = Σ R_{i,p}(u) P_i
 *
 * 当所有权重：
 *
 *      w_i = 1
 *
 * 时：
 *
 *      R_{i,p}(u) = N_{i,p}(u)
 *
 * 因此 NURBS 退化为普通 B-spline 曲线。
 *
 * @tparam Dim 几何空间维数
 */
template <std::size_t Dim>
class NURBSCurve
{
public:
    /**
     * @brief 控制点类型
     *
     * Dim = 2：
     *
     *      Point<double, 2>
     *
     * Dim = 3：
     *
     *      Point<double, 3>
     */
    using point_type = Point<double, Dim>;

    /**
     * @brief 构造 NURBS 曲线
     *
     * @param degree 曲线次数 p
     * @param knots 节点向量
     * @param control_points 控制点
     * @param weights 每个控制点对应的权重
     *
     * 必须满足：
     *
     *      控制点数量
     *          = B-spline 基函数数量
     *
     * 即：
     *
     *      control_point_count
     *          = knot_count - degree - 1
     *
     * 同时：
     *
     *      weights.size()
     *          = control_points.size()
     */
    NURBSCurve(
        std::size_t degree,
        std::vector<double> knots,
        std::vector<point_type> control_points,
        std::vector<double> weights)
        : knot_vector_(
              degree,
              std::move(knots)),
          control_points_(
              std::move(control_points)),
          weights_(
              std::move(weights))
    {
        // -----------------------------------------------------
        // 每个 B-spline 基函数必须对应一个控制点。
        //
        // NURBS 曲线仍然建立在 B-spline 基函数之上：
        //
        //      N_0,p ↔ P_0
        //      N_1,p ↔ P_1
        //      ...
        //      N_n,p ↔ P_n
        // -----------------------------------------------------
        if (control_points_.size()
            != knot_vector_.num_basis_functions())
        {
            throw std::invalid_argument(
                "NURBS control-point count must equal "
                "knot_count - degree - 1"
            );
        }

        // -----------------------------------------------------
        // 每一个控制点必须拥有一个对应权重：
        //
        //      P_i ↔ w_i
        // -----------------------------------------------------
        if (weights_.size()
            != control_points_.size())
        {
            throw std::invalid_argument(
                "NURBS requires one weight per control point"
            );
        }

        // -----------------------------------------------------
        // 当前 OpenCAGD 学习实现只允许正权重：
        //
        //      w_i > 0
        //
        // 正权重是 CAD / NURBS 中最常见的情况，
        // 同时能够保持良好的几何性质。
        // -----------------------------------------------------
        for (double weight : weights_)
        {
            if (weight <= 0.0)
            {
                throw std::invalid_argument(
                    "This learning implementation requires "
                    "positive NURBS weights"
                );
            }
        }
    }

    /**
     * @brief 返回 NURBS 曲线次数 p
     */
    [[nodiscard]]
    std::size_t degree() const noexcept
    {
        return knot_vector_.degree();
    }

    /**
     * @brief 返回节点向量
     */
    [[nodiscard]]
    const KnotVector& knot_vector() const noexcept
    {
        return knot_vector_;
    }

    /**
     * @brief 返回控制点
     */
    [[nodiscard]]
    const std::vector<point_type>&
    control_points() const noexcept
    {
        return control_points_;
    }

    /**
     * @brief 返回所有 NURBS 权重
     */
    [[nodiscard]]
    const std::vector<double>& weights() const noexcept
    {
        return weights_;
    }

    /**
     * @brief 返回有效参数域左端
     */
    [[nodiscard]]
    double domain_min() const noexcept
    {
        return knot_vector_.domain_min();
    }

    /**
     * @brief 返回有效参数域右端
     */
    [[nodiscard]]
    double domain_max() const noexcept
    {
        return knot_vector_.domain_max();
    }

    /**
     * @brief 计算参数 u 下所有 NURBS 有理基函数
     *
     * 普通 B-spline 基函数：
     *
     *      N_{i,p}(u)
     *
     * NURBS 有理基函数定义：
     *
     *                     N_{i,p}(u) w_i
     *      R_{i,p}(u) = -------------------
     *                    Σ N_{j,p}(u) w_j
     *
     * 返回：
     *
     *      [
     *          R_0,p(u),
     *          R_1,p(u),
     *          ...
     *          R_n,p(u)
     *      ]
     *
     * 这些有理基函数仍然满足：
     *
     *      Σ R_{i,p}(u) = 1
     *
     * @param u 曲线参数
     *
     * @return 所有 NURBS 有理基函数值
     */
    [[nodiscard]]
    std::vector<double>
    rational_basis_functions(double u) const
    {
        // -----------------------------------------------------
        // 首先计算普通 B-spline 基函数：
        //
        //      [
        //          N_0,p(u),
        //          ...
        //          N_n,p(u)
        //      ]
        // -----------------------------------------------------
        const auto nonrational =
            all_basis_functions(
                knot_vector_,
                u
            );

        // 保存最终有理基函数：
        //
        //      R_i,p(u)
        std::vector<double> rational(
            nonrational.size(),
            0.0
        );

        // -----------------------------------------------------
        // NURBS 公共分母：
        //
        //      W(u)
        //        = Σ N_{i,p}(u) w_i
        //
        // 这个量有时也称为：
        //
        //      weight function
        // -----------------------------------------------------
        double denominator = 0.0;

        for (std::size_t i = 0;
             i < nonrational.size();
             ++i)
        {
            denominator +=
                nonrational[i]
                *
                weights_[i];
        }

        // -----------------------------------------------------
        // 理论上，如果：
        //
        //      w_i > 0
        //
        // 且 B-spline 基函数具有单位分解性质：
        //
        //      Σ N_i,p = 1
        //
        // 那么 denominator 应该始终 > 0。
        //
        // 这里仍然保留数值安全检查。
        // -----------------------------------------------------
        if (std::abs(denominator) < 1.0e-15)
        {
            throw std::runtime_error(
                "NURBS rational basis denominator is zero"
            );
        }

        // -----------------------------------------------------
        // 计算：
        //
        //                     N_i,p(u) w_i
        //      R_i,p(u) = ------------------
        //                       W(u)
        // -----------------------------------------------------
        for (std::size_t i = 0;
             i < nonrational.size();
             ++i)
        {
            rational[i] =
                nonrational[i]
                *
                weights_[i]
                /
                denominator;
        }

        return rational;
    }

    /**
     * @brief 计算参数 u 对应的 NURBS 曲线点
     *
     * NURBS 曲线：
     *
     *             Σ N_i,p(u) w_i P_i
     *      C(u) = ----------------------
     *               Σ N_i,p(u) w_i
     *
     * 由于 B-spline 基函数具有局部支撑性，
     * 不需要计算所有控制点。
     *
     * 如果：
     *
     *      span = i
     *
     * 那么只需要：
     *
     *      P_{i-p}, ..., P_i
     *
     * 共 p+1 个控制点。
     *
     * 因此当前实现比：
     *
     *      rational_basis_functions()
     *
     * 再对全部控制点求和更加高效。
     *
     * @param u 参数
     *
     * @return NURBS 曲线点 C(u)
     */
    [[nodiscard]]
    point_type evaluate(double u) const
    {
        // -----------------------------------------------------
        // 第一步：
        //
        // 找到参数 u 所在 knot span：
        //
        //      U_span <= u < U_{span+1}
        // -----------------------------------------------------
        const std::size_t span =
            knot_vector_.find_span(u);

        // -----------------------------------------------------
        // 第二步：
        //
        // 只计算当前 span 上非零的 p+1 个
        // B-spline 基函数。
        //
        // basis[j] 对应：
        //
        //      N_{span-p+j,p}(u)
        // -----------------------------------------------------
        const auto basis =
            basis_functions(
                knot_vector_,
                span,
                u
            );

        // -----------------------------------------------------
        // 第一个参与求值的控制点索引：
        //
        //      first = span - p
        // -----------------------------------------------------
        const std::size_t first =
            span - degree();

        // -----------------------------------------------------
        // NURBS 分子：
        //
        //      Σ N_i,p(u) w_i P_i
        // -----------------------------------------------------
        point_type numerator{};

        // -----------------------------------------------------
        // NURBS 分母：
        //
        //      Σ N_i,p(u) w_i
        // -----------------------------------------------------
        double denominator = 0.0;

        // -----------------------------------------------------
        // 由于局部支撑性，
        // 只遍历 p+1 个非零基函数。
        // -----------------------------------------------------
        for (std::size_t j = 0;
             j <= degree();
             ++j)
        {
            // 当前全局控制点 / 权重索引。
            const std::size_t index =
                first + j;

            // -------------------------------------------------
            // 当前控制点的非归一化有理权重：
            //
            //      N_i,p(u) * w_i
            // -------------------------------------------------
            const double rational_factor =
                basis[j]
                *
                weights_[index];

            // 累加分子：
            //
            //      Σ N_i,p w_i P_i
            numerator +=
                control_points_[index]
                *
                rational_factor;

            // 累加分母：
            //
            //      Σ N_i,p w_i
            denominator +=
                rational_factor;
        }

        // 数值安全检查。
        if (std::abs(denominator) < 1.0e-15)
        {
            throw std::runtime_error(
                "NURBS rational denominator is zero"
            );
        }

        // -----------------------------------------------------
        // 最终：
        //
        //             numerator
        //      C(u) = -----------
        //            denominator
        // -----------------------------------------------------
        return numerator / denominator;
    }

    /**
     * @brief 计算 NURBS 曲线从 0 阶到指定阶数的导数。
     *
     * 先把 NURBS 写成齐次分子 / 权函数：
     *
     *      C(u) = A(u) / W(u)
     *
     * 其中：
     *
     *      A(u) = Σ N_i,p(u) w_i P_i
     *      W(u) = Σ N_i,p(u) w_i
     *
     * 根据乘积求导：
     *
     *      A^(k) = Σ C(k,i) W^(i) C^(k-i)
     *
     * 可递推得到有理曲线各阶导数。
     */
    [[nodiscard]]
    std::vector<point_type> derivatives(
        double u,
        std::size_t derivative_order) const
    {
        const std::size_t span = knot_vector_.find_span(u);
        const auto basis_ders = basis_function_derivatives(
            knot_vector_,
            span,
            u,
            derivative_order);

        const std::size_t first = span - degree();

        // A^(k)：加权控制点分子的导数。
        std::vector<point_type> A(
            derivative_order + 1,
            point_type{});

        // W^(k)：权函数导数。
        std::vector<double> W(
            derivative_order + 1,
            0.0);

        for (std::size_t k = 0; k <= derivative_order; ++k)
        {
            for (std::size_t j = 0; j <= degree(); ++j)
            {
                const std::size_t index = first + j;
                const double factor =
                    basis_ders[k][j] * weights_[index];

                A[k] += control_points_[index] * factor;
                W[k] += factor;
            }
        }

        if (std::abs(W[0]) <= math::default_tolerance)
        {
            throw std::runtime_error(
                "NURBS derivative denominator is numerically zero");
        }

        std::vector<point_type> result(
            derivative_order + 1,
            point_type{});

        // 0 阶就是曲线点。
        result[0] = A[0] / W[0];

        for (std::size_t k = 1; k <= derivative_order; ++k)
        {
            point_type value = A[k];

            for (std::size_t i = 1; i <= k; ++i)
            {
                value -=
                    result[k - i] *
                    (static_cast<double>(math::binomial(k, i)) * W[i]);
            }

            result[k] = value / W[0];
        }

        return result;
    }

    /**
     * @brief 计算指定阶 NURBS 曲线导数。
     */
    [[nodiscard]]
    point_type derivative(
        double u,
        std::size_t order = 1) const
    {
        return derivatives(u, order)[order];
    }

    /**
     * @brief 计算单位切向量。
     */
    [[nodiscard]]
    point_type tangent(double u) const
    {
        const point_type d1 = derivative(u, 1);
        const double speed = d1.norm();

        if (speed <= math::default_tolerance)
        {
            throw std::runtime_error(
                "NURBS tangent is undefined because the first derivative is zero");
        }

        return d1 / speed;
    }

    /**
     * @brief 计算 NURBS 曲率。
     *
     *      κ = sqrt(||C'||² ||C''||² - (C'·C'')²) / ||C'||³
     */
    [[nodiscard]]
    double curvature(double u) const
    {
        const auto ders = derivatives(u, 2);
        const point_type& d1 = ders[1];
        const point_type& d2 = ders[2];

        const double speed2 = dot(d1, d1);
        if (speed2 <= math::default_tolerance * math::default_tolerance)
        {
            throw std::runtime_error(
                "NURBS curvature is undefined because the first derivative is zero");
        }

        const double d12 = dot(d1, d2);
        const double gram = std::max(
            0.0,
            speed2 * dot(d2, d2) - d12 * d12);

        return std::sqrt(gram) / std::pow(speed2, 1.5);
    }

private:
    /**
     * @brief NURBS 节点向量
     */
    KnotVector knot_vector_;

    /**
     * @brief NURBS 控制点
     */
    std::vector<point_type> control_points_;

    /**
     * @brief 每个控制点对应的权重
     *
     *      P_i ↔ w_i
     */
    std::vector<double> weights_;
};


/**
 * @brief 二维 NURBS 曲线
 */
using NURBSCurve2d = NURBSCurve<2>;


/**
 * @brief 三维 NURBS 曲线
 */
using NURBSCurve3d = NURBSCurve<3>;

} // namespace opencagd::curve

