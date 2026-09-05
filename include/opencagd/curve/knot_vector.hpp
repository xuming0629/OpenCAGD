#pragma once

#include <algorithm>    // std::is_sorted
#include <cstddef>      // std::size_t
#include <stdexcept>    // std::invalid_argument, std::out_of_range
#include <utility>      // std::move
#include <vector>       // std::vector

#include <opencagd/math/numeric.hpp>

namespace opencagd::curve
{

/**
 * @brief B-spline / NURBS 节点向量
 *
 * 一个 B-spline 节点向量通常写成：
 *
 *      U = {u_0, u_1, ..., u_m}
 *
 * 节点必须满足非递减关系：
 *
 *      u_0 <= u_1 <= ... <= u_m
 *
 * 注意：
 *
 *      节点允许重复。
 *
 * 对一个 p 次 B-spline，如果：
 *
 *      控制点数量 = n + 1
 *      节点数量   = m + 1
 *
 * 那么必须满足：
 *
 *      m = n + p + 1
 *
 * 因而：
 *
 *      n + 1
 *        = (m + 1) - p - 1
 *
 * 即：
 *
 *      基函数数量
 *        = knot_count - degree - 1
 *
 * B-spline 的有效参数域通常为：
 *
 *      [u_p, u_{n+1}]
 *
 * KnotVector 主要负责：
 *
 * 1. 保存节点向量；
 * 2. 验证节点是否合法；
 * 3. 提供有效参数域；
 * 4. 计算基函数数量；
 * 5. 查找给定参数 u 所处的 knot span。
 */
class KnotVector
{
public:

    /**
     * @brief 构造节点向量
     *
     * @param degree B-spline 次数 p
     * @param knots 节点数组
     *
     * 节点数组必须：
     *
     * 1. 数量足够；
     * 2. 按非递减顺序排列。
     *
     * 对 p 次 B-spline，如果希望至少存在 p+1 个基函数，
     * 即至少能够构成一条 p 次曲线，则至少需要：
     *
     *      2p + 2
     *
     * 个 knot。
     *
     * 例如：
     *
     *      p = 3
     *
     * 至少需要：
     *
     *      2*3 + 2 = 8
     *
     * 个节点。
     */
    KnotVector(
        std::size_t degree,
        std::vector<double> knots)
        : degree_(degree),
          knots_(std::move(knots))
    {
        // -----------------------------------------------------
        // 对 p 次 B-spline：
        //
        //      knot_count >= 2p + 2
        //
        // 这意味着至少能够得到：
        //
        //      p + 1
        //
        // 个基函数。
        // -----------------------------------------------------
        if (knots_.size() < 2 * degree_ + 2)
        {
            throw std::invalid_argument(
                "Knot vector is too short for the requested degree"
            );
        }

        // -----------------------------------------------------
        // B-spline 节点向量必须非递减：
        //
        //      u_i <= u_{i+1}
        //
        // 注意这里不是严格递增。
        //
        // 因为 B-spline / NURBS 允许重复节点，例如：
        //
        //      [0, 0, 0, 0, 1, 2, 3, 3, 3, 3]
        //
        // 就是一个合法的 clamped cubic knot vector。
        // -----------------------------------------------------
        if (!std::is_sorted(
                knots_.begin(),
                knots_.end()))
        {
            throw std::invalid_argument(
                "Knot vector must be nondecreasing"
            );
        }
    }

    /**
     * @brief 返回 B-spline 次数 p
     */
    [[nodiscard]]
    std::size_t degree() const noexcept
    {
        return degree_;
    }

    /**
     * @brief 返回完整节点数组
     */
    [[nodiscard]]
    const std::vector<double>& values() const noexcept
    {
        return knots_;
    }

    /**
     * @brief 返回节点数量
     *
     * 如果：
     *
     *      U = {u_0, ..., u_m}
     *
     * 那么：
     *
     *      size() = m + 1
     */
    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return knots_.size();
    }

    /**
     * @brief 访问第 i 个节点 u_i
     *
     * 注意：
     *
     * 这里使用 operator[]，
     * 因此不进行越界检查。
     */
    [[nodiscard]]
    double operator[](std::size_t i) const noexcept
    {
        return knots_[i];
    }

    /**
     * @brief 返回 B-spline 基函数数量
     *
     * 如果节点数量为：
     *
     *      m + 1
     *
     * 次数为：
     *
     *      p
     *
     * 则基函数数量为：
     *
     *      n + 1 = m - p
     *
     * 使用 knot_count 表示：
     *
     *      num_basis_functions
     *          = knot_count - p - 1
     *
     * 每一个 B-spline 基函数：
     *
     *      N_{i,p}
     *
     * 通常对应一个控制点：
     *
     *      P_i
     */
    [[nodiscard]]
    std::size_t num_basis_functions() const noexcept
    {
        return knots_.size()
             - degree_
             - 1;
    }

    /**
     * @brief 返回 B-spline 有效参数域左端点
     *
     * 对 p 次 B-spline：
     *
     *      domain_min = u_p
     */
    [[nodiscard]]
    double domain_min() const noexcept
    {
        return knots_[degree_];
    }

    /**
     * @brief 返回 B-spline 有效参数域右端点
     *
     * 如果共有 n+1 个基函数，
     * 则有效参数域右端点：
     *
     *      domain_max = u_{n+1}
     *
     * 因为：
     *
     *      n + 1 = num_basis_functions()
     *
     * 这里对应的数组下标为：
     *
     *      knot_count - degree - 1
     */
    [[nodiscard]]
    double domain_max() const noexcept
    {
        return knots_[
            knots_.size()
            - degree_
            - 1
        ];
    }

    /**
     * @brief 查找参数 u 所在的 knot span
     *
     * 返回 span = i，使：
     *
     *      u_i <= u < u_{i+1}
     *
     * 对参数域最右端点：
     *
     *      u = u_{n+1}
     *
     * 按 B-spline 标准约定返回：
     *
     *      span = n
     *
     * 本函数使用二分搜索，因此查找复杂度约为：
     *
     *      O(log n)
     *
     * @param u B-spline 参数
     *
     * @return 参数 u 所处的 knot span
     *
     * @throws std::out_of_range
     *         当 u 位于有效 B-spline 参数域之外时抛出异常
     */
    [[nodiscard]]
    std::size_t find_span(double u) const
    {
        // -----------------------------------------------------
        // 如果共有：
        //
        //      n + 1
        //
        // 个基函数，那么最后一个基函数索引为：
        //
        //      n
        // -----------------------------------------------------
        const std::size_t n =
            num_basis_functions() - 1;

        // B-spline 有效参数域：
        //
        //      [u_p, u_{n+1}]
        const double umin =
            domain_min();

        const double umax =
            domain_max();

        // -----------------------------------------------------
        // 参数域检查。
        //
        // 因为 double 浮点计算存在舍入误差，
        // 所以这里使用 tolerance。
        //
        // 例如：
        //
        // 理论上：
        //
        //      u = 1.0
        //
        // 实际计算后可能变成：
        //
        //      1.0000000000000002
        //
        // 因此允许一个很小的误差。
        // -----------------------------------------------------
        if (u < umin - opencagd::math::default_tolerance ||
            u > umax + opencagd::math::default_tolerance)
        {
            throw std::out_of_range(
                "Parameter lies outside the B-spline domain"
            );
        }

        // -----------------------------------------------------
        // 最右端点特殊处理。
        //
        // 标准 span 定义通常使用半开区间：
        //
        //      u_i <= u < u_{i+1}
        //
        // 但是参数域最后一个端点：
        //
        //      u = u_{n+1}
        //
        // 不属于任何普通半开区间。
        //
        // 按标准 B-spline 约定：
        //
        //      find_span(u_max) = n
        //
        // 即使用最后一个有效 span。
        // -----------------------------------------------------
        if (opencagd::math::nearly_equal(u, umax) ||
            u >= umax)
        {
            return n;
        }

        // -----------------------------------------------------
        // 参数位于左端点时：
        //
        //      u = u_p
        //
        // 第一个有效 span 为：
        //
        //      span = p
        // -----------------------------------------------------
        if (u <= umin)
        {
            return degree_;
        }

        // -----------------------------------------------------
        // 使用二分搜索寻找 span。
        //
        // 搜索区间：
        //
        //      [p, n+1]
        //
        // 我们希望找到 mid，使：
        //
        //      u_mid <= u < u_{mid+1}
        // -----------------------------------------------------
        std::size_t low =
            degree_;

        std::size_t high =
            n + 1;

        std::size_t mid =
            (low + high) / 2;

        // -----------------------------------------------------
        // 如果还不满足：
        //
        //      u_mid <= u < u_{mid+1}
        //
        // 就继续进行二分搜索。
        // -----------------------------------------------------
        while (
            u < knots_[mid]
            ||
            u >= knots_[mid + 1])
        {
            if (u < knots_[mid])
            {
                // u 在当前 span 左侧，
                // 缩小右边界。
                high = mid;
            }
            else
            {
                // u >= u_{mid+1}
                //
                // 参数位于当前 span 右边，
                // 提高左边界。
                low = mid;
            }

            mid =
                (low + high) / 2;
        }

        // 此时满足：
        //
        //      knots_[mid]
        //          <= u
        //          < knots_[mid + 1]
        return mid;
    }

private:

    /**
     * @brief B-spline 次数 p
     */
    std::size_t degree_{};

    /**
     * @brief 节点向量
     *
     *      U = {u_0, ..., u_m}
     */
    std::vector<double> knots_;
};

} // namespace opencagd::curve

