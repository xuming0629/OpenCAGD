#include <opencagd/curve/bernstein.h>
#include <opencagd/math/numeric.h>

#include <cmath>

namespace opencagd {
namespace curve {

double bernstein(std::size_t i, std::size_t degree, double t)
{
    if (i > degree) return 0.0;
    if (t <= 0.0) return i == 0 ? 1.0 : 0.0;
    if (t >= 1.0) return i == degree ? 1.0 : 0.0;

    return static_cast<double>(math::binomial(degree, i)) *
           std::pow(t, static_cast<int>(i)) *
           std::pow(1.0 - t, static_cast<int>(degree - i));
}

std::vector<double> bernstein_basis(std::size_t degree, double t)
{
    std::vector<double> values(degree + 1, 0.0);
    for (std::size_t i = 0; i <= degree; ++i)
        values[i] = bernstein(i, degree, t);
    return values;
}

} // namespace curve
} // namespace opencagd
