
#pragma once

#include <cstddef>      // std::size_t
#include <stdexcept>    // std::invalid_argument
#include <type_traits>  // std::remove_cvref_t
#include <vector>       // std::vector

namespace opencagd::curve
{

/**
 * @brief 在给定参数区间上对一个曲线求值器进行均匀参数采样
 *
 * 本函数并不关心 evaluator 具体代表：
 *
 *      Bézier Curve
 *      B-spline Curve
 *      NURBS Curve
 *      Polynomial Curve
 *      或其他参数曲线
 *
 * 它只要求 evaluator 可以被调用：
 *
 *      evaluator(u)
 *
 * 并返回参数 u 对应的曲线点：
 *
 *      C(u)
 *
 * -------------------------------------------------------------
 * 数学思想
 * -------------------------------------------------------------
 *
 * 给定参数区间：
 *
 *      [u_min, u_max]
 *
 * 希望采样 sample_count 个点。
 *
 * 首先构造归一化参数：
 *
 *                     i
 *      alpha_i = ---------------
 *                  N - 1
 *
 * 其中：
 *
 *      i = 0, 1, ..., N-1
 *
 *      N = sample_count
 *
 * 所以：
 *
 *      alpha_0     = 0
 *      alpha_{N-1} = 1
 *
 * 然后将 alpha 从 [0,1] 映射到真正参数区间：
 *
 *      u_i
 *        = u_min
 *        + (u_max - u_min) alpha_i
 *
 * 最后计算：
 *
 *      P_i = C(u_i)
 *
 * 因此返回：
 *
 *      [C(u_0), C(u_1), ..., C(u_{N-1})]
 *
 * -------------------------------------------------------------
 * 注意
 * -------------------------------------------------------------
 *
 * 这里实现的是：
 *
 *      uniform parameter sampling
 *
 * 即：
 *
 *      参数均匀采样
 *
 * 而不是：
 *
 *      uniform arc-length sampling
 *
 * 因此相邻采样点在参数空间中的间隔相等，
 * 但在几何空间中的距离一般并不相等。
 *
 * @tparam Evaluator
 *         任意可调用对象，只要支持：
 *
 *             evaluator(double)
 *
 * @param evaluator
 *        曲线求值函数 / Lambda / Function Object
 *
 * @param umin
 *        参数区间左端点
 *
 * @param umax
 *        参数区间右端点
 *
 * @param sample_count
 *        采样点数量，至少为 2
 *
 * @return
 *        曲线离散采样点数组
 */
template <typename Evaluator>
[[nodiscard]]
auto sample_parameter_range(
    Evaluator&& evaluator,
    double umin,
    double umax,
    std::size_t sample_count)
{
    // ---------------------------------------------------------
    // 推导 evaluator(u) 的返回类型。
    //
    // 例如：
    //
    //      evaluator(u)
    //
    // 返回：
    //
    //      Point<double, 2>
    //
    // 那么：
    //
    //      point_type = Point<double, 2>
    //
    // 使用 remove_cvref_t 的原因是：
    //
    // 如果某个 evaluator 将来返回：
    //
    //      const Point<double,2>&
    //
    // decltype(...) 会得到引用类型。
    //
    // 但是：
    //
    //      std::vector<T&>
    //
    // 是非法的。
    //
    // 因此这里统一去掉：
    //
    //      const
    //      volatile
    //      &
    //      &&
    // ---------------------------------------------------------
    using point_type =
        std::remove_cvref_t<
            decltype(evaluator(umin))
        >;

    // ---------------------------------------------------------
    // 至少需要两个点才能描述一个参数区间。
    //
    // 同时后面的公式中存在：
    //
    //      sample_count - 1
    //
    // 如果 sample_count = 1，
    // 分母就会变成 0。
    // ---------------------------------------------------------
    if (sample_count < 2)
    {
        throw std::invalid_argument(
            "Curve sampling requires at least two samples"
        );
    }

    // 保存最终离散曲线点。
    std::vector<point_type> points;

    // 提前申请空间，避免 push_back 过程中
    // vector 多次重新分配内存。
    points.reserve(sample_count);

    // ---------------------------------------------------------
    // 生成：
    //
    //      i = 0, ..., sample_count - 1
    //
    // 共 sample_count 个采样点。
    // ---------------------------------------------------------
    for (std::size_t i = 0;
         i < sample_count;
         ++i)
    {
        // -----------------------------------------------------
        // 将离散索引 i 映射到：
        //
        //      alpha ∈ [0,1]
        //
        // 公式：
        //
        //                 i
        //      alpha = ---------
        //                N - 1
        //
        // 当：
        //
        //      i = 0
        //
        // 时：
        //
        //      alpha = 0
        //
        // 当：
        //
        //      i = N-1
        //
        // 时：
        //
        //      alpha = 1
        //
        // 因此能够保证左右两个端点都被采样。
        // -----------------------------------------------------
        const double alpha =
            static_cast<double>(i)
            /
            static_cast<double>(
                sample_count - 1
            );

        // -----------------------------------------------------
        // 将归一化参数 alpha ∈ [0,1]
        // 线性映射到：
        //
        //      u ∈ [umin, umax]
        //
        // 即：
        //
        //      u = (1-alpha) umin
        //          + alpha umax
        //
        // 等价于：
        //
        //      u = umin
        //          + (umax-umin) alpha
        // -----------------------------------------------------
        const double u =
            umin
            +
            (umax - umin) * alpha;

        // -----------------------------------------------------
        // 调用曲线求值器：
        //
        //      P_i = C(u_i)
        //
        // 并保存结果。
        // -----------------------------------------------------
        points.push_back(
            evaluator(u)
        );
    }

    return points;
}


/**
 * @brief 在曲线自身有效参数域上进行均匀参数采样
 *
 * Curve 类型需要提供：
 *
 *      curve.domain_min()
 *
 *      curve.domain_max()
 *
 *      curve.evaluate(u)
 *
 * 因此该函数可以直接作用于：
 *
 *      BSplineCurve
 *
 *      NURBSCurve
 *
 * 以及其他拥有统一 Curve 接口的类型。
 *
 * 实际调用过程为：
 *
 *      [domain_min, domain_max]
 *
 *              ↓
 *
 *      sample_parameter_range()
 *
 *              ↓
 *
 *      curve.evaluate(u)
 *
 *              ↓
 *
 *      离散曲线点
 *
 * @tparam Curve 曲线类型
 *
 * @param curve
 *        待采样的参数曲线
 *
 * @param sample_count
 *        采样点数量
 *
 * @return
 *        曲线离散采样点
 */
template <typename Curve>
[[nodiscard]]
auto sample_curve(
    const Curve& curve,
    std::size_t sample_count)
{
    return sample_parameter_range(
        // -----------------------------------------------------
        // 使用 Lambda 将：
        //
        //      curve.evaluate(u)
        //
        // 转换成通用的：
        //
        //      evaluator(u)
        //
        // 接口。
        // -----------------------------------------------------
        [&curve](double u)
        {
            return curve.evaluate(u);
        },

        // 曲线有效参数域左端点。
        curve.domain_min(),

        // 曲线有效参数域右端点。
        curve.domain_max(),

        // 采样数量。
        sample_count
    );
}

} // namespace opencagd::curve

