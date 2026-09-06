#include <cmath>
#include <iomanip>
#include <iostream>

#include <opencagd/curve/nurbs_curve.hpp>

int main()
{
    using opencagd::curve::NURBSCurve2d;

    const double w = std::sqrt(2.0) / 2.0;

    const NURBSCurve2d circle(
        2,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {{1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
        {1.0, w, 1.0});

    const double u = 0.5;
    const auto ders = circle.derivatives(u, 2);
    const auto tangent = circle.tangent(u);

    std::cout << std::fixed << std::setprecision(10);
    std::cout << "Quarter circle at u = 0.5\n";
    std::cout << "C      = (" << ders[0][0] << ", " << ders[0][1] << ")\n";
    std::cout << "C'     = (" << ders[1][0] << ", " << ders[1][1] << ")\n";
    std::cout << "C''    = (" << ders[2][0] << ", " << ders[2][1] << ")\n";
    std::cout << "T      = (" << tangent[0] << ", " << tangent[1] << ")\n";
    std::cout << "kappa  = " << circle.curvature(u) << "  (exact = 1)\n";
}
