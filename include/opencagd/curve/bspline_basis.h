#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/curve/knot_vector.h>

namespace opencagd {
namespace curve {

OPENCAGD_CURVE_API std::vector<double> basis_functions(
    const KnotVector& knot_vector,
    std::size_t span,
    double u);

OPENCAGD_CURVE_API std::vector<double> all_basis_functions(
    const KnotVector& knot_vector,
    double u);

OPENCAGD_CURVE_API std::vector<std::vector<double> > basis_function_derivatives(
    const KnotVector& knot_vector,
    std::size_t span,
    double u,
    std::size_t derivative_order);

OPENCAGD_CURVE_API std::vector<double> all_basis_function_derivatives(
    const KnotVector& knot_vector,
    double u,
    std::size_t derivative_order);

} // namespace curve
} // namespace opencagd
