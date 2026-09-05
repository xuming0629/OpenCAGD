#include <gtest/gtest.h>

#include <opencagd/curve/bezier_conversion.hpp>

TEST(OpenCAGDBernstein, PartitionOfUnity)
{
    for (double t : {0.0, 0.1, 0.25, 0.5, 0.9, 1.0})
    {
        const auto basis = opencagd::curve::bernstein_basis(5, t);
        double sum = 0.0;
        for (double value : basis) sum += value;
        EXPECT_NEAR(sum, 1.0, 1.0e-12);
    }
}

TEST(OpenCAGDBezier, EndpointsAndCasteljau)
{
    const opencagd::curve::BezierCurve2d curve({
        {0.0, 0.0}, {1.0, 2.0}, {3.0, 3.0}, {4.0, 0.0}
    });

    EXPECT_NEAR(curve.evaluate(0.0)[0], 0.0, 1.0e-12);
    EXPECT_NEAR(curve.evaluate(1.0)[0], 4.0, 1.0e-12);

    for (double t : {0.1, 0.3, 0.5, 0.8})
    {
        EXPECT_NEAR((curve.evaluate(t) - curve.evaluate_de_casteljau(t)).norm(), 0.0, 1.0e-12);
    }
}

TEST(OpenCAGDBezier, PowerBasisConversion)
{
    const opencagd::curve::BezierCurve2d curve({
        {0.0, 0.0}, {1.0, 2.0}, {3.0, 3.0}, {4.0, 0.0}
    });
    const auto polynomial = opencagd::curve::to_power_basis(curve);

    for (double t : {0.0, 0.17, 0.5, 0.83, 1.0})
    {
        EXPECT_NEAR((curve.evaluate(t) - polynomial.evaluate(t)).norm(), 0.0, 1.0e-11);
    }
}
