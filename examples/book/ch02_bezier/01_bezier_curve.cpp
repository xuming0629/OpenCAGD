#include <iostream>

#include <opencagd/curve/bezier_curve.hpp>

int main()
{
    const opencagd::curve::BezierCurve2d curve({
        {0.0, 0.0},
        {1.0, 2.0},
        {3.0, 3.0},
        {4.0, 0.0},
    });

    std::cout << "Bezier degree = " << curve.degree() << '\n';
    for (double t : {0.0, 0.25, 0.5, 0.75, 1.0})
    {
        const auto p = curve.evaluate(t);
        std::cout << "t=" << t << " -> (" << p[0] << ", " << p[1] << ")\n";
    }
}
