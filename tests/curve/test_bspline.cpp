#include <cmath>

#include <gtest/gtest.h>

#include <opencagd/curve/bspline_curve.hpp>
#include <opencagd/curve/bspline_derivatives.hpp>

TEST(OpenCAGDBSplineBasis, PartitionOfUnity)
{
    const opencagd::curve::KnotVector knots(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0});

    for (double u : {0.0, 0.2, 0.9, 1.0, 1.7, 2.8, 3.0})
    {
        const auto basis = opencagd::curve::all_basis_functions(knots, u);
        double sum = 0.0;
        for (double value : basis) sum += value;
        EXPECT_NEAR(sum, 1.0, 1.0e-12);
    }
}

TEST(OpenCAGDBSplineCurve, ClampedEndpoints)
{
    const opencagd::curve::BSplineCurve2d curve(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0},
        {{0.0, 0.0}, {1.0, 2.0}, {2.0, -0.5}, {3.2, 2.2}, {4.0, 0.0}});

    EXPECT_NEAR((curve.evaluate(0.0) - curve.control_points().front()).norm(), 0.0, 1.0e-12);
    EXPECT_NEAR((curve.evaluate(3.0) - curve.control_points().back()).norm(), 0.0, 1.0e-12);
}


TEST(OpenCAGDBSplineDerivatives, BasisDerivativeSums)
{
    const opencagd::curve::KnotVector knots(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0});

    for (double u : {0.2, 0.9, 1.35, 2.2, 2.8})
    {
        const auto span = knots.find_span(u);
        const auto ders =
            opencagd::curve::basis_function_derivatives(knots, span, u, 2);

        double sum0 = 0.0;
        double sum1 = 0.0;
        double sum2 = 0.0;

        for (std::size_t j = 0; j <= knots.degree(); ++j)
        {
            sum0 += ders[0][j];
            sum1 += ders[1][j];
            sum2 += ders[2][j];
        }

        EXPECT_NEAR(sum0, 1.0, 1.0e-12);
        EXPECT_NEAR(sum1, 0.0, 1.0e-12);
        EXPECT_NEAR(sum2, 0.0, 1.0e-12);
    }
}

TEST(OpenCAGDBSplineDerivatives, QuadraticBezierEquivalent)
{
    const opencagd::curve::BSplineCurve2d curve(
        2,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}});

    const auto ders = curve.derivatives(0.5, 2);

    EXPECT_NEAR(ders[0][0], 0.75, 1.0e-12);
    EXPECT_NEAR(ders[0][1], 0.25, 1.0e-12);

    EXPECT_NEAR(ders[1][0], 1.0, 1.0e-12);
    EXPECT_NEAR(ders[1][1], 1.0, 1.0e-12);

    EXPECT_NEAR(ders[2][0], -2.0, 1.0e-12);
    EXPECT_NEAR(ders[2][1], 2.0, 1.0e-12);

    EXPECT_NEAR(curve.tangent(0.5).norm(), 1.0, 1.0e-12);
    EXPECT_NEAR(curve.curvature(0.5), std::sqrt(2.0), 1.0e-12);
}
