#include <opencagd/curve/nurbs_curve.h>
#include <opencagd/curve/bspline_basis.h>
#include <opencagd/math/numeric.h>

#include <cmath>
#include <stdexcept>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief 构造 NURBS 曲线
         *
         * NURBS：
         *
         *     Non-Uniform Rational B-Spline
         *
         * 即：
         *
         *     非均匀有理 B 样条曲线
         *
         * 一条 p 次 NURBS 曲线定义为：
         *
         *                  n
         *                 Σ N_{i,p}(u) w_i P_i
         *                 i=0
         *     C(u) = --------------------------------
         *                  n
         *                 Σ N_{i,p}(u) w_i
         *                 i=0
         *
         * 其中：
         *
         *     P_i         为控制点
         *     w_i         为控制点权重
         *     N_{i,p}(u)  为 p 次 B-Spline 基函数
         *     U           为节点向量
         *
         * 也可以定义 NURBS 有理基函数：
         *
         *                       N_{i,p}(u) w_i
         *     R_{i,p}(u) = ---------------------------
         *                        Σ N_{j,p}(u) w_j
         *
         * 从而写成：
         *
         *     C(u) = Σ R_{i,p}(u) P_i
         *
         * @param degree         曲线次数 p
         * @param knots          节点向量
         * @param control_points 控制点
         * @param weights        与控制点一一对应的权重
         *
         * @throws std::invalid_argument
         *         当控制点数量、权重数量或权重值不合法时抛出异常
         */
        NURBSCurve::NURBSCurve(
            std::size_t degree,
            const std::vector<double> &knots,
            const std::vector<geometry::Point3D> &control_points,
            const std::vector<double> &weights)
            : knot_vector_(degree, knots),
              control_points_(control_points),
              weights_(weights)
        {
            // 检查控制点、节点向量及权重之间的关系是否合法。
            validate();
        }

        /**
         * @brief 验证 NURBS 曲线数据是否合法
         *
         * 主要检查：
         *
         * 1. 控制点数量必须等于 B-Spline 基函数数量；
         * 2. 每一个控制点必须有且仅有一个权重；
         * 3. 当前实现要求所有权重严格大于 0。
         */
        void NURBSCurve::validate() const
        {
            /*
             * 对 p 次 B-Spline / NURBS：
             *
             *     num_basis_functions
             *
             *       = knot_count - degree - 1
             *
             * 每一个基函数对应一个控制点，因此：
             *
             *     control_point_count
             *       == num_basis_functions
             */
            if (control_points_.size() !=
                knot_vector_.num_basis_functions())
            {
                throw std::invalid_argument(
                    "NURBS control-point count must equal "
                    "knot_count - degree - 1");
            }

            /*
             * 每一个控制点 P_i 都必须对应一个权重 w_i：
             *
             *     P_0 <-> w_0
             *     P_1 <-> w_1
             *     ...
             *     P_n <-> w_n
             */
            if (weights_.size() != control_points_.size())
            {
                throw std::invalid_argument(
                    "NURBS requires one weight per control point");
            }

            /*
             * 当前实现要求：
             *
             *     w_i > 0
             *
             * 正权重是 CAD / CAGD 中最常见的 NURBS 情况。
             *
             * 正权重可以保持有理基函数的非负性，
             * 并避免很多奇异情况。
             */
            for (std::size_t i = 0;
                 i < weights_.size();
                 ++i)
            {
                if (weights_[i] <= 0.0)
                {
                    throw std::invalid_argument(
                        "NURBS weights must be positive");
                }
            }
        }

        /**
         * @brief 返回 NURBS 曲线次数
         *
         * @return degree = p
         */
        std::size_t NURBSCurve::degree() const
        {
            return knot_vector_.degree();
        }

        /**
         * @brief 返回节点向量
         */
        const KnotVector &NURBSCurve::knot_vector() const
        {
            return knot_vector_;
        }

        /**
         * @brief 返回控制点集合
         */
        const std::vector<geometry::Point3D> &
        NURBSCurve::control_points() const
        {
            return control_points_;
        }

        /**
         * @brief 返回控制点权重集合
         *
         * 权重：
         *
         *     w_0, w_1, ..., w_n
         *
         * 与控制点：
         *
         *     P_0, P_1, ..., P_n
         *
         * 一一对应。
         */
        const std::vector<double> &
        NURBSCurve::weights() const
        {
            return weights_;
        }

        /**
         * @brief 返回 NURBS 曲线有效参数域
         *
         * NURBS 与对应 B-Spline 使用相同节点向量，
         * 因此参数域也完全由 KnotVector 决定。
         */
        geometry::Interval NURBSCurve::domain() const
        {
            return knot_vector_.domain();
        }

        /**
         * @brief 计算参数 u 处所有 NURBS 有理基函数
         *
         * 普通 B-Spline 基函数为：
         *
         *     N_{i,p}(u)
         *
         * NURBS 有理基函数定义为：
         *
         *                       N_{i,p}(u) w_i
         *     R_{i,p}(u) = ---------------------------
         *                       Σ N_{j,p}(u) w_j
         *
         * 因此：
         *
         *     C(u)
         *
         *       = Σ R_{i,p}(u) P_i
         *
         * ------------------------------------------------------------
         *
         * 有理基函数仍满足单位分解性质：
         *
         *     Σ R_{i,p}(u) = 1
         *
         * 当所有权重相同时：
         *
         *     w_0 = w_1 = ... = w_n
         *
         * 权重可以从分子、分母中约掉，
         * 此时 NURBS 自动退化为普通 B-Spline。
         *
         * @param u 参数
         *
         * @return 所有 NURBS 有理基函数值
         */
        std::vector<double>
        NURBSCurve::rational_basis_functions(double u) const
        {
            /*
             * 检查参数是否位于有效参数域。
             */
            require_parameter(u);

            /*
             * 先计算普通 B-Spline 基函数：
             *
             *     N_{0,p}(u),
             *     ...
             *     N_{n,p}(u)
             *
             * 这里返回完整基函数数组，
             * 局部支撑范围之外的值自动为 0。
             */
            const std::vector<double> nonrational =
                all_basis_functions(
                    knot_vector_,
                    u);

            /*
             * rational[i] 最终保存：
             *
             *     R_{i,p}(u)
             */
            std::vector<double> rational(
                nonrational.size(),
                0.0);

            /*
             * NURBS 公共分母：
             *
             *     W(u)
             *
             *       = Σ N_{i,p}(u) w_i
             */
            double denominator = 0.0;

            for (std::size_t i = 0;
                 i < nonrational.size();
                 ++i)
            {
                denominator +=
                    nonrational[i] * weights_[i];
            }

            /*
             * 如果：
             *
             *     W(u) ≈ 0
             *
             * 则有理基函数无法定义。
             *
             * 当前所有权重都要求为正，
             * 正常情况下 denominator 应当为正。
             *
             * 这里仍进行数值安全检查。
             */
            if (std::abs(denominator) <=
                math::tolerance())
            {
                throw std::runtime_error(
                    "NURBS rational basis denominator is zero");
            }

            /*
             * 根据定义：
             *
             *                       N_i(u) w_i
             *     R_i(u) = -------------------------
             *                    Σ N_j(u) w_j
             */
            for (std::size_t i = 0;
                 i < nonrational.size();
                 ++i)
            {
                rational[i] =
                    nonrational[i] * weights_[i] / denominator;
            }

            return rational;
        }

        /**
         * @brief 计算 NURBS 曲线在参数 u 处的位置
         *
         * NURBS 曲线：
         *
         *                  n
         *                 Σ N_{i,p}(u) w_i P_i
         *                 i=0
         *     C(u) = --------------------------------
         *                  n
         *                 Σ N_{i,p}(u) w_i
         *                 i=0
         *
         * 定义：
         *
         *     A(u) = Σ N_{i,p}(u) w_i P_i
         *
         *     W(u) = Σ N_{i,p}(u) w_i
         *
         * 则：
         *
         *              A(u)
         *     C(u) = --------
         *              W(u)
         *
         * ------------------------------------------------------------
         *
         * 由于 B-Spline 基函数具有局部支撑性，
         * 参数 u 处最多只有 p+1 个基函数非零。
         *
         * 因此这里没有遍历所有控制点，
         * 而只计算当前 span 对应的 p+1 个控制点。
         *
         * @param u 参数
         *
         * @return NURBS 曲线点 C(u)
         */
        geometry::Point3D
        NURBSCurve::evaluate(double u) const
        {
            // 检查参数合法性。
            require_parameter(u);

            /*
             * 查找参数所在节点区间：
             *
             *     U_span <= u < U_{span+1}
             */
            const std::size_t span =
                knot_vector_.find_span(u);

            /*
             * 只计算当前 span 中 p+1 个非零
             * B-Spline 基函数。
             */
            const std::vector<double> basis =
                basis_functions(
                    knot_vector_,
                    span,
                    u);

            /*
             * 当前第一个相关基函数 /
             * 控制点索引：
             *
             *     first = span - p
             */
            const std::size_t first =
                span - degree();

            /*
             * NURBS 分子：
             *
             *     A(u)
             *
             *       = Σ N_i(u) w_i P_i
             */
            geometry::Vector3D numerator;

            /*
             * NURBS 分母：
             *
             *     W(u)
             *
             *       = Σ N_i(u) w_i
             */
            double denominator = 0.0;

            /*
             * 根据局部支撑性，
             * 只遍历当前的 p+1 个控制点。
             */
            for (std::size_t j = 0;
                 j <= degree();
                 ++j)
            {
                /*
                 * 对应全局控制点索引：
                 *
                 *     i = first + j
                 */
                const std::size_t index =
                    first + j;

                /*
                 * 当前有理加权因子：
                 *
                 *     N_{i,p}(u) w_i
                 */
                const double factor =
                    basis[j] * weights_[index];

                /*
                 * 累加分子：
                 *
                 *     A(u)
                 *
                 *       += N_i(u) w_i P_i
                 */
                numerator +=
                    control_points_[index].as_vector() * factor;

                /*
                 * 累加分母：
                 *
                 *     W(u)
                 *
                 *       += N_i(u) w_i
                 */
                denominator += factor;
            }

            /*
             * 防止数值意义上的除零。
             */
            if (std::abs(denominator) <=
                math::tolerance())
            {
                throw std::runtime_error(
                    "NURBS curve denominator is numerically zero");
            }

            /*
             * 最终：
             *
             *              A(u)
             *     C(u) = --------
             *              W(u)
             */
            const geometry::Vector3D value =
                numerator / denominator;

            return geometry::Point3D(
                value.x(),
                value.y(),
                value.z());
        }

        /**
         * @brief 计算 NURBS 曲线在 u 处的任意阶导数
         *
         * NURBS 与普通 B-Spline 的重要区别在于：
         *
         * B-Spline：
         *
         *     C(u) = Σ N_i(u) P_i
         *
         * 可以直接写：
         *
         *     C^(k)(u) = Σ N_i^(k)(u) P_i
         *
         * ------------------------------------------------------------
         *
         * 但 NURBS 是一个有理函数：
         *
         *              A(u)
         *     C(u) = --------
         *              W(u)
         *
         * 其中：
         *
         *     A(u) = Σ N_i(u) w_i P_i
         *
         *     W(u) = Σ N_i(u) w_i
         *
         * 因此不能只对基函数求导后直接加权，
         * 而必须使用有理函数导数递推。
         *
         * ------------------------------------------------------------
         *
         * 因为：
         *
         *     A(u) = W(u) C(u)
         *
         * 对其求 k 阶导数，根据 Leibniz 乘积法则：
         *
         *               k
         *     A^(k) =   Σ C(k,i)
         *              i=0
         *
         *               W^(i) C^(k-i)
         *
         * 将 i = 0 的项分离：
         *
         *     A^(k)
         *
         *       = W C^(k)
         *
         *         +
         *
         *           k
         *           Σ C(k,i) W^(i) C^(k-i)
         *          i=1
         *
         * 因此：
         *
         *                1
         *     C^(k) = --------
         *               W
         *
         *        [
         *           A^(k)
         *
         *             -
         *
         *              k
         *              Σ C(k,i)
         *             i=1
         *
         *              W^(i) C^(k-i)
         *        ]
         *
         * 这就是本函数使用的递推公式。
         *
         * ------------------------------------------------------------
         *
         * @param u     曲线参数
         * @param order 导数阶数，必须 >= 1
         *
         * @return NURBS 曲线的 order 阶导数
         *
         * @throws std::invalid_argument
         *         当 order == 0 时抛出异常
         *
         * @throws std::runtime_error
         *         当有理函数分母 W(u) 数值上接近 0 时抛出异常
         */
        geometry::Vector3D
        NURBSCurve::derivative(
            double u,
            std::size_t order) const
        {
            /*
             * 参数必须处于有效域。
             */
            require_parameter(u);

            /*
             * derivative() 专门用于计算至少一阶导数。
             */
            if (order == 0)
            {
                throw std::invalid_argument(
                    "NURBSCurve::derivative order must be >= 1");
            }

            /*
             * 查找参数所在节点区间。
             */
            const std::size_t span =
                knot_vector_.find_span(u);

            /*
             * 计算当前 span 中所有非零 B-Spline 基函数
             * 从 0 阶到 order 阶的导数：
             *
             *     ders[k][j]
             *
             * 对应局部第 j 个基函数的第 k 阶导数。
             */
            const std::vector<std::vector<double>> ders =
                basis_function_derivatives(
                    knot_vector_,
                    span,
                    u,
                    order);

            /*
             * 当前第一个非零基函数对应控制点：
             *
             *     first = span - p
             */
            const std::size_t first =
                span - degree();

            /*
             * --------------------------------------------------------
             * A[k]
             * --------------------------------------------------------
             *
             * 保存：
             *
             *     A^(k)(u)
             *
             * 其中：
             *
             *     A(u)
             *
             *       = Σ N_i(u) w_i P_i
             *
             * 因此：
             *
             *     A^(k)(u)
             *
             *       = Σ N_i^(k)(u) w_i P_i
             */
            std::vector<geometry::Vector3D> A(
                order + 1,
                geometry::Vector3D());

            /*
             * --------------------------------------------------------
             * W[k]
             * --------------------------------------------------------
             *
             * 保存：
             *
             *     W^(k)(u)
             *
             * 其中：
             *
             *     W(u)
             *
             *       = Σ N_i(u) w_i
             *
             * 所以：
             *
             *     W^(k)(u)
             *
             *       = Σ N_i^(k)(u) w_i
             */
            std::vector<double> W(
                order + 1,
                0.0);

            /*
             * 计算：
             *
             *     A^(0), ..., A^(order)
             *
             * 和：
             *
             *     W^(0), ..., W^(order)
             */
            for (std::size_t k = 0;
                 k <= order;
                 ++k)
            {
                /*
                 * 根据局部支撑性，
                 * 参数 u 处只需要 p+1 个基函数。
                 */
                for (std::size_t j = 0;
                     j <= degree();
                     ++j)
                {
                    /*
                     * 对应全局控制点索引。
                     */
                    const std::size_t index =
                        first + j;

                    /*
                     * 当前项：
                     *
                     *     N_i^(k)(u) w_i
                     */
                    const double factor =
                        ders[k][j] * weights_[index];

                    /*
                     * A^(k)：
                     *
                     *     += N_i^(k)(u) w_i P_i
                     */
                    A[k] +=
                        control_points_[index].as_vector() * factor;

                    /*
                     * W^(k)：
                     *
                     *     += N_i^(k)(u) w_i
                     */
                    W[k] += factor;
                }
            }

            /*
             * W[0] 就是：
             *
             *     W(u)
             *
             * 即 NURBS 的公共分母。
             *
             * 如果：
             *
             *     W(u) ≈ 0
             *
             * 则曲线和导数均无法正常定义。
             */
            if (std::abs(W[0]) <=
                math::tolerance())
            {
                throw std::runtime_error(
                    "NURBS curve derivative denominator "
                    "is numerically zero");
            }

            /*
             * C[k] 保存：
             *
             *     C^(k)(u)
             *
             * 即：
             *
             *     C[0]     = C(u)
             *     C[1]     = C'(u)
             *     C[2]     = C''(u)
             *     ...
             */
            std::vector<geometry::Vector3D> C(
                order + 1,
                geometry::Vector3D());

            /*
             * 0 阶：
             *
             *              A(u)
             *     C(u) = --------
             *              W(u)
             */
            C[0] =
                A[0] / W[0];

            /*
             * 从一阶开始使用递推公式计算。
             */
            for (std::size_t k = 1;
                 k <= order;
                 ++k)
            {
                /*
                 * 先从：
                 *
                 *     A^(k)(u)
                 *
                 * 开始。
                 */
                geometry::Vector3D value =
                    A[k];

                /*
                 * 根据公式：
                 *
                 *                1
                 * C^(k) = ----------------
                 *               W^(0)
                 *
                 *      [
                 *        A^(k)
                 *
                 *        -
                 *
                 *         k
                 *         Σ C(k,i)
                 *        i=1
                 *
                 *         W^(i) C^(k-i)
                 *      ]
                 */
                for (std::size_t i = 1;
                     i <= k;
                     ++i)
                {
                    /*
                     * 二项式系数：
                     *
                     *     C(k,i)
                     *
                     * 乘以：
                     *
                     *     W^(i)
                     */
                    const double coefficient =
                        static_cast<double>(
                            math::binomial(k, i)) *
                        W[i];

                    /*
                     * 减去：
                     *
                     *     C(k,i)
                     *     W^(i)
                     *     C^(k-i)
                     */
                    value -=
                        C[k - i] * coefficient;
                }

                /*
                 * 最后除以：
                 *
                 *     W^(0) = W(u)
                 *
                 * 得到：
                 *
                 *     C^(k)(u)
                 */
                C[k] =
                    value / W[0];
            }

            /*
             * 返回用户请求的 order 阶导数。
             */
            return C[order];
        }

    } // namespace curve
} // namespace opencagd
