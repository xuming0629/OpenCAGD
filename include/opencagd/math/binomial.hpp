#pragma once

#include <cstddef>

namespace opencagd::math
{

[[nodiscard]] constexpr std::size_t binomial(std::size_t n, std::size_t k) noexcept
{
    if (k > n) return 0;
    if (k > n - k) k = n - k;

    std::size_t value = 1;
    for (std::size_t i = 1; i <= k; ++i)
    {
        value = value * (n - k + i) / i;
    }
    return value;
}

} // namespace opencagd::math
