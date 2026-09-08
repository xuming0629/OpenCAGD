#include <cmath>
#include <gtest/gtest.h>
#include <opencagd/curve.h>

namespace {
using opencagd::geometry::Point3D;

TEST(BezierCurveTest, BernsteinAndDeCasteljauAgree)
{
    const opencagd::curve::BezierCurve curve({Point3D(0,0), Point3D(1,2), Point3D(2,0)});
    for (int k = 0; k <= 20; ++k) {
        const double t = static_cast<double>(k) / 20.0;
        const Point3D a = curve.evaluate(t);
        const Point3D b = curve.evaluate_de_casteljau(t);
        EXPECT_NEAR(a.x(), b.x(), 1e-12);
        EXPECT_NEAR(a.y(), b.y(), 1e-12);
        EXPECT_NEAR(a.z(), b.z(), 1e-12);
    }
}

TEST(BezierCurveTest, SubdivisionJoinsAtSplitPoint)
{
    const opencagd::curve::BezierCurve curve({Point3D(0,0), Point3D(1,2), Point3D(2,-1), Point3D(4,1)});
    const auto parts = curve.subdivide(0.4);
    const Point3D q = curve.evaluate(0.4);
    const Point3D left = parts.first.evaluate(1.0);
    const Point3D right = parts.second.evaluate(0.0);
    EXPECT_NEAR(q.x(), left.x(), 1e-12); EXPECT_NEAR(q.y(), left.y(), 1e-12);
    EXPECT_NEAR(q.x(), right.x(), 1e-12); EXPECT_NEAR(q.y(), right.y(), 1e-12);
}

TEST(BSplineBasisTest, PartitionOfUnity)
{
    const opencagd::curve::KnotVector kv(3, {0,0,0,0,0.3,0.6,1,1,1,1});
    for (int k = 0; k <= 100; ++k) {
        const double u = static_cast<double>(k) / 100.0;
        const auto n = opencagd::curve::all_basis_functions(kv, u);
        double sum = 0.0;
        for (double value : n) sum += value;
        EXPECT_NEAR(sum, 1.0, 1e-10) << "u=" << u;
    }
}

TEST(NURBSCurveTest, QuadraticQuarterCircleIsExact)
{
    const double w = std::sqrt(0.5);
    const opencagd::curve::NURBSCurve circle(
        2, {0,0,0,1,1,1}, {Point3D(1,0), Point3D(1,1), Point3D(0,1)}, {1,w,1});
    for (int k = 0; k <= 20; ++k) {
        const Point3D p = circle.evaluate(static_cast<double>(k) / 20.0);
        EXPECT_NEAR(p.x()*p.x() + p.y()*p.y(), 1.0, 1e-10);
    }
    EXPECT_NEAR(circle.curvature(0.5), 1.0, 1e-9);
}

TEST(BezierConversionTest, PowerBasisMatchesBezier)
{
    const opencagd::curve::BezierCurve bezier({Point3D(0,0), Point3D(1,2), Point3D(2,0)});
    const opencagd::curve::PolynomialCurve power = opencagd::curve::to_power_basis(bezier);
    for (int k = 0; k <= 20; ++k) {
        const double t = static_cast<double>(k) / 20.0;
        EXPECT_NEAR(power.evaluate(t).x(), bezier.evaluate(t).x(), 1e-11);
        EXPECT_NEAR(power.evaluate(t).y(), bezier.evaluate(t).y(), 1e-11);
    }
}
} // namespace
