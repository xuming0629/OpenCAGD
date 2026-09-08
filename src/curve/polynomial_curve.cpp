#include <opencagd/curve/polynomial_curve.h>

#include <stdexcept>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief 构造幂基形式的参数多项式曲线
         *
         * 多项式曲线表示为：
         *
         *     C(t)
         *
         *       = A_0
         *       + A_1 t
         *       + A_2 t^2
         *       + ...
         *       + A_n t^n
         *
         * 其中：
         *
         *     A_0, A_1, ..., A_n
         *
         * 是三维向量系数。
         *
         * 当前接口使用 Point3D 存储这些系数，因此：
         *
         *     coefficients[0] -> A_0
         *     coefficients[1] -> A_1
         *     ...
         *     coefficients[n] -> A_n
         *
         * 曲线次数为：
         *
         *     n = coefficients.size() - 1
         *
         * ------------------------------------------------------------
         *
         * 注意：
         *
         * 这里的 coefficients 并不是 Bézier / B-Spline 的控制点。
         *
         * 它们是幂基：
         *
         *     1, t, t^2, ..., t^n
         *
         * 对应的多项式系数。
         *
         * 例如三次曲线：
         *
         *     C(t)
         *
         *       = A_0
         *       + A_1 t
         *       + A_2 t^2
         *       + A_3 t^3
         *
         * @param coefficients 多项式系数 A_0 ... A_n
         * @param domain       曲线有效参数区间
         *
         * @throws std::invalid_argument
         *         当系数数组为空时抛出异常
         */
        PolynomialCurve::PolynomialCurve(
            const std::vector<geometry::Point3D> &coefficients,
            const geometry::Interval &domain)
            : coefficients_(coefficients),
              domain_(domain)
        {
            /*
             * 至少需要一个系数。
             *
             * 一个系数：
             *
             *     C(t) = A_0
             *
             * 对应 0 次常值曲线。
             */
            if (coefficients_.empty())
            {
                throw std::invalid_argument(
                    "PolynomialCurve requires at least one coefficient");
            }
        }

        /**
         * @brief 返回多项式曲线次数
         *
         * 如果有 n+1 个系数：
         *
         *     A_0, A_1, ..., A_n
         *
         * 那么曲线次数为：
         *
         *     degree = n
         */
        std::size_t PolynomialCurve::degree() const
        {
            return coefficients_.size() - 1;
        }

        /**
         * @brief 返回幂基多项式系数
         *
         * 返回：
         *
         *     A_0, A_1, ..., A_n
         *
         * 对应：
         *
         *     C(t) = Σ A_i t^i
         */
        const std::vector<geometry::Point3D> &
        PolynomialCurve::coefficients() const
        {
            return coefficients_;
        }

        /**
         * @brief 返回曲线参数定义域
         *
         * PolynomialCurve 不强制使用 [0,1]，
         * 参数域由构造函数显式指定。
         */
        geometry::Interval PolynomialCurve::domain() const
        {
            return domain_;
        }

        /**
         * @brief 计算多项式曲线在参数 t 处的位置
         *
         * 曲线幂基形式：
         *
         *     C(t)
         *
         *       = A_0
         *       + A_1 t
         *       + A_2 t^2
         *       + ...
         *       + A_n t^n
         *
         * 如果直接计算，可以写成：
         *
         *     A_0
         *     + A_1 t
         *     + A_2 t*t
         *     + ...
         *
         * 但这种方式需要多次计算 t 的幂。
         *
         * 本函数使用 Horner 法：
         *
         *     C(t)
         *
         *       = A_0
         *       + t(
         *           A_1
         *           + t(
         *               A_2
         *               + ...
         *               + t A_n
         *             )
         *         )
         *
         * 例如三次多项式：
         *
         *     C(t)
         *
         *       = A_0 + A_1 t + A_2 t^2 + A_3 t^3
         *
         * Horner 形式：
         *
         *     C(t)
         *
         *       = ((A_3 t + A_2)t + A_1)t + A_0
         *
         * Horner 法具有：
         *
         *     - 更少的乘法运算
         *     - 不需要显式调用 pow()
         *     - 通常具有更好的数值效率
         *
         * @param t 曲线参数
         *
         * @return 曲线点 C(t)
         */
        geometry::Point3D
        PolynomialCurve::evaluate(double t) const
        {
            /*
             * 首先检查参数是否位于有效定义域。
             */
            require_parameter(t);

            /*
             * Horner 法从最高次系数开始。
             *
             * 初始：
             *
             *     value = A_n
             */
            geometry::Vector3D value =
                coefficients_.back().as_vector();

            /*
             * 从高次向低次逐步递推：
             *
             *     value = value * t + A_i
             *
             * 例如：
             *
             *     value = A_3
             *
             *     value = A_3 t + A_2
             *
             *     value = (A_3 t + A_2)t + A_1
             *
             *     value = ((A_3 t + A_2)t + A_1)t + A_0
             */
            for (std::size_t i = coefficients_.size() - 1;
                 i > 0;
                 --i)
            {
                value =
                    value * t + coefficients_[i - 1].as_vector();
            }

            /*
             * 将最终向量结果转换为 Point3D。
             */
            return geometry::Point3D(
                value.x(),
                value.y(),
                value.z());
        }

        /**
         * @brief 计算多项式曲线在参数 t 处的任意阶导数
         *
         * 原曲线：
         *
         *            n
         * C(t) =     Σ A_i t^i
         *           i=0
         *
         * 一阶导数：
         *
         *            n
         * C'(t) =    Σ i A_i t^(i-1)
         *           i=1
         *
         * 即新的系数为：
         *
         *     A_i^(1) = (i+1) A_(i+1)
         *
         * 例如：
         *
         *     C(t)
         *
         *       = A_0
         *       + A_1 t
         *       + A_2 t^2
         *       + A_3 t^3
         *
         * 则：
         *
         *     C'(t)
         *
         *       = A_1
         *       + 2 A_2 t
         *       + 3 A_3 t^2
         *
         * ------------------------------------------------------------
         *
         * 继续求导：
         *
         *     C''(t)
         *
         *       = 2 A_2
         *       + 6 A_3 t
         *
         * 任意 k 阶导数可以通过重复执行
         * “系数乘当前幂次数”得到。
         *
         * ------------------------------------------------------------
         *
         * 当完成系数求导后，
         * 本函数再次使用 Horner 法计算导数多项式。
         *
         * @param t     曲线参数
         * @param order 导数阶数，必须 >= 1
         *
         * @return C^(order)(t)
         *
         * @throws std::invalid_argument
         *         当 order == 0 时抛出异常
         */
        geometry::Vector3D
        PolynomialCurve::derivative(
            double t,
            std::size_t order) const
        {
            /*
             * 参数合法性检查。
             */
            require_parameter(t);

            /*
             * derivative() 只用于真正的求导。
             *
             * 如果 order == 0，
             * 应直接调用 evaluate()。
             */
            if (order == 0)
            {
                throw std::invalid_argument(
                    "PolynomialCurve::derivative order must be >= 1");
            }

            /*
             * n 次多项式高于 n 阶的导数恒为零：
             *
             *     C^(k)(t) = 0, k > n
             */
            if (order > degree())
            {
                return geometry::Vector3D();
            }

            /*
             * 将 Point3D 形式的多项式系数转换成 Vector3D，
             * 方便后续进行乘法和差分运算。
             *
             * 初始：
             *
             *     coefficients[i] = A_i
             */
            std::vector<geometry::Vector3D> coefficients;

            coefficients.reserve(
                coefficients_.size());

            for (std::size_t i = 0;
                 i < coefficients_.size();
                 ++i)
            {
                coefficients.push_back(
                    coefficients_[i].as_vector());
            }

            /*
             * 当前多项式次数。
             *
             * 每求一次导：
             *
             *     current_degree -= 1
             */
            std::size_t current_degree =
                degree();

            /*
             * 重复 order 次，
             * 构造 order 阶导数多项式的系数。
             */
            for (std::size_t k = 0;
                 k < order;
                 ++k)
            {
                /*
                 * m 次多项式求导后变成 m-1 次多项式，
                 * 因此新的系数数量为：
                 *
                 *     current_degree
                 */
                std::vector<geometry::Vector3D> next(
                    current_degree);

                /*
                 * 如果当前多项式：
                 *
                 *     P(t)
                 *
                 *       = A_0
                 *       + A_1 t
                 *       + ...
                 *       + A_m t^m
                 *
                 * 则：
                 *
                 *     P'(t)
                 *
                 *       = A_1
                 *       + 2A_2 t
                 *       + ...
                 *       + mA_m t^(m-1)
                 *
                 * 因此新的第 i-1 个系数为：
                 *
                 *     next[i-1] = i * coefficients[i]
                 */
                for (std::size_t i = 1;
                     i <= current_degree;
                     ++i)
                {
                    next[i - 1] =
                        coefficients[i] * static_cast<double>(i);
                }

                /*
                 * 用新的导数系数替换原系数。
                 */
                coefficients.swap(next);

                /*
                 * 多项式次数降低 1。
                 */
                --current_degree;
            }

            /*
             * 此时 coefficients 已经是：
             *
             *     C^(order)(t)
             *
             * 的幂基系数。
             *
             * 下面继续使用 Horner 法求值。
             */

            /*
             * 从最高次项开始：
             *
             *     value = highest coefficient
             */
            geometry::Vector3D value =
                coefficients.back();

            /*
             * Horner 递推：
             *
             *     value = value * t + coefficient
             */
            for (std::size_t i = coefficients.size() - 1;
                 i > 0;
                 --i)
            {
                value =
                    value * t + coefficients[i - 1];
            }

            return value;
        }

    } // namespace curve
} // namespace opencagd
