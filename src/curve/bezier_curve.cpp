
#include <opencagd/curve/bezier_curve.h>
#include <opencagd/curve/bernstein.h>
#include <opencagd/math/numeric.h>

#include <stdexcept>

namespace opencagd
{
    namespace curve
    {

        namespace
        {

            /**
             * @brief 根据一组权重对三维点进行线性组合
             *
             * 计算：
             *
             *     P = Σ w_i P_i
             *
             * 其中：
             *
             *     P_i   为输入点
             *     w_i   为对应权重
             *
             * 在 Bézier 曲线求值中，权重通常就是 Bernstein 基函数：
             *
             *     w_i = B_i^n(t)
             *
             * 因此：
             *
             *     C(t) = Σ B_i^n(t) P_i
             *
             * @param points  输入点集合
             * @param weights 与每个点对应的权重
             *
             * @return 加权求和后的三维点
             *
             * @note
             * 本函数默认：
             *
             *     points.size() == weights.size()
             *
             * 当前属于 cpp 文件内部辅助函数，因此放在匿名命名空间中，
             * 不向外部暴露。
             */
            geometry::Point3D point_from_weighted_sum(
                const std::vector<geometry::Point3D> &points,
                const std::vector<double> &weights)
            {
                /*
                 * 使用 Vector3D 进行累加。
                 *
                 * 初始值：
                 *
                 *     value = (0, 0, 0)
                 */
                geometry::Vector3D value;

                /*
                 * 计算：
                 *
                 *     value = Σ weights[i] * points[i]
                 */
                for (std::size_t i = 0; i < points.size(); ++i)
                    value += points[i].as_vector() * weights[i];

                /*
                 * 将最终向量转换回 Point3D。
                 */
                return geometry::Point3D(
                    value.x(),
                    value.y(),
                    value.z());
            }

        } // anonymous namespace

        /**
         * @brief 根据控制点构造 Bézier 曲线
         *
         * 若控制点数量为：
         *
         *     n + 1
         *
         * 则 Bézier 曲线次数为：
         *
         *     n
         *
         * Bézier 曲线定义：
         *
         *           n
         * C(t) =    Σ B_i^n(t) P_i
         *          i=0
         *
         * 其中：
         *
         *     P_i       为控制点
         *     B_i^n(t)  为 n 次 Bernstein 基函数
         *     t ∈ [0,1]
         *
         * @param control_points Bézier 曲线控制点
         *
         * @throws std::invalid_argument
         *         当控制点为空时抛出异常
         */
        BezierCurve::BezierCurve(
            const std::vector<geometry::Point3D> &control_points)
            : control_points_(control_points)
        {
            /*
             * Bézier 曲线至少需要一个控制点。
             *
             * 一个控制点对应 0 次 Bézier 曲线，
             * 即常值曲线：
             *
             *     C(t) = P_0
             */
            if (control_points_.empty())
                throw std::invalid_argument(
                    "BezierCurve requires at least one control point");
        }

        /**
         * @brief 返回 Bézier 曲线次数
         *
         * 若控制点数量为：
         *
         *     N
         *
         * 则曲线次数：
         *
         *     degree = N - 1
         *
         * 例如：
         *
         *     2 个控制点 -> 1 次 Bézier 曲线
         *     3 个控制点 -> 2 次 Bézier 曲线
         *     4 个控制点 -> 3 次 Bézier 曲线
         */
        std::size_t BezierCurve::degree() const
        {
            return control_points_.size() - 1;
        }

        /**
         * @brief 返回 Bézier 曲线控制点集合
         *
         * 返回 const 引用，避免不必要的数据复制，
         * 同时禁止调用者直接修改内部控制点。
         */
        const std::vector<geometry::Point3D> &
        BezierCurve::control_points() const
        {
            return control_points_;
        }

        /**
         * @brief 返回 Bézier 曲线标准参数域
         *
         * 标准 Bézier 曲线定义在：
         *
         *     t ∈ [0,1]
         */
        geometry::Interval BezierCurve::domain() const
        {
            return geometry::Interval(0.0, 1.0);
        }

        /**
         * @brief 使用 Bernstein 基函数直接计算 Bézier 曲线点
         *
         * Bézier 曲线定义：
         *
         *           n
         * C(t) =    Σ B_i^n(t) P_i
         *          i=0
         *
         * Bernstein 基函数：
         *
         *                n
         * B_i^n(t) =   (   ) t^i (1-t)^(n-i)
         *                i
         *
         * @param t 参数，必须满足 t ∈ [0,1]
         *
         * @return 曲线在参数 t 处的点 C(t)
         */
        geometry::Point3D BezierCurve::evaluate(double t) const
        {
            // 检查 t 是否位于合法参数区间 [0,1]。
            require_parameter(t);

            /*
             * bernstein_basis(degree(), t)
             *
             * 返回：
             *
             * [
             *   B_0^n(t),
             *   B_1^n(t),
             *   ...
             *   B_n^n(t)
             * ]
             *
             * 然后与控制点进行加权求和：
             *
             *     C(t) = Σ P_i B_i^n(t)
             */
            return point_from_weighted_sum(
                control_points_,
                bernstein_basis(degree(), t));
        }

        /**
         * @brief 使用 De Casteljau 算法计算 Bézier 曲线点
         *
         * De Casteljau 算法通过反复线性插值求值。
         *
         * 初始层：
         *
         *     P_i^(0) = P_i
         *
         * 递推关系：
         *
         *     P_i^(r)(t)
         *
         *       = (1-t) P_i^(r-1)(t)
         *         + t P_(i+1)^(r-1)(t)
         *
         * 当 r = n 时，只剩一个点：
         *
         *     C(t) = P_0^(n)(t)
         *
         * 与直接 Bernstein 求值相比，
         * De Casteljau 算法通常具有更好的数值稳定性，
         * 同时还可以直接用于 Bézier 曲线细分。
         *
         * @param t 参数
         *
         * @return 曲线在 t 处的点
         */
        geometry::Point3D
        BezierCurve::evaluate_de_casteljau(double t) const
        {
            /*
             * construction_levels(t) 返回完整的
             * De Casteljau 三角形。
             */
            const std::vector<std::vector<geometry::Point3D>> levels =
                construction_levels(t);

            /*
             * 最后一层只有一个点：
             *
             *     P_0^(n)(t)
             *
             * 它正是 Bézier 曲线点 C(t)。
             */
            return levels.back().front();
        }

        /**
         * @brief 计算完整的 De Casteljau 构造层级
         *
         * 对 n 次 Bézier 曲线：
         *
         * 第 0 层：
         *
         *     P_0, P_1, ..., P_n
         *
         * 第 1 层：
         *
         *     P_0^(1), ..., P_(n-1)^(1)
         *
         * 第 2 层：
         *
         *     P_0^(2), ..., P_(n-2)^(2)
         *
         * ...
         *
         * 第 n 层：
         *
         *     P_0^(n) = C(t)
         *
         * 例如三次 Bézier：
         *
         *     P0 -------- P1 -------- P2 -------- P3
         *
         *          Q0 -------- Q1 -------- Q2
         *
         *               R0 -------- R1
         *
         *                     C(t)
         *
         * 该函数不仅用于求值，也非常适合：
         *
         *     - 可视化 De Casteljau 构造过程
         *     - Bézier 曲线细分
         *     - 教学展示
         *
         * @param t 参数
         *
         * @return 从控制多边形到最终曲线点的所有插值层
         */
        std::vector<std::vector<geometry::Point3D>>
        BezierCurve::construction_levels(double t) const
        {
            require_parameter(t);

            /*
             * levels 用于保存 De Casteljau 每一级的点。
             */
            std::vector<std::vector<geometry::Point3D>> levels;

            /*
             * 第 0 层就是原始控制点：
             *
             *     P_i^(0) = P_i
             */
            levels.push_back(control_points_);

            /*
             * 每执行一层插值，点数量减少 1。
             *
             * 一直到最后只剩一个点。
             */
            while (levels.back().size() > 1)
            {
                const std::vector<geometry::Point3D> &previous =
                    levels.back();

                std::vector<geometry::Point3D> next;

                next.reserve(previous.size() - 1);

                /*
                 * 根据 De Casteljau 递推公式：
                 *
                 * P_i^(r)
                 *
                 *   = (1-t) P_i^(r-1)
                 *     + t P_(i+1)^(r-1)
                 *
                 * geometry::lerp(a, b, t)
                 *
                 * 即：
                 *
                 *     (1-t)a + tb
                 */
                for (std::size_t i = 0;
                     i + 1 < previous.size();
                     ++i)
                {
                    next.push_back(
                        geometry::lerp(
                            previous[i],
                            previous[i + 1],
                            t));
                }

                levels.push_back(next);
            }

            return levels;
        }

        /**
         * @brief 计算 Bézier 曲线任意阶导数
         *
         * 对 n 次 Bézier 曲线：
         *
         *           n
         * C(t) =    Σ P_i B_i^n(t)
         *          i=0
         *
         * 一阶导数仍然是一条 Bézier 曲线：
         *
         *                n-1
         * C'(t) = n      Σ (P_(i+1)-P_i) B_i^(n-1)(t)
         *                i=0
         *
         * 定义一阶导数控制点：
         *
         *     P_i^(1) = n(P_(i+1)-P_i)
         *
         * 继续求导可得到：
         *
         *     P_i^(k)
         *
         *       = (n-k+1)
         *         (P_(i+1)^(k-1) - P_i^(k-1))
         *
         * 因此 k 阶导数仍然可以使用 Bernstein 基函数求值。
         *
         * @param t     参数
         * @param order 导数阶数，必须 >= 1
         *
         * @return 曲线在 t 处的 order 阶导数向量
         *
         * @throws std::invalid_argument
         *         当 order == 0 时抛出异常
         */
        geometry::Vector3D
        BezierCurve::derivative(double t, std::size_t order) const
        {
            require_parameter(t);

            /*
             * derivative() API 专门用于求导，
             * 因此不允许 order == 0。
             *
             * 如果需要曲线本身，应调用 evaluate()。
             */
            if (order == 0)
                throw std::invalid_argument(
                    "BezierCurve::derivative order must be >= 1");

            /*
             * n 次多项式的 n+1 阶及更高阶导数恒为 0。
             */
            if (order > degree())
                return geometry::Vector3D();

            /*
             * 将控制点转换成向量形式，
             * 方便后续进行差分运算。
             *
             * 初始：
             *
             *     work[i] = P_i
             */
            std::vector<geometry::Vector3D> work;

            work.reserve(control_points_.size());

            for (std::size_t i = 0;
                 i < control_points_.size();
                 ++i)
            {
                work.push_back(
                    control_points_[i].as_vector());
            }

            /*
             * 当前 Bézier 曲线次数。
             *
             * 每求一次导，次数减少 1。
             */
            std::size_t current_degree = degree();

            /*
             * 重复执行控制点差分，
             * 得到 order 阶导数曲线的控制点。
             */
            for (std::size_t k = 0; k < order; ++k)
            {
                std::vector<geometry::Vector3D> next;

                next.reserve(current_degree);

                /*
                 * 如果当前曲线次数为 m，
                 * 那么它的一阶导数控制点为：
                 *
                 *     Q_i = m(P_(i+1)-P_i)
                 */
                for (std::size_t i = 0;
                     i < current_degree;
                     ++i)
                {
                    next.push_back(
                        (work[i + 1] - work[i]) *
                        static_cast<double>(current_degree));
                }

                /*
                 * 当前控制点替换为导数曲线控制点。
                 */
                work.swap(next);

                /*
                 * 每求一次导：
                 *
                 *     degree -> degree - 1
                 */
                --current_degree;
            }

            /*
             * 此时 work 已经保存 order 阶导数曲线控制点。
             *
             * 其次数为：
             *
             *     n - order
             *
             * 因此使用对应次数的 Bernstein 基函数进行求值。
             */
            const std::vector<double> basis =
                bernstein_basis(current_degree, t);

            geometry::Vector3D result;

            /*
             * 计算：
             *
             * C^(order)(t)
             *
             *     = Σ work[i] B_i^(current_degree)(t)
             */
            for (std::size_t i = 0; i < work.size(); ++i)
                result += work[i] * basis[i];

            return result;
        }

        /**
         * @brief 构造 Bézier 曲线的一阶导数曲线
         *
         * 原曲线：
         *
         *           n
         * C(t) =    Σ P_i B_i^n(t)
         *          i=0
         *
         * 导数曲线：
         *
         *                 n-1
         * C'(t) =         Σ Q_i B_i^(n-1)(t)
         *                 i=0
         *
         * 其中导数控制点：
         *
         *     Q_i = n(P_(i+1)-P_i)
         *
         * 因此：
         *
         *     n 次 Bézier 曲线
         *
         * 求导以后得到：
         *
         *     n-1 次 Bézier 曲线
         *
         * @return 表示 C'(t) 的 Bézier 曲线
         */
        BezierCurve BezierCurve::derivative_curve() const
        {
            /*
             * 对 0 次 Bézier 曲线：
             *
             *     C(t) = P_0
             *
             * 导数恒为：
             *
             *     C'(t) = 0
             *
             * 使用一个原点控制点表示零向量常值曲线。
             */
            if (degree() == 0)
            {
                return BezierCurve(
                    std::vector<geometry::Point3D>(
                        1,
                        geometry::Point3D()));
            }

            /*
             * n 次 Bézier 曲线的一阶导数有 n 个控制点：
             *
             *     Q_0, ..., Q_(n-1)
             */
            std::vector<geometry::Point3D> points;

            points.reserve(degree());

            const double n =
                static_cast<double>(degree());

            /*
             * 计算：
             *
             *     Q_i = n(P_(i+1)-P_i)
             */
            for (std::size_t i = 0;
                 i < degree();
                 ++i)
            {
                const geometry::Vector3D q =
                    (control_points_[i + 1] -
                     control_points_[i]) *
                    n;

                /*
                 * 当前 Polynomial/Bezier API 使用 Point3D
                 * 表示控制系数，因此将导数向量转换为 Point3D。
                 */
                points.push_back(
                    geometry::Point3D(
                        q.x(),
                        q.y(),
                        q.z()));
            }

            return BezierCurve(points);
        }

        /**
         * @brief 在参数 t 处将 Bézier 曲线细分成两条 Bézier 曲线
         *
         * 使用 De Casteljau 算法，可以将原曲线：
         *
         *     C(u), u ∈ [0,1]
         *
         * 在参数：
         *
         *     u = t
         *
         * 处分成：
         *
         *     左曲线  ：C_left
         *     右曲线  ：C_right
         *
         * 两条子曲线拼接后与原 Bézier 曲线完全相同。
         *
         * De Casteljau 三角形的左边界：
         *
         *     P_0^(0)
         *     P_0^(1)
         *     P_0^(2)
         *     ...
         *     P_0^(n)
         *
         * 构成左子曲线控制点。
         *
         * 右边界：
         *
         *     P_n^(0)
         *     P_(n-1)^(1)
         *     ...
         *     P_0^(n)
         *
         * 构成右子曲线控制点。
         *
         * 最后的公共点：
         *
         *     P_0^(n) = C(t)
         *
         * 同时是：
         *
         *     左曲线终点
         *     右曲线起点
         *
         * @param t 细分参数
         *
         * @return pair<左 Bézier 曲线, 右 Bézier 曲线>
         */
        std::pair<BezierCurve, BezierCurve>
        BezierCurve::subdivide(double t) const
        {
            require_parameter(t);

            const std::size_t n = degree();

            /*
             * work 用于原地执行 De Casteljau 插值。
             */
            std::vector<geometry::Point3D> work =
                control_points_;

            /*
             * 原曲线是 n 次，
             * 因此左右子曲线仍然各有 n+1 个控制点。
             */
            std::vector<geometry::Point3D> left(n + 1);
            std::vector<geometry::Point3D> right(n + 1);

            /*
             * 左子曲线第一个控制点：
             *
             *     L_0 = P_0
             */
            left[0] = work[0];

            /*
             * 右子曲线最后一个控制点：
             *
             *     R_n = P_n
             */
            right[n] = work[n];

            /*
             * 逐层执行 De Casteljau 算法。
             */
            for (std::size_t r = 1; r <= n; ++r)
            {
                /*
                 * 第 r 层：
                 *
                 * P_i^(r)
                 *
                 *   = (1-t)P_i^(r-1)
                 *     + tP_(i+1)^(r-1)
                 *
                 * 这里直接覆盖 work，
                 * 因为上一层中后续不再需要的元素可以复用。
                 */
                for (std::size_t i = 0;
                     i <= n - r;
                     ++i)
                {
                    work[i] =
                        geometry::lerp(
                            work[i],
                            work[i + 1],
                            t);
                }

                /*
                 * De Casteljau 三角形左边界：
                 *
                 *     P_0^(r)
                 *
                 * 构成左子曲线控制点。
                 */
                left[r] = work[0];

                /*
                 * De Casteljau 三角形右边界：
                 *
                 *     P_(n-r)^(r)
                 *
                 * 按反方向写入 right。
                 */
                right[n - r] = work[n - r];
            }

            /*
             * 返回：
             *
             *     first  -> 左子曲线
             *     second -> 右子曲线
             */
            return std::make_pair(
                BezierCurve(left),
                BezierCurve(right));
        }

    } // namespace curve
} // namespace opencagd
