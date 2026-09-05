#pragma once

#include <cmath>      // std::pow
#include <cstddef>    // std::size_t
#include <vector>     // std::vector

#include <opencagd/math/binomial.hpp>  // 二项式系数 C(n, i)

namespace opencagd::curve
{

/**
 * @brief 计算单个 Bernstein 基函数 B_i^n(t)
 *
 * Bernstein 基函数定义为：
 *
 *      B_i^n(t) = C(n, i) * t^i * (1 - t)^(n - i)
 *
 * 其中：
 *      n : Bernstein 多项式的次数（degree）
 *      i : 第 i 个基函数，范围为 0 <= i <= n
 *      t : 参数，通常取值范围为 [0, 1]
 *      C(n, i) : 二项式系数
 *
 * Bernstein 基函数是 Bézier 曲线的基础。
 * 一个 n 次 Bézier 曲线可以写成：
 *
 *      C(t) = Σ B_i^n(t) * P_i
 *             i=0...n
 *
 * 其中 P_i 为 Bézier 控制点。
 *
 * @param i 基函数索引
 * @param n 多项式次数
 * @param t 参数值，通常位于 [0, 1]
 *
 * @return Bernstein 基函数 B_i^n(t) 的值
 */
[[nodiscard]] inline double bernstein(
    std::size_t i,
    std::size_t n,
    double t)
{
    // Bernstein 基函数只定义 i = 0,...,n。
    // 当 i > n 时，对应基函数不存在，因此返回 0。
    if (i > n)
    {
        return 0.0;
    }

    // ---------------------------------------------------------
    // t = 0 时的特殊处理
    //
    // 根据 Bernstein 基函数性质：
    //
    //      B_0^n(0) = 1
    //      B_i^n(0) = 0, i > 0
    //
    // 这里显式处理还可以避免计算：
    //
    //      0^0
    //
    // 这种边界情况。
    // ---------------------------------------------------------
    if (t <= 0.0)
    {
        return i == 0 ? 1.0 : 0.0;
    }

    // ---------------------------------------------------------
    // t = 1 时的特殊处理
    //
    // Bernstein 基函数满足：
    //
    //      B_n^n(1) = 1
    //      B_i^n(1) = 0, i < n
    //
    // 同样可以避免 pow(0, 0) 等边界问题。
    // ---------------------------------------------------------
    if (t >= 1.0)
    {
        return i == n ? 1.0 : 0.0;
    }

    // ---------------------------------------------------------
    // Bernstein 基函数公式：
    //
    //      B_i^n(t)
    //        = C(n, i)
    //        * t^i
    //        * (1 - t)^(n - i)
    //
    // binomial(n, i) 计算：
    //
    //                 n!
    //      C(n,i) = --------
    //                i!(n-i)!
    //
    // 例如：
    //
    //      C(3,0) = 1
    //      C(3,1) = 3
    //      C(3,2) = 3
    //      C(3,3) = 1
    // ---------------------------------------------------------
    return static_cast<double>(
               opencagd::math::binomial(n, i)
           ) *
           std::pow(
               t,
               static_cast<int>(i)
           ) *
           std::pow(
               1.0 - t,
               static_cast<int>(n - i)
           );
}


/**
 * @brief 计算给定次数 degree 下的全部 Bernstein 基函数
 *
 * 对于 degree = n，一共有 n + 1 个 Bernstein 基函数：
 *
 *      B_0^n(t)
 *      B_1^n(t)
 *      ...
 *      B_n^n(t)
 *
 * 返回：
 *
 *      [ B_0^n(t), B_1^n(t), ..., B_n^n(t) ]
 *
 * 例如：
 *
 *      degree = 2
 *
 * 返回：
 *
 *      [
 *          (1-t)^2,
 *          2t(1-t),
 *          t^2
 *      ]
 *
 * @param degree Bernstein 多项式次数
 * @param t 参数值，通常位于 [0, 1]
 *
 * @return 所有 Bernstein 基函数值组成的数组
 */
[[nodiscard]] inline std::vector<double> bernstein_basis(
    std::size_t degree,
    double t)
{
    // n 次 Bernstein 多项式共有 n + 1 个基函数。
    std::vector<double> values(
        degree + 1,
        0.0
    );

    // 依次计算：
    //
    // B_0^n(t), B_1^n(t), ..., B_n^n(t)
    for (std::size_t i = 0; i <= degree; ++i)
    {
        values[i] = bernstein(
            i,
            degree,
            t
        );
    }

    return values;
}

} // namespace opencagd::curve

