#include <iostream>
#include <opencagd/parallel.h>
int main(int argc,char** argv){opencagd::parallel::MpiEnvironment mpi(argc,argv);std::cout<<"Hello from rank "<<mpi.rank()<<" / "<<mpi.size()<<'\n';return 0;}
