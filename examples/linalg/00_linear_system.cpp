#include <iostream>
#include <opencagd/linalg.h>
int main(){opencagd::linalg::DenseMatrix A(3,3);A(0,0)=3;A(0,1)=2;A(0,2)=-1;A(1,0)=2;A(1,1)=-2;A(1,2)=4;A(2,0)=-1;A(2,1)=.5;A(2,2)=-1;auto x=opencagd::linalg::solve_linear_system(A,{1,-2,0});for(std::size_t i=0;i<x.size();++i)std::cout<<"x["<<i<<"]="<<x[i]<<'\n';return 0;}
