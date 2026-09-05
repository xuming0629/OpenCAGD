add_executable(opencagd_hello basic/00_hello.cpp)
target_link_libraries(opencagd_hello PRIVATE OpenCAGD::OpenCAGD)

# The learning track follows the mathematical dependency order used in the book.
add_subdirectory(book)

if(TARGET OpenCAGD::Plotting)
    add_subdirectory(visualization)
endif()

if(OPENCAGD_ENABLE_MPI)
    add_executable(opencagd_mpi_hello mpi/00_hello_mpi.cpp)
    target_link_libraries(opencagd_mpi_hello PRIVATE OpenCAGD::OpenCAGD OpenCAGD::MPI)

    add_executable(opencagd_mpi_reduce mpi/01_reduce.cpp)
    target_link_libraries(opencagd_mpi_reduce PRIVATE OpenCAGD::OpenCAGD OpenCAGD::MPI)
endif()
