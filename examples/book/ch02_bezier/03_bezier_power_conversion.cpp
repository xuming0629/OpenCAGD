#include <iostream>

#include <opencagd/curve/bezier_conversion.hpp>

int main()
{
    const opencagd::curve::BezierCurve2d bezier({
        {0.0, 0.0},
        {1.0, 2.0},
        {3.0, 3.0},
        {4.0, 0.0},
    });
    const auto polynomial = opencagd::curve::to_power_basis(bezier);

    for (double t : {0.0, 0.2, 0.5, 0.9, 1.0})
    {
        const auto a = bezier.evaluate(t);
        const auto b = polynomial.evaluate(t);
        std::cout << "t=" << t << "  conversion error = " << (a - b).norm() << '\n';
    }
}
