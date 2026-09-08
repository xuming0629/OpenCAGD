#include <iostream>
#include <opencagd/linalg.h>
int main(){opencagd::linalg::DenseMatrix A(4,2);for(std::size_t i=0;i<4;++i){A(i,0)=1;A(i,1)=static_cast<double>(i);}auto x=opencagd::linalg::solve_least_squares(A,{1.1,2.9,5.2,6.8});std::cout<<"fit y=a+b*x\na="<<x[0]<<" b="<<x[1]<<'\n';return 0;}
