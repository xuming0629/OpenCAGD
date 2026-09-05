#include <cmath>
#include <iostream>
#include <numeric>

#include <opencagd/curve/nurbs_curve.hpp>

int main()
{
    const double w = std::sqrt(2.0) / 2.0;
    const opencagd::curve::NURBSCurve2d curve(
        2,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {{1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
        {1.0, w, 1.0});

    for (double u : {0.0, 0.25, 0.5, 0.75, 1.0})
    {
        const auto basis = curve.rational_basis_functions(u);
        const double sum = std::accumulate(basis.begin(), basis.end(), 0.0);
        std::cout << "u=" << u << " : ";
        for (double value : basis) std::cout << value << ' ';
        std::cout << " sum=" << sum << '\n';
    }
}
