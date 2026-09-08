#pragma once

#include <cstddef>
#include <opencagd/core/export.h>

namespace opencagd {
namespace math {

OPENCAGD_CORE_API double tolerance();
OPENCAGD_CORE_API bool nearly_equal(double a, double b, double eps = -1.0);
OPENCAGD_CORE_API std::size_t binomial(std::size_t n, std::size_t k);
OPENCAGD_CORE_API double falling_factorial(std::size_t n, std::size_t k);

} // namespace math
} // namespace opencagd
