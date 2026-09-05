#include <mpi.h>
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    int initialized = 0;
    MPI_Initialized(&initialized);

    const bool owns_mpi = initialized == 0;
    if (owns_mpi)
    {
        const int rc = MPI_Init(&argc, &argv);
        if (rc != MPI_SUCCESS)
        {
            return rc;
        }
    }

    ::testing::InitGoogleTest(&argc, argv);
    const int result = RUN_ALL_TESTS();

    int finalized = 0;
    MPI_Finalized(&finalized);
    if (owns_mpi && !finalized)
    {
        MPI_Finalize();
    }

    return result;
}
