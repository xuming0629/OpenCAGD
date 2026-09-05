#pragma once

#include <cstddef>      // std::size_t
#include <stdexcept>    // std::invalid_argument
#include <utility>      // std::move
#include <vector>       // std::vector

#include <opencagd/geometry/point.hpp>

namespace opencagd::curve
{

/**
 * @brief 任意维度的多项式参数曲线
 *
 * PolynomialCurve 表示一个向量值多项式：
 *
 *      C(t)
 *        = a_0
 *        + a_1 t
 *        + a_2 t^2
 *        + ...
 *        + a_n t^n
 *
 * 其中：
 *
 *      t   : 曲线参数
 *      a_i : 多项式系数
 *
 * 与普通标量多项式不同，
 * 这里每一个系数 a_i 都是一个 Dim 维向量：
 *
 *      a_i ∈ R^Dim
 *
 * 因此：
 *
 *      Dim = 2
 *
 * 时：
 *
 *      C(t) = (x(t), y(t))
 *
 *      Dim = 3
 *
 * 时：
 *
 *      C(t) = (x(t), y(t), z(t))
 *
 * 本类使用 Horner 方法进行多项式求值，
 * 避免显式计算：
 *
 *      t^2, t^3, ..., t^n
 *
 * @tparam Dim 曲线所在几何空间的维数
 */
template <std::size_t Dim>
class PolynomialCurve
{
public:
    /**
     * @brief 多项式系数 / 曲线点的数据类型
     *
     * 例如：
     *
     *      PolynomialCurve<2>
     *
     * 对应：
     *
     *      Point<double, 2>
     *
     * 而：
     *
     *      PolynomialCurve<3>
     *
     * 对应：
     *
     *      Point<double, 3>
     */
    using point_type = Point<double, Dim>;

    /**
     * @brief 使用多项式系数构造曲线
     *
     * coefficients 按幂从低到高排列：
     *
     *      coefficients[0] = a_0
     *      coefficients[1] = a_1
     *      ...
     *      coefficients[n] = a_n
     *
     * 因此曲线为：
     *
     *      C(t)
     *        = a_0
     *        + a_1 t
     *        + ...
     *        + a_n t^n
     *
     * 至少需要一个系数。
     *
     * 如果只有：
     *
     *      coefficients = {a_0}
     *
     * 则表示 0 次常值曲线：
     *
     *      C(t) = a_0
     *
     * @param coefficients 向量值多项式的系数
     */
    explicit PolynomialCurve(
        std::vector<point_type> coefficients)
        : coefficients_(std::move(coefficients))
    {
        // 一个多项式至少需要一个系数。
        if (coefficients_.empty())
        {
            throw std::invalid_argument(
                "PolynomialCurve requires at least one coefficient"
            );
        }
    }

    /**
     * @brief 返回多项式曲线次数
     *
     * 如果一共有：
     *
     *      n + 1
     *
     * 个系数：
     *
     *      a_0, ..., a_n
     *
     * 则多项式次数为：
     *
     *      n
     */
    [[nodiscard]]
    std::size_t degree() const noexcept
    {
        return coefficients_.size() - 1;
    }

    /**
     * @brief 返回多项式全部系数
     *
     * 系数顺序为：
     *
     *      [a_0, a_1, ..., a_n]
     *
     * 即按照参数 t 的幂从低到高排列。
     */
    [[nodiscard]]
    const std::vector<point_type>& coefficients() const noexcept
    {
        return coefficients_;
    }

    /**
     * @brief 在参数 t 处计算多项式曲线点 C(t)
     *
     * 原始多项式：
     *
     *      C(t)
     *        = a_0
     *        + a_1 t
     *        + a_2 t^2
     *        + ...
     *        + a_n t^n
     *
     * 如果直接计算，需要：
     *
     *      t^2
     *      t^3
     *      ...
     *      t^n
     *
     * 本实现采用 Horner 方法，将多项式改写为：
     *
     *      C(t)
     *        =
     *        a_0
     *        + t(
     *            a_1
     *            + t(
     *                a_2
     *                + ...
     *                + t a_n
     *              )
     *          )
     *
     * 例如三次多项式：
     *
     *      C(t)
     *        = a_0
     *        + a_1 t
     *        + a_2 t^2
     *        + a_3 t^3
     *
     * 可以改写成：
     *
     *      C(t)
     *        =
     *        ((a_3 t + a_2)t + a_1)t + a_0
     *
     * 这样每一层只需要：
     *
     *      一次乘法
     *      一次加法
     *
     * @param t 曲线参数
     *
     * @return 参数 t 对应的曲线点 C(t)
     */
    [[nodiscard]]
    point_type evaluate(double t) const noexcept
    {
        // -----------------------------------------------------
        // Horner 算法从最高次系数开始。
        //
        // 初始：
        //
        //      value = a_n
        //
        // 然后逐步执行：
        //
        //      value = value * t + a_{n-1}
        //
        //      value = value * t + a_{n-2}
        //
        //      ...
        //
        //      value = value * t + a_0
        //
        // 最终：
        //
        //      value = C(t)
        // -----------------------------------------------------
        point_type value =
            coefficients_.back();

        // -----------------------------------------------------
        // coefficients_.size() = n + 1
        //
        // 因此：
        //
        //      i = n
        //
        // 开始向：
        //
        //      i = 1
        //
        // 递减。
        //
        // 每一步使用：
        //
        //      coefficients_[i - 1]
        //
        // -----------------------------------------------------
        for (std::size_t i =
                 coefficients_.size() - 1;
             i > 0;
             --i)
        {
            // Horner 递推：
            //
            //      value
            //          = value * t
            //          + a_{i-1}
            value =
                value * t
                +
                coefficients_[i - 1];
        }

        return value;
    }

private:
    /**
     * @brief 多项式系数
     *
     * 按照幂从低到高存储：
     *
     *      coefficients_[0] -> a_0
     *      coefficients_[1] -> a_1
     *      ...
     *      coefficients_[n] -> a_n
     */
    std::vector<point_type> coefficients_;
};


/**
 * @brief 二维多项式曲线
 *
 *      C(t) = (x(t), y(t))
 */
using PolynomialCurve2d =
    PolynomialCurve<2>;


/**
 * @brief 三维多项式曲线
 *
 *      C(t) = (x(t), y(t), z(t))
 */
using PolynomialCurve3d =
    PolynomialCurve<3>;

} // namespace opencagd::curve

