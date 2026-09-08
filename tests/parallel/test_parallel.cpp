#include <gtest/gtest.h>
#include <mpi.h>
#include <opencagd/parallel.h>

TEST(MpiEnvironmentTest, ReportsValidWorldRankAndSize)
{
    int argc = 0;
    char** argv = nullptr;
    opencagd::parallel::MpiEnvironment mpi(argc, argv);
    EXPECT_GE(mpi.rank(), 0);
    EXPECT_GE(mpi.size(), 1);
    EXPECT_LT(mpi.rank(), mpi.size());
}
