#include <opencagd/math/power_basis.h>

namespace opencagd {
namespace math {

std::vector<double> power_basis(std::size_t degree, double t)
{
    std::vector<double> values(degree + 1, 1.0);
    for (std::size_t i = 1; i <= degree; ++i)
        values[i] = values[i - 1] * t;
    return values;
}

} // namespace math
} // namespace opencagd
