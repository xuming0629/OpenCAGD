#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace opencagd::surface
{
template <typename Point>
struct SurfaceSampleGrid
{
    using point_type = Point;

    std::size_t u_count{};
    std::size_t v_count{};
    std::vector<Point> points;

    [[nodiscard]] bool empty() const noexcept { return points.empty(); }

    [[nodiscard]] const Point& at(std::size_t i, std::size_t j) const
    {
        if (i >= u_count || j >= v_count)
            throw std::out_of_range("SurfaceSampleGrid index out of range");
        return points.at(i * v_count + j);
    }

    [[nodiscard]] Point& at(std::size_t i, std::size_t j)
    {
        if (i >= u_count || j >= v_count)
            throw std::out_of_range("SurfaceSampleGrid index out of range");
        return points.at(i * v_count + j);
    }
};
} // namespace opencagd::surface
