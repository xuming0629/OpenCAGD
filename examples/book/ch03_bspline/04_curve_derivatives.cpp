#include <iomanip>
#include <iostream>

#include <opencagd/curve/bspline_curve.hpp>

int main()
{
    using opencagd::curve::BSplineCurve2d;

    const BSplineCurve2d curve(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0},
        {{0.0, 0.0},
         {1.0, 2.0},
         {2.0, -0.5},
         {3.2, 2.2},
         {4.0, 0.0}});

    const double u = 1.5;
    const auto ders = curve.derivatives(u, 2);
    const auto tangent = curve.tangent(u);

    std::cout << std::fixed << std::setprecision(8);
    std::cout << "C(u)   = (" << ders[0][0] << ", " << ders[0][1] << ")\n";
    std::cout << "C'(u)  = (" << ders[1][0] << ", " << ders[1][1] << ")\n";
    std::cout << "C''(u) = (" << ders[2][0] << ", " << ders[2][1] << ")\n";
    std::cout << "T(u)   = (" << tangent[0] << ", " << tangent[1] << ")\n";
    std::cout << "kappa  = " << curve.curvature(u) << '\n';
}
