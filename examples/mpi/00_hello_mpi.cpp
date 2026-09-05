#include <iostream>

#include <opencagd/parallel/mpi.hpp>

int main(int argc, char** argv)
{
    opencagd::parallel::MpiEnvironment mpi(argc, argv);

    std::cout << "[rank " << mpi.rank() << "/" << mpi.size()
              << "] Hello from OpenCAGD MPI" << std::endl;

    return 0;
}
