#include <opencagd/curve/bspline_basis.h>

#include <algorithm>
#include <cstddef>
#include <utility>

namespace opencagd {
namespace curve {

std::vector<double> basis_functions(
    const KnotVector& knot_vector,
    std::size_t span,
    double u)
{
    const std::size_t p = knot_vector.degree();
    std::vector<double> N(p + 1, 0.0);
    std::vector<double> left(p + 1, 0.0);
    std::vector<double> right(p + 1, 0.0);
    N[0] = 1.0;

    for (std::size_t j = 1; j <= p; ++j)
    {
        left[j] = u - knot_vector[span + 1 - j];
        right[j] = knot_vector[span + j] - u;
        double saved = 0.0;
        for (std::size_t r = 0; r < j; ++r)
        {
            const double denominator = right[r + 1] + left[j - r];
            const double temp = denominator == 0.0 ? 0.0 : N[r] / denominator;
            N[r] = saved + right[r + 1] * temp;
            saved = left[j - r] * temp;
        }
        N[j] = saved;
    }
    return N;
}

std::vector<double> all_basis_functions(const KnotVector& knot_vector, double u)
{
    std::vector<double> result(knot_vector.num_basis_functions(), 0.0);
    const std::size_t span = knot_vector.find_span(u);
    const std::vector<double> local = basis_functions(knot_vector, span, u);
    const std::size_t first = span - knot_vector.degree();
    for (std::size_t j = 0; j <= knot_vector.degree(); ++j)
        result[first + j] = local[j];
    return result;
}

std::vector<std::vector<double> > basis_function_derivatives(
    const KnotVector& knot_vector,
    std::size_t span,
    double u,
    std::size_t derivative_order)
{
    const std::size_t p = knot_vector.degree();
    const std::size_t du = std::min(derivative_order, p);

    std::vector<std::vector<double> > ders(
        derivative_order + 1,
        std::vector<double>(p + 1, 0.0));
    std::vector<std::vector<double> > ndu(
        p + 1,
        std::vector<double>(p + 1, 0.0));
    std::vector<double> left(p + 1, 0.0);
    std::vector<double> right(p + 1, 0.0);

    ndu[0][0] = 1.0;

    for (std::size_t j = 1; j <= p; ++j)
    {
        left[j] = u - knot_vector[span + 1 - j];
        right[j] = knot_vector[span + j] - u;
        double saved = 0.0;

        for (std::size_t r = 0; r < j; ++r)
        {
            ndu[j][r] = right[r + 1] + left[j - r];
            const double denominator = ndu[j][r];
            const double temp = denominator == 0.0 ? 0.0 : ndu[r][j - 1] / denominator;
            ndu[r][j] = saved + right[r + 1] * temp;
            saved = left[j - r] * temp;
        }
        ndu[j][j] = saved;
    }

    for (std::size_t j = 0; j <= p; ++j)
        ders[0][j] = ndu[j][p];

    std::vector<std::vector<double> > a(2, std::vector<double>(p + 1, 0.0));

    for (std::size_t r = 0; r <= p; ++r)
    {
        std::size_t s1 = 0;
        std::size_t s2 = 1;
        a[0][0] = 1.0;

        for (std::size_t k = 1; k <= du; ++k)
        {
            double d = 0.0;
            const std::ptrdiff_t rk = static_cast<std::ptrdiff_t>(r) - static_cast<std::ptrdiff_t>(k);
            const std::ptrdiff_t pk = static_cast<std::ptrdiff_t>(p) - static_cast<std::ptrdiff_t>(k);
            std::fill(a[s2].begin(), a[s2].end(), 0.0);

            if (r >= k)
            {
                const double denominator = ndu[static_cast<std::size_t>(pk + 1)][static_cast<std::size_t>(rk)];
                a[s2][0] = denominator == 0.0 ? 0.0 : a[s1][0] / denominator;
                d = a[s2][0] * ndu[static_cast<std::size_t>(rk)][static_cast<std::size_t>(pk)];
            }

            const std::ptrdiff_t j1 = rk >= -1 ? 1 : -rk;
            const std::ptrdiff_t j2 =
                static_cast<std::ptrdiff_t>(r) - 1 <= pk
                    ? static_cast<std::ptrdiff_t>(k) - 1
                    : static_cast<std::ptrdiff_t>(p) - static_cast<std::ptrdiff_t>(r);

            for (std::ptrdiff_t j = j1; j <= j2; ++j)
            {
                const std::size_t column = static_cast<std::size_t>(rk + j);
                const double denominator = ndu[static_cast<std::size_t>(pk + 1)][column];
                const std::size_t jj = static_cast<std::size_t>(j);
                a[s2][jj] = denominator == 0.0
                    ? 0.0
                    : (a[s1][jj] - a[s1][jj - 1]) / denominator;
                d += a[s2][jj] * ndu[column][static_cast<std::size_t>(pk)];
            }

            if (static_cast<std::ptrdiff_t>(r) <= pk)
            {
                const double denominator = ndu[static_cast<std::size_t>(pk + 1)][r];
                a[s2][k] = denominator == 0.0 ? 0.0 : -a[s1][k - 1] / denominator;
                d += a[s2][k] * ndu[r][static_cast<std::size_t>(pk)];
            }

            ders[k][r] = d;
            std::swap(s1, s2);
        }
    }

    double factor = static_cast<double>(p);
    for (std::size_t k = 1; k <= du; ++k)
    {
        for (std::size_t j = 0; j <= p; ++j)
            ders[k][j] *= factor;
        factor *= static_cast<double>(p - k);
    }
    return ders;
}

std::vector<double> all_basis_function_derivatives(
    const KnotVector& knot_vector,
    double u,
    std::size_t derivative_order)
{
    std::vector<double> result(knot_vector.num_basis_functions(), 0.0);
    const std::size_t span = knot_vector.find_span(u);
    const std::vector<std::vector<double> > local =
        basis_function_derivatives(knot_vector, span, u, derivative_order);
    const std::size_t first = span - knot_vector.degree();
    for (std::size_t j = 0; j <= knot_vector.degree(); ++j)
        result[first + j] = local[derivative_order][j];
    return result;
}

} // namespace curve
} // namespace opencagd
