#include <opencagd/curve/sampling.h>

#include <stdexcept>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief 在参数曲线上进行均匀参数采样
         *
         * 对参数曲线：
         *
         *     C(u)
         *
         * 假设其有效参数域为：
         *
         *     u ∈ [u_min, u_max]
         *
         * 本函数在参数区间内均匀选取 count 个参数：
         *
         *     u_0, u_1, ..., u_(count-1)
         *
         * 并计算对应曲线点：
         *
         *     C(u_0), C(u_1), ..., C(u_(count-1))
         *
         * ------------------------------------------------------------
         *
         * 参数采样公式：
         *
         * 首先定义归一化参数：
         *
         *                i
         *     α_i = -------------
         *            count - 1
         *
         * 其中：
         *
         *     i = 0, 1, ..., count-1
         *
         * 因此：
         *
         *     α_i ∈ [0,1]
         *
         * 然后将 [0,1] 线性映射到曲线实际参数域：
         *
         *     u_i
         *
         *       = u_min
         *         + (u_max - u_min) α_i
         *
         * ------------------------------------------------------------
         *
         * 当 i = 0：
         *
         *     α = 0
         *
         * 因此：
         *
         *     u = u_min
         *
         * 当 i = count-1：
         *
         *     α = 1
         *
         * 因此：
         *
         *     u = u_max
         *
         * 所以本函数会包含曲线参数域的两个端点。
         *
         * ------------------------------------------------------------
         *
         * 注意：
         *
         * 本函数进行的是“均匀参数采样”：
         *
         *     Δu = constant
         *
         * 并不意味着生成的相邻曲线点之间具有相同几何距离：
         *
         *     ||C(u_{i+1}) - C(u_i)||
         *
         * 一般并不相等。
         *
         * 如果希望曲线点在几何空间中近似等距离分布，
         * 需要实现基于弧长的采样：
         *
         *     equal_arc_length_sampling(...)
         *
         * @param curve 待采样的参数曲线
         * @param count 采样点数量，必须至少为 2
         *
         * @return 曲线采样点集合
         *
         * @throws std::invalid_argument
         *         当 count < 2 时抛出异常
         */
        std::vector<geometry::Point3D>
        sample_curve(
            const ParametricCurve &curve,
            std::size_t count)
        {
            /*
             * 至少需要两个采样点，
             * 才能同时表示参数域的起点和终点。
             *
             * 同时后面的公式包含：
             *
             *     count - 1
             *
             * 因此也需要避免分母为 0。
             */
            if (count < 2)
            {
                throw std::invalid_argument(
                    "Curve sampling requires at least two samples");
            }

            /*
             * 获取曲线有效参数域：
             *
             *     d = [u_min, u_max]
             *
             * 不同曲线类型的参数域可能不同。
             *
             * 例如：
             *
             * Bézier:
             *
             *     [0,1]
             *
             * B-Spline / NURBS:
             *
             *     由 KnotVector 决定
             *
             * PolynomialCurve:
             *
             *     由构造函数显式指定
             */
            const geometry::Interval d =
                curve.domain();

            /*
             * 保存最终采样得到的曲线点。
             */
            std::vector<geometry::Point3D> points;

            /*
             * 提前申请 count 个元素的容量，
             * 避免 push_back() 过程中频繁重新分配内存。
             */
            points.reserve(count);

            /*
             * 生成：
             *
             *     i = 0, 1, ..., count-1
             *
             * 共 count 个采样点。
             */
            for (std::size_t i = 0;
                 i < count;
                 ++i)
            {
                /*
                 * 将离散索引 i 映射到归一化区间 [0,1]：
                 *
                 *                i
                 *     a = ----------------
                 *            count - 1
                 *
                 * 因此：
                 *
                 *     i = 0
                 *         -> a = 0
                 *
                 *     i = count-1
                 *         -> a = 1
                 */
                const double a =
                    static_cast<double>(i) / static_cast<double>(count - 1);

                /*
                 * 将归一化参数：
                 *
                 *     a ∈ [0,1]
                 *
                 * 线性映射到曲线实际参数域：
                 *
                 *     [d.min, d.max]
                 *
                 * 使用公式：
                 *
                 *     u
                 *
                 *       = d.min
                 *         + (d.max - d.min) * a
                 */
                const double u =
                    d.min + (d.max - d.min) * a;

                /*
                 * 通过 ParametricCurve 统一接口：
                 *
                 *     C(u)
                 *
                 * 计算实际曲线点。
                 *
                 * 由于使用基类引用，
                 * 这里可以统一采样：
                 *
                 *     BezierCurve
                 *     PolynomialCurve
                 *     BSplineCurve
                 *     NURBSCurve
                 *     ...
                 *
                 * evaluate() 会通过虚函数调用到
                 * 各具体曲线类型自己的实现。
                 */
                points.push_back(
                    curve.evaluate(u));
            }

            return points;
        }

    } // namespace curve
} // namespace opencagd