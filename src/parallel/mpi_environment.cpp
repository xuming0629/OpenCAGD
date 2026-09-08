#include <opencagd/parallel/mpi_environment.h>

#include <stdexcept>
#include <string>

namespace opencagd {
namespace parallel {

void check_mpi(int error_code, const char* operation)
{
    if (error_code == MPI_SUCCESS) return;
    char error_string[MPI_MAX_ERROR_STRING] = {0};
    int length = 0;
    MPI_Error_string(error_code, error_string, &length);
    throw std::runtime_error(
        std::string(operation) + " failed: " +
        std::string(error_string, static_cast<std::size_t>(length)));
}

MpiEnvironment::MpiEnvironment(int& argc, char**& argv) : owns_mpi_(false)
{
    int initialized = 0;
    check_mpi(MPI_Initialized(&initialized), "MPI_Initialized");
    if (!initialized)
    {
        check_mpi(MPI_Init(&argc, &argv), "MPI_Init");
        owns_mpi_ = true;
    }
}

MpiEnvironment::~MpiEnvironment()
{
    if (!owns_mpi_) return;
    int finalized = 0;
    if (MPI_Finalized(&finalized) == MPI_SUCCESS && !finalized) MPI_Finalize();
}

int MpiEnvironment::rank(MPI_Comm communicator) const
{
    int value = 0;
    check_mpi(MPI_Comm_rank(communicator, &value), "MPI_Comm_rank");
    return value;
}

int MpiEnvironment::size(MPI_Comm communicator) const
{
    int value = 0;
    check_mpi(MPI_Comm_size(communicator, &value), "MPI_Comm_size");
    return value;
}

} // namespace parallel
} // namespace opencagd
