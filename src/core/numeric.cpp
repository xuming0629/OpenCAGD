#include <opencagd/math/numeric.h>

#include <algorithm>
#include <cmath>

namespace opencagd {
namespace math {

namespace {
const double kTolerance = 1.0e-12;
}

double tolerance()
{
    return kTolerance;
}

bool nearly_equal(double a, double b, double eps)
{
    if (eps < 0.0) eps = kTolerance;
    const double scale = std::max(1.0, std::max(std::abs(a), std::abs(b)));
    return std::abs(a - b) <= eps * scale;
}

std::size_t binomial(std::size_t n, std::size_t k)
{
    if (k > n) return 0;
    if (k > n - k) k = n - k;

    std::size_t result = 1;
    for (std::size_t i = 1; i <= k; ++i)
    {
        result = (result * (n - k + i)) / i;
    }
    return result;
}

double falling_factorial(std::size_t n, std::size_t k)
{
    if (k > n) return 0.0;
    double result = 1.0;
    for (std::size_t i = 0; i < k; ++i)
    {
        result *= static_cast<double>(n - i);
    }
    return result;
}

} // namespace math
} // namespace opencagd
