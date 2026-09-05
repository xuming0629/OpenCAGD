#include <cstdlib>

#include <mpi.h>
#include <gtest/gtest.h>

TEST(OpenCAGDMPI, RuntimeInitialized)
{
    int initialized = 0;
    ASSERT_EQ(MPI_Initialized(&initialized), MPI_SUCCESS);
    EXPECT_EQ(initialized, 1);

    int rank = -1;
    int size = 0;
    ASSERT_EQ(MPI_Comm_rank(MPI_COMM_WORLD, &rank), MPI_SUCCESS);
    ASSERT_EQ(MPI_Comm_size(MPI_COMM_WORLD, &size), MPI_SUCCESS);

    EXPECT_GE(rank, 0);
    EXPECT_LT(rank, size);
    EXPECT_GE(size, 1);
}

TEST(OpenCAGDMPI, MultiRank)
{
    int rank = -1;
    int size = 0;
    ASSERT_EQ(MPI_Comm_rank(MPI_COMM_WORLD, &rank), MPI_SUCCESS);
    ASSERT_EQ(MPI_Comm_size(MPI_COMM_WORLD, &size), MPI_SUCCESS);

    if (const char* expected = std::getenv("OPENCAGD_EXPECT_MPI_SIZE"))
    {
        EXPECT_EQ(size, std::atoi(expected));
    }
    else
    {
        EXPECT_GE(size, 1);
    }

    EXPECT_GE(rank, 0);
    EXPECT_LT(rank, size);
}
