#include <iostream>

#include <mpi.h>
#include <opencagd/parallel/mpi.hpp>

int main(int argc, char** argv)
{
    opencagd::parallel::MpiEnvironment mpi(argc, argv);

    const int rank = mpi.rank();
    const double local_value = static_cast<double>(rank + 1);
    double global_sum = 0.0;

    opencagd::parallel::check_mpi(
        MPI_Reduce(
            &local_value,
            &global_sum,
            1,
            MPI_DOUBLE,
            MPI_SUM,
            0,
            MPI_COMM_WORLD),
        "MPI_Reduce");

    std::cout << "rank " << rank << ": local_value = " << local_value << '\n';

    if (rank == 0)
    {
        std::cout << "global_sum = " << global_sum << '\n';
    }

    return 0;
}
