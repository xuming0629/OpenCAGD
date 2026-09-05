#include <cmath>
#include <iomanip>
#include <iostream>

#include <opencagd/curve/nurbs_curve.hpp>

int main()
{
    const double w = std::sqrt(2.0) / 2.0;
    const opencagd::curve::NURBSCurve2d quarter_circle(
        2,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {{1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
        {1.0, w, 1.0});

    std::cout << std::setprecision(12);
    for (double u : {0.0, 0.25, 0.5, 0.75, 1.0})
    {
        const auto p = quarter_circle.evaluate(u);
        const double radius = std::sqrt(p[0] * p[0] + p[1] * p[1]);
        std::cout << "u=" << u << " -> (" << p[0] << ", " << p[1]
                  << "), radius=" << radius << '\n';
    }
}
