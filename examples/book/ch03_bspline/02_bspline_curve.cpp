#include <iostream>

#include <opencagd/curve/bspline_curve.hpp>

int main()
{
    const opencagd::curve::BSplineCurve2d curve(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0},
        {
            {0.0, 0.0},
            {1.0, 2.0},
            {2.0, -0.5},
            {3.2, 2.2},
            {4.0, 0.0},
        });

    for (double u : {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0})
    {
        const auto p = curve.evaluate(u);
        std::cout << "u=" << u << " -> (" << p[0] << ", " << p[1] << ")\n";
    }
}
