#pragma once

#include <cstddef>
#include <vector>

namespace opencagd::math
{

[[nodiscard]] inline std::vector<double> power_basis(std::size_t degree, double t)
{
    std::vector<double> values(degree + 1, 1.0);
    for (std::size_t i = 1; i <= degree; ++i)
    {
        values[i] = values[i - 1] * t;
    }
    return values;
}

} // namespace opencagd::math
