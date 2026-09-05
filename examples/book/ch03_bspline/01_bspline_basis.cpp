#include <iostream>
#include <numeric>

#include <opencagd/curve/bspline_basis.hpp>

int main()
{
    const opencagd::curve::KnotVector knots(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0});

    for (double u : {0.0, 0.5, 1.0, 1.5, 2.5, 3.0})
    {
        const auto basis = opencagd::curve::all_basis_functions(knots, u);
        const double sum = std::accumulate(basis.begin(), basis.end(), 0.0);
        std::cout << "u=" << u << " : ";
        for (double value : basis) std::cout << value << ' ';
        std::cout << " sum=" << sum << '\n';
    }
}
