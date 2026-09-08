#include <opencagd/curve/bspline_curve.h>
#include <opencagd/curve/bspline_basis.h>

#include <stdexcept>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief 构造 B-Spline 曲线
         *
         * 一条 p 次 B-Spline 曲线定义为：
         *
         *            n
         * C(u) =     Σ N_{i,p}(u) P_i
         *           i=0
         *
         * 其中：
         *
         *     p            为曲线次数 degree
         *     P_i          为控制点
         *     N_{i,p}(u)   为 p 次 B-Spline 基函数
         *     U            为节点向量
         *
         * ------------------------------------------------------------
         *
         * 如果节点向量写成：
         *
         *     U = {U_0, U_1, ..., U_m}
         *
         * 则节点数量为：
         *
         *     m + 1
         *
         * 控制点数量为：
         *
         *     n + 1
         *
         * B-Spline 中必须满足：
         *
         *     m = n + p + 1
         *
         * 因此：
         *
         *     n + 1 = (m + 1) - p - 1
         *
         * 即：
         *
         *     control_point_count
         *         = knot_count - degree - 1
         *
         * ------------------------------------------------------------
         *
         * 例如：
         *
         *     degree = 3
         *     knot_count = 10
         *
         * 则控制点数量必须为：
         *
         *     10 - 3 - 1 = 6
         *
         * @param degree         B-Spline 曲线次数 p
         * @param knots          节点向量
         * @param control_points 控制点集合
         *
         * @throws std::invalid_argument
         *         当控制点数量与节点向量不匹配时抛出异常
         */
        BSplineCurve::BSplineCurve(
            std::size_t degree,
            const std::vector<double> &knots,
            const std::vector<geometry::Point3D> &control_points)
            : knot_vector_(degree, knots),
              control_points_(control_points)
        {
            /*
             * KnotVector::num_basis_functions()
             *
             * 返回该节点向量能够定义的 B-Spline 基函数数量。
             *
             * 每一个基函数对应一个控制点，因此必须满足：
             *
             *     控制点数量 == 基函数数量
             */
            if (control_points_.size() !=
                knot_vector_.num_basis_functions())
            {
                throw std::invalid_argument(
                    "B-spline control-point count must equal "
                    "knot_count - degree - 1");
            }
        }

        /**
         * @brief 返回 B-Spline 曲线次数
         *
         * 曲线次数直接由节点向量对象维护。
         *
         * @return 曲线次数 p
         */
        std::size_t BSplineCurve::degree() const
        {
            return knot_vector_.degree();
        }

        /**
         * @brief 返回 B-Spline 曲线节点向量
         *
         * 返回 const 引用，避免节点向量复制，
         * 同时防止外部直接修改内部数据。
         */
        const KnotVector &BSplineCurve::knot_vector() const
        {
            return knot_vector_;
        }

        /**
         * @brief 返回 B-Spline 曲线控制点
         *
         * 对于：
         *
         *     P_0, P_1, ..., P_n
         *
         * 返回完整控制点集合。
         */
        const std::vector<geometry::Point3D> &
        BSplineCurve::control_points() const
        {
            return control_points_;
        }

        /**
         * @brief 返回 B-Spline 曲线有效参数域
         *
         * B-Spline 的有效参数区间并不一定是 [0,1]，
         * 而是由节点向量决定。
         *
         * 对 p 次 B-Spline，通常有效参数域为：
         *
         *     [U_p, U_{m-p}]
         *
         * 或根据 KnotVector 内部的索引定义，
         * 等价地取得曲线能够正常求值的有效节点区间。
         *
         * @return 曲线有效参数区间
         */
        geometry::Interval BSplineCurve::domain() const
        {
            return knot_vector_.domain();
        }

        /**
         * @brief 计算 B-Spline 曲线在参数 u 处的点
         *
         * B-Spline 曲线定义为：
         *
         *            n
         * C(u) =     Σ N_{i,p}(u) P_i
         *           i=0
         *
         * 但由于 B-Spline 基函数具有局部支撑性质，
         * 在任意一个参数 u 处，最多只有 p+1 个基函数非零。
         *
         * 如果：
         *
         *     U_span <= u < U_{span+1}
         *
         * 那么只有：
         *
         *     N_{span-p,p}(u)
         *     ...
         *     N_{span,p}(u)
         *
         * 可能非零。
         *
         * 因此实际上只需要计算：
         *
         *               p
         * C(u) =        Σ
         *              j=0
         *
         *     N_{span-p+j,p}(u)
         *     P_{span-p+j}
         *
         * 这正是 B-Spline 高效求值的关键。
         *
         * @param u 曲线参数
         *
         * @return 曲线点 C(u)
         */
        geometry::Point3D
        BSplineCurve::evaluate(double u) const
        {
            /*
             * 检查参数 u 是否位于当前曲线有效定义域。
             */
            require_parameter(u);

            /*
             * 找到 u 所在节点区间：
             *
             *     U_span <= u < U_{span+1}
             *
             * span 决定当前参数处哪些基函数非零。
             */
            const std::size_t span =
                knot_vector_.find_span(u);

            /*
             * 计算当前 span 中 p+1 个非零基函数：
             *
             *     N_{span-p,p}(u)
             *     ...
             *     N_{span,p}(u)
             *
             * 返回数组：
             *
             *     basis[0], ..., basis[p]
             */
            const std::vector<double> basis =
                basis_functions(
                    knot_vector_,
                    span,
                    u);

            /*
             * 当前第一个非零基函数对应的控制点索引：
             *
             *     first = span - p
             */
            const std::size_t first =
                span - degree();

            /*
             * 使用向量形式累加曲线点。
             *
             * value 初始为：
             *
             *     (0, 0, 0)
             */
            geometry::Vector3D value;

            /*
             * 根据局部支撑性，只需要 p+1 个控制点：
             *
             *     P_first
             *     ...
             *     P_{first+p}
             *
             * 计算：
             *
             *             p
             * C(u) =      Σ P_{first+j} basis[j]
             *            j=0
             */
            for (std::size_t j = 0;
                 j <= degree();
                 ++j)
            {
                value +=
                    control_points_[first + j].as_vector() * basis[j];
            }

            /*
             * 将向量累加结果转换成几何点。
             */
            return geometry::Point3D(
                value.x(),
                value.y(),
                value.z());
        }

        /**
         * @brief 计算 B-Spline 曲线在参数 u 处的指定阶导数
         *
         * 原曲线：
         *
         *            n
         * C(u) =     Σ N_{i,p}(u) P_i
         *           i=0
         *
         * 因为控制点 P_i 与参数 u 无关，
         * 所以可以直接对基函数求导：
         *
         *                   n
         * C^(k)(u) =        Σ N_{i,p}^(k)(u) P_i
         *                  i=0
         *
         * 同样由于局部支撑性，在参数 u 处只需要考虑 p+1 个基函数：
         *
         *                    p
         * C^(k)(u) =         Σ
         *                   j=0
         *
         *     N_{span-p+j,p}^(k)(u)
         *     P_{span-p+j}
         *
         * ------------------------------------------------------------
         *
         * 对 p 次 B-Spline：
         *
         *     k > p
         *
         * 时：
         *
         *     C^(k)(u) = 0
         *
         * 因为每一段本质上都是 p 次多项式。
         *
         * @param u     曲线参数
         * @param order 导数阶数，要求 order >= 1
         *
         * @return 曲线的 order 阶导数向量
         *
         * @throws std::invalid_argument
         *         当 order == 0 时抛出异常
         */
        geometry::Vector3D
        BSplineCurve::derivative(
            double u,
            std::size_t order) const
        {
            /*
             * 首先检查参数有效性。
             */
            require_parameter(u);

            /*
             * derivative() 只用于真正的求导。
             *
             * order == 0 对应曲线本身，
             * 应使用 evaluate()。
             */
            if (order == 0)
            {
                throw std::invalid_argument(
                    "BSplineCurve::derivative order must be >= 1");
            }

            /*
             * p 次分段多项式的高于 p 阶导数恒为 0。
             */
            if (order > degree())
                return geometry::Vector3D();

            /*
             * 找到当前参数所在节点区间。
             */
            const std::size_t span =
                knot_vector_.find_span(u);

            /*
             * 计算当前 span 中 p+1 个非零基函数
             * 从 0 阶到 order 阶的全部导数。
             *
             * ders[k][j]
             *
             * 表示：
             *
             *     当前第 j 个局部基函数
             *     的第 k 阶导数
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

            geometry::Vector3D value;

            /*
             * 根据：
             *
             *                  p
             * C^(order)(u) =   Σ
             *                 j=0
             *
             *     P_{first+j}
             *     N_{first+j,p}^(order)(u)
             *
             * 进行局部加权求和。
             */
            for (std::size_t j = 0;
                 j <= degree();
                 ++j)
            {
                value +=
                    control_points_[first + j].as_vector() * ders[order][j];
            }

            return value;
        }

    } // namespace curve
} // namespace opencagd
