#include <opencagd/curve/bernstein.h>
#include <opencagd/math/numeric.h>

#include <cmath>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief 计算 Bernstein 基函数 B_i^n(t)
         *
         * Bernstein 基函数定义为：
         *
         *              n!
         * B_i^n(t) = --------- t^i (1 - t)^(n-i)
         *            i!(n-i)!
         *
         * 即：
         *
         * B_i^n(t) = C(n, i) * t^i * (1 - t)^(n-i)
         *
         * 其中：
         *   - n      为 Bernstein 多项式的次数 degree
         *   - i      为基函数索引，0 <= i <= n
         *   - t      为参数，通常定义在区间 [0, 1]
         *   - C(n,i) 为二项式系数（组合数）
         *
         * Bernstein 基函数是 Bézier 曲线的基础。
         * n 次 Bézier 曲线可以写成：
         *
         * P(t) = Σ B_i^n(t) * P_i
         *        i=0...n
         *
         * 其中 P_i 为控制点。
         *
         * @param i      Bernstein 基函数索引
         * @param degree 多项式次数 n
         * @param t      参数值，通常位于 [0, 1]
         *
         * @return B_i^n(t) 的值
         */
        double bernstein(std::size_t i, std::size_t degree, double t)
        {
            // Bernstein 基函数只定义索引范围 0 <= i <= degree。
            // 如果 i 超出范围，则该基函数视为 0。
            if (i > degree)
                return 0.0;

            /*
             * 参数位于左端点 t = 0 时：
             *
             * B_0^n(0) = 1
             * B_i^n(0) = 0, i > 0
             *
             * 因此 Bézier 曲线具有端点插值性质：
             *
             * P(0) = P_0
             *
             * 这里同时处理 t < 0 的情况，
             * 将其按照左端点进行处理。
             */
            if (t <= 0.0)
                return i == 0 ? 1.0 : 0.0;

            /*
             * 参数位于右端点 t = 1 时：
             *
             * B_n^n(1) = 1
             * B_i^n(1) = 0, i < n
             *
             * 因此：
             *
             * P(1) = P_n
             *
             * 这里同时处理 t > 1 的情况，
             * 将其按照右端点进行处理。
             */
            if (t >= 1.0)
                return i == degree ? 1.0 : 0.0;

            /*
             * 根据 Bernstein 多项式显式公式计算：
             *
             * B_i^n(t)
             *   = C(n, i)
             *   * t^i
             *   * (1 - t)^(n-i)
             *
             * math::binomial(degree, i)
             * 用于计算组合数：
             *
             *            n!
             * C(n,i) = --------
             *          i!(n-i)!
             */
            return static_cast<double>(math::binomial(degree, i)) *
                   std::pow(t, static_cast<int>(i)) *
                   std::pow(1.0 - t, static_cast<int>(degree - i));
        }

        /**
         * @brief 计算某个参数 t 下全部 Bernstein 基函数
         *
         * 对于给定次数 n，将计算：
         *
         * B_0^n(t),
         * B_1^n(t),
         * ...
         * B_n^n(t)
         *
         * 因此返回向量大小始终为：
         *
         * degree + 1
         *
         * 例如，当 degree = 3 时，返回：
         *
         * [
         *     B_0^3(t),
         *     B_1^3(t),
         *     B_2^3(t),
         *     B_3^3(t)
         * ]
         *
         * Bernstein 基函数具有重要的单位分解性质：
         *
         * Σ B_i^n(t) = 1
         * i=0...n
         *
         * 对于 t ∈ [0,1]，同时满足：
         *
         * B_i^n(t) >= 0
         *
         * 这些性质保证 Bézier 曲线位于控制点凸包内部。
         *
         * @param degree Bernstein 多项式次数 n
         * @param t      参数值
         *
         * @return 包含全部 n+1 个 Bernstein 基函数值的向量
         */
        std::vector<double> bernstein_basis(std::size_t degree, double t)
        {
            // n 次 Bernstein 多项式共有 n + 1 个基函数：
            //
            // B_0^n, B_1^n, ..., B_n^n
            std::vector<double> values(degree + 1, 0.0);

            // 逐个计算 B_i^n(t)。
            for (std::size_t i = 0; i <= degree; ++i)
            {
                values[i] = bernstein(i, degree, t);
            }

            return values;
        }

    } // namespace curve
} // namespace opencagd