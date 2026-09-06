#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>

namespace opencagd
{

template <typename T, std::size_t Dim>
class Point
{
public:
    using value_type = T;
    static constexpr std::size_t dimension = Dim;

    constexpr Point() = default;

    constexpr explicit Point(const std::array<T, Dim>& values)
        : data_(values)
    {
    }

    constexpr Point(std::initializer_list<T> values)
    {
        if (values.size() != Dim)
        {
            throw std::invalid_argument("Point initializer size must match dimension");
        }

        std::size_t i = 0;
        for (const auto& value : values)
        {
            data_[i++] = value;
        }
    }

    [[nodiscard]] constexpr T& operator[](std::size_t i) noexcept { return data_[i]; }
    [[nodiscard]] constexpr const T& operator[](std::size_t i) const noexcept { return data_[i]; }
    [[nodiscard]] constexpr const std::array<T, Dim>& data() const noexcept { return data_; }

    constexpr Point& operator+=(const Point& rhs) noexcept
    {
        for (std::size_t i = 0; i < Dim; ++i) data_[i] += rhs[i];
        return *this;
    }

    constexpr Point& operator-=(const Point& rhs) noexcept
    {
        for (std::size_t i = 0; i < Dim; ++i) data_[i] -= rhs[i];
        return *this;
    }

    constexpr Point& operator*=(T scalar) noexcept
    {
        for (auto& value : data_) value *= scalar;
        return *this;
    }

    constexpr Point& operator/=(T scalar)
    {
        if (scalar == T{}) throw std::invalid_argument("Point division by zero");
        for (auto& value : data_) value /= scalar;
        return *this;
    }

    [[nodiscard]] constexpr T squared_norm() const noexcept
    {
        T result{};
        for (const auto& value : data_) result += value * value;
        return result;
    }

    [[nodiscard]] double norm() const noexcept
    {
        return std::sqrt(static_cast<double>(squared_norm()));
    }

private:
    std::array<T, Dim> data_{};
};

template <typename T, std::size_t Dim>
[[nodiscard]] constexpr Point<T, Dim> operator+(Point<T, Dim> lhs, const Point<T, Dim>& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

template <typename T, std::size_t Dim>
[[nodiscard]] constexpr Point<T, Dim> operator-(Point<T, Dim> lhs, const Point<T, Dim>& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

template <typename T, std::size_t Dim>
[[nodiscard]] constexpr Point<T, Dim> operator*(Point<T, Dim> point, T scalar) noexcept
{
    point *= scalar;
    return point;
}

template <typename T, std::size_t Dim>
[[nodiscard]] constexpr Point<T, Dim> operator*(T scalar, Point<T, Dim> point) noexcept
{
    point *= scalar;
    return point;
}

template <typename T, std::size_t Dim>
[[nodiscard]] constexpr Point<T, Dim> operator/(Point<T, Dim> point, T scalar)
{
    point /= scalar;
    return point;
}

template <typename T, std::size_t Dim>
[[nodiscard]] constexpr T dot(const Point<T, Dim>& a, const Point<T, Dim>& b) noexcept
{
    T result{};
    for (std::size_t i = 0; i < Dim; ++i) result += a[i] * b[i];
    return result;
}

/**
 * @brief 三维向量叉积。
 *
 * Point 在 OpenCAGD 当前学习阶段同时承担点和自由向量的线性代数表示，
 * 因而叉积返回 Point<T,3>。
 */
template <typename T>
[[nodiscard]] constexpr Point<T, 3> cross(
    const Point<T, 3>& a,
    const Point<T, 3>& b) noexcept
{
    return Point<T, 3>{
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    };
}

using Point2d = Point<double, 2>;
using Point3d = Point<double, 3>;
using Point4d = Point<double, 4>;

} // namespace opencagd
