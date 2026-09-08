#pragma once

#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/linalg/dense_matrix.h>

namespace opencagd {
namespace linalg {

OPENCAGD_LINALG_API std::vector<double> solve_linear_system(
    const DenseMatrix& matrix,
    const std::vector<double>& rhs);

OPENCAGD_LINALG_API std::vector<double> solve_least_squares(
    const DenseMatrix& matrix,
    const std::vector<double>& rhs);

} // namespace linalg
} // namespace opencagd
