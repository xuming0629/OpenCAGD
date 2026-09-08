#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>

namespace opencagd {
namespace curve {

OPENCAGD_CURVE_API double bernstein(std::size_t i, std::size_t degree, double t);
OPENCAGD_CURVE_API std::vector<double> bernstein_basis(std::size_t degree, double t);

} // namespace curve
} // namespace opencagd
