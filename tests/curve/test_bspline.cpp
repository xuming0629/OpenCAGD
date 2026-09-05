#include <gtest/gtest.h>

#include <opencagd/curve/bspline_curve.hpp>

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
