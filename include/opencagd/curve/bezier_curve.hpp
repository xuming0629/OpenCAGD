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
 *      t        : 曲线参数，通常 t ∈ [0, 1]
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
        // Bézier 曲线至少需要一个控制点。
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
     *
     * 例如：
     *
     *      4 个控制点 -> 三次 Bézier 曲线
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
     * 这里的算法完全按照 Bézier 曲线定义直接计算。
     *
     * @param t 曲线参数，通常位于 [0, 1]
     *
     * @return 参数 t 对应的曲线点 C(t)
     */
    [[nodiscard]]
    point_type evaluate(double t) const
    {
        // -----------------------------------------------------
        // 计算当前参数 t 下全部 Bernstein 基函数：
        //
        //      [
        //          B_0^n(t),
        //          B_1^n(t),
        //          ...
        //          B_n^n(t)
        //      ]
        // -----------------------------------------------------
        const auto basis = bernstein_basis(
            degree(),
            t
        );

        // 最终 Bézier 曲线点。
        //
        // point{} 默认初始化为零向量：
        //
        //      (0, 0)
        //
        // 或：
        //
        //      (0, 0, 0)
        point_type point{};

        // -----------------------------------------------------
        // 实现公式：
        //
        //      C(t) = Σ P_i B_i^n(t)
        //
        // 每一个 Bernstein 基函数值可以理解为：
        //
        //      当前控制点 P_i 对曲线点 C(t) 的权重
        // -----------------------------------------------------
        for (std::size_t i = 0;
             i < control_points_.size();
             ++i)
        {
            point += control_points_[i] * basis[i];
        }

        return point;
    }

    /**
     * @brief 使用 De Casteljau 算法计算 Bézier 曲线点
     *
     * De Casteljau 算法通过连续线性插值计算 Bézier 曲线：
     *
     * 第一层：
     *
     *      P_i^(1)
     *          = (1-t) P_i + t P_{i+1}
     *
     * 第二层：
     *
     *      P_i^(2)
     *          = (1-t) P_i^(1) + t P_{i+1}^(1)
     *
     * 一直递推到最后只剩一个点：
     *
     *      P_0^(n) = C(t)
     *
     * 相比直接 Bernstein 多项式求值，
     * De Casteljau 通常具有更好的数值稳定性。
     *
     * @param t 曲线参数
     *
     * @return 曲线点 C(t)
     */
    [[nodiscard]]
    point_type evaluate_de_casteljau(double t) const
    {
        // 创建控制点的临时副本。
        //
        // De Casteljau 算法会不断覆盖中间控制点。
        auto work = control_points_;

        const std::size_t n = degree();

        // -----------------------------------------------------
        // r 表示 De Casteljau 三角形的层数。
        //
        // r = 1:
        //
        //      P0^1 P1^1 ... P(n-1)^1
        //
        // r = 2:
        //
        //      P0^2 P1^2 ... P(n-2)^2
        //
        // ...
        //
        // r = n:
        //
        //      P0^n = C(t)
        // -----------------------------------------------------
        for (std::size_t r = 1; r <= n; ++r)
        {
            // 当前这一层包含：
            //
            //      n-r+1
            //
            // 个点。
            for (std::size_t i = 0;
                 i <= n - r;
                 ++i)
            {
                // 线性插值：
                //
                // P_i^r
                //
                // =
                //
                // (1-t) P_i^(r-1)
                //
                // +
                //
                // t P_(i+1)^(r-1)
                work[i] =
                    work[i] * (1.0 - t)
                    +
                    work[i + 1] * t;
            }
        }

        // 最终 work[0] 就是：
        //
        //      C(t)
        return work.front();
    }

    /**
     * @brief 构造 Bézier 曲线的一阶导数曲线
     *
     * 原 Bézier 曲线：
     *
     *      C(t)
     *        = Σ B_i^n(t) P_i
     *
     * 一阶导数为：
     *
     *      C'(t)
     *        = n Σ B_i^(n-1)(t)
     *              (P_{i+1} - P_i)
     *
     * 因此导数本身仍然是一条 Bézier 曲线。
     *
     * 导数曲线的控制点为：
     *
     *      Q_i
     *        = n (P_{i+1} - P_i)
     *
     * 原曲线：
     *
     *      degree = n
     *
     * 导数曲线：
     *
     *      degree = n - 1
     *
     * @return Bézier 导数曲线
     */
    [[nodiscard]]
    BezierCurve derivative() const
    {
        // -----------------------------------------------------
        // 如果原曲线是 0 次曲线：
        //
        //      C(t) = P0
        //
        // 那么：
        //
        //      C'(t) = 0
        //
        // 因此返回一个控制点为零向量的
        // 0 次 Bézier 曲线。
        // -----------------------------------------------------
        if (degree() == 0)
        {
            return BezierCurve({
                point_type{}
            });
        }

        std::vector<point_type> derivative_points;

        // n 次曲线的导数是 n-1 次曲线，
        // 所以导数控制点一共有 n 个。
        derivative_points.reserve(degree());

        const double n =
            static_cast<double>(degree());

        // -----------------------------------------------------
        // 根据公式：
        //
        //      Q_i = n(P_{i+1} - P_i)
        //
        // 构造导数曲线控制点。
        // -----------------------------------------------------
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
            std::move(derivative_points)
        );
    }

    /**
     * @brief 在参数 t 处将 Bézier 曲线细分成两条 Bézier 曲线
     *
     * 使用 De Casteljau 算法。
     *
     * 原曲线：
     *
     *      C(u), u ∈ [0,1]
     *
     * 在 t 处分割后得到：
     *
     *      left
     *
     * 和：
     *
     *      right
     *
     * 两条 Bézier 曲线。
     *
     * De Casteljau 三角形最左边的点构成：
     *
     *      left 控制点
     *
     * 最右边的点构成：
     *
     *      right 控制点
     *
     * 两条曲线都会经过原曲线的分割点 C(t)。
     *
     * @param t 分割参数
     *
     * @return {左侧 Bézier 曲线, 右侧 Bézier 曲线}
     */
    [[nodiscard]]
    std::pair<BezierCurve, BezierCurve>
    subdivide(double t) const
    {
        const std::size_t n = degree();

        // De Casteljau 工作数组。
        std::vector<point_type> work =
            control_points_;

        // 左半部分 Bézier 控制点。
        std::vector<point_type> left(
            n + 1
        );

        // 右半部分 Bézier 控制点。
        std::vector<point_type> right(
            n + 1
        );

        // 左曲线第一个控制点一定是原始 P0。
        left[0] = work[0];

        // 右曲线最后一个控制点一定是原始 Pn。
        right[n] = work[n];

        // -----------------------------------------------------
        // 构造完整 De Casteljau 三角形。
        //
        // 每完成一层：
        //
        //      work[0]
        //
        // 是这一层最左边的点，
        //
        //      work[n-r]
        //
        // 是这一层最右边的点。
        // -----------------------------------------------------
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

            // De Casteljau 三角形左边界：
            //
            //      P0
            //      P0^1
            //      P0^2
            //      ...
            //      C(t)
            //
            // 构成左侧 Bézier 曲线的控制点。
            left[r] = work[0];

            // De Casteljau 三角形右边界：
            //
            //      C(t)
            //      ...
            //      P_(n-1)^1
            //      Pn
            //
            // 构成右侧 Bézier 曲线的控制点。
            right[n - r] = work[n - r];
        }

        return {
            BezierCurve(std::move(left)),
            BezierCurve(std::move(right))
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
     * 则曲线次数：
     *
     *      degree = n
     */
    std::vector<point_type> control_points_;
};


/**
 * @brief 二维 Bézier 曲线
 */
using BezierCurve2d = BezierCurve<2>;


/**
 * @brief 三维 Bézier 曲线
 */
using BezierCurve3d = BezierCurve<3>;

} // namespace opencagd::curve
