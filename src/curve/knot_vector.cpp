#include <opencagd/curve/knot_vector.h>
#include <opencagd/math/numeric.h>

#include <algorithm>
#include <stdexcept>

namespace opencagd
{
    namespace curve
    {

        /**
         * @brief 构造一个默认 KnotVector
         *
         * 默认构造：
         *
         *     degree = 0
         *
         * 节点向量：
         *
         *     U = {0, 1}
         *
         * 因此：
         *
         *     domain = [0, 1]
         *
         * 这是最简单的 0 次 B-Spline 节点向量。
         */
        KnotVector::KnotVector()
            : degree_(0),
              knots_(2, 0.0)
        {
            /*
             * knots_ 初始化后为：
             *
             *     {0, 0}
             *
             * 将第二个节点修改为 1：
             *
             *     {0, 1}
             */
            knots_[1] = 1.0;
        }

        /**
         * @brief 根据次数和节点序列构造 KnotVector
         *
         * B-Spline 节点向量通常表示为：
         *
         *     U = {U_0, U_1, ..., U_m}
         *
         * 其中要求：
         *
         *     U_0 <= U_1 <= ... <= U_m
         *
         * 即节点向量必须是非递减序列。
         *
         * 注意节点允许重复，因此不是严格递增。
         *
         * 对 p 次 B-Spline：
         *
         *     p = degree
         *
         * 如果控制点数量为：
         *
         *     n + 1
         *
         * 则节点数量满足：
         *
         *     m + 1 = n + p + 2
         *
         * 等价地：
         *
         *     控制点数
         *       = 节点数 - degree - 1
         *
         * @param degree B-Spline 次数 p
         * @param knots  节点向量
         *
         * @throws std::invalid_argument
         *         当节点向量不满足基本合法性条件时抛出异常
         */
        KnotVector::KnotVector(
            std::size_t degree,
            const std::vector<double> &knots)
            : degree_(degree),
              knots_(knots)
        {
            // 对输入节点向量进行完整合法性检查。
            validate();
        }

        /**
         * @brief 验证节点向量是否合法
         *
         * 当前检查三个基本条件：
         *
         * 1. 节点数量足够；
         * 2. 节点值非递减；
         * 3. 有效参数域合法。
         *
         * 对 p 次 B-Spline，
         * 一个最基本的节点向量至少需要：
         *
         *     2p + 2
         *
         * 个节点。
         *
         * 这对应最小情况下：
         *
         *     p + 1
         *
         * 个控制点 / 基函数。
         */
        void KnotVector::validate() const
        {
            /*
             * --------------------------------------------------------
             * 1. 检查节点数量
             * --------------------------------------------------------
             *
             * B-Spline 中：
             *
             *     num_basis_functions
             *       = knot_count - degree - 1
             *
             * 为了至少存在 p+1 个基函数，
             * 节点数量至少应为：
             *
             *     (p+1) + p + 1
             *
             *   = 2p + 2
             */
            if (knots_.size() < 2 * degree_ + 2)
            {
                throw std::invalid_argument(
                    "Knot vector is too short for the requested degree");
            }

            /*
             * --------------------------------------------------------
             * 2. 节点必须非递减
             * --------------------------------------------------------
             *
             * 节点向量必须满足：
             *
             *     U_0 <= U_1 <= ... <= U_m
             *
             * 注意：
             *
             *     相邻节点允许相等。
             *
             * 节点重复在 B-Spline / NURBS 中非常重要，
             * 因为节点重数会影响曲线连续性。
             */
            if (!std::is_sorted(
                    knots_.begin(),
                    knots_.end()))
            {
                throw std::invalid_argument(
                    "Knot vector must be nondecreasing");
            }

            /*
             * --------------------------------------------------------
             * 3. 检查有效参数域
             * --------------------------------------------------------
             *
             * 对 p 次 B-Spline，
             * 本实现采用有效参数区间：
             *
             *     [U_p, U_{m-p}]
             *
             * 因此必须满足：
             *
             *     domain_min <= domain_max
             */
            if (domain_max() < domain_min())
            {
                throw std::invalid_argument(
                    "Invalid knot-vector domain");
            }
        }

        /**
         * @brief 返回 B-Spline 次数
         *
         * @return degree = p
         */
        std::size_t KnotVector::degree() const
        {
            return degree_;
        }

        /**
         * @brief 返回节点数量
         *
         * 如果节点向量为：
         *
         *     U_0, ..., U_m
         *
         * 则：
         *
         *     size() = m + 1
         */
        std::size_t KnotVector::size() const
        {
            return knots_.size();
        }

        /**
         * @brief 返回该节点向量对应的 B-Spline 基函数数量
         *
         * B-Spline 基本关系：
         *
         *     m = n + p + 1
         *
         * 其中：
         *
         *     m + 1 = 节点数量
         *     n + 1 = 基函数数量 / 控制点数量
         *     p     = degree
         *
         * 因此：
         *
         *     n + 1
         *       = (m + 1) - p - 1
         *
         * 即：
         *
         *     num_basis_functions
         *       = knot_count - degree - 1
         *
         * @return 基函数数量
         */
        std::size_t KnotVector::num_basis_functions() const
        {
            return knots_.size() - degree_ - 1;
        }

        /**
         * @brief 返回完整节点数组
         *
         * 返回 const 引用以避免复制，
         * 同时不允许调用者直接修改内部节点向量。
         */
        const std::vector<double> &KnotVector::values() const
        {
            return knots_;
        }

        /**
         * @brief 访问指定索引处的节点值
         *
         * 对节点向量：
         *
         *     U = {U_0, ..., U_m}
         *
         * operator[](i) 返回：
         *
         *     U_i
         *
         * 与 std::vector::operator[] 不同，
         * 本实现显式进行越界检查。
         *
         * @param index 节点索引
         *
         * @return U_index
         *
         * @throws std::out_of_range
         *         当 index 超出节点范围时抛出异常
         */
        double KnotVector::operator[](std::size_t index) const
        {
            if (index >= knots_.size())
            {
                throw std::out_of_range(
                    "KnotVector index out of range");
            }

            return knots_[index];
        }

        /**
         * @brief 返回 B-Spline 的有效参数区间
         *
         * 本实现定义：
         *
         *     domain = [U_p, U_{m-p}]
         *
         * 其中：
         *
         *     p = degree
         *     m = knot_count - 1
         *
         * @return 有效参数区间
         */
        geometry::Interval KnotVector::domain() const
        {
            return geometry::Interval(
                domain_min(),
                domain_max());
        }

        /**
         * @brief 返回有效参数域左端点
         *
         * 对 p 次 B-Spline：
         *
         *     u_min = U_p
         *
         * @return 参数域最小值
         */
        double KnotVector::domain_min() const
        {
            return knots_[degree_];
        }

        /**
         * @brief 返回有效参数域右端点
         *
         * 节点数量：
         *
         *     knot_count = m + 1
         *
         * 因此索引：
         *
         *     knot_count - degree - 1
         *
         * 等价于：
         *
         *     m - p
         *
         * 所以：
         *
         *     u_max = U_{m-p}
         *
         * @return 参数域最大值
         */
        double KnotVector::domain_max() const
        {
            return knots_[knots_.size() - degree_ - 1];
        }

        /**
         * @brief 查找参数 u 所在的节点区间 span
         *
         * 对 B-Spline 求值，需要寻找索引 span，
         * 使得：
         *
         *     U_span <= u < U_{span+1}
         *
         * 这个 span 决定了参数 u 处哪些基函数非零。
         *
         * 对 p 次 B-Spline，
         * 当 u 位于 span 中时，最多只有：
         *
         *     N_{span-p,p}(u),
         *     ...
         *     N_{span,p}(u)
         *
         * 这 p+1 个基函数非零。
         *
         * ------------------------------------------------------------
         *
         * 对参数域右端点采用特殊约定：
         *
         *     u == u_max
         *
         * 返回：
         *
         *     span = n
         *
         * 其中 n 是最后一个基函数 / 控制点索引。
         *
         * 这是因为普通 span 定义：
         *
         *     U_i <= u < U_{i+1}
         *
         * 使用右开区间，
         * 而曲线右端点又必须能够正常求值，
         * 因此需要单独处理。
         *
         * ------------------------------------------------------------
         *
         * 本函数使用二分查找，
         * 时间复杂度约为：
         *
         *     O(log n)
         *
         * @param u 参数值
         *
         * @return u 所在节点区间的 span 索引
         *
         * @throws std::out_of_range
         *         当 u 超出有效 B-Spline 参数域时抛出异常
         */
        std::size_t KnotVector::find_span(double u) const
        {
            /*
             * num_basis_functions() = n + 1
             *
             * 因此最后一个基函数 / 控制点索引：
             *
             *     n = num_basis_functions() - 1
             */
            const std::size_t n =
                num_basis_functions() - 1;

            /*
             * B-Spline 有效参数域：
             *
             *     [umin, umax]
             */
            const double umin =
                domain_min();

            const double umax =
                domain_max();

            /*
             * 统一数值容差。
             *
             * 用于处理类似：
             *
             *     u = umax + 1e-15
             *
             * 这样的浮点计算误差。
             */
            const double eps =
                math::tolerance();

            /*
             * --------------------------------------------------------
             * 参数范围检查
             * --------------------------------------------------------
             *
             * 只有真正超过容差范围才认为非法。
             */
            if (u < umin - eps ||
                u > umax + eps)
            {
                throw std::out_of_range(
                    "Parameter lies outside the B-spline domain");
            }

            /*
             * --------------------------------------------------------
             * 右端点特殊处理
             * --------------------------------------------------------
             *
             * 正常 span 定义为：
             *
             *     U_span <= u < U_{span+1}
             *
             * 但是 u = umax 时，
             * 右侧严格小于关系不成立。
             *
             * 标准 B-Spline 算法通常规定：
             *
             *     span(umax) = n
             *
             * 从而保证曲线右端点能够正常求值。
             */
            if (u >= umax - eps)
                return n;

            /*
             * --------------------------------------------------------
             * 左端点特殊处理
             * --------------------------------------------------------
             *
             * 有效参数域左端点为：
             *
             *     U_p
             *
             * 因此：
             *
             *     span(umin) = p
             */
            if (u <= umin + eps)
                return degree_;

            /*
             * --------------------------------------------------------
             * 二分查找初始化
             * --------------------------------------------------------
             *
             * 有效 span 范围：
             *
             *     p <= span <= n
             *
             * 由于 high 在二分搜索中作为右侧界限，
             * 这里初始化为：
             *
             *     n + 1
             */
            std::size_t low =
                degree_;

            std::size_t high =
                n + 1;

            std::size_t mid =
                (low + high) / 2;

            /*
             * --------------------------------------------------------
             * 二分查找
             * --------------------------------------------------------
             *
             * 目标是找到：
             *
             *     U_mid <= u < U_{mid+1}
             */
            while (
                u < knots_[mid] ||
                u >= knots_[mid + 1])
            {
                /*
                 * 如果：
                 *
                 *     u < U_mid
                 *
                 * 说明目标 span 位于左侧。
                 */
                if (u < knots_[mid])
                {
                    high = mid;
                }
                else
                {
                    /*
                     * 否则：
                     *
                     *     u >= U_{mid+1}
                     *
                     * 说明目标 span 位于右侧。
                     */
                    low = mid;
                }

                /*
                 * 更新中间位置。
                 */
                mid = (low + high) / 2;
            }

            /*
             * 此时满足：
             *
             *     U_mid <= u < U_{mid+1}
             */
            return mid;
        }

    } // namespace curve
} // namespace opencagd
