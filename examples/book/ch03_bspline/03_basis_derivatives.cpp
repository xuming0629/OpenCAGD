#include <iomanip>
#include <iostream>

#include <opencagd/curve/bspline_derivatives.hpp>
#include <opencagd/curve/knot_vector.hpp>

int main()
{
    using namespace opencagd::curve;

    const KnotVector knots(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0});

    const double u = 1.35;
    const std::size_t span = knots.find_span(u);
    const auto ders = basis_function_derivatives(knots, span, u, 2);

    std::cout << std::fixed << std::setprecision(8);
    std::cout << "u = " << u << ", span = " << span << '\n';

    for (std::size_t k = 0; k < ders.size(); ++k)
    {
        double sum = 0.0;
        std::cout << "order " << k << ": ";
        for (double value : ders[k])
        {
            std::cout << std::setw(12) << value << ' ';
            sum += value;
        }
        std::cout << "  sum = " << sum << '\n';
    }

    std::cout << "\nExpected identities:\n"
              << "  sum N_i(u)   = 1\n"
              << "  sum N_i'(u)  = 0\n"
              << "  sum N_i''(u) = 0\n";
}
