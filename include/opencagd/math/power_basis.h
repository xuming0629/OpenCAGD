#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>

namespace opencagd {
namespace math {

OPENCAGD_CORE_API std::vector<double> power_basis(std::size_t degree, double t);

} // namespace math
} // namespace opencagd
