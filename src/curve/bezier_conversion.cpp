#include <opencagd/curve/bezier_conversion.h>
#include <opencagd/math/numeric.h>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief 将 Bézier 曲线从 Bernstein 基表示转换为幂基表示
         *
         * Bézier 曲线通常表示为：
         *
         *      n
         * C(t) = Σ P_i B_i^n(t)
         *      i=0
         *
         * 其中 Bernstein 基函数为：
         *
         * B_i^n(t)
         *     = C(n, i) t^i (1 - t)^(n-i)
         *
         * 将 (1 - t)^(n-i) 使用二项式定理展开：
         *
         *                    n-i
         * (1 - t)^(n-i) =     Σ C(n-i, j) (-1)^j t^j
         *                    j=0
         *
         * 因此：
         *
         *                        n-i
         * B_i^n(t) = C(n,i) t^i Σ C(n-i,j)(-1)^j t^j
         *                        j=0
         *
         *            n-i
         *          =  Σ C(n,i) C(n-i,j) (-1)^j t^(i+j)
         *            j=0
         *
         * 代回 Bézier 曲线：
         *
         *      n   n-i
         * C(t) = Σ   Σ
         *      i=0 j=0
         *
         *        P_i C(n,i) C(n-i,j) (-1)^j t^(i+j)
         *
         * 令：
         *
         * k = i + j
         *
         * 即可整理成普通幂基形式：
         *
         * C(t) = A_0
         *      + A_1 t
         *      + A_2 t^2
         *      + ...
         *      + A_n t^n
         *
         * 其中 A_k 是三维向量系数。
         *
         * 本函数计算这些 A_k，并构造 PolynomialCurve。
         *
         * @param curve 输入 Bézier 曲线
         *
         * @return 与输入 Bézier 曲线几何等价的幂基 PolynomialCurve
         */
        PolynomialCurve to_power_basis(const BezierCurve &curve)
        {
            /*
             * Bézier 曲线次数：
             *
             * n = 控制点数 - 1
             */
            const std::size_t n = curve.degree();

            /*
             * 幂基表示需要 n + 1 个系数：
             *
             * C(t) =
             *     A_0
             *   + A_1 t
             *   + A_2 t^2
             *   + ...
             *   + A_n t^n
             *
             * 这里先使用 Vector3D 存储系数，
             * 因为转换过程中需要不断进行向量累加。
             *
             * coefficient_vectors[k] 对应：
             *
             * A_k
             */
            std::vector<geometry::Vector3D> coefficient_vectors(
                n + 1,
                geometry::Vector3D());

            /*
             * Bézier 控制点：
             *
             * P_0, P_1, ..., P_n
             */
            const std::vector<geometry::Point3D> &control =
                curve.control_points();

            /*
             * 对每一个 Bernstein 基函数：
             *
             * B_i^n(t)
             *
             * 进行幂展开。
             */
            for (std::size_t i = 0; i <= n; ++i)
            {
                /*
                 * Bernstein 基函数：
                 *
                 * B_i^n(t)
                 *   = C(n,i) t^i (1-t)^(n-i)
                 *
                 * 二项式展开：
                 *
                 * (1-t)^(n-i)
                 *
                 *      n-i
                 *   =   Σ C(n-i,j)(-1)^j t^j
                 *      j=0
                 *
                 * 所以 j 的取值范围为：
                 *
                 * 0 <= j <= n-i
                 */
                for (std::size_t j = 0; j <= n - i; ++j)
                {
                    /*
                     * 当前项的幂次数：
                     *
                     * t^i * t^j = t^(i+j)
                     *
                     * 因此：
                     *
                     * k = i + j
                     */
                    const std::size_t k = i + j;

                    /*
                     * (-1)^j
                     *
                     * 当 j 为偶数：
                     *
                     * (-1)^j = +1
                     *
                     * 当 j 为奇数：
                     *
                     * (-1)^j = -1
                     */
                    const double sign =
                        (j % 2 == 0) ? 1.0 : -1.0;

                    /*
                     * 当前展开项的组合系数：
                     *
                     * C(n,i) * C(n-i,j) * (-1)^j
                     *
                     * 分别来自：
                     *
                     * C(n,i)
                     *
                     *     Bernstein 基函数本身
                     *
                     * C(n-i,j)
                     *
                     *     (1-t)^(n-i) 的二项式展开
                     *
                     * (-1)^j
                     *
                     *     由于展开的是 (1-t)，而不是 (1+t)
                     */
                    const double factor =
                        static_cast<double>(math::binomial(n, i)) *
                        static_cast<double>(math::binomial(n - i, j)) *
                        sign;

                    /*
                     * Bézier 曲线中的该项为：
                     *
                     * P_i *
                     * C(n,i) *
                     * C(n-i,j) *
                     * (-1)^j *
                     * t^(i+j)
                     *
                     * 因为 k = i+j，
                     * 所以把它累加到 t^k 的系数 A_k 中。
                     */
                    coefficient_vectors[k] +=
                        control[i].as_vector() * factor;
                }
            }

            /*
             * PolynomialCurve 当前使用 Point3D
             * 保存多项式系数。
             *
             * 因此将前面计算得到的 Vector3D：
             *
             * A_0, A_1, ..., A_n
             *
             * 转换成 Point3D。
             */
            std::vector<geometry::Point3D> coefficients;

            coefficients.reserve(coefficient_vectors.size());

            for (std::size_t i = 0;
                 i < coefficient_vectors.size();
                 ++i)
            {
                coefficients.push_back(
                    geometry::Point3D(
                        coefficient_vectors[i].x(),
                        coefficient_vectors[i].y(),
                        coefficient_vectors[i].z()));
            }

            /*
             * 返回幂基曲线：
             *
             * C(t) =
             * A_0 + A_1 t + ... + A_n t^n
             *
             * 参数区间继续保持 Bézier 曲线原有的 domain。
             */
            return PolynomialCurve(
                coefficients,
                curve.domain());
        }

    } // namespace curve
} // namespace opencagd