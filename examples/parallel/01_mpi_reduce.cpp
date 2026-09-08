#include <iostream>
#include <mpi.h>
#include <opencagd/parallel.h>
int main(int argc,char** argv){opencagd::parallel::MpiEnvironment env(argc,argv);const int local=env.rank()+1;int total=0;opencagd::parallel::check_mpi(MPI_Reduce(&local,&total,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD),"MPI_Reduce");if(env.rank()==0)std::cout<<"sum(rank+1)="<<total<<" across "<<env.size()<<" ranks\n";return 0;}
