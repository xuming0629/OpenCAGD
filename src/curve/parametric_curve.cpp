#include <opencagd/curve/parametric_curve.h>
#include <opencagd/math/numeric.h>

#include <cmath>
#include <stdexcept>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief ParametricCurve 虚析构函数
         *
         * ParametricCurve 是参数曲线的抽象基类，通常会通过基类指针
         * 指向具体曲线类型，例如：
         *
         *   - PolynomialCurve
         *   - BezierCurve
         *   - BSplineCurve
         *   - NURBSCurve
         *
         * 因此需要提供虚析构函数，保证通过 ParametricCurve*
         * 删除派生类对象时能够正确调用派生类析构函数。
         */
        ParametricCurve::~ParametricCurve() {}

        /**
         * @brief 检查参数 u 是否位于曲线定义域中
         *
         * 参数曲线通常定义在某个区间：
         *
         *     u ∈ [u_min, u_max]
         *
         * 例如 Bézier 曲线通常使用：
         *
         *     u ∈ [0, 1]
         *
         * B 样条 / NURBS 曲线的有效参数区间则由节点向量决定。
         *
         * 由于浮点运算存在舍入误差，这里不是直接进行严格比较，
         * 而是使用 math::tolerance() 作为数值容差。
         *
         * @param u 待检查的曲线参数
         *
         * @throws std::out_of_range
         *         当参数 u 超出曲线有效定义域时抛出异常
         */
        void ParametricCurve::require_parameter(double u) const
        {
            /*
             * domain() 返回当前曲线的参数定义域。
             *
             * contains(u, tolerance) 判断：
             *
             *     u_min - eps <= u <= u_max + eps
             *
             * 其中 eps 为数值容差。
             */
            if (!domain().contains(u, math::tolerance()))
                throw std::out_of_range(
                    "Curve parameter lies outside the valid domain");
        }

        /**
         * @brief 计算参数曲线在 u 处的参数速度
         *
         * 对参数曲线：
         *
         *     C(u)
         *
         * 一阶导数为：
         *
         *     C'(u) = dC/du
         *
         * 曲线的参数速度定义为一阶导数的模：
         *
         *     v(u) = ||C'(u)||
         *
         * 注意：
         *
         * 这里的 speed 并不一定表示物理意义上的运动速度，
         * 而是表示参数 u 改变时，曲线上位置变化的快慢。
         *
         * 如果曲线使用弧长 s 作为参数，则：
         *
         *     ||dC/ds|| = 1
         *
         * 即单位弧长参数化曲线的 speed 恒等于 1。
         *
         * @param u 曲线参数
         *
         * @return 参数速度 ||C'(u)||
         */
        double ParametricCurve::speed(double u) const
        {
            // 首先检查参数是否合法。
            require_parameter(u);

            /*
             * derivative(u, 1)
             *
             * 计算曲线在 u 处的一阶导数：
             *
             *     C'(u)
             *
             * norm() 计算向量长度：
             *
             *     ||C'(u)||
             */
            return derivative(u, 1).norm();
        }

        /**
         * @brief 计算参数曲线在 u 处的单位切向量
         *
         * 曲线的一阶导数：
         *
         *     C'(u)
         *
         * 指向曲线参数增加的方向。
         *
         * 对其进行单位化可以得到单位切向量：
         *
         *                C'(u)
         *     T(u) = --------------
         *             ||C'(u)||
         *
         * 单位切向量描述曲线在该点的局部前进方向。
         *
         * @param u 曲线参数
         *
         * @return 曲线在 u 处的单位切向量 T(u)
         *
         * @note
         * 如果 C'(u) = 0，则该点为奇异参数点，
         * 单位切向量在数学上没有定义。
         * normalized() 的具体处理方式取决于 Vector3D 的实现。
         */
        geometry::Vector3D ParametricCurve::tangent(double u) const
        {
            // 检查参数是否位于有效定义域中。
            require_parameter(u);

            /*
             * 一阶导数：
             *
             *     C'(u)
             *
             * normalized()：
             *
             *             C'(u)
             *     ---------------------
             *          ||C'(u)||
             */
            return derivative(u, 1).normalized();
        }

        /**
         * @brief 计算参数曲线在 u 处的曲率
         *
         * 对三维参数曲线：
         *
         *     C(u)
         *
         * 一阶导数：
         *
         *     C'(u)
         *
         * 二阶导数：
         *
         *     C''(u)
         *
         * 曲率定义为：
         *
         *                ||C'(u) × C''(u)||
         *     κ(u) = ---------------------------
         *                     ||C'(u)||^3
         *
         * 曲率 κ 用于描述曲线弯曲程度：
         *
         *     κ 越大 -> 曲线弯曲越剧烈
         *     κ 越小 -> 曲线越平缓
         *     κ = 0  -> 局部为直线
         *
         * 对半径为 R 的圆：
         *
         *     κ = 1 / R
         *
         * 因此半径越小，圆弯曲得越厉害。
         *
         * @param u 曲线参数
         *
         * @return 曲率 κ(u)
         *
         * @throws std::runtime_error
         *         当一阶导数接近零时，曲率无定义
         */
        double ParametricCurve::curvature(double u) const
        {
            // 检查参数是否处于曲线有效定义域。
            require_parameter(u);

            /*
             * C'(u)
             *
             * 曲线的一阶导数，也称速度向量。
             */
            const geometry::Vector3D d1 = derivative(u, 1);

            /*
             * C''(u)
             *
             * 曲线的二阶导数。
             *
             * 它描述一阶导数随参数变化的变化率，
             * 与曲线弯曲程度密切相关。
             */
            const geometry::Vector3D d2 = derivative(u, 2);

            /*
             * ||C'(u)||
             *
             * 即参数曲线在 u 处的 speed。
             */
            const double speed_value = d1.norm();

            /*
             * 曲率公式分母包含：
             *
             *     ||C'(u)||^3
             *
             * 因此当：
             *
             *     C'(u) = 0
             *
             * 时曲率公式失效。
             *
             * 实际数值计算中不能直接判断 speed_value == 0，
             * 因为浮点数存在误差，所以使用统一数值容差。
             */
            if (speed_value <= math::tolerance())
                throw std::runtime_error(
                    "Curve curvature is undefined because the first derivative is zero");

            /*
             * 三维曲线曲率公式：
             *
             *              ||C'(u) × C''(u)||
             *     κ(u) = -----------------------
             *                   ||C'(u)||³
             *
             * cross(d1, d2)
             *
             * 对应：
             *
             *     C'(u) × C''(u)
             *
             * cross(...).norm()
             *
             * 对应：
             *
             *     ||C'(u) × C''(u)||
             */
            return geometry::cross(d1, d2).norm() /
                   (speed_value * speed_value * speed_value);
        }

    } // namespace curve
} // namespace opencagd