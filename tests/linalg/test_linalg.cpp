#include <gtest/gtest.h>
#include <opencagd/linalg.h>

TEST(LinearSolverTest, SolvesDenseSystem)
{
    opencagd::linalg::DenseMatrix a(3,3);
    a(0,0)=3;a(0,1)=2;a(0,2)=-1;
    a(1,0)=2;a(1,1)=-2;a(1,2)=4;
    a(2,0)=-1;a(2,1)=0.5;a(2,2)=-1;
    const auto x = opencagd::linalg::solve_linear_system(a, {1,-2,0});
    ASSERT_EQ(x.size(), 3u);
    EXPECT_NEAR(x[0], 1.0, 1e-12); EXPECT_NEAR(x[1], -2.0, 1e-12); EXPECT_NEAR(x[2], -2.0, 1e-12);
}

TEST(LinearSolverTest, SolvesLeastSquaresLineFit)
{
    opencagd::linalg::DenseMatrix a(3,2);
    a(0,0)=1;a(0,1)=0; a(1,0)=1;a(1,1)=1; a(2,0)=1;a(2,1)=2;
    const auto x = opencagd::linalg::solve_least_squares(a, {1,3,5});
    ASSERT_EQ(x.size(), 2u);
    EXPECT_NEAR(x[0], 1.0, 1e-12); EXPECT_NEAR(x[1], 2.0, 1e-12);
}
