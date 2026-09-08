#include <iomanip>
#include <iostream>
#include <opencagd/core.h>
int main() {
    const double t=0.4; const auto b=opencagd::math::power_basis(4,t);
    std::cout << "Power basis [1,t,t^2,...] at t="<<t<<":\n";
    for(std::size_t i=0;i<b.size();++i) std::cout << "  t^"<<i<<" = "<<std::setprecision(8)<<b[i]<<'\n';
    return 0;
}
