#include <iostream>
#include <opencagd/curve.h>
int main(){ const double t=0.35; const auto b=opencagd::curve::bernstein_basis(4,t); double sum=0; std::cout<<"Bernstein degree 4 at t="<<t<<"\n"; for(std::size_t i=0;i<b.size();++i){sum+=b[i];std::cout<<"B["<<i<<"]="<<b[i]<<'\n';} std::cout<<"sum="<<sum<<'\n'; return 0; }
