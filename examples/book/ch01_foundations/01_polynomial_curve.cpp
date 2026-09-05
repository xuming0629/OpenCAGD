#include <iostream>

#include <opencagd/curve/polynomial_curve.hpp>

int main()
{
    // C(t) = a0 + a1 t + a2 t^2
    const opencagd::curve::PolynomialCurve2d curve({
        {0.0, 0.0},
        {2.0, 4.0},
        {1.0, -2.0},
    });

    for (double t : {0.0, 0.25, 0.5, 0.75, 1.0})
    {
        const auto p = curve.evaluate(t);
        std::cout << "t=" << t << " -> (" << p[0] << ", " << p[1] << ")\n";
    }
}
