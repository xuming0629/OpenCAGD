#include <opencagd/curve/bspline_basis.h>

#include <algorithm>
#include <cstddef>
#include <utility>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief 计算参数 u 所在 knot span 内所有非零 B-Spline 基函数
         *
         * 对 p 次 B-Spline 基函数：
         *
         *     N_{i,p}(u)
         *
         * 如果参数 u 位于节点区间：
         *
         *     u ∈ [U_span, U_{span+1})
         *
         * 那么在该参数位置最多只有 p + 1 个非零基函数：
         *
         *     N_{span-p,p}(u),
         *     ...
         *     N_{span,p}(u)
         *
         * 本函数只计算这 p+1 个非零基函数，而不是计算整个基函数集合。
         *
         * 这是 Cox-de Boor 递推公式的高效三角递推实现。
         *
         * Cox-de Boor 递推公式：
         *
         *     N_{i,0}(u) =
         *
         *         1,  U_i <= u < U_{i+1}
         *         0,  otherwise
         *
         * 对 p > 0：
         *
         *                    u - U_i
         *     N_{i,p}(u) = ---------------- N_{i,p-1}(u)
         *                  U_{i+p} - U_i
         *
         *                    U_{i+p+1} - u
         *                + ---------------------- N_{i+1,p-1}(u)
         *                  U_{i+p+1} - U_{i+1}
         *
         * @param knot_vector B-Spline 节点向量
         * @param span        参数 u 所处的节点区间索引
         * @param u           参数值
         *
         * @return 长度为 p+1 的局部非零基函数值：
         *
         *     N[0] = N_{span-p,p}(u)
         *     ...
         *     N[p] = N_{span,p}(u)
         */
        std::vector<double> basis_functions(
            const KnotVector &knot_vector,
            std::size_t span,
            double u)
        {
            /*
             * B-Spline 次数：
             *
             *     p
             */
            const std::size_t p = knot_vector.degree();

            /*
             * N[j] 保存当前递推过程中得到的基函数值。
             *
             * 最终：
             *
             *     N[0], ..., N[p]
             *
             * 对应参数 u 处 p+1 个非零基函数。
             */
            std::vector<double> N(p + 1, 0.0);

            /*
             * left[j]：
             *
             *     u - U_{span+1-j}
             *
             * 表示参数 u 到左侧相关节点的距离。
             */
            std::vector<double> left(p + 1, 0.0);

            /*
             * right[j]：
             *
             *     U_{span+j} - u
             *
             * 表示参数 u 到右侧相关节点的距离。
             */
            std::vector<double> right(p + 1, 0.0);

            /*
             * 0 次基函数递推起点：
             *
             * 参数已经确定处于 span 中，因此初始局部基函数为 1。
             */
            N[0] = 1.0;

            /*
             * 从 1 次逐级递推到 p 次。
             *
             * j 表示当前正在构造的基函数次数。
             */
            for (std::size_t j = 1; j <= p; ++j)
            {
                /*
                 * 当前递推层左侧节点距离：
                 *
                 *     left[j] = u - U_{span+1-j}
                 */
                left[j] =
                    u - knot_vector[span + 1 - j];

                /*
                 * 当前递推层右侧节点距离：
                 *
                 *     right[j] = U_{span+j} - u
                 */
                right[j] =
                    knot_vector[span + j] - u;

                /*
                 * saved 用于保存当前三角递推中
                 * 尚未写入 N 的右侧贡献。
                 */
                double saved = 0.0;

                /*
                 * 利用上一层 N[r] 构造当前 j 次基函数。
                 */
                for (std::size_t r = 0; r < j; ++r)
                {
                    /*
                     * 分母：
                     *
                     *     right[r+1] + left[j-r]
                     *
                     * 展开后对应一个节点区间长度。
                     */
                    const double denominator =
                        right[r + 1] + left[j - r];

                    /*
                     * Cox-de Boor 递推中的除法部分。
                     *
                     * 重复节点可能导致 denominator == 0。
                     *
                     * 按 B-Spline 的标准约定：
                     *
                     *     0 / 0 形式的贡献视为 0。
                     */
                    const double temp =
                        denominator == 0.0
                            ? 0.0
                            : N[r] / denominator;

                    /*
                     * 当前 N[r] 由两部分组成：
                     *
                     *     上一次留下的左侧贡献 saved
                     *
                     * 加上：
                     *
                     *     right[r+1] * temp
                     */
                    N[r] =
                        saved + right[r + 1] * temp;

                    /*
                     * 保存当前项的另一部分贡献，
                     * 供下一个基函数使用。
                     */
                    saved =
                        left[j - r] * temp;
                }

                /*
                 * 当前递推层最后一个基函数值。
                 */
                N[j] = saved;
            }

            return N;
        }

        /**
         * @brief 计算参数 u 处全部 B-Spline 基函数
         *
         * basis_functions() 只返回当前 span 中的 p+1 个非零基函数。
         *
         * 本函数将这些局部值放回完整基函数数组：
         *
         *     N_0,p(u),
         *     N_1,p(u),
         *     ...
         *     N_n,p(u)
         *
         * 由于 B-Spline 具有局部支撑性质，
         * 除当前 span 附近的 p+1 个基函数之外，
         * 其他基函数全部为 0。
         *
         * @param knot_vector 节点向量
         * @param u           参数值
         *
         * @return 所有 B-Spline 基函数值
         */
        std::vector<double>
        all_basis_functions(
            const KnotVector &knot_vector,
            double u)
        {
            /*
             * 完整基函数数组。
             *
             * 初始全部为 0。
             */
            std::vector<double> result(
                knot_vector.num_basis_functions(),
                0.0);

            /*
             * 找到：
             *
             *     U_span <= u < U_{span+1}
             */
            const std::size_t span =
                knot_vector.find_span(u);

            /*
             * 计算当前 span 内 p+1 个非零基函数。
             */
            const std::vector<double> local =
                basis_functions(
                    knot_vector,
                    span,
                    u);

            /*
             * 当前第一个非零基函数索引：
             *
             *     span - p
             */
            const std::size_t first =
                span - knot_vector.degree();

            /*
             * 将局部基函数放回全局数组。
             *
             * local[j]
             *
             * 对应：
             *
             * N_{first+j,p}(u)
             */
            for (std::size_t j = 0;
                 j <= knot_vector.degree();
                 ++j)
            {
                result[first + j] = local[j];
            }

            return result;
        }

        /**
         * @brief 计算参数 u 处非零 B-Spline 基函数及其各阶导数
         *
         * 计算：
         *
         *     N_{i,p}(u)
         *     N'_{i,p}(u)
         *     N''_{i,p}(u)
         *     ...
         *
         * 返回矩阵：
         *
         *     ders[k][j]
         *
         * 表示第 j 个局部非零基函数的第 k 阶导数。
         *
         * 对于 span：
         *
         *     j = 0
         *
         * 对应：
         *
         *     N_{span-p,p}
         *
         * 而：
         *
         *     j = p
         *
         * 对应：
         *
         *     N_{span,p}
         *
         * ------------------------------------------------------------
         *
         * 返回结构：
         *
         *     ders[0][j] -> 基函数本身
         *     ders[1][j] -> 一阶导数
         *     ders[2][j] -> 二阶导数
         *     ...
         *
         * ------------------------------------------------------------
         *
         * p 次多项式的高于 p 阶导数恒为 0，
         * 因此真正需要计算的最大阶数为：
         *
         *     du = min(derivative_order, p)
         *
         * @param knot_vector      节点向量
         * @param span             参数所在 knot span
         * @param u                参数
         * @param derivative_order 希望计算的最高导数阶数
         *
         * @return 基函数及其各阶导数矩阵
         */
        std::vector<std::vector<double>>
        basis_function_derivatives(
            const KnotVector &knot_vector,
            std::size_t span,
            double u,
            std::size_t derivative_order)
        {
            /*
             * B-Spline 次数：
             *
             *     p
             */
            const std::size_t p =
                knot_vector.degree();

            /*
             * p 次多项式最多只有 p 阶非零导数。
             *
             * 如果用户要求：
             *
             *     derivative_order > p
             *
             * 则超过 p 阶的部分保持为 0。
             */
            const std::size_t du =
                std::min(
                    derivative_order,
                    p);

            /*
             * ders[k][j]
             *
             * k：导数阶数
             * j：局部基函数索引
             *
             * 尺寸：
             *
             *     (derivative_order + 1) × (p + 1)
             *
             * 之所以仍然分配 derivative_order+1，
             * 是为了让调用者请求高于 p 阶导数时，
             * 对应结果自然保持为 0。
             */
            std::vector<std::vector<double>> ders(
                derivative_order + 1,
                std::vector<double>(p + 1, 0.0));

            /*
             * ndu 是算法的核心三角表。
             *
             * 它同时保存：
             *
             * 1. B-Spline 基函数递推结果
             * 2. 后续计算导数需要的节点区间分母
             *
             * ndu[r][j]
             *
             * 的上三角部分用于保存递推得到的基函数值；
             *
             * ndu[j][r]
             *
             * 的下三角部分保存对应分母。
             */
            std::vector<std::vector<double>> ndu(
                p + 1,
                std::vector<double>(p + 1, 0.0));

            std::vector<double> left(
                p + 1,
                0.0);

            std::vector<double> right(
                p + 1,
                0.0);

            /*
             * 0 次局部基函数初始值。
             */
            ndu[0][0] = 1.0;

            /*
             * --------------------------------------------------------
             * 第一阶段：
             *
             * 构造 B-Spline 基函数三角表 ndu。
             * --------------------------------------------------------
             */
            for (std::size_t j = 1;
                 j <= p;
                 ++j)
            {
                /*
                 * 左节点距离：
                 *
                 *     u - U_{span+1-j}
                 */
                left[j] =
                    u - knot_vector[span + 1 - j];

                /*
                 * 右节点距离：
                 *
                 *     U_{span+j} - u
                 */
                right[j] =
                    knot_vector[span + j] - u;

                double saved = 0.0;

                for (std::size_t r = 0;
                     r < j;
                     ++r)
                {
                    /*
                     * 保存递推分母。
                     *
                     * 这个值稍后在导数计算阶段还会再次使用。
                     */
                    ndu[j][r] =
                        right[r + 1] + left[j - r];

                    const double denominator =
                        ndu[j][r];

                    /*
                     * 当前递推比例：
                     *
                     *     ndu[r][j-1] / denominator
                     */
                    const double temp =
                        denominator == 0.0
                            ? 0.0
                            : ndu[r][j - 1] / denominator;

                    /*
                     * 更新当前层基函数。
                     */
                    ndu[r][j] =
                        saved + right[r + 1] * temp;

                    /*
                     * 保存另一部分递推贡献。
                     */
                    saved =
                        left[j - r] * temp;
                }

                /*
                 * 当前层最后一个值。
                 */
                ndu[j][j] = saved;
            }

            /*
             * ndu 的最后一列就是 p 次基函数值：
             *
             *     N_{span-p,p}(u)
             *     ...
             *     N_{span,p}(u)
             *
             * 因此它们就是 0 阶导数。
             */
            for (std::size_t j = 0;
                 j <= p;
                 ++j)
            {
                ders[0][j] = ndu[j][p];
            }

            /*
             * a 是计算某一个基函数导数时使用的临时数组。
             *
             * 只需要两行：
             *
             *     a[s1] -> 上一阶结果
             *     a[s2] -> 当前阶结果
             *
             * 每计算完一阶后交换 s1 / s2，
             * 避免重复分配二维数组。
             */
            std::vector<std::vector<double>> a(
                2,
                std::vector<double>(p + 1, 0.0));

            /*
             * --------------------------------------------------------
             * 第二阶段：
             *
             * 对每一个局部非零基函数分别计算导数。
             * --------------------------------------------------------
             */
            for (std::size_t r = 0;
                 r <= p;
                 ++r)
            {
                /*
                 * s1：
                 *     上一阶数据所在行
                 *
                 * s2：
                 *     当前阶数据所在行
                 */
                std::size_t s1 = 0;
                std::size_t s2 = 1;

                /*
                 * 0 阶初值。
                 */
                a[0][0] = 1.0;

                /*
                 * 逐阶计算：
                 *
                 *     1, 2, ..., du
                 */
                for (std::size_t k = 1;
                     k <= du;
                     ++k)
                {
                    /*
                     * d 最终保存：
                     *
                     * 当前第 r 个局部基函数
                     * 第 k 阶导数的未缩放结果。
                     */
                    double d = 0.0;

                    /*
                     * rk = r - k
                     *
                     * 使用 ptrdiff_t 是因为这个值可能小于 0。
                     *
                     * 如果使用 size_t，会发生无符号整数下溢。
                     */
                    const std::ptrdiff_t rk =
                        static_cast<std::ptrdiff_t>(r) - static_cast<std::ptrdiff_t>(k);

                    /*
                     * pk = p - k
                     *
                     * 同样作为有符号整数处理。
                     */
                    const std::ptrdiff_t pk =
                        static_cast<std::ptrdiff_t>(p) - static_cast<std::ptrdiff_t>(k);

                    /*
                     * 清空当前工作行。
                     */
                    std::fill(
                        a[s2].begin(),
                        a[s2].end(),
                        0.0);

                    /*
                     * ------------------------------------------------
                     * 处理当前递推左边界。
                     * ------------------------------------------------
                     */
                    if (r >= k)
                    {
                        const double denominator =
                            ndu[static_cast<std::size_t>(pk + 1)][static_cast<std::size_t>(rk)];

                        /*
                         * 计算左边界递推系数。
                         */
                        a[s2][0] =
                            denominator == 0.0
                                ? 0.0
                                : a[s1][0] / denominator;

                        /*
                         * 累加当前导数值。
                         */
                        d =
                            a[s2][0] *
                            ndu[static_cast<std::size_t>(rk)][static_cast<std::size_t>(pk)];
                    }

                    /*
                     * 中间递推范围的起始位置。
                     */
                    const std::ptrdiff_t j1 =
                        rk >= -1
                            ? 1
                            : -rk;

                    /*
                     * 中间递推范围的结束位置。
                     */
                    const std::ptrdiff_t j2 =
                        static_cast<std::ptrdiff_t>(r) - 1 <= pk
                            ? static_cast<std::ptrdiff_t>(k) - 1
                            : static_cast<std::ptrdiff_t>(p) - static_cast<std::ptrdiff_t>(r);

                    /*
                     * ------------------------------------------------
                     * 处理中间项。
                     * ------------------------------------------------
                     */
                    for (std::ptrdiff_t j = j1;
                         j <= j2;
                         ++j)
                    {
                        /*
                         * ndu 中对应的列索引：
                         *
                         *     rk + j
                         */
                        const std::size_t column =
                            static_cast<std::size_t>(
                                rk + j);

                        const double denominator =
                            ndu[static_cast<std::size_t>(pk + 1)][column];

                        const std::size_t jj =
                            static_cast<std::size_t>(j);

                        /*
                         * 当前递推系数：
                         *
                         * a_current[j]
                         *
                         *   =
                         *
                         * (a_previous[j]
                         *  - a_previous[j-1])
                         *
                         * / denominator
                         */
                        a[s2][jj] =
                            denominator == 0.0
                                ? 0.0
                                : (
                                      a[s1][jj] - a[s1][jj - 1]) /
                                      denominator;

                        /*
                         * 累加到当前第 k 阶导数。
                         */
                        d +=
                            a[s2][jj] *
                            ndu[column][static_cast<std::size_t>(pk)];
                    }

                    /*
                     * ------------------------------------------------
                     * 处理当前递推右边界。
                     * ------------------------------------------------
                     */
                    if (static_cast<std::ptrdiff_t>(r) <= pk)
                    {
                        const double denominator =
                            ndu[static_cast<std::size_t>(pk + 1)][r];

                        /*
                         * 右边界递推项带负号。
                         */
                        a[s2][k] =
                            denominator == 0.0
                                ? 0.0
                                : -a[s1][k - 1] / denominator;

                        d +=
                            a[s2][k] *
                            ndu[r][static_cast<std::size_t>(pk)];
                    }

                    /*
                     * 当前是尚未乘以阶乘缩放因子的导数值。
                     */
                    ders[k][r] = d;

                    /*
                     * 当前行变成下一次递推的上一行。
                     */
                    std::swap(s1, s2);
                }
            }

            /*
             * --------------------------------------------------------
             * 第三阶段：
             *
             * 对导数结果乘上正确的阶乘系数。
             * --------------------------------------------------------
             *
             * 第 k 阶导数需要乘：
             *
             *     p!
             * -----------
             *   (p-k)!
             *
             * 即：
             *
             * k = 1:
             *
             *     p
             *
             * k = 2:
             *
             *     p(p-1)
             *
             * k = 3:
             *
             *     p(p-1)(p-2)
             *
             * ...
             */
            double factor =
                static_cast<double>(p);

            for (std::size_t k = 1;
                 k <= du;
                 ++k)
            {
                /*
                 * ders[k][j] 当前是未缩放导数，
                 * 乘 factor 后得到真正的第 k 阶导数。
                 */
                for (std::size_t j = 0;
                     j <= p;
                     ++j)
                {
                    ders[k][j] *= factor;
                }

                /*
                 * 更新下一阶导数的阶乘系数。
                 *
                 * 例如：
                 *
                 * p
                 *
                 * -> p(p-1)
                 *
                 * -> p(p-1)(p-2)
                 */
                factor *=
                    static_cast<double>(p - k);
            }

            return ders;
        }

        /**
         * @brief 计算参数 u 处全部 B-Spline 基函数的指定阶导数
         *
         * basis_function_derivatives() 只返回当前 span 中
         * p+1 个非零基函数的导数。
         *
         * 本函数将局部导数展开到完整基函数数组。
         *
         * 例如 derivative_order = 1 时返回：
         *
         *     [
         *       N'_{0,p}(u),
         *       N'_{1,p}(u),
         *       ...
         *       N'_{n,p}(u)
         *     ]
         *
         * derivative_order = 2 时返回：
         *
         *     [
         *       N''_{0,p}(u),
         *       ...
         *       N''_{n,p}(u)
         *     ]
         *
         * @param knot_vector      节点向量
         * @param u                参数
         * @param derivative_order 导数阶数
         *
         * @return 所有基函数的指定阶导数
         */
        std::vector<double>
        all_basis_function_derivatives(
            const KnotVector &knot_vector,
            double u,
            std::size_t derivative_order)
        {
            /*
             * 完整结果数组。
             *
             * B-Spline 局部支撑范围外的基函数导数也为 0。
             */
            std::vector<double> result(
                knot_vector.num_basis_functions(),
                0.0);

            /*
             * 找到参数所在 knot span。
             */
            const std::size_t span =
                knot_vector.find_span(u);

            /*
             * 计算当前 span 内：
             *
             *     0 ~ derivative_order
             *
             * 阶全部局部导数。
             */
            const std::vector<std::vector<double>> local =
                basis_function_derivatives(
                    knot_vector,
                    span,
                    u,
                    derivative_order);

            /*
             * 当前第一个非零基函数：
             *
             *     N_{span-p,p}
             */
            const std::size_t first =
                span - knot_vector.degree();

            /*
             * 将用户指定的 derivative_order 阶导数
             * 写入完整全局基函数数组。
             */
            for (std::size_t j = 0;
                 j <= knot_vector.degree();
                 ++j)
            {
                result[first + j] =
                    local[derivative_order][j];
            }

            return result;
        }

    } // namespace curve
} // namespace opencagd
