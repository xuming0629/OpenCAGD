#pragma once

#include <mpi.h>
#include <opencagd/core/export.h>

namespace opencagd {
namespace parallel {

OPENCAGD_PARALLEL_API void check_mpi(int error_code, const char* operation);

class OPENCAGD_PARALLEL_API MpiEnvironment
{
public:
    MpiEnvironment(int& argc, char**& argv);
    ~MpiEnvironment();

    MpiEnvironment(const MpiEnvironment&) = delete;
    MpiEnvironment& operator=(const MpiEnvironment&) = delete;

    int rank(MPI_Comm communicator = MPI_COMM_WORLD) const;
    int size(MPI_Comm communicator = MPI_COMM_WORLD) const;

private:
    bool owns_mpi_;
};

} // namespace parallel
} // namespace opencagd
