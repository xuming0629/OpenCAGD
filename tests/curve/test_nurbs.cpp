#include <cmath>

#include <gtest/gtest.h>
#include <opencagd/curve/nurbs_curve.hpp>

TEST(OpenCAGDNURBS, QuadraticQuarterCircleIsExact)
{
    const double w = std::sqrt(2.0) / 2.0;
    const opencagd::curve::NURBSCurve2d curve(
        2,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {{1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
        {1.0, w, 1.0});

    for (double u : {0.0, 0.1, 0.25, 0.5, 0.75, 0.9, 1.0})
    {
        const auto p = curve.evaluate(u);
        EXPECT_NEAR(p[0] * p[0] + p[1] * p[1], 1.0, 1.0e-12);
    }
}

TEST(OpenCAGDNURBS, RationalBasisIsPartitionOfUnity)
{
    const double w = std::sqrt(2.0) / 2.0;
    const opencagd::curve::NURBSCurve2d curve(
        2,
        {0.0, 0.0, 0.0, 1.0, 1.0, 1.0},
        {{1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
        {1.0, w, 1.0});

    for (double u : {0.0, 0.2, 0.5, 0.8, 1.0})
    {
        const auto basis = curve.rational_basis_functions(u);
        double sum = 0.0;
        for (double value : basis) sum += value;
        EXPECT_NEAR(sum, 1.0, 1.0e-12);
    }
}
