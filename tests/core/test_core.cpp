#include <gtest/gtest.h>
#include <opencagd/core.h>

TEST(Vector3DTest, DotAndCrossProducts)
{
    using namespace opencagd::geometry;
    const Vector3D x(1.0, 0.0, 0.0);
    const Vector3D y(0.0, 1.0, 0.0);
    const Vector3D z = cross(x, y);
    EXPECT_NEAR(z.x(), 0.0, 1e-12);
    EXPECT_NEAR(z.y(), 0.0, 1e-12);
    EXPECT_NEAR(z.z(), 1.0, 1e-12);
    EXPECT_NEAR(dot(x, y), 0.0, 1e-12);
}

TEST(Point3DTest, PointVectorArithmetic)
{
    using namespace opencagd::geometry;
    const Point3D p(1.0, 2.0, 3.0);
    const Vector3D v(0.5, -1.0, 2.0);
    const Point3D q = p + v;
    EXPECT_DOUBLE_EQ(q.x(), 1.5);
    EXPECT_DOUBLE_EQ(q.y(), 1.0);
    EXPECT_DOUBLE_EQ(q.z(), 5.0);
}

TEST(NumericTest, BinomialCoefficient)
{
    EXPECT_EQ(opencagd::math::binomial(5, 2), 10u);
    EXPECT_EQ(opencagd::math::binomial(8, 0), 1u);
    EXPECT_EQ(opencagd::math::binomial(8, 8), 1u);
}

TEST(PowerBasisTest, EvaluatesExpectedMonomials)
{
    const std::vector<double> basis = opencagd::math::power_basis(3, 0.5);
    ASSERT_EQ(basis.size(), 4u);
    EXPECT_DOUBLE_EQ(basis[0], 1.0);
    EXPECT_DOUBLE_EQ(basis[1], 0.5);
    EXPECT_DOUBLE_EQ(basis[2], 0.25);
    EXPECT_DOUBLE_EQ(basis[3], 0.125);
}
