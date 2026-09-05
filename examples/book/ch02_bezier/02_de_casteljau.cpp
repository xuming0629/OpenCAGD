#include <cmath>
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

    for (double t : {0.1, 0.3, 0.5, 0.8})
    {
        const auto bernstein = curve.evaluate(t);
        const auto casteljau = curve.evaluate_de_casteljau(t);
        const double error = (bernstein - casteljau).norm();
        std::cout << "t=" << t << "  |Bernstein - deCasteljau| = " << error << '\n';
    }
}
