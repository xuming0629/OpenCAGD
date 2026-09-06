#pragma once

#include <algorithm>    // std::max
#include <cmath>        // std::pow, std::sqrt
#include <cstddef>      // std::size_t
#include <stdexcept>    // std::invalid_argument
#include <utility>      // std::move
#include <vector>       // std::vector

#include <opencagd/curve/bspline_basis.hpp>
#include <opencagd/curve/bspline_derivatives.hpp>
#include <opencagd/geometry/point.hpp>
#include <opencagd/math/numeric.hpp>

namespace opencagd::curve
{

/**
 * @brief 任意维度的 B-spline 曲线
 *
 * B-spline 曲线定义为：
 *
 *      C(u) = Σ N_{i,p}(u) P_i
 *             i=0...n
 *
 * 其中：
 *
 *      P_i        : B-spline 控制点
 *      N_{i,p}(u) : 第 i 个 p 次 B-spline 基函数
 *      p          : 曲线次数
 *      u          : 曲线参数
 *      U          : knot vector（节点向量）
 *
 * B-spline 与 Bézier 曲线的重要区别：
 *
 * 1. B-spline 具有 knot vector；
 * 2. B-spline 基函数具有局部支撑性；
 * 3. 对任意参数 u，最多只有 p+1 个基函数非零；
 * 4. 修改一个控制点通常只影响曲线的一部分。
 *
 * @tparam Dim 几何空间维数，例如：
 *
 *      Dim = 2 -> 二维 B-spline 曲线
 *      Dim = 3 -> 三维 B-spline 曲线
 */
template <std::size_t Dim>
class BSplineCurve
{
public:
    /**
     * @brief B-spline 控制点类型
     */
    using point_type = Point<double, Dim>;

    /**
     * @brief 构造 B-spline 曲线
     *
     * @param degree 曲线次数 p
     * @param knots 节点向量 U
     * @param control_points 控制点数组 P0 ... Pn
     *
     * B-spline 中必须满足：
     *
     *      m = n + p + 1
     *
     * 其中：
     *
     *      m + 1 = knot 数量
     *      n + 1 = 控制点数量
     *      p     = degree
     *
     * 因而：
     *
     *      控制点数量
     *          = knot_count - degree - 1
     *
     * 同时：
     *
     *      控制点数量
     *          = B-spline 基函数数量
     */
    BSplineCurve(
        std::size_t degree,
        std::vector<double> knots,
        std::vector<point_type> control_points)
        : knot_vector_(
              degree,
              std::move(knots)),
          control_points_(
              std::move(control_points))
    {
        // -----------------------------------------------------
        // 每一个控制点 P_i 必须对应一个 B-spline 基函数：
        //
        //      C(u)
        //          = N_0,p(u) P_0
        //          + N_1,p(u) P_1
        //          + ...
        //          + N_n,p(u) P_n
        //
        // 因此：
        //
        //      control_points.size()
        //
        // 必须等于：
        //
        //      num_basis_functions()
        // -----------------------------------------------------
        if (control_points_.size()
            != knot_vector_.num_basis_functions())
        {
            throw std::invalid_argument(
                "B-spline control-point count must equal "
                "knot_count - degree - 1"
            );
        }
    }

    /**
     * @brief 返回 B-spline 曲线次数 p
     */
    [[nodiscard]]
    std::size_t degree() const noexcept
    {
        return knot_vector_.degree();
    }

    /**
     * @brief 返回曲线的节点向量
     */
    [[nodiscard]]
    const KnotVector& knot_vector() const noexcept
    {
        return knot_vector_;
    }

    /**
     * @brief 返回 B-spline 控制点
     */
    [[nodiscard]]
    const std::vector<point_type>&
    control_points() const noexcept
    {
        return control_points_;
    }

    /**
     * @brief 返回 B-spline 曲线参数域左端点
     *
     * 对标准 p 次 B-spline：
     *
     *      domain_min = U_p
     *
     * 即有效参数域通常是：
     *
     *      [U_p, U_{n+1}]
     */
    [[nodiscard]]
    double domain_min() const noexcept
    {
        return knot_vector_.domain_min();
    }

    /**
     * @brief 返回 B-spline 曲线参数域右端点
     */
    [[nodiscard]]
    double domain_max() const noexcept
    {
        return knot_vector_.domain_max();
    }

    /**
     * @brief 计算参数 u 对应的 B-spline 曲线点 C(u)
     *
     * B-spline 曲线完整定义：
     *
     *      C(u)
     *          = Σ N_{i,p}(u) P_i
     *            i=0...n
     *
     * 但是 B-spline 基函数具有局部支撑性。
     *
     * 如果：
     *
     *      span = i
     *
     * 且：
     *
     *      U_i <= u < U_{i+1}
     *
     * 那么只有：
     *
     *      N_{i-p,p}(u)
     *      ...
     *      N_{i,p}(u)
     *
     * 这 p+1 个基函数可能非零。
     *
     * 因此实际上只需要计算：
     *
     *      C(u)
     *        =
     *        Σ N_{i-p+j,p}(u) P_{i-p+j}
     *        j=0...p
     *
     * 这样就不需要计算所有 B-spline 基函数。
     *
     * @param u 曲线参数
     *
     * @return 曲线点 C(u)
     */
    [[nodiscard]]
    point_type evaluate(double u) const
    {
        // -----------------------------------------------------
        // 第一步：
        //
        // 找到参数 u 所在的 knot span。
        //
        // span 满足：
        //
        //      U_span <= u < U_{span+1}
        //
        // 参数域最右端通常由 KnotVector::find_span()
        // 做特殊处理。
        // -----------------------------------------------------
        const std::size_t span =
            knot_vector_.find_span(u);

        // -----------------------------------------------------
        // 第二步：
        //
        // 计算当前 span 上 p+1 个非零基函数。
        //
        // 返回：
        //
        //      basis[0]
        //          = N_{span-p,p}(u)
        //
        //      basis[1]
        //          = N_{span-p+1,p}(u)
        //
        //      ...
        //
        //      basis[p]
        //          = N_{span,p}(u)
        // -----------------------------------------------------
        const auto basis =
            basis_functions(
                knot_vector_,
                span,
                u
            );

        // -----------------------------------------------------
        // 当前参与计算的第一个控制点索引：
        //
        //      first = span - p
        //
        // 因此参与曲线计算的控制点为：
        //
        //      P_{span-p}
        //      ...
        //      P_span
        // -----------------------------------------------------
        const std::size_t first =
            span - degree();

        // 最终曲线点。
        point_type point{};

        // -----------------------------------------------------
        // 根据局部 B-spline 曲线公式：
        //
        //      C(u)
        //        =
        //        Σ basis[j] * P_{first+j}
        //
        //        j = 0...p
        //
        // 实际只需要 p+1 个控制点，
        // 而不是所有控制点。
        // -----------------------------------------------------
        for (std::size_t j = 0;
             j <= degree();
             ++j)
        {
            point +=
                control_points_[first + j]
                *
                basis[j];
        }

        return point;
    }

    /**
     * @brief 计算 B-spline 曲线从 0 阶到指定阶数的导数。
     *
     * 对于：
     *
     *      C(u) = Σ N_{i,p}(u) P_i
     *
     * 有：
     *
     *      C^(k)(u) = Σ N_{i,p}^(k)(u) P_i
     *
     * 返回数组：
     *
     *      result[0] = C(u)
     *      result[1] = C'(u)
     *      result[2] = C''(u)
     *      ...
     *
     * @param u 曲线参数
     * @param derivative_order 最高导数阶数
     */
    [[nodiscard]]
    std::vector<point_type> derivatives(
        double u,
        std::size_t derivative_order) const
    {
        const std::size_t span = knot_vector_.find_span(u);
        const auto ders = basis_function_derivatives(
            knot_vector_,
            span,
            u,
            derivative_order);

        const std::size_t first = span - degree();

        std::vector<point_type> result(
            derivative_order + 1,
            point_type{});

        for (std::size_t k = 0; k <= derivative_order; ++k)
        {
            for (std::size_t j = 0; j <= degree(); ++j)
            {
                result[k] +=
                    control_points_[first + j] * ders[k][j];
            }
        }

        return result;
    }

    /**
     * @brief 计算指定阶 B-spline 曲线导数。
     */
    [[nodiscard]]
    point_type derivative(
        double u,
        std::size_t order = 1) const
    {
        return derivatives(u, order)[order];
    }

    /**
     * @brief 计算单位切向量 T(u)。
     *
     *      T(u) = C'(u) / ||C'(u)||
     */
    [[nodiscard]]
    point_type tangent(double u) const
    {
        const point_type d1 = derivative(u, 1);
        const double speed = d1.norm();

        if (speed <= math::default_tolerance)
        {
            throw std::runtime_error(
                "B-spline tangent is undefined because the first derivative is zero");
        }

        return d1 / speed;
    }

    /**
     * @brief 计算曲率。
     *
     * 使用适用于二维和三维曲线的 Gram determinant 形式：
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
                "B-spline curvature is undefined because the first derivative is zero");
        }

        const double gram = std::max(
            0.0,
            speed2 * dot(d2, d2) - dot(d1, d2) * dot(d1, d2));

        return std::sqrt(gram) / std::pow(speed2, 1.5);
    }

private:
    /**
     * @brief B-spline 节点向量
     *
     * KnotVector 负责保存：
     *
     *      degree
     *      knots
     *
     * 并提供：
     *
     *      find_span()
     *      domain_min()
     *      domain_max()
     *      num_basis_functions()
     */
    KnotVector knot_vector_;

    /**
     * @brief B-spline 控制点
     *
     * 每个控制点 P_i 对应一个基函数：
     *
     *      N_{i,p}(u)
     */
    std::vector<point_type> control_points_;
};


/**
 * @brief 二维 B-spline 曲线
 */
using BSplineCurve2d = BSplineCurve<2>;


/**
 * @brief 三维 B-spline 曲线
 */
using BSplineCurve3d = BSplineCurve<3>;

} // namespace opencagd::curve

