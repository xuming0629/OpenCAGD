#pragma once

#include <cstddef>
#include <vector>

#include <opencagd/curve/knot_vector.hpp>

namespace opencagd::curve
{

/**
 * @brief 计算参数 u 所在 knot span 上的全部非零 B-spline 基函数
 *
 * 本实现对应《The NURBS Book》中的 Algorithm A2.2：BasisFuns。
 *
 * 对于一个 p 次 B-spline，在给定参数 u 下，
 * 最多只有 p + 1 个基函数非零。
 *
 * 如果：
 *
 *      span = i
 *
 * 即：
 *
 *      U_i <= u < U_{i+1}
 *
 * 那么非零基函数为：
 *
 *      N_{i-p,p}(u),
 *      N_{i-p+1,p}(u),
 *      ...
 *      N_{i,p}(u)
 *
 * 本函数返回的 values 数组依次保存：
 *
 *      values[0] = N_{span-p,p}(u)
 *      values[1] = N_{span-p+1,p}(u)
 *      ...
 *      values[p] = N_{span,p}(u)
 *
 * @param knot_vector B-spline 节点向量
 * @param span 参数 u 所在的 knot span
 * @param u 参数值
 *
 * @return 当前 span 上 p+1 个非零 B-spline 基函数值
 */
[[nodiscard]]
inline std::vector<double> basis_functions(
    const KnotVector& knot_vector,
    std::size_t span,
    double u)
{
    // B-spline 的次数：
    //
    //      p = degree
    //
    // 对于 p 次 B-spline，
    // 任意参数位置最多只有 p + 1 个非零基函数。
    const std::size_t p = knot_vector.degree();

    // ---------------------------------------------------------
    // values:
    //
    // 保存当前参数 u 对应的局部非零基函数。
    //
    // 最终：
    //
    // values[0] = N_{span-p,p}(u)
    // ...
    // values[p] = N_{span,p}(u)
    // ---------------------------------------------------------
    std::vector<double> values(
        p + 1,
        0.0
    );

    // ---------------------------------------------------------
    // left[j]:
    //
    //      u - U_{span+1-j}
    //
    // 表示参数 u 到左侧某个 knot 的距离。
    //
    // right[j]:
    //
    //      U_{span+j} - u
    //
    // 表示参数 u 到右侧某个 knot 的距离。
    //
    // 这两个数组用于避免在 Cox-de Boor 递推中
    // 反复访问和计算节点差值。
    // ---------------------------------------------------------
    std::vector<double> left(
        p + 1,
        0.0
    );

    std::vector<double> right(
        p + 1,
        0.0
    );

    // ---------------------------------------------------------
    // 递推起点。
    //
    // 0 次 B-spline 基函数在当前 span 内：
    //
    //      N_{span,0}(u) = 1
    //
    // 因此从：
    //
    //      values[0] = 1
    //
    // 开始逐阶递推。
    // ---------------------------------------------------------
    values[0] = 1.0;

    // ---------------------------------------------------------
    // j 表示当前正在构造的基函数次数。
    //
    // j = 1 -> 一次 B-spline
    // j = 2 -> 二次 B-spline
    // ...
    // j = p -> p 次 B-spline
    // ---------------------------------------------------------
    for (std::size_t j = 1; j <= p; ++j)
    {
        // 参数 u 到当前左侧 knot 的距离：
        //
        //      left[j]
        //        = u - U_{span+1-j}
        left[j] =
            u -
            knot_vector[span + 1 - j];

        // 参数 u 到当前右侧 knot 的距离：
        //
        //      right[j]
        //        = U_{span+j} - u
        right[j] =
            knot_vector[span + j] -
            u;

        // saved 保存 Cox-de Boor 递推中
        // 上一个基函数贡献到下一个基函数的部分。
        double saved = 0.0;

        // -----------------------------------------------------
        // r 遍历当前第 j 层已有的局部基函数。
        //
        // 这里实际上是在原地更新 values：
        //
        //      degree j-1
        //
        //              ↓
        //
        //      degree j
        // -----------------------------------------------------
        for (std::size_t r = 0; r < j; ++r)
        {
            // -------------------------------------------------
            // denominator 对应 Cox-de Boor 递推公式中的
            // knot interval 长度。
            //
            //      right[r+1] + left[j-r]
            //
            // 展开后实际上就是：
            //
            //      U_{...+j} - U_{...}
            //
            // -------------------------------------------------
            const double denominator =
                right[r + 1]
                +
                left[j - r];

            // -------------------------------------------------
            // 如果节点重复，可能出现：
            //
            //      denominator = 0
            //
            // 按 B-spline 的标准约定：
            //
            //      0 / 0 对应项视为 0
            //
            // 因此这里显式返回 0。
            // -------------------------------------------------
            const double temp =
                denominator == 0.0
                    ? 0.0
                    : values[r] / denominator;

            // -------------------------------------------------
            // 当前基函数值由两部分组成：
            //
            // 1. 上一次递推留下的 saved
            //
            // 2. 当前基函数通过右侧权重产生的贡献
            //
            // 对应 Cox-de Boor 递推中的第一项和第二项。
            // -------------------------------------------------
            values[r] =
                saved
                +
                right[r + 1] * temp;

            // 当前基函数剩余的一部分，
            // 将贡献给下一个基函数。
            saved =
                left[j - r] * temp;
        }

        // 当前第 j 阶最后一个基函数值。
        values[j] = saved;
    }

    return values;
}


/**
 * @brief 计算参数 u 下所有 B-spline 基函数
 *
 * basis_functions() 只计算当前 knot span 上
 * p+1 个非零基函数。
 *
 * 本函数创建完整数组：
 *
 *      [
 *          N_0,p(u),
 *          N_1,p(u),
 *          ...
 *          N_n,p(u)
 *      ]
 *
 * 不在当前 span 支撑范围内的基函数值全部为 0。
 *
 * @param knot_vector B-spline 节点向量
 * @param u 参数值
 *
 * @return 所有全局 B-spline 基函数值
 */
[[nodiscard]]
inline std::vector<double> all_basis_functions(
    const KnotVector& knot_vector,
    double u)
{
    // B-spline 总基函数数量。
    //
    // 如果有 n+1 个控制点，
    // 通常也有 n+1 个基函数：
    //
    //      N_0,p ... N_n,p
    std::vector<double> result(
        knot_vector.num_basis_functions(),
        0.0
    );

    // ---------------------------------------------------------
    // 首先找到参数 u 所在的 knot span。
    //
    // span 满足：
    //
    //      U_span <= u < U_{span+1}
    //
    // 对于参数域最右端点通常做特殊处理。
    // ---------------------------------------------------------
    const std::size_t span =
        knot_vector.find_span(u);

    // ---------------------------------------------------------
    // 只计算当前 span 上非零的 p+1 个基函数。
    // ---------------------------------------------------------
    const auto local =
        basis_functions(
            knot_vector,
            span,
            u
        );

    // ---------------------------------------------------------
    // 当前非零基函数的第一个全局索引：
    //
    //      span - p
    //
    // 因为非零基函数是：
    //
    //      N_{span-p,p}
    //      ...
    //      N_{span,p}
    // ---------------------------------------------------------
    const std::size_t first =
        span -
        knot_vector.degree();

    // 将局部结果放回完整的全局基函数数组中。
    for (std::size_t j = 0;
         j < local.size();
         ++j)
    {
        result[first + j] =
            local[j];
    }

    return result;
}


/**
 * @brief 计算单个 B-spline 基函数 N_{index,p}(u)
 *
 * 当前实现为了接口简单，
 * 先计算所有基函数，
 * 再返回指定 index 对应的基函数值。
 *
 * 该实现清晰但不是最高效的。
 *
 * 后续如果大量调用单个基函数，
 * 可以根据 index 与 span 的关系进行局部计算优化。
 *
 * @param knot_vector B-spline 节点向量
 * @param index 基函数索引
 * @param u 参数值
 *
 * @return N_{index,p}(u)
 */
[[nodiscard]]
inline double basis_function(
    const KnotVector& knot_vector,
    std::size_t index,
    double u)
{
    const auto all =
        all_basis_functions(
            knot_vector,
            u
        );

    // index 越界时认为对应基函数不存在，
    // 返回 0。
    return index < all.size()
        ? all[index]
        : 0.0;
}

} // namespace opencagd::curve

