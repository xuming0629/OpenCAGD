#pragma once

#include <cstddef>      // std::size_t
#include <stdexcept>    // std::invalid_argument
#include <utility>      // std::move, std::pair
#include <vector>       // std::vector

#include <opencagd/curve/bernstein.hpp>
#include <opencagd/geometry/point.hpp>

namespace opencagd::curve
{

/**
 * @brief 任意维度的 Bézier 曲线
 *
 * Bézier 曲线定义为：
 *
 *      C(t) = Σ B_i^n(t) P_i
 *             i=0...n
 *
 * 其中：
 *
 *      P_i      : 控制点
 *      B_i^n(t) : n 次 Bernstein 基函数
 *      n        : Bézier 曲线次数
 *      t        : 曲线参数，t ∈ [0, 1]
 *
 * 如果有 n + 1 个控制点，则 Bézier 曲线次数为 n。
 *
 * 例如：
 *
 *      2 个控制点 -> 1 次 Bézier 曲线（直线）
 *      3 个控制点 -> 2 次 Bézier 曲线
 *      4 个控制点 -> 3 次 Bézier 曲线
 *
 * @tparam Dim 几何空间维数，例如 2 或 3
 */
template <std::size_t Dim>
class BezierCurve
{
public:
    /**
     * @brief Bézier 控制点类型
     *
     * 例如：
     *
     *      Dim = 2 -> Point<double, 2>
     *      Dim = 3 -> Point<double, 3>
     */
    using point_type = Point<double, Dim>;

    /**
     * @brief 使用控制点构造 Bézier 曲线
     *
     * @param control_points Bézier 控制点
     *
     * Bézier 曲线至少需要一个控制点。
     *
     * 一个控制点表示 0 次 Bézier 曲线：
     *
     *      C(t) = P0
     */
    explicit BezierCurve(std::vector<point_type> control_points)
        : control_points_(std::move(control_points))
    {
        if (control_points_.empty())
        {
            throw std::invalid_argument(
                "BezierCurve requires at least one control point"
            );
        }
    }

    /**
     * @brief 返回 Bézier 曲线次数
     *
     * 如果控制点个数为 N，则：
     *
     *      degree = N - 1
     */
    [[nodiscard]]
    std::size_t degree() const noexcept
    {
        return control_points_.size() - 1;
    }

    /**
     * @brief 获取 Bézier 曲线控制点
     */
    [[nodiscard]]
    const std::vector<point_type>& control_points() const noexcept
    {
        return control_points_;
    }

    /**
     * @brief 返回 Bézier 曲线参数域左端点
     *
     * 标准 Bézier 曲线定义于：
     *
     *      t ∈ [0, 1]
     *
     * 该接口主要用于与 BSplineCurve、NURBSCurve
     * 保持统一，从而支持通用：
     *
     *      sample_curve(curve, count)
     */
    [[nodiscard]]
    static constexpr double domain_min() noexcept
    {
        return 0.0;
    }

    /**
     * @brief 返回 Bézier 曲线参数域右端点
     *
     * 标准 Bézier 曲线定义于：
     *
     *      t ∈ [0, 1]
     */
    [[nodiscard]]
    static constexpr double domain_max() noexcept
    {
        return 1.0;
    }

    /**
     * @brief 使用 Bernstein 基函数直接计算 Bézier 曲线点
     *
     * Bézier 曲线数学定义：
     *
     *      C(t) = Σ B_i^n(t) P_i
     *
     * 其中：
     *
     *      B_i^n(t)
     *        = C(n,i) t^i (1-t)^(n-i)
     *
     * @param t 曲线参数，通常位于 [0, 1]
     *
     * @return 参数 t 对应的曲线点 C(t)
     */
    [[nodiscard]]
    point_type evaluate(double t) const
    {
        const auto basis =
            bernstein_basis(
                degree(),
                t
            );

        point_type point{};

        for (std::size_t i = 0;
             i < control_points_.size();
             ++i)
        {
            point +=
                control_points_[i]
                *
                basis[i];
        }

        return point;
    }

    /**
     * @brief 使用 De Casteljau 算法计算 Bézier 曲线点
     *
     * De Casteljau 递推：
     *
     *      P_i^r
     *        = (1-t) P_i^(r-1)
     *        + t P_(i+1)^(r-1)
     *
     * 最终：
     *
     *      P_0^n = C(t)
     *
     * @param t 曲线参数
     *
     * @return 曲线点 C(t)
     */
    [[nodiscard]]
    point_type evaluate_de_casteljau(double t) const
    {
        auto work = control_points_;

        const std::size_t n =
            degree();

        for (std::size_t r = 1;
             r <= n;
             ++r)
        {
            for (std::size_t i = 0;
                 i <= n - r;
                 ++i)
            {
                work[i] =
                    work[i] * (1.0 - t)
                    +
                    work[i + 1] * t;
            }
        }

        return work.front();
    }

    /**
     * @brief 构造 Bézier 曲线的一阶导数曲线
     *
     * 原曲线：
     *
     *      C(t)
     *        = Σ B_i^n(t) P_i
     *
     * 一阶导数：
     *
     *      C'(t)
     *        = n Σ B_i^(n-1)(t)
     *              (P_{i+1} - P_i)
     *
     * 因此导数曲线控制点为：
     *
     *      Q_i
     *        = n(P_{i+1} - P_i)
     *
     * @return Bézier 导数曲线
     */
    [[nodiscard]]
    BezierCurve derivative() const
    {
        if (degree() == 0)
        {
            return BezierCurve({
                point_type{}
            });
        }

        std::vector<point_type> derivative_points;

        derivative_points.reserve(
            degree()
        );

        const double n =
            static_cast<double>(
                degree()
            );

        for (std::size_t i = 0;
             i < degree();
             ++i)
        {
            derivative_points.push_back(
                (
                    control_points_[i + 1]
                    -
                    control_points_[i]
                ) * n
            );
        }

        return BezierCurve(
            std::move(
                derivative_points
            )
        );
    }

    /**
     * @brief 在参数 t 处将 Bézier 曲线细分为左右两段
     *
     * 使用 De Casteljau 算法。
     *
     * De Casteljau 三角形左边界构成左曲线控制点，
     * 右边界构成右曲线控制点。
     *
     * @param t 分割参数
     *
     * @return {左侧 Bézier 曲线, 右侧 Bézier 曲线}
     */
    [[nodiscard]]
    std::pair<BezierCurve, BezierCurve>
    subdivide(double t) const
    {
        const std::size_t n =
            degree();

        std::vector<point_type> work =
            control_points_;

        std::vector<point_type> left(
            n + 1
        );

        std::vector<point_type> right(
            n + 1
        );

        // 左曲线起点 = 原曲线起点。
        left[0] =
            work[0];

        // 右曲线终点 = 原曲线终点。
        right[n] =
            work[n];

        for (std::size_t r = 1;
             r <= n;
             ++r)
        {
            for (std::size_t i = 0;
                 i <= n - r;
                 ++i)
            {
                work[i] =
                    work[i] * (1.0 - t)
                    +
                    work[i + 1] * t;
            }

            // De Casteljau 三角形左边界。
            left[r] =
                work[0];

            // De Casteljau 三角形右边界。
            right[n - r] =
                work[n - r];
        }

        return {
            BezierCurve(
                std::move(left)
            ),
            BezierCurve(
                std::move(right)
            )
        };
    }

private:
    /**
     * @brief Bézier 曲线控制点
     *
     * 如果：
     *
     *      control_points_.size() = n + 1
     *
     * 则：
     *
     *      degree = n
     */
    std::vector<point_type> control_points_;
};


/**
 * @brief 二维 Bézier 曲线
 */
using BezierCurve2d =
    BezierCurve<2>;


/**
 * @brief 三维 Bézier 曲线
 */
using BezierCurve3d =
    BezierCurve<3>;

} // namespace opencagd::curve

